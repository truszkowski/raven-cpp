#include "raven/raven.h"

#include <iostream>
#include <boost/algorithm/string/trim.hpp>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <url>" << endl;
		cout << endl;
		cout << "On stdin enter message to send" << endl;
		cout << "Example input:" << endl;
		cout << endl;
		cout << "message: hello world!" << endl;
		cout << "level: debug" << endl;
		cout << "extra.param.x: 101" << endl;
		cout << "extra.param.y: 102" << endl;
		cout << "extra.what: nothing" << endl;
		cout << endl;
		return -1;
	}

	if (!raven::init(argv[1], false)) {
		cout << "cannot initialize raven module, incorrect url" << endl;
		return -1;
	}

	raven::Message msg;
	// by default "message: none" and "level: error"
	msg.put("message", "none");
	msg.put("level", "error"); 

	std::string line;
	while (std::getline(std::cin, line)) {
		size_t pos = line.find(':');

		if (pos != std::string::npos) {
			msg.put(line.substr(0, pos), boost::trim_copy(line.substr(pos+1)));
		}
	}
	
	// using without macros, not need source info here
	raven::capture(msg);
	return 0;
}
