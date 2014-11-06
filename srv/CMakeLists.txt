find_library(gecko_libs xpcomglue PATHS ${GECKO_SDK_PATH}/lib)
find_library(gecko_libs_mozglue mozglue PATHS ${GECKO_SDK_PATH}/lib)
find_library(gecko_libs_memory memory PATHS ${GECKO_SDK_PATH}/lib)

find_package(GTK2 2.6 REQUIRED gtk)

include_directories ("${PROJECT_BINARY_DIR}/Gecko")
include_directories ("${PROJECT_SOURCE_DIR}/Gecko")
include_directories ("${PROJECT_SOURCE_DIR}/Gecko/common")
include_directories ("${PROJECT_SOURCE_DIR}/srvlistener")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${GECKO_SDK_PATH}/include ${GECKO_SDK_PATH}/include/nspr)
include_directories(${GTK2_INCLUDE_DIRS})

set(geckosrv_src geckosrv.cpp)

add_definitions(-fno-rtti )
add_executable(geckosrv ${geckosrv_src})

target_link_libraries(geckosrv srvlistener geckoembed ${gecko_libs} ${GTK2_LIBRARIES} dl -Wl,--whole-archive ${gecko_libs_mozglue} -Wl,--no-whole-archive ${gecko_libs_memory} pthread )

