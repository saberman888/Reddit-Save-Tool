cmake_minimum_required(VERSION 3.15)
project(reddit-save-archiver)
set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wfatal-errors")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb3")

if(CMAKE_BUILD_TYPE|"Release")
  add_compile_option(-DNDEBUG=1)
endif()

add_compile_options(-DVERSION="2020-11-02")

add_executable(reddit-saver RedditObject.cpp BasicRequest.cpp Imgur.cpp Reddit.cpp Saver.cpp Source.cpp )

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	set(LIBRARIES libcurl.lib nlohmann_json::nlohmann_json)
else()
	set(LIBRARIES curl nlohmann_json::nlohmann_json stdc++fs)
endif()

target_link_libraries(reddit-saver LINK_PUBLIC ${LIBRARIES})
