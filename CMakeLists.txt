cmake_minimum_required(VERSION 2.6)

project(gpr)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

SET(EXTRA_CXX_COMPILE_FLAGS "-std=c++11 -I./src -I./test -Werror -Wall")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_COMPILE_FLAGS}")

INCLUDE_DIRECTORIES(./src/)

SET(GPR_HEADERS src/gcode_program.h
		src/parser.h)

SET(GPR_CPPS src/parser.cpp
	     src/gcode_program.cpp)

add_library(gpr ${GPR_CPPS})

SET(TEST_FILES test/parser_tests.cpp
	       test/main.cpp)

add_executable(all-tests ${TEST_FILES})
target_link_libraries(all-tests gpr)

add_executable(parse-gcode src/main.cpp)
target_link_libraries(parse-gcode gpr)
