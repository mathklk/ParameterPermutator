cmake_minimum_required(VERSION 3.12)

project(ParameterPermutatorExample)

set(HEADERS
)
set(SOURCES
    example.cpp
)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(executable_name "example")
add_executable(${executable_name} ${SOURCES} ${HEADERS})