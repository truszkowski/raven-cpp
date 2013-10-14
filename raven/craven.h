#ifndef __RAVEN_C_H__
#define __RAVEN_C_H__

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
	 * @brief Pointer to C++ raven::Dsn 
	 */
	typedef void* dsn_t;

#define CRAVEN_ATTACH_PROC_LOAD = 1;
	//!< Attach process status from /proc/self/status
	//!< Add values for: VmPeak, VmSize, VmLck, VmHWM, VmRSS, 
	//!< VmData, VmStk, VmExe, VmLib, VmPTE, FDSize and Threads.
#define CRAVEN_ATTACH_PROC_STATUS = 2;
	//!< attach machine load and process status
#define CRAVEN_ATTACH_PROC = 3;

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
	 * @param url    url
	 * @param flags  attach information from /proc/ = CRAVEN_ATTACH_*
	 * 
	 * @return returns -1 if url is invalid or cannot open socket
	 *         if ok returns 0
	 */
	int craven_init(const char* url, int flags);

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
	 * @param proc  attach information from /proc/ = CRAVEN_ATTACH_*
	 * 
	 * @return returns NULL if url is invalid or cannot open socket
	 *         if ok returns pointer to dsn_t object
	 */
	dsn_t craven_init_dsn(const char* url, int flags);

	/**
	 * @brief dsn_t destroyer
	 * @param dsn   pointer to dsn_t object
	 */
	void craven_destroy_dsn(dsn_t dsn);

	/** 
	 * @brief Init module
	 * 
	 *        Works link init(url, proc) but reads url from
	 *        environment variable: "SENTRY_DSN".
	 *
	 * @param flags  attach information from /proc/ = CRAVEN_ATTACH_*
	 * 
	 * @return return -1 if url is invalid or cannot open socket
	 *         if ok returns 0
	 */
	int craven_init_env(int flags);

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
	void craven_add_global_dsn(dsn_t dsn, const char* key, const char* value);
	void craven_add_globalf_dsn(dsn_t dsn, const char* key, const char* fmt, ...)
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

	void craven_capture_directly_dsn(dsn_t dsn, const char* level, const char* message, ...);

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
	void craven_message_send_dsn(dsn_t dsn, void* message);

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

#define craven_capture_dsn(dsn, level, message, args...) do {\
  char source_file[64];                                      \
  snprintf(source_file, sizeof(source_file), "%s:%d",        \
      __FILE__, __LINE__);                                   \
  craven_capture_directly_dsn(dsn, level, message,           \
      "culprit", __func__,                                   \
      "extra.source.file", source_file, ##args);             \
} while (0)


#endif
