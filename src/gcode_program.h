#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace gpr {

  enum address_type {
    ADDRESS_TYPE_INTEGER = 0,
    ADDRESS_TYPE_DOUBLE
  };

  class address {
  public:
    virtual address_type addr_tp() const { assert(false); }
    virtual bool equals(const address& other) const = 0;
    virtual address_type tp() const = 0;
    virtual void print(std::ostream& stream) const = 0;
    virtual address* copy() const = 0;
    virtual ~address() {}
  };

  class int_address : public address {
  protected:
    const int val;

  public:
    int_address(const int p_val) : val(p_val) {}

    address* copy() const {
      return new int_address(val);
    }

    int value() const { return val; }

    address_type tp() const { return ADDRESS_TYPE_INTEGER; }

    void print(std::ostream& stream) const {
      stream << val;
    }

    virtual bool equals(const address& other) const {
      if (other.tp() != ADDRESS_TYPE_INTEGER) {
	return false;
      }
      const int_address& other_int = static_cast<const int_address&>(other);
      return val == other_int.val;
    }

  };

  class double_address : public address {
  protected:
    const double val;

  public:
    double_address(const double p_val) : val(p_val) {}

    address* copy() const {
      return new double_address(val);
    }
    
    double value() const { return val; }

    address_type tp() const { return ADDRESS_TYPE_DOUBLE; }

    void print(std::ostream& stream) const {
      stream << val;
    }
    
    virtual bool equals(const address& other) const {
      if (other.tp() != ADDRESS_TYPE_DOUBLE) {
	return false;
      }
      const double_address& other_int = static_cast<const double_address&>(other);
      return val == other_int.val;
    }
    
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
    virtual void print(std::ostream& stream) const = 0;
    virtual chunk* copy() const = 0;
  };

  class comment : public chunk {
  protected:
    char left_delim;
    char right_delim;
    std::string comment_text;

  public:
    comment(const char p_left_delim,
	    const char p_right_delim,
	    const std::string& p_comment_text) :
      left_delim(p_left_delim),
      right_delim(p_right_delim),
      comment_text(p_comment_text)
    {}

    chunk_type tp() const { return CHUNK_TYPE_COMMENT; }

    chunk* copy() const {
      return new comment(left_delim, right_delim, comment_text);
    }
      
    virtual bool equals(const chunk& other) const {
      if (other.tp() != CHUNK_TYPE_COMMENT) {
	return false;
      }

      const comment& other_comment =
	static_cast<const comment&>(other);

      return (comment_text == other_comment.comment_text) &&
	(left_delim == other_comment.left_delim) &&
	(right_delim == other_comment.right_delim);
    }

    void print(std::ostream& stream) const {
      stream << left_delim << " " << comment_text << " " << right_delim;
    }
    
    ~comment() {}

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

    chunk* copy() const {
      return new word_address(wd, addr->copy());
    }
    
    virtual chunk_type tp() const { return CHUNK_TYPE_WORD_ADDRESS; }

    void print(std::ostream& stream) const {
      stream << wd;
      addr->print(stream);
    }

    bool equals(const chunk& other) const {
      if (other.tp() != CHUNK_TYPE_WORD_ADDRESS) {
	return false;
      }

      const word_address& other_addr = static_cast<const word_address&>(other);
      return (wd == other_addr.wd) && (addr->equals(*(other_addr.addr)));
    }

  };

  std::unique_ptr<word_address> make_word_int(const char c, const int i);
  std::unique_ptr<word_address> make_word_double(const char c, const double i);

  bool operator==(const chunk& l, const chunk& r);
  bool operator!=(const chunk& l, const chunk& r);

  class block {
  protected:
    bool has_line_no;
    int line_no;
    bool slashed_out;
    std::vector<chunk*> chunks;

  public:
    block(const int p_line_no,
	  const bool p_slashed_out,
	  const std::vector<chunk*> p_chunks) :
      has_line_no(true),
      line_no(p_line_no),
      slashed_out(p_slashed_out),
      chunks(p_chunks) {}

    block(const bool p_slashed_out,
	  const std::vector<chunk*> p_chunks) :
      has_line_no(false),
      line_no(-1),
      slashed_out(p_slashed_out),
      chunks(p_chunks) {}

    ~block() {
      for (unsigned i = 0; i < chunks.size(); i++) {
	delete chunks[i];
      }
    }

    block(const block& other) :
      has_line_no(other.has_line_no),
      line_no(other.line_no),
      slashed_out(other.slashed_out) {

      for (unsigned i = 0; i < other.chunks.size(); i++) {
	chunks.push_back( (other.chunks[i])->copy() );
      }
    }
    
    block& operator=(const block& other) {
      has_line_no = other.has_line_no;
      line_no = other.line_no;
      slashed_out = other.slashed_out;
      for (unsigned i = 0; i < other.chunks.size(); i++) {
	chunks.push_back( (other.chunks[i])->copy() );
      }

      return *this;
    }

    int size() const { return chunks.size(); }

    const chunk& get_chunk(const int i) {
      assert(i < size());

      return *(chunks[i]);
    }

    bool is_deleted() const { return slashed_out; }

    bool has_line_number() const {
      return has_line_no;
    }

    int line_number() const {
      assert(has_line_number());
      return line_no;
    }

    std::vector<chunk*>::const_iterator begin() const { return std::begin(chunks); }
    std::vector<chunk*>::const_iterator end() const { return std::end(chunks); }

    std::vector<chunk*>::iterator begin() { return std::begin(chunks); }
    std::vector<chunk*>::iterator end() { return std::end(chunks); }
    
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
  
}
