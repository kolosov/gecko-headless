find_library(gecko_libs xpcomglue PATHS ${GECKO_SDK_PATH}/lib)

find_package(GTK2 2.6 REQUIRED gtk)

include_directories ("${PROJECT_BINARY_DIR}/Gecko")
include_directories ("${PROJECT_SOURCE_DIR}/Gecko")
include_directories ("${PROJECT_SOURCE_DIR}/Gecko/common")

include_directories(${GTK2_INCLUDE_DIRS})
include_directories ("${PROJECT_SOURCE_DIR}/common")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${GECKO_SDK_PATH}/include ${GECKO_SDK_PATH}/include/nspr)


#add_definitions(-std=c++11 )
add_definitions(-fshort-wchar -fno-rtti)

set(srvlistener_src srvlistener.cpp)

add_library(srvlistener STATIC ${srvlistener_src})

