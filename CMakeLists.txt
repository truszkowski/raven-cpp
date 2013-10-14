cmake_minimum_required (VERSION 2.8) 
project (ravenpp)

if (DEFINED ENV{VIRTUAL_ENV})
	message (STATUS "Detected active virtualenv: $ENV{VIRTUAL_ENV}")
	include (venv/pkg)
else ()
	message (STATUS "Not detected active virtualenv")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra -ggdb3 -pg -pipe -O2")
	#set (CMAKE_CXX_FLAGS "-Wall -Wextra -ggdb3 -pg -pipe -O0")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")
	set (CMAKE_VERBOSE_MAKEFILE   off)
	set (CMAKE_COLOR_MAKEFILE     on)
	set (CMAKE_USE_RELATIVE_PATHS on)
endif ()
	
file (READ VERSION RAVEN_VERSION)
string (STRIP "${RAVEN_VERSION}" RAVEN_VERSION)
add_definitions (-DVERSION="${RAVEN_VERSION}")

add_library (ravenpp raven.cpp craven.cpp)

add_executable (example example.cpp)
target_link_libraries (example ravenpp boost_iostreams z)

add_executable (cexample example.c)
target_link_libraries (cexample ravenpp boost_iostreams z stdc++)

add_executable (raven-notify raven-notify.cpp)
target_link_libraries (raven-notify ravenpp boost_iostreams z)

add_executable (raven-server raven-server.cpp)
target_link_libraries (raven-server ravenpp boost_iostreams z)

add_executable (raven_decode raven_decode.cpp)
target_link_libraries (raven_decode ravenpp boost_iostreams z)

enable_testing ()

add_executable (tests_encode tests/encode.cpp)
target_link_libraries (tests_encode ravenpp boost_iostreams z)
add_test (encode tests_encode)

add_executable (tests_init tests/init.cpp)
target_link_libraries (tests_init ravenpp boost_iostreams z)
add_test (init tests_init)

add_executable (tests_send tests/send.cpp)
target_link_libraries (tests_send ravenpp boost_iostreams z)
add_test (send tests_send 16002) ## FIXME <<-- be careful!!! that port could be used!!!


install (FILES raven/raven.h raven/craven.h DESTINATION include/raven)
install (TARGETS ravenpp DESTINATION lib/)
install (TARGETS raven-notify raven-server DESTINATION bin/)

if (DEFINED ENV{VIRTUAL_ENV})
	include (CPack)
endif ()
