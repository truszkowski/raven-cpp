#include "raven/raven.h"
#include <iostream>


int main(void)
{
	try {
		raven::Dsn::set_default("udp://12345678123456781234567812345678:01230123012301230123012301230123@127.0.0.1:12340/1");
	} catch (std::exception& e) {
		std::cout << "raven_init failed " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	raven::Message msg;
	msg.put("message", "this is message");
	msg.put("extra.anything", "bla ba da ba da");
	raven_info(msg);
	
	raven::to_json_stream(msg, std::cout);

	// raven custom 
	try { 
		raven::Dsn dsn("udp://11111111111111111111111111111111:22222222222222222222222222222222@127.0.0.1:12340/1", 0);
		raven_debug(msg, dsn);
	} catch (std::exception& e) {
		std::cout << "raven_init_failed " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
