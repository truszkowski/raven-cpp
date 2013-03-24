cmake_minimum_required (VERSION 2.8) 
project (ravenpp)
#include (venv/pkg)

set (CMAKE_CXX_FLAGS "-Wall -Wextra -g -pg -pipe -O2")
set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")
set (CMAKE_VERBOSE_MAKEFILE off)
include_directories (BEFORE ${CMAKE_SOURCE_DIR}) # $ENV{VIRTUAL_ENV}/include)
#link_directories ($ENV{VIRTUAL_ENV}/lib)

add_library (ravenpp raven.cpp craven.cpp)

add_executable (example example.cpp)
target_link_libraries (example ravenpp boost_iostreams z)

add_executable (raven_decode raven_decode.cpp)
target_link_libraries (raven_decode ravenpp boost_iostreams z)

#enable_testing ()
#add_test ()

install (FILES raven/raven.h raven/craven.h DESTINATION include/raven)
install (TARGETS ravenpp DESTINATION lib/)
#include (CPack)
