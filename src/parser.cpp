#include "parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>

using namespace std;

namespace gpr {
  
  template<typename T>
  struct parse_stream {
    size_t i;
    vector<T> s;

    template<typename R>
    parse_stream<T>(R v) : s(v.begin(), v.end()) {
      i = 0;
    }

    char next() {
      return s[i];
    }

    int chars_left() const {
      return i < s.size();
    }

    parse_stream<T>& operator++(int) {
      i++;
      return *this;
    }

    parse_stream<T>& operator--(int) {
      i--;
      return *this;
    }

    typename vector<T>::iterator end() {
      return s.end();
    }

    typename vector<T>::iterator begin() {
      return s.end();
    }
    
    typename vector<T>::iterator remaining() {
      return s.begin() + i;
    }

  };

  typedef parse_stream<char> parse_state;
  
  void ignore_whitespace(parse_state& s) {
    while (s.chars_left() && (isspace(s.next()) || s.next() == '%' || s.next() == '\r')) { s++; }
  }

  string string_remaining(parse_state& ps) {
    return string(ps.remaining(), ps.end());
  }

  void parse_char(char c, parse_state& s) {
    if (s.next() == c) {
      s++;
      return;
    }
    cout << "Cannot parse char " << c << " from string " << string_remaining(s) << endl;
    assert(false);
  }

  double parse_double(parse_state& s) {
    size_t j = s.i;
    double v = stod(string_remaining(s), &j);
    s.i += j;
    return v;
  }

  int parse_int(parse_state& s) {
    size_t j = s.i;

    //cout << "Remaining = " << string_remaining(s) << endl;
    int v = stoi(string_remaining(s), &j);

    s.i += j;
    return v;
  }

  double parse_coordinate(char c, parse_state& s) {
    ignore_whitespace(s);
    assert(s.next() == c);
    s++;
    return parse_double(s);
  }

  double parse_option_coordinate(char c, parse_state& s, double def) {
    ignore_whitespace(s);
    if (s.next() == c) {
      s++;
      return parse_double(s);
    }
    return def;
  }

  addr parse_address(char c, parse_state& s) {
    switch(c) {
    case 'X':
    case 'Y':
    case 'Z':
    case 'I':
    case 'J':
    case 'K':
    case 'F':
    case 'R':
    case 'Q':
    case 'S':
    case 'x':
    case 'y':
    case 'z':
    case 'i':
    case 'j':
    case 'k':
    case 'f':
    case 'r':
    case 's':
    case 'q':
    case 'E':
      return make_double_address(parse_double(s));
    case 'G':
    case 'H':
    case 'M':
    case 'N':
    case 'O':
    case 'T':
    case 'P':
    case 'D':
    case 'L':
    case 'g':
    case 'h':
    case 'm':
    case 'n':
    case 'o':
    case 't':
    case 'p':
    case 'd':
    case 'l':
      return make_int_address(parse_int(s));
    default:
      cout << "Invalid c = " << c << endl;
      cout << "Inavlid c as int = " << ((int) c) << endl;
      cout << "Is EOF? " << (((int) c) == EOF) << endl;
      assert(false);
    }
  }

  string parse_line_comment_with_delimiter(char sc, parse_state& s) {
    string text = "";
    while (s.chars_left() && s.next() != '\n') {
      text += s.next();
      s++;
    }

    return text;
  }

  string parse_comment_with_delimiters(char sc, char ec, parse_state& s) {
    int depth = 0;
    string text = "";
    do {
      if (s.next() == sc) { depth++; }
      else if (s.next() == ec) { depth--; }
      else {
	text += s.next();
      }
      s++;      
    } while (s.chars_left() && depth > 0);

    return text;
  }

  chunk parse_word_address(parse_state& s) {
    char c = s.next();
    s++;

    addr a = parse_address(c, s);
    return chunk(c, a);
  }

  chunk parse_chunk(parse_state& s) {

    if (s.next() == '[') {
      string cs = parse_comment_with_delimiters('[', ']', s);
      return chunk('[', ']', cs);
    } else if (s.next() == '(') {
      string cs = parse_comment_with_delimiters('(', ')', s);
      return chunk('(', ')', cs);
    } else if (s.next() == ';') {
      s++;
      string cs = parse_line_comment_with_delimiter(';', s);
      return chunk(';', ';', cs);
    } else {
      return parse_word_address(s);
    }
    
  }

  bool parse_slash(parse_state& s) {
    if (s.next() == '/') {
      s++;
      return true;
    }

    return false;
  }

  std::pair<bool, int> parse_line_number(parse_state& s) {
    if (s.next() == 'N') {
      s++;

      int ln = parse_int(s);

      return std::make_pair(true, ln);
    }
    return std::make_pair(false, -1);
  }

  block lex_gprog_line(const string& str) {
    parse_state s(str);

    vector<chunk> chunks;

    ignore_whitespace(s);

    bool is_slashed = parse_slash(s);

    ignore_whitespace(s);

    std::pair<bool, int> line_no =
      parse_line_number(s);

    while (s.chars_left()) {
      ignore_whitespace(s);
      if (!s.chars_left()) { break; }

      chunk ch = parse_chunk(s);
      chunks.push_back(ch);
      
    }

    if (line_no.first) {
      return block(line_no.second, is_slashed, chunks);
    } else {
      return block(is_slashed, chunks);
    }
  }

  vector<block> lex_gprog(const string& str) {
    vector<block> blocks;
    string::const_iterator line_start = str.begin();
    string::const_iterator line_end;

    while (line_start < str.end()) {
      line_end = find(line_start, str.end(), '\n');
      string line(line_start, line_end);
      if (line.size() > 0) {
	block b = lex_gprog_line(line);
	//if (b.size() > 0) {
	  blocks.push_back(b);
	  //}
      }
      line_start += line.size() + 1;
    }
    return blocks;
  }
  
  gcode_program parse_gcode(const std::string& program_text) {
    auto blocks = lex_gprog(program_text);
    return gcode_program(blocks);
  }

  gcode_program parse_gcode_saving_block_text(const std::string& program_text) {
    auto blocks = lex_gprog(program_text);
    for (auto& b : blocks) {
      b.set_debug_text();
    }
    return gcode_program(blocks);
  }

  bool is_num_char(const char c) {
    return (isdigit(c) ||
	    (c == '.') ||
	    (c == '-'));
  }

  std::string digit_string(parse_state& s) {
    string num_str = "";

    while (s.chars_left() && is_num_char(s.next())) {
      num_str += s.next();
      s++;
    }

    return num_str;
  }

  std::string lex_token(parse_state& s) {
    char c = s.next();
    string next_token = "";

    if (is_num_char(c)) {
      return digit_string(s);
    }

    switch(c) {
    case '%':
      s++;
      return "%";
    case '/':
      s++;
      return "/";

    case 'X':
    case 'Y':
    case 'Z':
    case 'I':
    case 'J':
    case 'K':
    case 'F':
    case 'R':
    case 'Q':
    case 'S':
    case 'x':
    case 'y':
    case 'z':
    case 'i':
    case 'j':
    case 'k':
    case 'f':
    case 'r':
    case 's':
    case 'q':
    case 'E':
    case 'G':
    case 'H':
    case 'M':
    case 'N':
    case 'O':
    case 'T':
    case 'P':
    case 'D':
    case 'L':
    case 'g':
    case 'h':
    case 'm':
    case 'n':
    case 'o':
    case 't':
    case 'p':
    case 'd':
    case 'l':
      next_token += c;
      s++;
      return next_token;
    default:
      cout << "Invalid c = " << c << endl;
      cout << "Inavlid c as int = " << ((int) c) << endl;
      cout << "Is EOF? " << (((int) c) == EOF) << endl;
      assert(false);
    }
    
  }

  std::vector<std::string> lex_block(const std::string& block_text) {
    parse_state s(block_text);

    vector<string> tokens;

    ignore_whitespace(s);

    while (s.chars_left()) {
      ignore_whitespace(s);
      string token = lex_token(s);
      tokens.push_back(token);
    }

    return tokens;
  }

}
