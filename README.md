# About

This is C/C++ client to sentry. 

I wrote all of that yesterday (2013-03-23), so it's not tested very much.

# Compilation:

Requires boost and zlib, works only on linux

	$ git clone git@github.com:truszkowski/raven-cpp.git
	$ cd raven-cpp
	$ mkdir build
	$ cd build
	$ cmake ..
	$ make

It's compatibile with [VENV](https://github.com/truszkowski/venv), then:

	$ venv create raven
	$ mkdir build
	$ cd build
	$ cmake /path/to/raven-cpp
	$ make all package

# Use it

In C++

```cpp
	#include <raven/raven.h>

	void test1(void)
	{
		raven::Message msg;
		msg.put("message", "hello world!");
		raven_info(msg);
	}

	void test2(void)
	{
		raven::Message msg;
		msg.put("message", "hello world!!");
		msg.put("extra.param1", "abc");
		msg.put("extra.param2.a", "1000");
		msg.put("extra.param2.b", "1010");
		msg.put("extra.param2.c", "1200");
		raven_warning(msg);
	}

	int main(int argc, char** argv)
	{
		raven::init(argv[1], true);
		test1();
		test2();
		return 0;
	}
```

In C (use C++ wrappers)

```c
	#include <raven/craven.h>

	void test1(void)
	{
		craven_info("hello world!", NULL);
	}

	void test2(void)
	{
		craven_warning("hellow world!!", 
				"extra.param1", "abc",
				"extra.param2.a", "1000",
				"extra.param2.b", "1010",
				"extra.param2.c", "1200",
				NULL);
	}

	int main(int argc, char** argv)
	{
		craven_init(argv[1], 1);
		test1();
		test2();
		return 0;
	}
```

Compile with ravenpp

```
	g++ -o test-cpp test.cpp -lboost_iostreams -lz
	gcc -o test-c test.c -lboost_iostreams -lz -lstdc++
```

# Compatibility

Only UDP for now.

# License WTFPL

If you copy, this will also your code now, including benefits, problems and bugs. No guarantee.
Do what you wanna do, if you use it to create weapon of mass destruction - it's only depends on you:)
