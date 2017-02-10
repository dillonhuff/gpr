#include "gcode_program.h"

namespace gpr {

  word_address* make_word_int(const char c, const int i) {
    const int_address* const int_addr = new int_address(i);
    return new word_address(c, int_addr);
  }

  bool operator==(const chunk& l, const chunk& r) {
    return true;
  }

}
