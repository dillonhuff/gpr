#define CATCH_CONFIG_MAIN

#include <iostream>

#include "catch.hpp"

#include "parser.h"

using namespace std;

namespace gpr {

  TEST_CASE("One line program") {
    gcode_program p = parse_gcode("G0 X1.0 Y1.0");
    REQUIRE(p.num_blocks() == 1);
  }

  TEST_CASE("Two line program") {
    gcode_program p = parse_gcode("G0 X1.0 Y1.0\nG1 X0.0 Y0.0 Z1.2 F12.0");
    REQUIRE(p.num_blocks() == 2);
  }

  TEST_CASE("Correct first token") {
    gcode_program p = parse_gcode("G0 X1.0 Y1.0\nG1 X0.0 Y0.0 Z1.2 F12.0");

    unique_ptr<word_address> g1 = make_word_int('G', 1);

    SECTION("First token is G1") {
      REQUIRE(p.get_block(1).get_chunk(0) == *g1);
    }

    SECTION("Second token is not G1") {
      REQUIRE(p.get_block(0).get_chunk(0) != *g1);
    }

    unique_ptr<word_address> f12 = make_word_double('F', 12.0);

    SECTION("Double addresses") {
      REQUIRE(p.get_block(1).get_chunk(4) == *f12);
    }

    SECTION("Double addresses") {
      REQUIRE(p.get_block(1).get_chunk(3) != *f12);
    }
    
  }

  TEST_CASE("Comments with same delimiters equality test") {
    gcode_program p =
      parse_gcode("( This is a comment )\n M23 ( And so is this ) G54");
    REQUIRE(p.get_block(1).get_chunk(0) != p.get_block(1).get_chunk(1));
  }

}
