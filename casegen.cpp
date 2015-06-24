#include "graph.h"
#include "traveller.h"

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
  cout << "                   " << "  one: generate one case from start to end state" << endl;
  cout << "                   " << "  path: generate all the cases until all transitions are covered" << endl;
  cout << "  -o start         " << "The original state the test case start from, default: 0" << endl;
  cout << "  -e end           " << "The termination state the test case end with, default: 0" << endl;
  cout << "  -f file          " << "The input file of the state machine, default: m.txt" << endl;
  cout << "  --random         " << "Generating cases random-walking" << endl;
};

int main(int argc, char * argv[]) {
  //CCmConfigInitFile::SetWebexHomeEnv("HOME"); // set the home to user home dir
  //CCmConfigInitFile::SetWebexConfigFileName(".casegen");

  string strConfigFileName = "m.txt";
  string strStrategy = "all";
  string start = "0";
  string end = "0";
  size_t max_depth = UINT_MAX;
  size_t max_cases = UINT_MAX;
  size_t random = 0;

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
  }

  cout << "generating test cases:" << endl;
  cout << "\tinput file=" << strConfigFileName << endl;
  cout << "\tstrategy=" << strStrategy << endl;
  cout << "\tstart=" << start << endl;
  cout << "\tend=" << end << endl;
  cout << "\tmax depth=" << max_depth << endl;;
  cout << "\tmax cases=" << max_cases << endl;;

  Graph g;
  g.init(strConfigFileName);
  if (!g.good()) {
    cerr << "invalid input " << strConfigFileName << endl;
    return -1;
  }
  // g.dump();

  IGraphTraveller::GT_ALGORITHM algorithm = IGraphTraveller::GT_BFS_ALL;
  if (strStrategy == "one") {
    algorithm = IGraphTraveller::GT_BFS_ONE;
  }
  if (strStrategy == "path") {
    algorithm = IGraphTraveller::GT_DFS_PATH;
  }

  IGraphTraveller * pTraveller = IGraphTraveller::createInstance(algorithm);
  Properties config;
  config["MAX_DEPTH"] = max_depth;
  config["MAX_CASES"] = max_cases;
  config["RANDOM_WALK"] = random;

  vector<VERTEX_ID> start_points;
  vector<VERTEX_ID> end_points;
  if (start == "any") {
    for (VertexList::const_iterator it = g.getVertices().begin(); it != g.getVertices().end(); ++it) {
      start_points.push_back((*it)->id);
    }
  } else {
    start_points.push_back(atoi(start.c_str()));
  }
  if (end == "any") {
    for (VertexList::const_iterator it = g.getVertices().begin(); it != g.getVertices().end(); ++it) {
      end_points.push_back((*it)->id);
    }
  } else {
    end_points.push_back(atoi(end.c_str()));
  }

  for (size_t i = 0; i < start_points.size(); ++i)
    for (size_t j = 0; j < end_points.size(); ++j) {
      config["START"] = start_points[i];
      config["END"] = end_points[j];
      pTraveller->configure(config);
      pTraveller->travel(g);
    }
  return 0;
}
