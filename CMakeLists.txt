cmake_minimum_required (VERSION 2.6)
project(gecko-headless)
cmake_policy(SET CMP0011 OLD)

add_definitions(-std=c++11)
add_definitions(-DMOZ_NO_MOZALLOC)

add_subdirectory (Gecko)
add_subdirectory (srvlistener)
add_subdirectory (srv)

