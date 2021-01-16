cmake_minimum_required(VERSION 3.15)
project(reddit-save-archiver)
set(CMAKE_CXX_STANDARD 20)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenMP)

option(USE_OPENMP OFF)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "/W2 ")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -Werror -Wfatal-errors")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb3")
endif()



if(CMAKE_BUILD_TYPE|"Release")
  add_compile_option(-DNDEBUG=1)
endif()

add_compile_options(-DVERSION="2021-01-15RC")

add_executable(reddit-saver BasicRequest.cpp base.cpp sbjson.cpp RedditCommon.cpp Galleries.cpp Comment.cpp SelfPost.cpp Image.cpp Video.cpp Imgur.cpp Reddit.cpp Saver.cpp Source.cpp )

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set(STDFS stdc++fs)
endif()

if(USE_OPENMP)
  set(INCLUDE_OMP OpenMP::OpenMP_CXX)
endif()
target_link_libraries(reddit-saver LINK_PUBLIC ${STDFS} nlohmann_json::nlohmann_json curl ${INCLUDE_OMP})
