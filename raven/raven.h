#ifndef __RAVEN_CPP_H__
#define __RAVEN_CPP_H__

/*
                    +IMMMM .~MMZ.
                 .MM NMMMMM  .MMMM
                MMM. MMMMMMZ   MMMM.
              .MMM, .MMMMMMM  ..MMMM
              .MMM. ZMMMMMMM.   MMMM.
              .MMM  =MMMMMMM.   MMMM.
              .MMM . MMMMMMM.  MMMM
                MMM: MMMMMMM .ZMMM
                  MMM MMMMMM.~MO
                      ~MMMN..   ...M.
                        .?M8 .. +.NI
                       . .....  MNM D
                        : D..Z...MO.?.
                          NM . M..  .~
                         .~I...     .,
                          .M.       M.
                         .M.        :
                        .M        .MM
                       .7           M.
                       M            MO
                      M.            .8       .=MMMMMMM .
                     M.             .I    MM$          ,M
                    .                M MM .             .M
                    M                 N              .   M
                   .:                                M   ,.
                                                     +   .
                    ,                                Z  .M
                   .M                               ..  ,
                    M                  .            M.  M..  =+, .
                     M        ?        D           :+   7  ..M$ ..
                     .Z        M.       ,         DMM.     .. =M.
                      ,M      .8,       M        M.. .MMM,...
               .. N  M$ MI.    MM.      :.   ..M$
             .$...  =  MM . D,,MM7       MMMM,
                   Z...    MN   MM.      MMOMMM7..
                      .D.,8      M.      :..:NM$
                                 MM.      .MM~.,
                                  MM.  ~=7DMMM$.

          S   E   A    L       O   F      T   H   E       D   A   Y
*/


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

	
	typedef struct dsn_s {
		std::map<std::string,std::string> global_message;
		std::string global_key;
		time_t global_started;
		bool global_attach_proc;
		unsigned int global_seed;
		int global_socket;
		struct sockaddr_in global_addr;
		
		int port;
		char key[100];
		char secret[100];
		char host[100];
		char project[100];
		char none;
	} dsn_t;
	
	/**
	 * @brief Init custom dsn, by initializing dsn_t type structure
	 * 
	 * Url must be like that:
	 *        "udp://<key>:<secret>@<host>:<port>/<project>"
	 *
	 *        If proc == true:
	 *
	 *          File "/proc/self/status" 
	 *          Add values for: VmPeak, VmSize, VmLck, VmHWM, VmRSS, 
	 *          VmData, VmStk, VmExe, VmLib, VmPTE, FDSize and Threads.
	 *
	 *          File "/proc/loadavg" - gets whole line. 
	 * 
     * @param dsn - pointer to dsn_t struct
     * @param url - url
     * @param proc - attach information from /proc/
	 * 
     * @return false if url is invalid or cannot initialize (open socket etc.) 
     */
	bool init_dsn(dsn_t* dsn, const std::string& url, const bool proc);

	/** 
	 * @brief Init module
	 *
	 *        Url must be like that:
	 *        "udp://<key>:<secret>@<host>:<port>/<project>"
	 *
	 *        If proc == true:
	 *
	 *          File "/proc/self/status" 
	 *          Add values for: VmPeak, VmSize, VmLck, VmHWM, VmRSS, 
	 *          VmData, VmStk, VmExe, VmLib, VmPTE, FDSize and Threads.
	 *
	 *          File "/proc/loadavg" - gets whole line.
	 * 
	 * @param url   url
	 * @param proc  attach information from /proc/
	 * 
	 * @return returns false if url is invalid or cannot open socket 
	 */
	bool init(const std::string& url, const bool proc = false);

	/** 
	 * @brief Init module
	 *
	 *        Works link init(url, proc) but reads url from
	 *        environment variable: "SENTRY_DSN".
	 * 
	 * @param proc  attach information from /proc
	 * 
	 * @return returns false if url is invalid or cannot open socket
	 */
	bool init(const bool proc = false);

	/** 
	 * @brief Add key-value to attach to sentry message 
	 * 
	 * @param key    key
	 * @param value  value
	 */
	void add_global(const std::string& key, const std::string& value, dsn_t* dsn=NULL);
	void add_global(const std::string& key, const long long& value, dsn_t* dsn=NULL);
	void add_global(const std::string& key, const unsigned long long& value, dsn_t* dsn=NULL);
	void add_global(const std::string& key, const long double& value, dsn_t* dsn=NULL);

	/** 
	 * @brief Send message
	 * 
	 * @param message  message to send
	 * @param dsn custom dsn, leave or send null to use default
	 */
	void capture(Message message, dsn_t* dsn=NULL);

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

#define raven_debug(message) do {                          \
	(message).put("level", "debug");                       \
	raven_capture((message));                              \
} while (0)

#define raven_info(message) do {                           \
	(message).put("level", "info");                        \
	raven_capture((message));                              \
} while (0)

#define raven_warning(message) do {                        \
	(message).put("level", "warning");                     \
	raven_capture((message));                              \
} while (0)

#define raven_error(message) do {                          \
	(message).put("level", "error");                       \
	raven_capture((message));                              \
} while (0)

#define raven_fatal(message) do {                          \
	(message).put("level", "fatal");                       \
	raven_capture((message));                              \
} while (0)

#define raven_capture(message) do {                        \
  char source_file[64];                                    \
  snprintf(source_file, sizeof(source_file), "%s:%d",      \
      __FILE__, __LINE__);                                 \
	(message).put("culprit", __PRETTY_FUNCTION__);         \
	(message).put("extra.source.file", source_file);       \
	raven::capture((message));                             \
} while (0)


/**
 * @brief Macros raven_{debug,info,warning,error,fatal}_dsn.
 * with custom dsn parameter
 */

#define raven_debug_dsn(message, dsn) do {                 \
	(message).put("level", "debug");                       \
	raven_capture_dsn((message), (dsn));                   \
} while (0)

#define raven_info_dsn(message, dsn) do {                  \
	(message).put("level", "info");                        \
	raven_capture_dsn((message), (dsn));                   \
} while (0)

#define raven_warning_dsn(message, dsn) do {               \
	(message).put("level", "warning");                     \
	raven_capture_dsn((message), (dsn));                   \
} while (0)

#define raven_error_dsn(message, dsn) do {                 \
	(message).put("level", "error");                       \
	raven_capture_dsn((message), (dsn));                   \
} while (0)

#define raven_fatal_dsn(message, dsn) do {                 \
	(message).put("level", "fatal");                       \
	raven_capture_dsn((message), (dsn));                   \
} while (0)

#define raven_capture_dsn(message, dsn) do {               \
  char source_file[64];                                    \
  snprintf(source_file, sizeof(source_file), "%s:%d",      \
      __FILE__, __LINE__);                                 \
	(message).put("culprit", __PRETTY_FUNCTION__);         \
	(message).put("extra.source.file", source_file);       \
	raven::capture((message), (dsn));                      \
} while (0)

#endif
