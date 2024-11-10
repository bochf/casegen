/**
 *
 */

#ifndef CASEGEN_STATE_MACHINE_H_
#define CASEGEN_STATE_MACHINE_H_

#include "bitmap.h"
#include "graph.h"
#include "traveller.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class StateMachine {
public:
  Graph *generate(const std::string &state_file);
  void load(const std::string &state_file);

  std::string cases();

  void configure(const Properties &config);

  size_t size() const { return m_stateGraph.size(); };

private:
  Graph *generate(size_t rows, size_t cols, size_t *states[]);
  Graph m_stateGraph;
  std::shared_ptr<IGraphTraveller> m_pTrasition = nullptr;
};

#endif
