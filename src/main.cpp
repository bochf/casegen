#include "graph.h"
#include "state_machine.h"
#include "traveller.h"

#include <climits>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

//#include "platform/CmConfigInitFile.h"

using namespace std;

void printHelp() {
  cout << "Case Generation Tool (CGT)\n";
  cout << "Usage: casegen [options]\n";
  cout << "Options:\n";
  cout << "  -h, --help       "
       << "Display this information\n";
  cout << "  -s strategy      "
       << "The strategy to be used to generate the test cases\n";
  cout << "                   "
       << "Available strategies include:\n";
  cout << "                   "
       << "  node: generate one case from start to end state\n";
  cout << "                   "
       << "  path: generate all the cases until all transitions are covered\n";
  cout << "                   "
       << "  euler: cover all cases by one sequence\n";
  cout << "  -o start         "
       << "The original state the test case start from, default: 0\n";
  cout << "  -e end           "
       << "The termination state the test case end with, default: 0\n";
  cout << "  -f file          "
       << "The input file of the state machine, default: m.txt\n";
  cout << "  --random         "
       << "Generating cases random-walking\n";
  cout << "  --dump           "
       << "Print out the graph\n";
  cout << "  --gensm          "
       << "Generating state machine from state list\n";
  cout << "  --sf file        "
       << "The input file of the state list\n";
};

int main(int argc, char *argv[]) {
  string strConfigFileName = "m.txt";
  string strStateFileName = "s.txt";
  bool readFromStateFile = false;
  string strStrategy = "node";
  string start = "0";
  string end = "0";
  size_t max_depth = UINT_MAX;
  size_t max_cases = UINT_MAX;
  size_t random = 0;
  bool dump = false;

  for (int i = 1; i < argc; ++i) {
    if (string("-s") == argv[i]) {
      if (i < argc) {
        strStrategy = string(argv[++i]);
      }
      continue;
    }

    if (string("-o") == argv[i]) {
      if (i < argc) {
        start = string(argv[++i]);
      }
      continue;
    }

    if (string("-e") == argv[i]) {
      if (i < argc) {
        end = string(argv[++i]);
      }
      continue;
    }

    if (string("-d") == argv[i]) {
      if (i < argc) {
        max_depth = atoi(argv[++i]);
      }
      continue;
    }

    if (string("-n") == argv[i]) {
      if (i < argc) {
        max_cases = atoi(argv[++i]);
      }
      continue;
    }

    if (string("-f") == argv[i]) {
      if (i < argc) {
        strConfigFileName = string(argv[++i]);
      }
      continue;
    }

    if (string("--random") == argv[i]) {
      random = 1;
      srand((unsigned int)time(NULL));
      continue;
    }

    if (string("--help") == argv[i] || string("-h") == argv[i]) {
      printHelp();
      return 0;
    }

    if (string("--dump") == argv[i]) {
      dump = true;
    }

    if (string("-sf") == argv[i]) {
      if (i < argc) {
        strStateFileName = string(argv[++i]);
        readFromStateFile = true;
      }
      continue;
    }
  }

  cout << "generating test cases:";
  cout << "\n\tinput file=" << strConfigFileName;
  cout << "\n\tstrategy=" << strStrategy;
  cout << "\n\tstart=" << start;
  cout << "\n\tend=" << end;
  cout << "\n\tmax depth=" << max_depth;
  cout << "\n\tmax cases=" << max_cases << "\n";

  /*
if (gensm) {
StateMachine sm;
Graph * g = sm.generate(strStateFileName);
g->dump();
}

Graph g;
g.init(strConfigFileName);
if (!g.good()) {
cerr << "invalid input " << strConfigFileName << endl;
return -1;
}
if (dump) {
g.dump();
return 0;
}
  */

  Properties config;
  config["ALGORITHM"] = IGraphTraveller::GT_BFS_ONE;
  if (strStrategy == "node") {
    config["ALGORITHM"] = IGraphTraveller::GT_BFS_ONE;
  }
  if (strStrategy == "path") {
    config["ALGORITHM"] = IGraphTraveller::GT_DFS_PATH;
  }
  if (strStrategy == "all") {
    config["ALGORITHM"] = IGraphTraveller::GT_BFS_ALL;
  }
  if (strStrategy == "euler") {
    config["ALGORITHM"] = IGraphTraveller::GT_EULER;
  }

  config["MAX_DEPTH"] = max_depth;
  config["MAX_CASES"] = max_cases;
  config["RANDOM_WALK"] = random;

  StateMachine stateMachine;
  if (readFromStateFile) {
    stateMachine.generate(strStateFileName);
  } else {
    stateMachine.load(strConfigFileName);
  }

  vector<VERTEX_ID> start_points;
  vector<VERTEX_ID> end_points;
  if (start == "any") {
    for (size_t i = 0; i < stateMachine.size(); ++i) {
      start_points.push_back(i);
    }
  } else {
    start_points.push_back(atoi(start.c_str()));
  }
  if (end == "any") {
    for (size_t i = 0; i < stateMachine.size(); ++i) {
      end_points.push_back(i);
    }
  } else {
    end_points.push_back(atoi(end.c_str()));
  }

  for (const auto start : start_points) {
    for (const auto end : end_points) {
      config["START"] = start;
      config["END"] = end;
      stateMachine.configure(config);
      cout << stateMachine.cases() << '\n';
    }
  }
  return 0;
}
