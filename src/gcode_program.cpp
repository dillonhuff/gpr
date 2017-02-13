#include "gcode_program.h"

using namespace std;

namespace gpr {

  ostream& operator<<(ostream& stream, const chunk& ic) {
    ic.print(stream);
    return stream;
   }

  ostream& operator<<(ostream& stream, const block& block) {
    for (auto i : block) { stream << *i << " "; }
    return stream;
  }

  ostream& operator<<(ostream& stream, const gcode_program& program) {
    for (auto b : program) { stream << b << endl; }
    return stream;
  }
  
  std::unique_ptr<word_address> make_word_int(const char c, const int i) {
    const int_address* const int_addr = new int_address(i);
    return std::unique_ptr<word_address>(new word_address(c, int_addr));
  }

  std::unique_ptr<word_address> make_word_double(const char c, const double i) {
    const double_address* const double_addr = new double_address(i);
    return std::unique_ptr<word_address>(new word_address(c, double_addr));
  }

  bool operator==(const chunk& l, const chunk& r) {
    return l.equals(r);
  }

  bool operator!=(const chunk& l, const chunk& r) {
    return !(l == r);
  }
  
}
