#include "raven/raven.h"
#include "raven/craven.h"

int craven_init(const char* url, int proc)
{
	return raven::init(url, proc) ? 0 : -1;
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
