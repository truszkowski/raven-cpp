#include "raven/raven.h"

#include <iostream>

int main(void)
{
	bool ready = raven::init("udp://12345678123456781234567812345678:01230123012301230123012301230123@127.0.0.1:12340/1", false);
	
	if (!ready) {
		std::cout << "raven_init failed" << std::endl;
		return EXIT_FAILURE;
	}

	raven::Message msg;
	msg.put("message", "this is message");
	msg.put("extra.anything", "bla ba da ba da");
	raven_info(msg);
	
	raven::to_json_stream(msg, std::cout);

	// raven custom 
	raven::dsn_t dsn;
	ready = raven::init_dsn(
	  &dsn, // struct dsn_t
	  "udp://11111111111111111111111111111111:22222222222222222222222222222222@127.0.0.1:12340/1", // url
	  false // include /proc/ info
	);
	
	if(!ready) {
		std::cout << "raven_init_failed" << std::endl;
		return EXIT_FAILURE;
	}
	raven_debug_dsn(msg, &dsn);
	return EXIT_SUCCESS;
}
