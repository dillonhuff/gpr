#include "gcode_program.h"

namespace gpr {

  token make_ilit(const char control_char, const int i) {
    token t;
    return t;
  }

  bool operator==(const token& l, const token& r) {
    return true;
  }

}
