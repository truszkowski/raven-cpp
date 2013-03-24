#include "raven/raven.h"

#include <iostream>

int main(void)
{
	bool ready = raven::init("udp://12345678123456781234567812345678:01230123012301230123012301230123@127.0.0.1:12340/1", true);

	if (!ready) {
		std::cout << "raven_init failed" << std::endl;
		return -1;
	}

	raven::Message msg;
	msg.put("message", "this is message");
	msg.put("extra.anything", "bla ba da ba da");
	raven_info(msg);

	raven::to_json_stream(msg, std::cout);

	return 0;
}
