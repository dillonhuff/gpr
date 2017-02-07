#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "parser.h"

namespace gpr {

  TEST_CASE("Parse comment") {
    gcode_program p = parse_gcode("G0 X1.0 Y1.0");
    REQUIRE(p.num_blocks() == 1);
  }

}
