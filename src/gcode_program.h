#pragma once

#include <vector>

namespace gpr {

  enum orientation {
    GCA_ABSOLUTE = 0,
    GCA_RELATIVE = 1,
    GCA_NONE
  };

  enum token_type {
    TOKEN_TYPE_PAREN_COMMENT = 0,
    TOKEN_TYPE_BRACKET_COMMENT,
    TOKEN_TYPE_ICODE,
    TOKEN_TYPE_OTHER
  };

  class token {

    void print(std::ostream& stream) {}

  };

  token make_ilit(const char control_char, const int i);

  bool operator==(const token& l, const token& r);

  class block {
  protected:
    std::vector<token> tokens;

  public:
    int size() const { return 1; }
    token get_token(const int i) { return tokens[i]; }

  };

  class gcode_program {
  protected:
    std::vector<block> blocks;

  public:
    gcode_program(const std::vector<block>& p_blocks) :
      blocks(p_blocks) {}

    int num_blocks() const { return blocks.size(); }

    block get_block(const int i) { return blocks[i]; }

    std::vector<block>::const_iterator begin() const { return std::begin(blocks); }
    std::vector<block>::const_iterator end() const { return std::end(blocks); }

    std::vector<block>::iterator begin() { return std::begin(blocks); }
    std::vector<block>::iterator end() { return std::end(blocks); }

  };

}
