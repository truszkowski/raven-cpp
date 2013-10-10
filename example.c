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
	
	// custom dsn example
	c_dsn_t dsn;
	ret = craven_init_dsn(&dsn, "udp://77777777777777777777777777777777:88888888888888888888888888888888@127.0.0.1:12340/1", 1 );
	
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
