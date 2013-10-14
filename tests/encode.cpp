#include "../raven/raven.h"
#include <iostream>
#include <cassert>
using namespace std;
using namespace raven;

int main(void)
{
	Message m1, m2;
	string str;
	m1.put("aaaaaaa", "dsdsad");
	m1.put("sdsdsdsd", 12321312);
	m1.put("aa.bbb", "1.0");
	m1.put("aa.ccc", "00000");
	encode(m1, str);
	decode(str, m2);

	assert (m2.get<string>("aaaaaaa") == "dsdsad");
	assert (m2.get<int>("sdsdsdsd") == 12321312);
	assert (m2.get<string>("aa.bbb") == "1.0");
	assert (m2.get<string>("aa.ccc") == "00000");
	return 0;
}
