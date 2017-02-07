#pragma once

#include <string>

#include "gcode_program.h"

namespace gpr {


  gcode_program parse_gcode(const std::string& program_text);

}
