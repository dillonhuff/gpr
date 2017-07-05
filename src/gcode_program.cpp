#include "gcode_program.h"

using namespace std;

namespace gpr {

  ostream& operator<<(ostream& stream, const chunk& ic) {
    ic.print(stream);
    return stream;
   }

  ostream& operator<<(ostream& stream, const block& block) {
    if (block.has_line_number()) {
      stream << "N" << block.line_number() << " ";
    }
    for (auto i : block) { stream << *i << " "; }
    return stream;
  }

  ostream& operator<<(ostream& stream, const gcode_program& program) {
    for (auto b : program) { stream << b << endl; }
    return stream;
  }

  addr make_int_address(const int i) {
    addr_value v;
    v.int_val = i;
    return addr{ADDRESS_TYPE_INTEGER, v};
  }

  addr make_double_address(const double i) {
    addr_value v;
    v.dbl_val = i;
    return addr{ADDRESS_TYPE_DOUBLE, v};
  }
  
  std::unique_ptr<word_address> make_word_int(const char c, const int i) {
    //const int_address* const int_addr = new int_address(i);
    addr int_address = make_int_address(i);
    return std::unique_ptr<word_address>(new word_address(c, int_address));
  }

  std::unique_ptr<word_address> make_word_double(const char c, const double i) {
    addr double_addr = make_double_address(i);
    return std::unique_ptr<word_address>(new word_address(c, double_addr));
  }

  bool operator==(const chunk& l, const chunk& r) {
    return l.equals(r);
  }

  bool operator!=(const chunk& l, const chunk& r) {
    return !(l == r);
  }
  
}
