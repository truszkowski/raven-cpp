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


#include "raven/raven.h"
#include "raven/craven.h"

int craven_init(const char* url, int proc)
{
	return raven::init(url, proc) ? 0 : -1;
}

int craven_init_dsn(c_dsn_t* cdsn, const char* url, int proc)
{
	cdsn->url = url;
	cdsn->proc = proc;
	cdsn->pdsn_t = NULL;
	raven::dsn_t* dsn = new raven::dsn_t;
	dsn->global_attach_proc = (bool)proc;
	cdsn->pdsn_t = dsn;
	return raven::init_dsn(dsn, url, proc);
}

void craven_destroy_dsn(c_dsn_t* cdsn)
{
	if(cdsn->pdsn_t) {
		delete (raven::dsn_t*)cdsn->pdsn_t;
		cdsn->pdsn_t = NULL;
	}
}

int craven_init_env(int proc)
{
	return raven::init(proc);
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

void craven_add_global_dsn(c_dsn_t* dsn, const char* key, const char* value)
{
	raven::add_global(key, value, (raven::dsn_t*)dsn->pdsn_t);
}

void craven_add_globalf_dsn(c_dsn_t* dsn, const char* key, const char* fmt, ...)
{
	char value[4096]; // XXX: enough!:)

	va_list args;
	va_start(args, fmt);
	vsnprintf(value, sizeof(value), fmt, args);
	va_end(args);

	raven::add_global(key, value, (raven::dsn_t*)dsn->pdsn_t);
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

void craven_capture_directly_dsn(c_dsn_t* dsn, const char* level, const char* message, ...)
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

	raven::capture(msg, (raven::dsn_t*)dsn->pdsn_t);
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
	raven::Message* msg = (raven::Message*)message;
	raven::capture(*msg);
}

void craven_message_send_dsn(c_dsn_t* dsn, void* message)
{
	raven::Message* msg = (raven::Message*)message;
	raven::capture(*msg, (raven::dsn_t*)dsn->pdsn_t);
}
