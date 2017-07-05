#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace gpr {

  enum address_type {
    ADDRESS_TYPE_INTEGER = 0,
    ADDRESS_TYPE_DOUBLE
  };

  union addr_value {
    double dbl_val;
    int int_val;
  };

  class addr {
  private:
    address_type ad_type;
    addr_value val;

  public:

    addr(const address_type& p_ad_type,
	 const addr_value& p_val) {
      ad_type = p_ad_type;
      val = p_val;
    }

    address_type tp() const { return ad_type; }

    double double_value() const {
      assert(tp() == ADDRESS_TYPE_DOUBLE);

      return val.dbl_val;
    }

    int int_value() const {
      assert(tp() == ADDRESS_TYPE_INTEGER);

      return val.int_val;
    }

    bool equals(const addr& other) const {
      if (other.tp() != tp()) {
	return false;
      }

      if (other.tp() == ADDRESS_TYPE_DOUBLE) {
	return other.double_value() == double_value();
      }

      return other.int_value() == int_value();
    }

    void print(std::ostream& out) const {
      if (tp() == ADDRESS_TYPE_DOUBLE) {
	out << double_value();
	return;
      }

      out << int_value();

    }

  };

  enum chunk_type {
    CHUNK_TYPE_COMMENT,
    CHUNK_TYPE_WORD_ADDRESS
  };

  // chunk is the superclass of all objects that can appear in a block.
  // A chunk can be either a comment or a word-address pair
  // For example in the G-code program below:
  //      (*** Toolpath 1 ***)
  //      G0 X0.0 Y0.0 Z0.0
  //      G1 X1.0 F23.0
  //      G1 Z-1.0 F10.0
  // The program consits of 4 blocks (block is just G-code speak for line).
  // The first block contains 1 chunk, which is the comment "(*** Toolpath 1 ***)".
  // The second block contains 4 chunks, each of which is a pair of a word
  // (a character) and an address (a number). The 4 chunks are:
  // G0, X0.0, Y0.0, and Z0.0. In G0 the word is 'G' and the address is '0'.
  // In X0.0 the word is 'X' and the address is '0.0', and so on.
  class chunk {
  private:
    chunk_type chunk_tp;

    // Comment fields;
    char left_delim;
    char right_delim;
    std::string comment_text;

    // Word-address fields
    char wd;
    addr adr;
    
  public:
    chunk(const char p_left_delim,
	  const char p_right_delim,
	  const std::string& p_comment_text) :
      chunk_tp(CHUNK_TYPE_COMMENT),
      left_delim(p_left_delim),
      right_delim(p_right_delim),
      comment_text(p_comment_text),
      wd('\0'),
      adr(ADDRESS_TYPE_INTEGER, {-1})
    {}

    chunk(const char p_wd,
	  const addr p_adr) :
      chunk_tp(CHUNK_TYPE_WORD_ADDRESS),
      wd(p_wd),
      adr(p_adr)
    {}
    
    chunk_type tp() const { return chunk_tp; }

    char get_left_delim() const { return left_delim; }
    char get_right_delim() const { return right_delim; }
    std::string get_comment_text() const { return comment_text; }


    char get_word() const { return wd; }
    addr get_address() const { return adr; }
    
    bool equals_word_address(const chunk& other_addr) const {
      assert(other_addr.tp() == CHUNK_TYPE_WORD_ADDRESS);

      return (get_word() == other_addr.get_word()) &&
	(get_address().equals(other_addr.get_address()));
    }
    
    bool equals_comment(const chunk& other_comment) const {
      assert(other_comment.tp() == CHUNK_TYPE_COMMENT);

      return (get_comment_text() == other_comment.get_comment_text()) &&
  	(get_left_delim() == other_comment.get_left_delim()) &&
  	(get_right_delim() == other_comment.get_right_delim());
    }
    
    virtual bool equals(const chunk& other) const {
      if (other.tp() != tp()) {
	return false;
      }

      if (tp() == CHUNK_TYPE_WORD_ADDRESS) {
	return equals_word_address(other);
      } else if (tp() == CHUNK_TYPE_COMMENT) {
	return equals_comment(other);
      } else {
	assert(false);
      }

    }

    void print_comment(std::ostream& stream) const {
      stream << left_delim << " " << comment_text << " " << right_delim;
    }

    void print_word_address(std::ostream& stream) const {
      stream << wd;
      adr.print(stream);
    }
    
    void print(std::ostream& stream) const {
      if (tp() == CHUNK_TYPE_COMMENT) {
	print_comment(stream);
      } else if (tp() == CHUNK_TYPE_WORD_ADDRESS) {
	print_word_address(stream);
      } else {
	assert(false);
      }
    }

  };

  chunk make_word_int(const char c, const int i);
  chunk make_word_double(const char c, const double i);

  bool operator==(const chunk& l, const chunk& r);
  bool operator!=(const chunk& l, const chunk& r);

  // A block is really just a line of code, so for example the following program:
  //      (*** Toolpath 1 ***)
  //      G0 X0.0 Y0.0 Z0.0
  //      G1 X1.0 F23.0
  //      G1 Z-1.0 F10.0
  // consists of 4 blocks
  class block {
  protected:
    bool has_line_no;
    int line_no;
    bool slashed_out;
    std::vector<chunk> chunks;

  public:
    block(const int p_line_no,
	  const bool p_slashed_out,
	  const std::vector<chunk> p_chunks) :
      has_line_no(true),
      line_no(p_line_no),
      slashed_out(p_slashed_out),
      chunks(p_chunks) {}

    block(const bool p_slashed_out,
	  const std::vector<chunk> p_chunks) :
      has_line_no(false),
      line_no(-1),
      slashed_out(p_slashed_out),
      chunks(p_chunks) {}

    block(const block& other) :
      has_line_no(other.has_line_no),
      line_no(other.line_no),
      slashed_out(other.slashed_out) {

      for (unsigned i = 0; i < other.chunks.size(); i++) {
	chunks.push_back( other.chunks[i] );
      }
    }
    
    block& operator=(const block& other) {
      has_line_no = other.has_line_no;
      line_no = other.line_no;
      slashed_out = other.slashed_out;
      for (unsigned i = 0; i < other.chunks.size(); i++) {
	chunks.push_back( other.chunks[i] );
      }

      return *this;
    }

    int size() const { return chunks.size(); }

    const chunk& get_chunk(const int i) {
      assert(i < size());

      return chunks[i];
    }

    bool is_deleted() const { return slashed_out; }

    bool has_line_number() const {
      return has_line_no;
    }

    int line_number() const {
      assert(has_line_number());
      return line_no;
    }

    std::vector<chunk>::const_iterator begin() const { return std::begin(chunks); }
    std::vector<chunk>::const_iterator end() const { return std::end(chunks); }

    std::vector<chunk>::iterator begin() { return std::begin(chunks); }
    std::vector<chunk>::iterator end() { return std::end(chunks); }
    
  };

  class gcode_program {
  protected:
    std::vector<block> blocks;

  public:
    gcode_program(const std::vector<block>& p_blocks) :
      blocks(p_blocks) {}

    int num_blocks() const { return blocks.size(); }

    block get_block(const int i) {
      assert(i < blocks.size());
      return blocks[i];
    }

    std::vector<block>::const_iterator begin() const { return std::begin(blocks); }
    std::vector<block>::const_iterator end() const { return std::end(blocks); }

    std::vector<block>::iterator begin() { return std::begin(blocks); }
    std::vector<block>::iterator end() { return std::end(blocks); }

  };

  std::ostream& operator<<(std::ostream& stream, const chunk& ic);

  std::ostream& operator<<(std::ostream& stream, const block& block);

  std::ostream& operator<<(std::ostream& stream, const gcode_program& program);

  addr make_int_address(const int i);

  addr make_double_address(const double i);
  
}
