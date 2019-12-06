cmake_minimum_required(VERSION 3.15)
project(reddit-save-archiver)
set(CMAKE_CXX_STANDARD 17)
include_directories(reddit-save-archiver)

option(DEBUG "Enable debugging" OFF)
option(RELEASE "release build" ON)

if(USE_HOME_DIR)
	add_compile_options(-DUSE_HOME_DIR)
endif()

if(USE_EXP_FS)
	add_compile_options(-DUSE_EXP_FS)
endif()

if(DEBUG)
	add_compile_options(-D_DEBUG=1)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
	set(CMAKE_CXX_FLAGS	"-g -Wall -Wextra -Wfatal-errors -Wno-error")
else(RELEASE)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release)
endif()
add_compile_options(-DVERSION="2019.12.5-Git")

add_executable(reddit-save-archiver base.cpp Reddit.cpp Saver.cpp Source.cpp)

find_package(Boost REQUIRED)
find_package(CURL REQUIRED)

include_directories(${BOOST_INCLUDE_DIRS})

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	set(LIBRARIES LIBCURL)

elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "MINGW")
	set(LIBRARIES curl stdc++fs)
endif()

target_link_libraries(reddit-save-archiver LINK_PUBLIC ${LIBRARIES} ${CMAKE_CXX_FLAGS})
