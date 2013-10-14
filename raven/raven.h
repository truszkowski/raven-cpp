#ifndef __RAVEN_CPP_H__
#define __RAVEN_CPP_H__

#include <cstdio>
#include <cstdlib>
#include <string>
#include <ostream>
#include <map>
#include <netinet/in.h>
#include <boost/property_tree/ptree.hpp>


namespace raven {

	/**
	 * @brief How to use:
	 *
	 *        raven::Message msg;
	 *        msg.put("message", "something");
	 *        msg.put("level", "error");
	 *        msg.put("extra.color", "red");
	 *        msg.put("extra.dim.x", "100");
	 *        msg.put("extra.dim.y", "200");
	 *        raven::capture(msg);
	 *
	 *        but recommended is macro:
	 *
	 *        raven::Message msg;
	 *        msg.put("message", "something");
	 *        msg.put("extra.color", "red");
	 *        msg.put("extra.dim.x", "100");
	 *        msg.put("extra.dim.y", "200");
	 *        raven_error(msg);
	 *
	 *        Sentry documentation:
	 *        http://sentry.readthedocs.org/en/latest/developer/client/index.html
	 */
	typedef boost::property_tree::ptree Message;

	//!< module is useless, do nothing, initial mode
	static const int DONT = -1;
	//!< Attach machine load from /proc/loadavg (gets whole line)
	static const int ATTACH_PROC_LOAD = 1;
	//!< Attach process status from /proc/self/status
	//!< Add values for: VmPeak, VmSize, VmLck, VmHWM, VmRSS, 
	//!< VmData, VmStk, VmExe, VmLib, VmPTE, FDSize and Threads.
	static const int ATTACH_PROC_STATUS = 2;
	//!< attach machine load and process status
	static const int ATTACH_PROC = 3;

	class Dsn {
		public:
			/**
			 * @brief Init custom dsn, by initializing dsn_t type structure
			 * 
			 * @note Url must be like that:
			 *       "udp://<key>:<secret>@<host>:<port>/<project>"
			 *
			 * @param url - url
			 * @param flags - attach information from /proc/ for example
			 */
			explicit Dsn(const std::string& url, const int flags = ATTACH_PROC) 
				: started(0), flags(flags), socket(-1)
			{
				memset(&addr, 0x00, sizeof(addr));
				init(url, flags);
			}

			/** 
			 * @brief Init module
			 *
			 *        Works like Dsn::Dsn(url, proc) but reads url from
			 *        environment variable: "SENTRY_DSN".
			 * 
			 * @param flags  attach information from /proc for example
			 */
			explicit Dsn(const int flags = ATTACH_PROC)
				: started(0), flags(flags), socket(-1)
			{
				memset(&addr, 0x00, sizeof(addr));
				init(flags);
			}

			virtual ~Dsn(void) 
			{
				/* nothing to clean */
			}
			
			/*  Just like costructors */
			void init(const std::string& url, const int flags = ATTACH_PROC);
			void init(const int flags = ATTACH_PROC);

			/** 
			 * @brief Add key-value to attach to sentry message 
			 * 
			 * @param key    key
			 * @param value  value
			 */
			template <class Value>
			void add_global(const std::string& key, const Value& value)
			{
				extras[key] = value;
			}
			
			/** 
			 * @brief Send message
			 *
			 * @note `message` will be modified!
			 *
			 * @param message  message to send
			 */
			virtual void capture(Message& message) const;

			/** 
			 * @brief Set default instance.
			 *
			 * @note No mutex!
			 * 
			 * @param url    url
			 * @param flags  flags
			 */
			static void set_default(const std::string& url, const int flags = ATTACH_PROC)
			{
				default_instance.init(url, flags);
			}

			/** 
			 * @brief Set default instance.
			 *
			 * @note url from environment variable SENTRY_DSN
			 * @note No mutex!
			 * 
			 * @param flags  flags
			 */
			static void set_default(const int flags = ATTACH_PROC)
			{
				default_instance.init(flags);
			}

			/** 
			 * @brief Get default instance
			 *
			 * @note First use `raven::Dsn::set_default(...)`, otherwise = SIGSEGV
			 * @note No mutex!
			 * 
			 * @return Reference to default instance
			 */
			static Dsn& get_default(void) 
			{
				return default_instance;
			}

		protected:
			/** 
			 * @brief Default/global instance of Dsn, 
			 */			
			static Dsn default_instance;

			//!< additional data to send
			std::map<std::string,std::string> extras;
			//!<  key
			std::string key;
			//!< timestamp of object creation/init
			time_t started;
			//!< using flags
			int flags;
			//!< UDP socket
			int socket;
			//!< sentry instance address
			struct sockaddr_in addr;
				
			/** 
			 * @brief Add to message `timestamp`, `event_id`, `extra.sys.uptime`.
			 * 
			 * @param message  message
			 * 
			 * @return current timestamp
			 */
			virtual time_t attach_main(Message& message) const;

			/** 
			 * @brief Add to message proc values
			 * 
			 * @param message  message
			 */
			virtual void attach_proc(Message& message) const;

			/** 
			 * @brief Make packet ready to send via UDP
			 * 
			 * @param message  message
			 * @param t_now    current timestamp
			 * @param packet   packet ready to send
			 */
			virtual void make_packet(const Message& message, time_t t_now, std::string& packet) const;

			/** 
			 * @brief Send packet via UDP
			 * 
			 * @param packet  packet
			 */
			virtual void send_packet(const std::string& packet) const;
	};
	
	/** 
	 * @brief Set default instance.
	 *
	 * @note No mutex!
	 * 
	 * @param url    url
	 * @param flags  flags
	 */
	static inline void set_default(const std::string& url, const int flags = ATTACH_PROC)
	{
		Dsn::set_default(url, flags);
	}

	/** 
	 * @brief Set default instance.
	 *
	 * @note url from environment variable SENTRY_DSN
	 * @note No mutex!
	 * 
	 * @param flags  flags
	 */
	static inline void set_default(const int flags = ATTACH_PROC)
	{
		Dsn::set_default(flags);
	}

	/** 
	 * @brief Send message
	 * 
	 * @param message  message to send
	 */
	static inline void capture(Message& message)
	{
		Dsn::get_default().capture(message);
	}

	/** 
	 * @brief Send message
	 * 
	 * @param message  message to send
	 * @param dsn      custom dsn instance
	 */
	static inline void capture(Message& message, const Dsn& dsn)
	{
		dsn.capture(message);
	}

	template <class Value>
	void add_global(const std::string& key, const Value& value)
	{
		Dsn::get_default().add_global(key, value);
	}
	
	/** 
	 * @brief Get json string
	 * 
	 * @param message  your message
	 * @param output   string output
	 */
	void to_json_string(const Message& message, std::string& output);

	/** 
	 * @brief Get json stream
	 * 
	 * @param message  your message
	 * @param output   stream output
	 */
	void to_json_stream(const Message& message, std::ostream& output);

	/** 
	 * @brief Encode message to buffer (zlib + base64)
	 * 
	 * @param message  your message
	 * @param encoded  result string
	 */
	void encode(const Message& message, std::string& encoded);

	/** 
	 * @brief Decode sent message (base64 + zlib) 
	 * 
	 * @param encoded  encoded message
	 * @param message  decoded message
	 * 
	 * @return false if cannot decode
	 */
	bool decode(const std::string& encoded, Message& message);
};

/**
 * @brief Macros raven_{debug,info,warning,error,fatal}.
 */

#define raven_debug(message, args...) do {                 \
  (message).put("level", "debug");                         \
  raven_capture((message), ##args);                        \
} while (0)

#define raven_info(message, args...) do {                  \
  (message).put("level", "info");                          \
  raven_capture((message), ##args);                        \
} while (0)

#define raven_warning(message, args...) do {               \
  (message).put("level", "warning");                       \
  raven_capture((message), ##args);                        \
} while (0)

#define raven_error(message, args...) do {                 \
  (message).put("level", "error");                         \
  raven_capture((message), ##args);                        \
} while (0)

#define raven_fatal(message, args...) do {                 \
  (message).put("level", "fatal");                         \
  raven_capture((message), ##args);                        \
} while (0)

#define raven_capture(message, args...) do {               \
  char source_file[64];                                    \
  snprintf(source_file, sizeof(source_file), "%s:%d",      \
      __FILE__, __LINE__);                                 \
  (message).put("culprit", __PRETTY_FUNCTION__);           \
  (message).put("extra.source.file", source_file);         \
  raven::capture((message), ##args);                       \
} while (0)

#endif
