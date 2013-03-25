#include "raven/craven.h"

#include <stdio.h>

int main(void)
{
	int ret = craven_init("udp://12345678123456781234567812345678:01230123012301230123012301230123@127.0.0.1:12340/1", 1);

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
	craven_message_free(msg);
	return 0;
}
