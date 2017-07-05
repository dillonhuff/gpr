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

    chunk g1 = make_word_int('G', 1);

    SECTION("First token is G1") {
      REQUIRE(p.get_block(1).get_chunk(0) == g1);
    }

    SECTION("Second token is not G1") {
      REQUIRE(p.get_block(0).get_chunk(0) != g1);
    }

    chunk f12 = make_word_double('F', 12.0);

    SECTION("Double addresses") {
      REQUIRE(p.get_block(1).get_chunk(4) == f12);
    }

    SECTION("Double addresses") {
      REQUIRE(p.get_block(1).get_chunk(3) != f12);
    }
    
  }

  TEST_CASE("Different comments with same delimiters are not equal") {
    gcode_program p =
      parse_gcode("( This is a comment )\n M23 ( And so is this ) G54");

    REQUIRE(p.get_block(0).get_chunk(0) != p.get_block(1).get_chunk(1));
  }

  TEST_CASE("Same comments with same delimiters are equal") {
    gcode_program p =
      parse_gcode("( This is a comment G2 )\n M23 ( This is a comment G2 ) G54");

    REQUIRE(p.get_block(0).get_chunk(0) == p.get_block(1).get_chunk(1));
  }

  TEST_CASE("Same comments with different delimiters are not equal") {
    gcode_program p =
      parse_gcode("( This is a comment G2 )\n M23 [ This is a comment G2 ] G54");

    REQUIRE(p.get_block(0).get_chunk(0) != p.get_block(1).get_chunk(1));
  }

  TEST_CASE("Second block is deleted") {
    gcode_program p =
      parse_gcode("( This is a comment G2 )\n /M23 [ This is a comment G2 ] G54");

    REQUIRE(p.get_block(1).is_deleted());
  }

  TEST_CASE("First block is not deleted") {
    gcode_program p =
      parse_gcode("( This is a comment G2 )\n /M23 [ This is a comment G2 ] G54");

    REQUIRE(!(p.get_block(0).is_deleted()));
  }

  TEST_CASE("3rd block is labeled line 103") {
    gcode_program p =
      parse_gcode("(*** Toolpath 1 ***)\n G0 X0.0 Y0.0 Z0.0 \n N103 G1 X1.0 F23.0\nG1 Z-1.0 F10.0");

    SECTION("Has line number") {
      REQUIRE(p.get_block(2).has_line_number());
    }

    SECTION("Number is 103") {
      REQUIRE(p.get_block(2).line_number() == 103);
    }

    SECTION("4th block does not have a line number") {
      REQUIRE(!(p.get_block(3).has_line_number()));
    }

  }

  TEST_CASE("Parsing Cura style-semicolon comments") {
    gcode_program p =
      parse_gcode(";Generated with Cura_SteamEngine 2.5.0\nM190 S60\nM104 S200\nM109 S200\nG28 ;Home");

    block b = p.get_block(0);
    REQUIRE(b.get_chunk(0).tp() == CHUNK_TYPE_COMMENT);
  }

  TEST_CASE("Parse 3D printer E-block") {
    gcode_program p = parse_gcode(";Prime the extruder\nG92 E0");

    REQUIRE(p.num_blocks() == 2);
  }

  
}
