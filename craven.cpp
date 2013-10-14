#include "raven/raven.h"
#include "raven/craven.h"


int craven_init(const char* url, int flags)
{
	try {
		raven::set_default(url, flags);
		return 0;
	} catch (std::exception) {
		return -1;
	}
}


dsn_t craven_init_dsn(const char* url, int flags)
{
	try {
		raven::Dsn* dsn = new raven::Dsn(url, flags);
		return (void*)dsn;
	} catch (std::exception) {
		return NULL;
	}
}

void craven_destroy_dsn(dsn_t dsn)
{
	delete (raven::Dsn*)dsn;
}

int craven_init_env(int flags)
{
	try {
		raven::set_default(flags);
		return 0;
	} catch (std::exception) {
		return -1;
	}
}

void craven_add_global(const char* key, const char* value)
{
	raven::add_global(key, value);
}

void craven_add_globalf(const char* key, const char* fmt, ...)
{
	char value[4096]; // XXX: enough!:)

	va_list args;
	va_start(args, fmt);
	vsnprintf(value, sizeof(value), fmt, args);
	va_end(args);

	raven::add_global(key, value);
}

void craven_add_global_dsn(dsn_t dsn, const char* key, const char* value)
{
	((raven::Dsn*)dsn)->add_global(key, value);
}

void craven_add_globalf_dsn(dsn_t dsn, const char* key, const char* fmt, ...)
{
	char value[4096]; // XXX: enough!:)

	va_list args;
	va_start(args, fmt);
	vsnprintf(value, sizeof(value), fmt, args);
	va_end(args);

	((raven::Dsn*)dsn)->add_global(key, value);
}


void craven_capture_directly(const char* level, const char* message, ...)
{
	raven::Message msg;
	msg.put("level", level);
	msg.put("message", message);

	va_list args;
	va_start(args, message);
	while (1) {
		const char* key = va_arg(args, const char*);
		if (key == NULL) break;
		const char* val = va_arg(args, const char*);
		if (val == NULL) break;

		msg.put(key, val);
	}
	va_end(args);

	raven::capture(msg);
}

void craven_capture_directly_dsn(dsn_t dsn, const char* level, const char* message, ...)
{
		raven::Message msg;
	msg.put("level", level);
	msg.put("message", message);

	va_list args;
	va_start(args, message);
	while (1) {
		const char* key = va_arg(args, const char*);
		if (key == NULL) break;
		const char* val = va_arg(args, const char*);
		if (val == NULL) break;

		msg.put(key, val);
	}
	va_end(args);

	((raven::Dsn*)dsn)->capture(msg);
}

void* craven_message_new(void)
{
	// XXX: ignore std::bad_alloc
	return new raven::Message;
}

void craven_message_free(void* message)
{
	raven::Message* msg = (raven::Message*)message;
	delete msg;
}

void craven_message_put(void* message, const char* key, const char* value)
{
	raven::Message* msg = (raven::Message*)message;
	msg->put(key, value);
}

void craven_message_putf(void* message, const char* key, const char* fmt, ...)
{
	raven::Message* msg = (raven::Message*)message;
	char value[4096]; // XXX: enough!:)

	va_list args;
	va_start(args, fmt);
	vsnprintf(value, sizeof(value), fmt, args);
	va_end(args);

	msg->put(key, value);
}

void craven_message_send(void* message)
{
	raven::capture(*((raven::Message*)message));
}

void craven_message_send_dsn(dsn_t dsn, void* message)
{
	((raven::Dsn*)dsn)->capture(*((raven::Message*)message));
}
