#include "raven/raven.h"

#include <iostream>


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
