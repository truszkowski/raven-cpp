#include "raven/craven.h"

#include <stdio.h>

int main(void)
{
	int ret = craven_init("udp://12345678123456781234567812345678:01230123012301230123012301230123@127.0.0.1:12340/1", 3);

	if (ret) {
		printf("craven_init failed\n");
		return -1;
	}

	craven_info("this is message",
			"extra.anything", "bla ba da ba da",
			NULL);

	void* msg = craven_message_new();
	craven_message_put(msg, "message", "this is second message");
	craven_message_putf(msg, "extra.param", "%d, %d, %d, %d", 123, 120, 112, 102);
	craven_message_send(msg);
	
	// custom dsn example
	dsn_t dsn = craven_init_dsn("udp://77777777777777777777777777777777:88888888888888888888888888888888@127.0.0.1:12340/1", 0);
	
	craven_message_send_dsn(&dsn, msg);

	craven_info_dsn(&dsn, "this is message",
		"extra.anything", "bla ba da ba da",
		NULL);
	
	// dsn object holds pointer to c++ dsn_t object, it should remove before
	// destroy
	craven_destroy_dsn(&dsn);
	
	craven_message_free(msg);
	return 0;
}
