cmake_minimum_required(VERSION 2.6)

project(gpr)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(CMAKE_COMPILER_IS_GNUCXX OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
	SET(EXTRA_CXX_COMPILE_FLAGS "-std=c++11 -I./src -I./test -Werror -Wall")

	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_COMPILE_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))

INCLUDE_DIRECTORIES(./src/)

SET(GPR_HEADERS src/gcode_program.h
		src/parser.h)

SET(GPR_CPPS src/parser.cpp
	     src/gcode_program.cpp)

set(CMAKE_MACOSX_RPATH 1)

add_library(gpr SHARED ${GPR_CPPS})

SET(TEST_FILES test/parser_tests.cpp
	       test/main.cpp)

add_executable(all-tests ${TEST_FILES})
target_link_libraries(all-tests gpr)

add_executable(parse-gcode src/main.cpp)
target_link_libraries(parse-gcode gpr)

install(TARGETS gpr LIBRARY DESTINATION lib)
