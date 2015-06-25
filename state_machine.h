/**
 *
 */

#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include "bitmap.h"
#include "graph.h"
#include "traveller.h"

#include <vector>
#include <string>

using namespace std;

class StateMachine {
 public:
  StateMachine() : m_pTrasition(NULL) {
  };

  virtual Graph * generate(const string & state_file);
  virtual Graph * load(const string & state_file);

	virtual string cases();

	virtual void configure(const Properties & config);

	inline virtual size_t size() const { return m_StateEvent.size(); };

 private:
  Graph * generate(const size_t rows, const size_t cols, size_t * states[]);
  Graph m_StateEvent;
  IGraphTraveller * m_pTrasition;
};

#endif
