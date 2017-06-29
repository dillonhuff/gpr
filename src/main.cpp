#include "parser.h"

#include <string>
#include <fstream>
#include <streambuf>

using namespace gpr;
using namespace std;

int main(int argc, char** argv) {
  assert(argc == 2);

  string file = argv[1];


  std::ifstream t(file);
  std::string file_contents((std::istreambuf_iterator<char>(t)),
			    std::istreambuf_iterator<char>());

  

  gcode_program p = parse_gcode(file_contents);

  cout << p << endl;
}
