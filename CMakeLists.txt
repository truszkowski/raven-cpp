cmake_minimum_required (VERSION 2.8) 
project (ravenpp)

if (DEFINED ENV{VIRTUAL_ENV})
	message (STATUS "Detected active virtualenv: $ENV{VIRTUAL_ENV}")
	include (venv/pkg)
else ()
	message (STATUS "Not detected active virtualenv")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra -g -pg -pipe -O2")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")
	set (CMAKE_VERBOSE_MAKEFILE   off)
	set (CMAKE_COLOR_MAKEFILE     on)
	set (CMAKE_USE_RELATIVE_PATHS on)
endif ()

add_library (ravenpp raven.cpp craven.cpp)

add_executable (example example.cpp)
target_link_libraries (example ravenpp boost_iostreams z)

add_executable (raven-cmd raven-cmd.cpp)
target_link_libraries (raven-cmd ravenpp boost_iostreams z)

add_executable (raven_decode raven_decode.cpp)
target_link_libraries (raven_decode ravenpp boost_iostreams z)

#enable_testing ()
#add_test ()

install (FILES raven/raven.h raven/craven.h DESTINATION include/raven)
install (TARGETS ravenpp DESTINATION lib/)
install (TARGETS raven-cmd DESTINATION bin/)

if (DEFINED ENV{VIRTUAL_ENV})
	include (CPack)
endif ()
