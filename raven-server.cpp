#include "raven/raven.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " <ip-address> <port-number>" << endl;
		cout << endl;
		return -1;
	}

	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	sin.sin_port = htons(atoi(argv[2]));

	if (bind(fd, (struct sockaddr*)&sin, sizeof(sin))) {
		cout << "Cannot bind: " << errno << ", " << strerror(errno) << endl;
		close(fd);
		return -1;
	}

	char buffer[65536];

	while (true) {
		int n = recv(fd, buffer, sizeof(buffer), 0);
		
		if (n < 0) {
			cout << "=== error: " << errno << ", " << strerror(errno) << endl;
		} else {
			char* eoleol = (char*)memmem(buffer, n, "\n\n", 2);
			if (eoleol) {
				raven::Message msg;
				std::string encoded(eoleol + 2, buffer + n);
				(*eoleol) = '\0';

				if (raven::decode(encoded, msg)) {
					cout << "=== received: " << buffer << endl;
					raven::to_json_stream(msg, cout);
				} else {
					cout << "=== error: cannot decode message" << endl;
				}
			}
		}			
	}

	return 0;
}
