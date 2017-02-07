cmake_minimum_required(VERSION 2.6)

project(gpr)

SET(EXTRA_CXX_COMPILE_FLAGS "-std=c++11 -I./src -I./test -I/opt/local/include -O2 -Werror -Wall")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_COMPILE_FLAGS}")

INCLUDE_DIRECTORIES(./src/)

SET(GPR_HEADERS src/gcode_program.h
		src/parser.h)

SET(GPR_CPPS src/parser.cpp
	     src/gcode_program.cpp)

add_library(gpr ${GPR_CPPS})

SET(TEST_FILES test/parser_tests.cpp)

add_executable(all-tests ${TEST_FILES})
target_link_libraries(all-tests gpr)
