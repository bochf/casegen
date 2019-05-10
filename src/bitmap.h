#ifndef __BITMAP_H__
#define __BITMAP_H__

#define MARKER_BLOCK_BITS 8

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <vector>

class BitMap {
 public:
  static inline size_t offset(const size_t n) {
    return (n / MARKER_BLOCK_BITS);
  };
  static inline unsigned char bits(const size_t n) {
    return (n % MARKER_BLOCK_BITS);
  };

  static void mem_dump(void * mem, const size_t len) {
    unsigned char * p = static_cast<unsigned char *>(mem);
    for (size_t i = 0; i < len; ++i) {
      std::cout << " " << std::hex << p + i;
    }
    std::cout << "\n";
  };

  inline BitMap(const size_t size) : m_size(size) {
    m_table_len = (m_size + MARKER_BLOCK_BITS - 1) / MARKER_BLOCK_BITS;
    m_bits = new unsigned char[m_table_len];
    reset();
  }

  inline BitMap(const BitMap & bm) {
    m_bits = NULL;
    copy(bm);
  };

  inline virtual ~BitMap() {
    if (m_bits) {
      delete []m_bits;
    }
  };

  // set all bits to 1
  inline virtual void set() {
    memset(m_bits, 0xff, m_table_len);
  }

  // set the bit to 1 at pos
  inline virtual void set(const size_t pos) {
    if (pos < m_size) {
      m_bits[offset(pos)] |= (1 << bits(pos));
    }
  };

  // set all bits to 0
  inline virtual void reset() {
    memset(m_bits, 0, m_table_len);
  };

  // set the bit to 0 at pos
  inline virtual void reset(const size_t pos) {
    if (pos < m_size) {
      m_bits[offset(pos)] &= ~(1 << bits(pos));
    }
  };

  // get the val at pos
  inline virtual bool get(const size_t pos) const {
    if (pos < m_size) {
      unsigned char mask = 1 << bits(pos);
      return (mask == (m_bits[offset(pos)] & mask));
    }

    return false;
  };

  inline virtual bool operator[] (const size_t pos) const {
    return get(pos);
  };

  // the capacity of bit map
  inline virtual size_t size() const {
    return m_size;
  };

  // all the bits are 0
  inline virtual bool all0() const {
    unsigned char c = 0;
    for (size_t i = 0; i < offset(m_size); ++i) {
      if (m_bits[i] ^ 0) {
        return false;
      }
    }

    // if has remains
    if (bits(m_size) != 0) {
      for (size_t i = bits(m_size); i > 0; --i) {
        c = (m_bits[m_table_len - 1] >> i - 1) & 1;
        if (c ^ 0) {
          return false;
        }
      }
    }

    return true;
  };

  // all the bits are 1
  inline virtual bool all1() const {
    unsigned char c = 0xff;
    for (size_t i = 0; i < offset(m_size); ++i) {
      if (m_bits[i] ^ 0xff) {
        return false;
      }
    }

    // if has remains
    if (bits(m_size) != 0) {
      for (size_t i = bits(m_size); i > 0; --i) {
        c = (m_bits[m_table_len - 1] >> i - 1) & 1;
        if (c ^ 1) {
          return false;
        }
      }
    }

    return true;
  };

  // assignment
  inline BitMap & operator= (const BitMap & bm) {
    copy(bm);
    return *this;
  }

  // compare
  inline bool operator==(const BitMap & bm) const {
    if (m_size != bm.m_size) {
      return false;
    }
    for (size_t i = 0; i < m_table_len; ++i) {
      if (m_bits[i] ^ bm.m_bits[i]) {
        return false;
      }
    }
    return true;
  }
  inline bool operator!=(const BitMap & bm) const {
    return !(*this == bm);
  }

  inline bool operator>(const BitMap & bm) const {
    if (*this == bm) {
      return false;
    }
    return (*this >= bm);
  }
  inline bool operator<(const BitMap & bm) const {
    if (*this == bm) {
      return false;
    }
    return (*this <= bm);
  }

  inline bool operator>=(const BitMap & bm) const {
    if (m_size >= bm.m_size && m_table_len >= bm.m_table_len) {
      for (size_t i = 0; i < bm.m_table_len; ++i)
        if (m_bits[i] ^ (m_bits[i] | bm.m_bits[i])) {
          return false;
        }
      return true;
    }
    return false;
  }
  inline bool operator<=(const BitMap & bm) const {
    if (m_size <= bm.m_size && m_table_len <= bm.m_table_len) {
      for (size_t i = 0; i < m_table_len; ++i)
        if (bm.m_bits[i] ^ (m_bits[i] | bm.m_bits[i])) {
          return false;
        }
      return true;
    }
    return false;
  }

  // 2 bit maps AND
  inline BitMap & operator& (const BitMap & bm) const {
    BitMap * value = new BitMap(m_size);
    size_t i = 0;
    while (i < m_table_len && i < bm.m_table_len) {
      value->m_bits[i] = m_bits[i] & bm.m_bits[i];
      ++i;
    }
    if (m_table_len > bm.m_table_len)
      while (i++ < m_table_len) {
        value->m_bits[i] = m_bits[i] & bm.m_bits[i];
      }

    return *value;
  };
  inline BitMap & operator&= (const BitMap & bm) {
    size_t i = 0;
    while (i < m_table_len && i < bm.m_table_len) {
      m_bits[i] &= bm.m_bits[i];
      ++i;
    }
    return *this;
  }

  // 2 bit maps OR
  inline BitMap & operator| (const BitMap & bm) const {
    BitMap * value = new BitMap(m_size);
    size_t i = 0;
    while (i < m_table_len && i < bm.m_table_len) {
      value->m_bits[i] = m_bits[i] | bm.m_bits[i];
      ++i;
    }
    if (m_table_len > bm.m_table_len)
      while (i++ < m_table_len) {
        value->m_bits[i] = m_bits[i] | bm.m_bits[i];
      }

    return *value;
  }
  inline BitMap & operator|= (const BitMap & bm) {
    for (size_t i = 0; i < m_table_len && i < bm.m_table_len; ++i) {
      m_bits[i] |= bm.m_bits[i];
    }

    return *this;
  };

  // 2 bit maps XOR
  inline BitMap & operator^ (const BitMap & bm) const {
    BitMap * value = new BitMap(m_size);
    size_t i = 0;
    while (i < m_table_len && i < bm.m_table_len) {
      value->m_bits[i] = m_bits[i] ^ bm.m_bits[i];
      ++i;
    }
    if (m_table_len > bm.m_table_len)
      while (i++ < m_table_len) {
        value->m_bits[i] = m_bits[i] ^ bm.m_bits[i];
      }

    return *value;
  };
  inline BitMap & operator^= (const BitMap & bm) {
    size_t i = 0;
    while (i < m_table_len && i < bm.m_table_len) {
      m_bits[i] ^= bm.m_bits[i];
      ++i;
    }
    return *this;
  }

  inline void diff(const BitMap & bm, std::vector<size_t> & diff) const {
    if (m_size != bm.m_size) { // only comparable on the same size
      throw std::logic_error("different size bitmaps are not comparable");
    }

    diff.clear();
    BitMap bm_diff = *this ^ bm;
    for (size_t i = 0; i < m_table_len; ++i) {
      unsigned char c = bm_diff.m_bits[i];
      for (size_t j = 0; j < MARKER_BLOCK_BITS; ++j) {
        if (((c >> j) & 1) == 1) {
          diff.push_back(i * MARKER_BLOCK_BITS + j);
        }
      }
    }
  }

 private:
  inline BitMap & copy(const BitMap & bm) {
    m_size = bm.m_size;
    m_table_len = bm.m_table_len;
    if (m_bits) {
      delete []m_bits;
    }
    m_bits = new unsigned char[m_table_len];
    memcpy(m_bits, bm.m_bits, m_table_len);
    return *this;
  };

  BitMap();
  size_t m_size;
  size_t m_table_len;
  unsigned char * m_bits;

  friend class BitMap2;
};

// 2D bit map
class BitMap2 {
 public:
  // constructor
  inline BitMap2(const size_t row, const size_t col) : m_row(row), m_col(col) {
    m_bitmap = new BitMap(row * col);
  };
  inline BitMap2(const BitMap2 & bm) : m_row(bm.m_row), m_col(bm.m_col) {
    m_bitmap = NULL;
    copy(bm);
  };

  // destructor
  inline ~BitMap2() {
    if (m_bitmap) {
      delete m_bitmap;
    }
  };

  // read-write
  inline virtual void set() {
    m_bitmap->set();
  };
  inline virtual void set(const size_t i, const size_t j) {
    if (i < m_row && j < m_col) {
      m_bitmap->set(i * m_col + j);
    }
  };
  inline virtual void reset() {
    m_bitmap->reset();
  };
  inline virtual void reset(const size_t i, const size_t j) {
    if (i < m_row && j < m_col) {
      m_bitmap->reset(i * m_col + j);
    }
  };
  inline virtual size_t size() const {
    return m_bitmap->size();
  };
  inline virtual size_t rows() const {
    return m_row;
  };
  inline virtual size_t cols() const {
    return m_col;
  };
  inline virtual bool get(const size_t i, const size_t j) const {
    if (i < m_row && j < m_col) {
      return m_bitmap->get(i * m_col + j);
    }
    return false;
  };
  inline virtual const BitMap & operator[] (const size_t row) const {
    return get_row(row);
  };

  // assignment
  inline virtual BitMap2 & operator= (const BitMap2 & bm) {
    return copy(bm);
  };

  // compare
  inline virtual bool all0() const {
    return m_bitmap->all0();
  };
  inline virtual bool all1() const {
    return m_bitmap->all1();
  };
  inline virtual bool operator== (const BitMap2 & bm) const {
    if (m_row == bm.m_row && m_col == bm.m_col) {
      return *m_bitmap == *bm.m_bitmap;
    }
    return false;
  };
  inline virtual bool operator!= (const BitMap2 & bm) const {
    return !(*this == bm);
  };
  inline virtual bool operator> (const BitMap2 & bm) const {
    if (m_row < bm.m_row || m_col < bm.m_col) {
      return false;
    }
    return ((*this >= bm) && (*this != bm));
  };
  inline virtual bool operator< (const BitMap2 & bm) const {
    if (m_row > bm.m_row || m_col > bm.m_col) {
      return false;
    }
    return ((*this <= bm) && (*this != bm));
  };
  inline virtual bool operator>= (const BitMap2 & bm) const {
    if (m_row < bm.m_row || m_col < bm.m_col) {
      return false;
    }

    size_t row = 0;
    while(row < bm.m_row && get_row(row) >= bm[row]) {
      ++row;
    }

    return (row == bm.m_row);
  };
  inline virtual bool operator<= (const BitMap2 & bm) const {
    if (m_row > bm.m_row || m_col > bm.m_col) {
      return false;
    }

    size_t row = 0;
    while(row < bm.m_row && get_row(row) <= bm[row]) {
      ++row;
    }

    return (row == bm.m_row);
  };

  // manipulate
  inline virtual BitMap2 & operator& (const BitMap2 & bm) const {
    if (m_row != bm.m_row || m_col != bm.m_col) {
      throw std::out_of_range("can't compare bit map with different size");
    }

    BitMap2 * value = new BitMap2(m_row, m_col);
    for (size_t i = 0; i < m_bitmap->m_table_len; ++i) {
      value->m_bitmap->m_bits[i] = m_bitmap->m_bits[i] & bm.m_bitmap->m_bits[i];
    }

    return *value;
  };
  inline virtual BitMap2 & operator&= (const BitMap2 & bm) {
    if (m_row != bm.m_row || m_col != bm.m_col) {
      throw std::out_of_range("can't compare bit map with different size");
    }

    for (size_t i = 0; i < m_bitmap->m_table_len; ++i) {
      m_bitmap->m_bits[i] &= bm.m_bitmap->m_bits[i];
    }

    return *this;
  };
  inline virtual BitMap2 & operator| (const BitMap2 & bm) const {
    if (m_row != bm.m_row || m_col != bm.m_col) {
      throw std::out_of_range("can't compare bit map with different size");
    }

    BitMap2 * value = new BitMap2(m_row, m_col);
    for (size_t i = 0; i < m_bitmap->m_table_len; ++i) {
      value->m_bitmap->m_bits[i] = m_bitmap->m_bits[i] | bm.m_bitmap->m_bits[i];
    }

    return *value;
  };
  inline virtual BitMap2 & operator|= (const BitMap2 & bm) {
    if (m_row != bm.m_row || m_col != bm.m_col) {
      throw std::out_of_range("can't compare bit map with different size");
    }

    for (size_t i = 0; i < m_bitmap->m_table_len; ++i) {
      m_bitmap->m_bits[i] |= bm.m_bitmap->m_bits[i];
    }

    return *this;
  };

 private:
  inline BitMap2 & copy(const BitMap2 & bm) {
    m_row = bm.m_row;
    m_col = bm.m_col;
    if (m_bitmap) {
      delete m_bitmap;
    }
    m_bitmap = new BitMap(*bm.m_bitmap);
    return *this;
  }

  inline virtual const BitMap & get_row(const size_t row) const {
    if (row >= m_row) {
      throw std::out_of_range("2D bit map line access out of range");
    }

    // get one line from a 2 dimention bit map
    // tricky here is the bits saved in the 2D bit map is not row aligned
    // so we need to get the value bit by bit
    size_t start_bit = row * m_col;
    size_t start_block = BitMap::offset(start_bit);
    size_t moving_bits = BitMap::bits(start_bit);

    // create a new bit map
    BitMap * bm = new BitMap(m_col);
    // copy bit blocks from bitmap2 to bitmap memory
    memcpy(bm->m_bits, m_bitmap->m_bits + start_block, bm->m_table_len);
    // if not block aligned, do bit moving
    if (moving_bits > 0) {
      size_t i = 0;
      while (i < bm->m_table_len - 1) {
        // move the original bits left
        bm->m_bits[i] = bm->m_bits[i] << moving_bits;
        // move the rest bits from next block to the current block
        unsigned char c = bm->m_bits[i + 1] >> (MARKER_BLOCK_BITS - moving_bits);
        bm->m_bits[i] |= c;
        ++i;
      }
      // deal the last block
      unsigned char c = m_bitmap->m_bits[start_block + 1];
      c = c >> (MARKER_BLOCK_BITS - moving_bits);
      bm->m_bits[i] |= c;
    }

    return *bm;
  };

  BitMap2();
  size_t m_row;
  size_t m_col;
  BitMap * m_bitmap;
};

#endif
