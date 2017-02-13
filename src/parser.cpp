#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>

#include "parser.h"

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

  int parse_i(parse_state& s) {
    string digits = "";
    while (s.chars_left() && isdigit(s.next())) {
      digits += s.next();
      s++;
    }

    cout << "Parsing str = " << digits << endl;
    int v = stoi(digits);
    cout << "Done parsign str" << endl;
    
    return v;
  }

  double parse_dbl(parse_state& s) {
    string text = "";
    if (s.next() == '-') { text += s.next(); s++; }
    while (s.chars_left() && isdigit(s.next())) { text += s.next(); s++; }
    if (s.next() == '.') { text += s.next(); s++; }
    while (s.chars_left() && isdigit(s.next())) { text += s.next(); s++; }
    return atof(text.c_str());
  }
  
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
    double v =stod(string_remaining(s), &j);
    s.i += j;
    return v;
  }

  int parse_int(parse_state& s) {
    size_t j = s.i;

    cout << " Parsing str" << endl;
    int v = stoi(string_remaining(s), &j);
    cout << "Done parsign str" << endl;

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

  address* parse_address(char c, parse_state& s) {
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
      return new double_address(parse_double(s));
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
      return new int_address(parse_i(s));
    default:
      cout << "Invalid c = " << c << endl;
      cout << "Inavlid c as int = " << ((int) c) << endl;
      cout << "Is EOF? " << (((int) c) == EOF) << endl;
      assert(false);
    }
  }
  
  string parse_comment_with_delimiters(char sc, char ec, parse_state& s) {
    int depth = 0;
    string text = "";
    do {
      if (s.next() == sc) { depth++; }
      else if (s.next() == ec) { depth--; }
      text += s.next();
      s++;
    } while (s.chars_left() && depth > 0);
    // while (s.next() != ec) {
    //   text += s.next();
    //   s++;
    // }
    // parse_char(ec, s);
    return text;
  }

  word_address* parse_word_address(parse_state& s) {
    cout << "Remaining str = " << string_remaining(s) << endl;
    char c = s.next();
    s++;

    cout << "Next char = " << c << endl;

    address* a = parse_address(c, s);
    return new word_address(c, a);
  }

  chunk* parse_chunk(parse_state& s) {
    cout << "Parsign chunk" << endl;

    if (s.next() == '[') {
      string cs = parse_comment_with_delimiters('[', ']', s);
      return new comment('[', ']', cs);
    } else if (s.next() == '(') {
      string cs = parse_comment_with_delimiters('(', ')', s);
      return new comment('(', ')', cs);
    } else {
      return parse_word_address(s);
    }
    
  }

  block lex_gprog_line(const string& str, int line_no) {
    parse_state s(str);

    vector<chunk*> chunks;
    
    while (s.chars_left()) {
      ignore_whitespace(s);
      if (!s.chars_left()) { break; }

      chunk* ch = parse_chunk(s);
      chunks.push_back(ch);
      
    //   token t = parse_token(s);
    //   t.line_no = line_no;
    //   ts.push_back(t);

    }

    return block(chunks);
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
	//if (b.size() > 0) {
	  blocks.push_back(b);
	  //}
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
