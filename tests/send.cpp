#include "../raven/raven.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	assert (argc == 2); // one arg = port number

	// first - bind
	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(atoi(argv[1]));

	if (bind(fd, (struct sockaddr*)&sin, sizeof(sin))) {
		cout << "Cannot bind: " << errno << ", " << strerror(errno) << endl;
		close(fd);
		return -1;
	}

	listen(fd, 100);
	cout << "binded" << endl;
	
	// second - send
	
	char url[100]; 
	snprintf(url, sizeof(url), 
			"udp://12345678123456781234567812345678:01230123012301230123012301230123@127.0.0.1:%d/1", atoi(argv[1]));
	raven::set_default(url, raven::ATTACH_PROC_LOAD);
	raven::add_global("extra.glob", "yes");

	raven::Message m1;
	m1.put("message", "anything");
	m1.put("extra.todo", "nothing");
	raven_error(m1);
	cout << "sent" << endl;
	
	// third - read
	
	char buffer[65536];

	int n = recv(fd, buffer, sizeof(buffer), 0);
		
	if (n < 0) {
		cout << "recv error: " << errno << ", " << strerror(errno) << endl;
		return -1;
	} 
		
	char* eoleol = (char*)memmem(buffer, n, "\n\n", 2);
	if (!eoleol) {
		cout << "no newlines" << endl;
		return -1;
	}

	raven::Message m2;
	std::string encoded(eoleol + 2, buffer + n);
	(*eoleol) = '\0';

	if (!raven::decode(encoded, m2)) {
		cout << "cannot decode message" << endl;
		return -1;
	}

	raven::to_json_stream(m2, cout);

	assert (m2.get<string>("message") == "anything");
	assert (m2.get<string>("level") == "error");
	assert (m2.get<string>("extra.todo") == "nothing");
	assert (m2.get<string>("extra.glob") == "yes");
	assert (m2.get<string>("extra.sys.load") != "");
	assert (m2.get<string>("extra.sys.uptime") != "");
	assert (m2.get<string>("timestamp") != "");
	assert (m2.get<string>("event_id") != "");
	assert (m2.get<string>("culprit") != "int main(int argc, char** argv)");
	assert (m2.get<string>("extra.source.file").find("send.cpp:44") != string::npos);
	
	return 0;
}

