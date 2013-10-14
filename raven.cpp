#include "raven/raven.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace raven {

	//!< initialize as useless
	Dsn Dsn::default_instance("<useless>", DONT);

	void Dsn::init(const std::string& url, const int flags_)
	{
		if (socket >= 0) {
			close(socket);
		}

		started = time(0); 
		flags = flags_; 
		socket = -1;
		memset(&addr, 0x00, sizeof(addr));

		key = "";
		extras.clear();

		//!< don't use
		if (flags == DONT) {
			return;
		}

		int port;
		char c_key[100];
		char c_secret[100];
		char c_host[100];
		char c_project[100];
		char c_none;

		int n = sscanf(url.c_str(),
				"udp://%100[a-z0-9]:%100[a-z0-9]@%100[^:]:%d/%[0-9]%c",
				c_key, c_secret, c_host, &port, c_project, &c_none);

		if (n != 5 || port <= 0 || port >= 65536) {
			std::cerr << "raven::Dsn::init(): cannot read url '" << url << "'" << std::endl;
			throw std::invalid_argument("unknown port");
		}

		struct hostent* he = gethostbyname(c_host);
		if (!he) {
			std::cerr << "raven::Dsn::init(): gethostbyname('" << c_host << "') failed: "
				<< h_errno << ", " << hstrerror(h_errno) << std::endl;
			throw std::invalid_argument("unknown host");
		}

		struct in_addr** addr_list = (struct in_addr**) he->h_addr_list;
		addr.sin_family = AF_INET;
		addr.sin_addr = *addr_list[0];
		addr.sin_port = htons(port);

		struct utsname sysname;
		uname(&sysname);

		key = c_key;

		extras["project"] = c_project;
		extras["server_name"] = sysname.nodename;
		extras["platform"] = "C++";
		extras["logger"] = "root";
		extras["extra.sys.machine"] = sysname.machine;

		socket = ::socket(AF_INET, SOCK_DGRAM, 0);
		if (socket < 0) {
			std::cerr << "raven::Dsn::init(): socket failed: " << errno << ", " << strerror(errno) << std::endl;
			throw std::runtime_error("cannot create socket");
		}
	}

	void Dsn::init(const int flags) 
	{
		const char* sentry_dsn = getenv("SENTRY_DSN");

		if (sentry_dsn == NULL) {
			std::cerr << "raven::Dsn::init(): missing environment variable SENTRY_DSN" << std::endl;
			throw std::invalid_argument("missing environment variable SENTRY_DSN");
		}

		init(sentry_dsn, flags);
	}

	static inline const char* gen_event_id(char* event_id, struct timeval& tv_now)
	{
		static unsigned int pid = getpid();
		unsigned int rnd = rand();

		sprintf(event_id, "%.8x%.8x%.8x%.8x",
				(unsigned int) tv_now.tv_sec, (unsigned int) tv_now.tv_usec, pid, rnd);
		return event_id;
	}

	time_t Dsn::attach_main(Message& message) const
	{
		struct timeval tv_now;
		gettimeofday(&tv_now, NULL);
		time_t t_now = tv_now.tv_sec;

		struct tm tm_now;
		gmtime_r(&t_now, &tm_now);

		char tstamp_now[32];
		strftime(tstamp_now, sizeof(tstamp_now), "%FT%T", &tm_now);

		char event_id[33];
		message.put("event_id", gen_event_id(event_id, tv_now));
		message.put("timestamp", tstamp_now);
		message.put("extra.sys.uptime", (t_now - started));
		return t_now;
	}

	void Dsn::attach_proc(Message& message) const
	{
		if (flags & ATTACH_PROC_STATUS) {
			std::ifstream status_file("/proc/self/status");
			std::string line;

			while (std::getline(status_file, line)) {
				size_t pos = line.find(':');

				if (line.compare(0, 2, "Vm", 0, 2) == 0 ||
						line.compare(0, 6, "FDSize", 0, 6) == 0 ||
						line.compare(0, 7, "Threads", 0, 7) == 0) {
					message.put("extra.sys." + line.substr(0, pos), boost::trim_copy(line.substr(pos + 1)));
				}
			}
		}

		if (flags & ATTACH_PROC_LOAD) {
			std::ifstream load_file("/proc/loadavg");
			std::string line;

			std::getline(load_file, line);
			message.put("extra.sys.load", line);
		}
	}

	void Dsn::make_packet(const Message& message, time_t t_now, std::string& packet) const
	{
		std::string encoded;
		std::stringstream packet_stream;

		encode(message, encoded);

		packet_stream << "Sentry sentry_timestamp=" << t_now << ".0, sentry_client=raven-cpp/0.0.1, sentry_version=2.0, sentry_key=" << key << "\n\n" << encoded;
		packet = packet_stream.str();
	}

	void Dsn::send_packet(const std::string& packet) const
	{
		ssize_t n = sendto(socket, packet.data(), packet.size(), MSG_DONTWAIT,
				(struct sockaddr*) &addr, sizeof(addr));

		if (n < 0) {
			std::cerr << "raven::Dsn::send_packet(): not sent: " << errno << ", " << strerror(errno) << std::endl;
		}
	}

	void Dsn::capture(Message& message) const
	{
		// do nothing if init() not called
		if (started == 0) return;

		try {
			time_t t_now = attach_main(message);
			attach_proc(message);

			for (std::map<std::string, std::string>::const_iterator
					it = extras.begin(); it != extras.end(); ++it) {
				message.put(it->first, it->second);
			}

			std::string packet;
			make_packet(message, t_now, packet);
			send_packet(packet);
		} catch (const std::exception& e) {
			std::cerr << "raven::Dsn::capture(): exception catched: " << e.what() << std::endl;
		}
	}

	void to_json_stream(const Message& message, std::ostream& output)
	{
		// XXX: escaping "/" => "\/"
		boost::property_tree::json_parser::write_json(output, message);
	}

	void to_json_string(const Message& message, std::string& output)
	{
		std::stringstream json;
		to_json_stream(message, json);
		output = json.str();
	}

	void encode(const Message& message, std::string& output)
	{
		// returns nothing, encode base64 and zlib always works
		std::stringstream json;
		to_json_stream(message, json);

		boost::iostreams::filtering_istream filtering;
		filtering.push(boost::iostreams::zlib_compressor());
		filtering.push(json);

		std::stringstream compressing;
		boost::iostreams::copy(filtering, compressing);
		std::string compressed = compressing.str();

		using boost::archive::iterators::transform_width;
		using boost::archive::iterators::base64_from_binary;
		typedef base64_from_binary<transform_width<std::string::const_iterator, 6, 8> > b64iter;

		output.assign(b64iter(compressed.begin()), b64iter(compressed.end()));

		// need to fill '=' - boost doesn't use '=', just converts bits
		int padding = output.size() % 4;
		if (padding > 0) output.append(4 - padding, '=');
	}

	bool decode(const std::string& encoded, Message& message)
	{
		try {
			// need to replace '=' to 'A' - boost doesn't use '=', just converts bits
			std::string prepared = encoded;
			if (prepared.size() > 0 && prepared[prepared.size() - 1] == '=') prepared[prepared.size() - 1] = 'A';
			if (prepared.size() > 1 && prepared[prepared.size() - 2] == '=') prepared[prepared.size() - 2] = 'A';

			using boost::archive::iterators::transform_width;
			using boost::archive::iterators::binary_from_base64;
			typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6> b64iter;
			std::string compressed2(b64iter(prepared.begin()), b64iter(prepared.end()));
			std::stringstream compressed;
			//std::copy(b64iter(prepared.begin()), b64iter(prepared.end()), compressed);
			compressed << compressed2;

			boost::iostreams::filtering_istream filtering;
			filtering.push(boost::iostreams::zlib_decompressor());
			filtering.push(compressed);

			std::stringstream decompressed;
			boost::iostreams::copy(filtering, decompressed);

			boost::property_tree::json_parser::read_json(decompressed, message);
			return true;
		} catch (const std::exception& e) {
			std::cerr << "raven::decode(): cannot decode: " << e.what() << std::endl;
			return false;
		}
	}

};


