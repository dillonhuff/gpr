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
    virtual ~address() {}
  };

  class int_address : public address {
  protected:
    const int val;

  public:
    int_address(const int p_val) : val(p_val) {}

    int value() const { return val; }

    address_type tp() const { return ADDRESS_TYPE_INTEGER; }

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

    double value() const { return val; }

    address_type tp() const { return ADDRESS_TYPE_DOUBLE; }

    virtual bool equals(const address& other) const {
      assert(false);
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
      
    virtual bool equals(const chunk& other) const {
      if (other.tp() != CHUNK_TYPE_WORD_ADDRESS) {
	return false;
      }

      return true;
    }

    void print(std::ostream& stream) const {
      stream << comment_text << std::endl;
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

    virtual chunk_type tp() const { return CHUNK_TYPE_WORD_ADDRESS; }

    void print(std::ostream& stream) const {
      //stream << "a";
    }

    bool equals(const chunk& other) const {
      if (other.tp() != CHUNK_TYPE_WORD_ADDRESS) {
	return false;
      }

      const word_address& other_addr = static_cast<const word_address&>(other);
      return (wd == other_addr.wd) && (addr->equals(*(other_addr.addr)));
    }

  };

  word_address* make_word_int(const char c, const int i);

  bool operator==(const chunk& l, const chunk& r);
  bool operator!=(const chunk& l, const chunk& r);

  class block {
  protected:
    std::vector<chunk*> chunks;

  public:
    block(const std::vector<chunk*> p_chunks) :
      chunks(p_chunks) {}

    int size() const { return chunks.size(); }

    const chunk& get_chunk(const int i) {
      assert(i < size());

      return *(chunks[i]);
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

}
