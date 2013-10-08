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

	bool init_dsn(dsn_t* dsn, const std::string& url, const bool proc)
	{
		dsn->global_started = 0;
		dsn->global_attach_proc = proc;
		dsn->global_seed = 0;
		dsn->global_socket = -1;
		
		int n = sscanf(url.c_str(),
		  "udp://%100[a-z0-9]:%100[a-z0-9]@%100[^:]:%d/%[0-9]%c",
		  dsn->key, dsn->secret, dsn->host, &dsn->port, dsn->project, &dsn->none);

		if (n != 5 || dsn->port <= 0 || dsn->port >= 65536) {
			std::cerr << "raven::init(): cannot read url '" << url << "'" << std::endl;
			return false;
		}

		struct hostent* he = gethostbyname(dsn->host);
		if (!he) {
			std::cerr << "raven::init(): gethostbyname('" << dsn->host << "') failed: "
			  << h_errno << ", " << hstrerror(h_errno) << std::endl;
			return false;
		}

		struct in_addr** addr_list = (struct in_addr**) he->h_addr_list;
		dsn->global_addr.sin_family = AF_INET;
		dsn->global_addr.sin_addr = *addr_list[0];
		dsn->global_addr.sin_port = htons(dsn->port);

		struct utsname sysname;
		uname(&sysname);

		dsn->global_key = dsn->key;
		dsn->global_attach_proc = proc;
		dsn->global_started = time(0);
		dsn->global_seed = dsn->global_started;

		dsn->global_message["project"] = dsn->project;
		dsn->global_message["server_name"] = sysname.nodename;
		dsn->global_message["platform"] = "C++";
		dsn->global_message["logger"] = "root";
		dsn->global_message["extra.sys.machine"] = sysname.machine;

		dsn->global_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (dsn->global_socket < 0) {
			std::cerr << "raven::init(): socket failed: " << errno << ", " << strerror(errno) << std::endl;
			return false;
		}

		return true;
	}


	static std::map<std::string, std::string> global_message;
	static std::string global_key;

	static time_t global_started = 0;
	static bool global_attach_proc = false;
	static unsigned int global_seed = 0;
	static int global_socket = -1;
	static struct sockaddr_in global_addr;

	bool init(const std::string& url, const bool proc)
	{
		char key[100], secret[100], host[100], project[100], none;
		int port;

		int n = sscanf(url.c_str(),
		  "udp://%100[a-z0-9]:%100[a-z0-9]@%100[^:]:%d/%[0-9]%c",
		  key, secret, host, &port, project, &none);

		if (n != 5 || port <= 0 || port >= 65536) {
			std::cerr << "raven::init(): cannot read url '" << url << "'" << std::endl;
			return false;
		}

		struct hostent* he = gethostbyname(host);
		if (!he) {
			std::cerr << "raven::init(): gethostbyname('" << host << "') failed: "
			  << h_errno << ", " << hstrerror(h_errno) << std::endl;
			return false;
		}

		struct in_addr** addr_list = (struct in_addr**)he->h_addr_list;
		global_addr.sin_family = AF_INET;
		global_addr.sin_addr = *addr_list[0];
		global_addr.sin_port = htons(port);

		struct utsname sysname;
		uname(&sysname);

		global_key = key;
		global_attach_proc = proc;
		global_started = time(0);
		global_seed = global_started;

		global_message["project"] = project;
		global_message["server_name"] = sysname.nodename;
		global_message["platform"] = "C++";
		global_message["logger"] = "root";
		global_message["extra.sys.machine"] = sysname.machine;

		global_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (global_socket < 0) {
			std::cerr << "raven::init(): socket failed: " << errno << ", " << strerror(errno) << std::endl;
			return false;
		}

		return true;
	}

	bool init(const bool proc)
	{
		const char* sentry_dsn = getenv("SENTRY_DSN");
		if (sentry_dsn == NULL) return false;

		return init(sentry_dsn, proc);
	}

	void add_global(const std::string& key, const std::string& value)
	{
		global_message[key] = value;
	}

	void add_global(const std::string& key, const long long& value)
	{
		global_message[key] = boost::lexical_cast<std::string>(value);
	}

	void add_global(const std::string& key, const unsigned long long& value)
	{
		global_message[key] = boost::lexical_cast<std::string>(value);
	}

	void add_global(const std::string& key, const long double& value)
	{
		global_message[key] = boost::lexical_cast<std::string>(value);
	}

	static inline time_t capture_attach_main(Message& message)
	{
		struct timeval tv_now;
		gettimeofday(&tv_now, NULL);
		time_t t_now = tv_now.tv_sec;

		struct tm tm_now;
		gmtime_r(&t_now, &tm_now);

		char tstamp_now[32];
		strftime(tstamp_now, sizeof(tstamp_now), "%FT%T", &tm_now);

		char event_id[33];
		sprintf(event_id, "%.8x%.8x%.8x%.8x",
		  (unsigned int) tv_now.tv_sec, (unsigned int) tv_now.tv_usec,
		  rand_r(&global_seed), rand_r(&global_seed));

		message.put("event_id", event_id);
		message.put("timestamp", tstamp_now);
		message.put("extra.sys.uptime", boost::lexical_cast<std::string>(t_now - global_started));

		return t_now;
	}

	static inline void capture_attach_proc(Message& message)
	{
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

		std::ifstream load_file("/proc/loadavg");
		std::getline(load_file, line);
		message.put("extra.sys.load", line);
	}

	static inline void capture_prepare_packet(const Message& message, time_t t_now, std::string& packet)
	{
		std::string encoded;
		encode(message, encoded);

		std::stringstream packet_stream;
		packet_stream << "Sentry sentry_timestamp=" << t_now << ".0, sentry_client=raven-cpp/0.0.1, sentry_version=2.0, sentry_key=" << global_key << "\n\n" << encoded;
		packet = packet_stream.str();
	}

	static inline void capture_send_packet(const std::string& packet)
	{
		ssize_t n = sendto(global_socket, packet.data(), packet.size(), MSG_DONTWAIT,
		  (struct sockaddr*) &global_addr, sizeof(global_addr));

		if (n < 0) {
			std::cerr << "raven::capture_send_packet(): not sent: " << errno << ", " << strerror(errno) << std::endl;
		}
	}

	void capture(Message& message)
	{
		// do nothing if init() not called
		if (global_started == 0) return;

		try {
			time_t t_now = capture_attach_main(message);

			if (global_attach_proc) {
				capture_attach_proc(message);
			}

			for (std::map<std::string, std::string>::const_iterator
				it = global_message.begin(); it != global_message.end(); ++it) {
				message.put(it->first, it->second);
			}

			std::string packet;
			capture_prepare_packet(message, t_now, packet);
			capture_send_packet(packet);
		} catch (const std::exception& e) {
			std::cerr << "raven::capture(): exception catched: " << e.what() << std::endl;
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
		int padding = output.size() % 4;
		if (padding > 0) output.append(4 - padding, '=');
	}

	bool decode(const std::string& encoded, Message& message)
	{
		try {
			// need to replace '=' to 'A' - boost sense of humor
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


