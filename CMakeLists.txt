cmake_minimum_required(VERSION 3.15)
project(reddit-save-archiver)
set(CMAKE_CXX_STANDARD 17)
include_directories(reddit-save-archiver)

option(DEBUG "Enable debugging" OFF)
option(SINGLE_ID_DUMP, "Enable dumping single branches of Json for debugging purposes" OFF)
option(RELEASE "release build" ON)
if(DEBUG)
	add_compile_options(-D_DEBUG=1)
endif()

if(SINGLE_ID_DUMP)
	add_compile_options(-D_SINGLE_JSON_DUMP=1)
endif()

if(DEBUG OR SINGLE_ID_DUMP)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
else(RELEASE)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release)
endif()

add_executable(reddit-save-archiver base.cpp Reddit.cpp Saver.cpp Source.cpp)

find_package(Boost REQUIRED)
find_package(curl REQUIRED)
include_directories(${BOOST_INCLUDE_DIRS})

if(MSVC)
	find_library(LIBRARIES LIBCURL lib)

elseif(MINGW OR MSYS OR GNU)
	find_library(LIBRARIES libcurl stdc++fs lib)
endif()

target_link_libraries(reddit-save-archiver LINK_PUBLIC ${LIBRARIES})





