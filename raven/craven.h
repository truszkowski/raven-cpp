#ifndef __RAVEN_C_H__
#define __RAVEN_C_H__

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


/**
 * @brief This is C wrappers on C++ functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief c_dsn_s struct, containing info of c++ struct
	 * Should be initialized before use, and destroyed by craven_destroy_dsn
	 *  when not used anymore
	 */
	typedef struct c_dsn_s {
		const char* url;
		int proc; // default 0
		void* pdsn_t; // pointer to dsn in c++ code, default NULL
	} c_dsn_t;
	
	/** 
	 * @brief Init module
	 *
	 *        Url must be like that:
	 *        "udp://<key>:<secret>@<host>:<port>/<project>"
	 *
	 *        If proc != 0:
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
	 * @return returns -1 if url is invalid or cannot open socket
	 *         if ok returns 0
	 */
	int craven_init(const char* url, int proc);

	/** 
	 * @brief Init module
	 *
	 *        Url must be like that:
	 *        "udp://<key>:<secret>@<host>:<port>/<project>"
	 *
	 *        If proc != 0:
	 *
	 *          File "/proc/self/status" 
	 *          Add values for: VmPeak, VmSize, VmLck, VmHWM, VmRSS, 
	 *          VmData, VmStk, VmExe, VmLib, VmPTE, FDSize and Threads.
	 *
	 *          File "/proc/loadavg" - gets whole line.
	 *
	 * @param cdsn  reference to instance of c_dsn_t object
	 * @param url   url
	 * @param proc  attach information from /proc/
	 * 
	 * @return returns -1 if url is invalid or cannot open socket
	 *         if ok returns 0
	 */
	int craven_init_dsn(c_dsn_t* cdsn, const char* url, int proc);
	
	/**
	 * @brief c_dsn_t destroyer
	 * @param cdsn   pointer to c_dsn_t object
	 */
	void craven_destroy_dsn(c_dsn_t* cdsn);
	
	/** 
	 * @brief Init module
	 * 
	 *        Works link init(url, proc) but reads url from
	 *        environment variable: "SENTRY_DSN".
	 *
	 * @param proc  attach information from /proc/
	 * 
	 * @return return -1 if url is invalid or cannot open socket
	 *         if ok returns 0
	 */
	int craven_init_env(int proc);

	/** 
	 * @brief Add key-value to attach to sentry message 
	 * 
	 * @param key    key
	 * @param value  value
	 */
	void craven_add_global(const char* key, const char* value);
	void craven_add_globalf(const char* key, const char* fmt, ...)
		__attribute__((format(printf, 2, 3)));

	/** 
	 * @brief Add key-value to attach to sentry message to custom dsn
	 * 
	 * @param key    key
	 * @param value  value
	 */
	void craven_add_global_dsn(c_dsn_t* dsn, const char* key, const char* value);
	void craven_add_globalf_dsn(c_dsn_t* dsn, const char* key, const char* fmt, ...)
		__attribute__((format(printf, 3, 4)));
	
	/** 
	 * @brief Send message
	 *
	 * @note Much more better to use craven_{debug,info,warning,error,fatal}
	 *       macros.
	 * 
	 * @param level    message level
	 * @param message  your message
	 * @param ...      optional arguments, must ends with NULL, arguments are
	 *                 pairs - keys and values.
	 *
	 * @note For example:
	 *       
	 *       craven_error("something goes badly", 
	 *           "key1", "value1", "key2", "value2", NULL);
	 */
	void craven_capture_directly(const char* level, const char* message, ...);

	
	void craven_capture_directly_dsn(c_dsn_t* dsn, const char* level, const char* message, ...);

	/** 
	 * @brief Create message structure
	 * 
	 * @note Fields "message" and "level" MUST be set by
	 *       method craven_message_put(...).
	 *
	 *       For example:
	 *
	 *       void* msg = craven_message_new();
	 *       craven_message_put(msg, "level", "info");
	 *       craven_message_put(msg, "message", "something");
	 *       craven_message_put(msg, "extra.param", "super");
	 *       craven_message_send(msg);
	 *       craven_message_free(msg);
	 *
	 * @return pointer to message - always not NULL
	 */
	void* craven_message_new(void);

	/** 
	 * @brief Destroy message structure
	 * 
	 * @param message  pointer to message
	 */
	void craven_message_free(void* message);

	/** 
	 * @brief Add key and value
	 * 
	 * @param message  pointer to message
	 * @param key      key
	 * @param value    value
	 */
	void craven_message_put(void* message, const char* key, const char* value);

	/** 
	 * @brief Add key and formatted value
	 * 
	 * @param message  pointer to message
	 * @param key      key
	 * @param fmt      formatted value (printf style)
	 * @param ...      optional arguments
	 */
	void craven_message_putf(void* message, const char* key, const char* fmt, ...)
		__attribute__((format(printf, 3, 4)));

	/** 
	 * @brief Send message to Sentry
	 * 
	 * @param message  pointer to message
	 */
	void craven_message_send(void* message);

	/** 
	 * @brief Send message to Sentry with custom dsn
	 * 
	 * @param dsn    dsn
	 * @param message  pointer to message
	 */
	void craven_message_send_dsn(c_dsn_t* dsn, void* message);
	
	//ssize_t craven_encode(const char* data, char** encoded);
	//ssize_t craven_decode(const char* data, char** decoded);

#ifdef __cplusplus
};
#endif

/**
 * @brief Macros craven_{debug,info,warning,error,fatal}.
 */

#define craven_debug(message, args...) \
	craven_capture("debug", message, ##args)

#define craven_info(message, args...) \
	craven_capture("info", message, ##args)

#define craven_warning(message, args...) \
	craven_capture("warning", message, ##args)

#define craven_error(message, args...) \
	craven_capture("error", message, ##args)

#define craven_fatal(message, args...) \
	craven_capture("fatal", message, ##args)

#define craven_capture(level, message, args...) do {       \
  char source_file[64];                                    \
  snprintf(source_file, sizeof(source_file), "%s:%d",      \
      __FILE__, __LINE__);                                 \
	craven_capture_directly(level, message,                  \
			"culprit", __func__,                                 \
			"extra.source.file", source_file, ##args);           \
} while (0)

/*
 * custom dsn support
 */
#define craven_debug_dsn(dsn, message, args...) \
	craven_capture_dsn(dsn, "debug", message, ##args)

#define craven_info_dsn(dsn, message, args...) \
	craven_capture_dsn(dsn, "info", message, ##args)

#define craven_warning_dsn(dsn, message, args...) \
	craven_capture_dsn(dsn, "warning", message, ##args)

#define craven_error_dsn(dsn, message, args...) \
	craven_capture_dsn(dsn, "error", message, ##args)

#define craven_fatal_dsn(dsn, message, args...) \
	craven_capture_dsn(dsn, "fatal", message, ##args)

#define craven_capture_dsn(dsn, level, message, args...) do {       \
  char source_file[64];                                    \
  snprintf(source_file, sizeof(source_file), "%s:%d",      \
      __FILE__, __LINE__);                                 \
	craven_capture_directly_dsn(dsn, level, message,                  \
			"culprit", __func__,                                 \
			"extra.source.file", source_file, ##args);           \
} while (0)


#endif
