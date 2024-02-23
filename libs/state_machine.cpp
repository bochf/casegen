#include "state_machine.h"
#include "bitmap.h"
#include "graph.h"
#include "matrix.h"
#include "traveller.h"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Graph *StateMachine::generate(const size_t rows, const size_t cols,
                              size_t *states[]) {
  if (rows == 0 || cols == 0 || states == NULL) {
    return NULL;
  }

  vector<BitMap *> states_attr;
  m_StateEvent.init(rows, cols);

  // create all states
  bool error = false;
  for (size_t i = 0; i < rows && !error; ++i) {
    BitMap *attr = new BitMap(cols);
    for (size_t j = 0; j < cols && !error; ++j) {
      switch (states[i][j]) {
      case 0:
        attr->reset(j);
        break;
      case 1:
        attr->set(j);
        break;
      default: // only accept 0 or 1 as input
        error = true;
        break;
      }
    }
    states_attr.push_back(attr);
  }
  if (error) {
    for (size_t i = 0; i < states_attr.size(); ++i) {
      delete states_attr[i];
    }
    states_attr.clear();
    return NULL;
  }

  // detect possible connections
  // definition: we use bit map to describe a state
  // one state is a combination of several attributes
  // each attributes takes vaule 1 or 0 as feature turns on or off
  // the possible onnections between 2 states is one bit difference on the
  // feature sets that means it is switchable by one stop to turn on/off one
  // feature
  vector<size_t> attr_diff;
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < rows; ++j) {
      if (i == j) {
        continue;
      }
      states_attr[i]->diff(*states_attr[j], attr_diff);
      if (attr_diff.size() == 1) {
        EDGE_TYPE edge_type =
            attr_diff[0] +
            static_cast<size_t>(states_attr[i]->get(attr_diff[0])) * cols;
        m_StateEvent.link(i, j, edge_type);
        cout << "add link " << i << ", " << j << '\n';
      }
    }
  }

  m_StateEvent.scan();

  return &m_StateEvent;
}

Graph *StateMachine::generate(const string &state_file) {
  ifstream fs(state_file.c_str());
  if (!fs.good()) {
    cerr << "open file " << state_file << " error" << '\n';
    return NULL;
  }

  Matrix<size_t> matrix;
  matrix.read(fs);
  fs.close();

  matrix.write(cout);

  return generate(matrix.rows(), matrix.cols(), matrix.buf());
}

void StateMachine::load(const string &matrix_file) {
  m_StateEvent.init(matrix_file);
}

string StateMachine::cases() {
  if (NULL == m_pTrasition) {
    return "";
  }

  if (m_pTrasition->algorithm() == IGraphTraveller::GT_EULER &&
      !m_StateEvent.eulerian()) {
    m_StateEvent.eulerize();
  }

  string trace;
  m_pTrasition->travel(m_StateEvent, trace);
  return trace;
  // return m_pTrasition->print();
}

void StateMachine::configure(const Properties &config) {
  IGraphTraveller::GT_ALGORITHM algorithm = IGraphTraveller::GT_BFS;
  Properties::const_iterator const it = config.find("ALGORITHM");
  if (it != config.end()) {
    algorithm = static_cast<IGraphTraveller::GT_ALGORITHM>(it->second);
  }

  if ((m_pTrasition != nullptr) && m_pTrasition->algorithm() != algorithm) {
    m_pTrasition = nullptr;
  }
  if (NULL == m_pTrasition) {
    m_pTrasition = IGraphTraveller::createInstance(algorithm);
  }

  if (m_pTrasition != nullptr) {
    m_pTrasition->configure(config);
  }
}
