cmake_minimum_required(VERSION 3.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

project(PNGLoader)

file(GLOB SOURCES src/*.cpp src/*.h)

add_executable(PNGLoader ${SOURCES})