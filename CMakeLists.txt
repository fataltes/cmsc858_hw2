cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(bf LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(BF_CXX_WARN "-Wno-unused-result;-Wno-strict-aliasing;-Wno-unused-function;-Wno-sign-compare")
set(BF_CXX_FLAGS "${MANTIS_CXX_WARN}")


include_directories(include)
add_subdirectory(src)