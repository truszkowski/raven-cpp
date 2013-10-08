#include "raven/raven.h"

#include <iostream>

int main(void)
{
	bool ready = raven::init("udp://12345678123456781234567812345678:01230123012301230123012301230123@127.0.0.1:12340/1", true);
	
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
	  "udp://12345678123456781234567812345678:01230123012301230123012301230123@127.0.0.1:12340/1", // url
	  true // include /proc/ info
	);
	
	if(!ready) {
		std::cout << "raven_init_failed" << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
