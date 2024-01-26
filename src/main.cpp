#include "graph.h"
#include "traveller.h"
#include "state_machine.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

//#include "platform/CmConfigInitFile.h"

using namespace std;

void printHelp() {
  cout << "Case Generation Tool (CGT)" << endl;
  cout << "Usage: casegen [options]" << endl;
  cout << "Options:" << endl;
  cout << "  -h, --help       " << "Display this information" << endl;
  cout << "  -s strategy      " << "The strategy to be used to generate the test cases" << endl;
  cout << "                   " << "Available strategies include:" << endl;
  cout << "                   " << "  node: generate one case from start to end state" << endl;
  cout << "                   " << "  path: generate all the cases until all transitions are covered" << endl;
  cout << "                   " << "  euler: cover all cases by one sequence" << endl;
  cout << "  -o start         " << "The original state the test case start from, default: 0" << endl;
  cout << "  -e end           " << "The termination state the test case end with, default: 0" << endl;
  cout << "  -f file          " << "The input file of the state machine, default: m.txt" << endl;
  cout << "  --random         " << "Generating cases random-walking" << endl;
  cout << "  --dump           " << "Print out the graph" << endl;
  cout << "  --gensm          " << "Generating state machine from state list" << endl;
  cout << "  --sf file        " << "The input file of the state list" << endl;
};

int main(int argc, char * argv[]) {
  //CCmConfigInitFile::SetWebexHomeEnv("HOME"); // set the home to user home dir
  //CCmConfigInitFile::SetWebexConfigFileName(".casegen");

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
    if (string("-s").compare(argv[i]) == 0) {
      if (i < argc) {
        strStrategy = string(argv[++i]);
      }
      continue;
    }

    if (string("-o").compare(argv[i]) == 0) {
      if (i < argc) {
        start = string(argv[++i]);
      }
      continue;
    }

    if (string("-e").compare(argv[i]) == 0) {
      if (i < argc) {
        end = string(argv[++i]);
      }
      continue;
    }

    if (string("-d").compare(argv[i]) == 0) {
      if (i < argc) {
        max_depth = atoi(argv[++i]);
      }
      continue;
    }

    if (string("-n").compare(argv[i]) == 0) {
      if (i < argc) {
        max_cases = atoi(argv[++i]);
      }
      continue;
    }

    if (string("-f").compare(argv[i]) == 0) {
      if (i < argc) {
        strConfigFileName = string(argv[++i]);
      }
      continue;
    }

    if (string("--random").compare(argv[i]) == 0) {
      random = 1;
      srand((unsigned int)time(NULL));
      continue;
    }

    if (string("--help").compare(argv[i]) == 0 || string("-h").compare(argv[i]) == 0) {
      printHelp();
      return 0;
    }

    if (string("--dump").compare(argv[i]) == 0) {
      dump = true;
    }

    if (string("-sf").compare(argv[i]) == 0) {
      if (i < argc) {
        strStateFileName = string(argv[++i]);
				readFromStateFile = true;
      }
      continue;
    }
  }

  cout << "generating test cases:" << endl;
  cout << "\tinput file=" << strConfigFileName << endl;
  cout << "\tstrategy=" << strStrategy << endl;
  cout << "\tstart=" << start << endl;
  cout << "\tend=" << end << endl;
  cout << "\tmax depth=" << max_depth << endl;;
  cout << "\tmax cases=" << max_cases << endl;;

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

	StateMachine sm;
	if (readFromStateFile) {
		if (!sm.generate(strStateFileName))
			return -1;
	}
	else {
		if (!sm.load(strConfigFileName))
			return -1;
	}

  vector<VERTEX_ID> start_points;
  vector<VERTEX_ID> end_points;
  if (start == "any") {
		for (size_t i = 0; i < sm.size(); ++i) start_points.push_back(i);
  } else {
    start_points.push_back(atoi(start.c_str()));
  }
  if (end == "any") {
		for (size_t i = 0; i < sm.size(); ++i) end_points.push_back(i);
  } else {
    end_points.push_back(atoi(end.c_str()));
  }

  for (size_t i = 0; i < start_points.size(); ++i)
    for (size_t j = 0; j < end_points.size(); ++j) {
      config["START"] = start_points[i];
      config["END"] = end_points[j];
      sm.configure(config);
			cout << sm.cases() << endl;
    }
  return 0;
}
