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

    T next() {
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
      return s.begin();
    }
    
    typename vector<T>::iterator remaining() {
      return s.begin() + i;
    }

  };

  typedef parse_stream<char> parse_state;
  
  void ignore_whitespace(parse_state& s) {
    while (s.chars_left() && (isspace(s.next()) || s.next() == '\r')) { s++; }
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

  double parse_double(parse_stream<string>& s) {
    double v = stod(s.next());
    s++;
    return v;
  }
  
  int parse_int(parse_stream<string>& s) {
    int i = stoi(s.next());
    s++;
    return i;
  }

  int parse_int(parse_state& s) {
    size_t j = s.i;

    //cout << "Remaining = " << string_remaining(s) << endl;
    int v = stoi(string_remaining(s), &j);

    s.i += j;
    return v;
  }

  addr parse_address(char c, parse_stream<string>& s) {
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
  
  string parse_line_comment_with_delimiter(string sc, parse_stream<string>& s) {
    string text = "";
    while (s.chars_left()) {
      text += s.next();
      s++;
    }

    return text;
  }

  string parse_comment_with_delimiters(char sc, char ec, parse_state& s) {
    int depth = 0;
    string text = "";
    do {
      if (s.next() == sc) {
	depth++;
	text += s.next();
      } else if (s.next() == ec) {
	depth--;
	text += s.next();
      }
      else {
	text += s.next();
      }
      s++;      
    } while (s.chars_left() && depth > 0);

    return text;
  }

  string parse_comment_with_delimiters(string sc,
				       string ec,
				       parse_stream<string>& s) {
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

  chunk parse_word_address(parse_stream<string>& s) {
    assert(s.chars_left());
    assert(s.next().size() == 1);

    char c = s.next()[0];
    s++;

    addr a = parse_address(c, s);
    return chunk(c, a);
  }

  chunk parse_chunk(parse_stream<string>& s) {
    assert(s.chars_left());

    if (s.next()[0] == '[') {
      string cs = s.next();
      s++;
      return chunk('[', ']', cs.substr(1, cs.size() - 2));
    } else if (s.next()[0] == '(') {

      string cs = s.next();
      s++;
      return chunk('(', ')', cs.substr(1, cs.size() - 2));

    } else if (s.next() == "%") {
      s++;
      return make_percent_chunk();
    } else if (s.next() == ";") {
      s++;
      string cs = parse_line_comment_with_delimiter(";", s);
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

  bool is_slash(const string& s) {
    if (s.size() != 1) { return false; }

    return s[0] == '/';
  }

  bool parse_slash(parse_stream<string>& s) {
    if (is_slash(s.next())) {
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

  std::pair<bool, int> parse_line_number(parse_stream<string>& s) {
    if (s.next() == "N") {
      s++;

      int ln = parse_int(s);

      return std::make_pair(true, ln);
    }
    return std::make_pair(false, -1);
  }

  block parse_tokens(const std::vector<string>& tokens) {

    parse_stream<string> s(tokens);
    vector<chunk> chunks;
    bool is_slashed = parse_slash(s);

    std::pair<bool, int> line_no =
      parse_line_number(s);

    while (s.chars_left()) {
      chunk ch = parse_chunk(s);
      chunks.push_back(ch);
    }

    if (line_no.first) {
      return block(line_no.second, is_slashed, chunks);
    } else {
      return block(is_slashed, chunks);
    }

  }

  // block lex_gprog_line(const string& str) {
  //   parse_state s(str);

  //   vector<chunk> chunks;

  //   ignore_whitespace(s);

  //   bool is_slashed = parse_slash(s);

  //   ignore_whitespace(s);

  //   std::pair<bool, int> line_no =
  //     parse_line_number(s);

  //   while (s.chars_left()) {
  //     ignore_whitespace(s);
  //     if (!s.chars_left()) { break; }

  //     chunk ch = parse_chunk(s);
  //     chunks.push_back(ch);
      
  //   }

  //   if (line_no.first) {
  //     return block(line_no.second, is_slashed, chunks);
  //   } else {
  //     return block(is_slashed, chunks);
  //   }
  // }

  // vector<block> lex_gprog(const string& str) {
  //   vector<block> blocks;
  //   string::const_iterator line_start = str.begin();
  //   string::const_iterator line_end;

  //   while (line_start < str.end()) {
  //     line_end = find(line_start, str.end(), '\n');
  //     string line(line_start, line_end);
  //     if (line.size() > 0) {
  // 	block b = lex_gprog_line(line);
  // 	//if (b.size() > 0) {
  // 	  blocks.push_back(b);
  // 	  //}
  //     }
  //     line_start += line.size() + 1;
  //   }
  //   return blocks;
  // }

  vector<block> lex_gprog(const string& str) {
    vector<block> blocks;
    string::const_iterator line_start = str.begin();
    string::const_iterator line_end;

    while (line_start < str.end()) {
      line_end = find(line_start, str.end(), '\n');
      string line(line_start, line_end);

      if (line.size() > 0) {
	vector<string> line_tokens = lex_block(line);
	block b = parse_tokens(line_tokens);
	blocks.push_back(b);
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
    assert(s.chars_left());

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

    case '(':
      return parse_comment_with_delimiters('(', ')', s);

    case '[':
      return parse_comment_with_delimiters('[', ']', s);

    case ')':
      assert(false);

    case ']':
      assert(false);
      
    default:
      next_token += c;
      s++;
      return next_token;
    }
    
  }

  std::vector<std::string> lex_block(const std::string& block_text) {
    parse_state s(block_text);

    vector<string> tokens;

    ignore_whitespace(s);

    while (s.chars_left()) {
      ignore_whitespace(s);

      if (s.chars_left()) {
	string token = lex_token(s);
	tokens.push_back(token);
      }
    }

    return tokens;
  }

}
