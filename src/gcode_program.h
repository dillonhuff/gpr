#pragma once

#include <cassert>
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

  enum address_type {
    ADDRESS_TYPE_INTEGER = 0,
    ADDRESS_TYPE_DOUBLE
  };

  class address {
  public:
    virtual address_type addr_tp() const { assert(false); }
    virtual ~address() {}
  };

  class int_address : public address {
  protected:
    const int val;

  public:
    int_address(const int p_val) : val(p_val) {}

    int value() const { return val; }

    address_type tp() const { return ADDRESS_TYPE_INTEGER; }

  };

  enum chunk_type {
    CHUNK_TYPE_COMMENT,
    CHUNK_TYPE_WORD_ADDRESS
  };

  class chunk {
  public:
    virtual chunk_type tp() const { assert(false); }
    virtual ~chunk() {}
    virtual bool equals(const chunk& other) const = 0;
  };

  class word_address : public chunk {
  protected:
    const char wd;
    const address* const addr;

  public:

    word_address(const char p_wd,
		 const address* const p_addr) :
      wd(p_wd),
      addr(p_addr)
    {}

    ~word_address() {
      delete addr;
    }

    virtual chunk_type tp() const { assert(false); }

    bool equals(const chunk& other) const {
      if (other.tp() != CHUNK_TYPE_WORD_ADDRESS) {
	return false;
      }

      return true;
    }

  };

  word_address* make_word_int(const char c, const int i);

  bool operator==(const chunk& l, const chunk& r);
  bool operator!=(const chunk& l, const chunk& r);

  class block {
  protected:
    std::vector<chunk*> chunks;

  public:
    int size() const { return 1; }
    const chunk& get_chunk(const int i) { return *(chunks[i]); }

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
