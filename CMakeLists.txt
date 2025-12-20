cmake_minimum_required(VERSION 3.10)

project(PNGLoader)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
file(GLOB SOURCES src/*.cpp src/*.h)
add_executable(PNGLoader ${SOURCES})