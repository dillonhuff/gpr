#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "parser.h"

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

    word_address* g1 = make_word_int('G', 1);

    SECTION("First token is G1") {
      REQUIRE(p.get_block(1).get_chunk(0) == *g1);
    }

    // SECTION("Second token is not G1") {
    //   REQUIRE(p.get_block(1).get_token(0) != expected);
    // }

    delete g1;

  }

}
