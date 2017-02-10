#pragma once

#include <string>
#include <vector>

namespace gpr {

  // enum n_type {
  //   TOKEN_TYPE_PAREN_COMMENT = 0,
  //   TOKEN_TYPE_BRACKET_COMMENT,
  //   TOKEN_TYPE_ICODE_DBL,
  //   TOKEN_TYPE_ICODE_INT,
  //   TOKEN_TYPE_OTHER
  // };

  // union icode_value {
  //   double dbl;
  //   int i;
  // };

  // struct icode {
  //   char c;
  //   icode_value v;
  // };

  // union token_data {
  //   std::string str;
  //   icode ic;
  // };

  class address {
  };

  class chunk {};

  class word_address {
  protected:
    char wd;
    address addr;

  public:


  };

  class block {
  protected:
    std::vector<chunk> chunks;

  public:
    int size() const { return 1; }
    chunk get_token(const int i) { return chunks[i]; }

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
