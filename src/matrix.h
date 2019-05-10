#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

#define MAX_MATRIX_ROWS 16386
#define MAX_MATRIX_COLS 1024

template <class _T>
class Matrix {
 public:
  Matrix() : m_buf(NULL), m_rows(0), m_cols(0) {
  };
  Matrix(const size_t rows, const size_t cols) :
    m_rows(rows), m_cols(cols) {
    m_buf = new _T*[rows];
    for (size_t i = 0; i < rows; ++i) {
      m_buf[i] = new _T[cols];
    }
  };
  virtual ~Matrix() {
    clear();
  };

  inline size_t rows() const {
    return m_rows;
  };

  inline size_t cols() const {
    return m_cols;
  }

  inline size_t size() const {
    return m_rows * m_cols;
  };

  inline _T ** buf() const {
    return m_buf;
  };

  inline size_t readline(const string & line, const size_t cols) {
    stringstream ss(line);
    size_t elements = 0;
    m_buf[m_rows] = new _T[cols];
    while (ss.good() && elements < cols) {
      ss >> m_buf[m_rows][elements++];
    }
    if (ss.good()) {
      clear();
      throw std::overflow_error("matrix columns overflow");
    }

    ++m_rows;

    return elements;
  }

  inline void read(istream & is) {
    if (!is.good()) {
      return;
    }

    clear();
    m_buf = new _T*[MAX_MATRIX_ROWS];

    string line;
    // get first line elements, dtermine the columns
    getline(is, line);
    m_cols = readline(line, MAX_MATRIX_COLS);

    // get remaining lines
    while (is.good()) {
      getline(is, line);
      if (is.eof()) {
        break;
      }

      if (m_cols != readline(line, m_cols)) {
        clear();
        throw std::logic_error("asymmetric matrix detected");
      }

      if (m_rows > MAX_MATRIX_ROWS) {
        cerr << "input file is too big, stop at line" << m_rows << endl;
        return;
      }
    }
  }

  inline void write(ostream & os) {
    if (m_rows > 0 && m_cols > 0) {
      for (size_t i = 0; i < m_rows; ++i) {
        for (size_t j = 0; j < m_cols - 1; ++j) {
          os << m_buf[i][j] << " ";
        }
        os << m_buf[i][m_cols - 1] << endl;
      }
    }
  };

 private:
  void clear() {
    m_rows = 0;
    m_cols = 0;
    if (m_buf) {
      delete []*m_buf;
      delete []m_buf;
    }
  };

  _T ** m_buf;
  size_t m_rows;
  size_t m_cols;
};

#endif
