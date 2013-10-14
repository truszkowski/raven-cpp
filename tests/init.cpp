#include "../raven/raven.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
using namespace std;
using namespace raven;

int main(void)
{
	try {
		unsetenv("SENTRY_DSN");
		set_default();
		assert (!"env var test");
	} catch (std::invalid_argument& e) {
		cout << "Ok, invalid argument, " << e.what() << endl;
	}

	try {
		setenv("SENTRY_DSN", "http://77777777777777777777777777777777:88888888888888888888888888888888@127.0.0.1:12340/1", 1);
		set_default();
		assert (!"no http test");
	} catch (std::invalid_argument& e) {
		cout << "Ok, only udp, " << e.what() << endl;
	}

	try { 
		setenv("SENTRY_DSN", "udp://88888888888888888888888888888888@127.0.0.1:12340/1", 1);
		set_default();
		assert (!"no key/secret test");
	} catch (std::invalid_argument& e) {
		cout << "Ok, no key/secret, " << e.what() << endl;
	}

	try {
		setenv("SENTRY_DSN", "udp://77777777777777777777777777777777:88888888888888888888888888888888@127.0.0.1/1", 1);
		set_default();
		assert (!"no port test");
	} catch (std::invalid_argument& e) {
		cout << "Ok, no port, " << e.what() << endl;
	}

	try {
		setenv("SENTRY_DSN", "udp://77777777777777777777777777777777:88888888888888888888888888888888@:12340/1", 1);
		set_default();
		assert (!"no host test");
	} catch (std::invalid_argument& e) {
		cout << "Ok, no host, " << e.what() << endl;
	}

	try {
		setenv("SENTRY_DSN", "udp://77777777777777777777777777777777:88888888888888888888888888888888@127.0.0.1:12340", 1);
		set_default();
		assert (!"no project test");
	} catch (std::invalid_argument& e) {
		cout << "Ok, no project, " << e.what() << endl;
	}

	try {
		setenv("SENTRY_DSN", "udp://77777777777777777777777777777777:88888888888888888888888888888888@127.0.0.1:12340/1foo", 1);
		set_default();
		assert (!"bad project test");
	} catch (std::invalid_argument& e) {
		cout << "Ok, bad project, " << e.what() << endl;
	}

	try {
		setenv("SENTRY_DSN", "udp://77777777777777777777777777777777:88888888888888888888888888888888@127.0.0.1:12340/1", 1);
		set_default();
		cout << "Ok" << endl;
	} catch (std::invalid_argument& e) {
		cout << "unexpected: " << e.what() << endl;
		assert (!"should be ok...");
	}

	return 0;
}
