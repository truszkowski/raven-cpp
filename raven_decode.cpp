#include <raven/raven.h>
#include <iostream>

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " <encoded-message>" << std::endl;
		return -1;
	}

	raven::Message msg;
	bool ok = raven::decode(argv[1], msg);

	if (ok) {
		raven::to_json_stream(msg, std::cout);
		return 0;
	} else {
		std::cout << "cannot decode" << std::endl;
		return -1;
	}
}
