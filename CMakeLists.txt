cmake_minimum_required(VERSION 3.10)
project(SZTronics_miscellaneous VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "source/*.cpp")

add_library(sztronics_miscellaneous STATIC ${SOURCES})
target_include_directories(sztronics_miscellaneous PUBLIC headers)