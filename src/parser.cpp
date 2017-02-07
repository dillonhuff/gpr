#include <algorithm>
#include <fstream>
#include <sstream>
#include <streambuf>

#include "parser.h"

using namespace std;

namespace gpr {

  ostream& operator<<(ostream& stream, const token& ic) {
    //ic.print(stream);
    return stream;
  }

  ostream& operator<<(ostream& stream, const block& block) {
    //for (auto i : block) { stream << i << " "; }
    return stream;
  }

  ostream& operator<<(ostream& stream, const vector<block>& blocks) {
    //for (auto b : blocks) { stream << b << endl; }
    return stream;
  }
  
  // int parse_i(parse_state& s) {
  //   string digits = "";
  //   while (s.chars_left() && isdigit(s.next())) {
  //     digits += s.next();
  //     s++;
  //   }
  //   return stoi(digits);
  // }

  // double parse_dbl(parse_state& s) {
  //   string text = "";
  //   if (s.next() == '-') { text += s.next(); s++; }
  //   while (s.chars_left() && isdigit(s.next())) { text += s.next(); s++; }
  //   if (s.next() == '.') { text += s.next(); s++; }
  //   while (s.chars_left() && isdigit(s.next())) { text += s.next(); s++; }
  //   return atof(text.c_str());
  // }
  
  // value parse_c_val(char c, parse_state& s) {
  //   switch(c) {
  //   case 'X':
  //   case 'Y':
  //   case 'Z':
  //   case 'I':
  //   case 'J':
  //   case 'K':
  //   case 'F':
  //   case 'R':
  //   case 'Q':
  //   case 'S':
  //   case 'x':
  //   case 'y':
  //   case 'z':
  //   case 'i':
  //   case 'j':
  //   case 'k':
  //   case 'f':
  //   case 'r':
  //   case 's':
  //   case 'q':
  //     return lit::make(parse_dbl(s));
  //   case 'G':
  //   case 'H':
  //   case 'M':
  //   case 'N':
  //   case 'O':
  //   case 'T':
  //   case 'P':
  //   case 'D':
  //   case 'L':
  //   case 'g':
  //   case 'h':
  //   case 'm':
  //   case 'n':
  //   case 'o':
  //   case 't':
  //   case 'p':
  //   case 'd':
  //   case 'l':
  //     return ilit::make(parse_i(s));
  //   default:
  //     cout << "Invalid c = " << c << endl;
  //     cout << "Inavlid c as int = " << ((int) c) << endl;
  //     cout << "Is EOF? " << (((int) c) == EOF) << endl;
  //     DBG_ASSERT(false);
  //   }
  // }

  // token parse_token(parse_state& s) {
  //   if (s.next() == '[') {
  //     string cs = parse_comment_with_delimiters('[', ']', s);
  //     return token(cs);
  //   } else if (s.next() == '(') {
  //     string cs = parse_comment_with_delimiters('(', ')', s);
  //     return token(cs);
  //   } else {
  //     char c = s.next();
  //     s++;
  //     value* v = parse_c_val(c, s);
  //     return token(c, v);
  //   }
  // }

  block lex_gprog_line(const string& str, int line_no) {
    return block();
  //   block ts;
  //   parse_state s(str);
  //   int i = 0;
  //   while (s.chars_left()) {
  //     ignore_whitespace(s);
  //     if (!s.chars_left()) { break; }
  //     token t = parse_token(s);
  //     t.line_no = line_no;
  //     ts.push_back(t);
  //     i++;
  //   }
  //   return ts;
  }

  vector<block> lex_gprog(const string& str) {
    vector<block> blocks;
    string::const_iterator line_start = str.begin();
    string::const_iterator line_end;
    int line_no = 1;
    while (line_start < str.end()) {
      line_end = find(line_start, str.end(), '\n');
      string line(line_start, line_end);
      if (line.size() > 0) {
	block b = lex_gprog_line(line, line_no);
	if (b.size() > 0) {
	  blocks.push_back(b);
	}
      }
      line_start += line.size() + 1;
      line_no++;
    }
    return blocks;
  }
  
  gcode_program parse_gcode(const std::string& program_text) {
    auto blocks = lex_gprog(program_text);
    return gcode_program(blocks);
  }

}
