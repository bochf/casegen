#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <list>
#include <set>
#include <iostream>

using namespace std;
class Graph {
 private:
  map<std::string, std::set<string>* > graph;

 public:
  void addEdge(string node1, string node2) {
    std::map<string, std::set<string>*>::iterator it = graph.find(node1);

    if(it == graph.end()) {
      std::pair<string, std::set<string>*> verticeInfo(node1, new std::set<string>);
      graph.insert(verticeInfo);
      it = graph.find(node1);
    }
    it->second->insert(node2);
  }

  void addTwoWayVertex(string & node1, string & node2) {
    addEdge(node1, node2);
    addEdge(node2, node1);
  }

  /*
  bool isConnected(string & node1, string & node2) {
    std::map<string, std::set<string>*>::iterator adjIterator = graph.find(node1);
    if(adjIterator == graph.end()) {
      return false;
    }
    return (adjIterator->second->find(node2) != adjIterator->second->end());
  }
  */

  std::set<string> * adjacentNodes(string & last) {
    std::map<string, std::set<string>*>::iterator adjacentSet = graph.find(last);
    if(adjacentSet == graph.end()) {
      return NULL;
    }
    return adjacentSet->second;
  }
};


string start = "B";
string dest = "E";

void track(Graph & graph, list<string> & visited) {
  cout << "BFS:: visited nodes:";
  for (list<string>::iterator visited_node = visited.begin(); visited_node != visited.end(); ++visited_node) {
    cout << " " << *visited_node;
  }
  cout << endl;

  /*
  string current_node = visited.back();
  cout << "BFS: start node=" << current_node << ", adjacencies=";
  set<string> nodes = *(graph.adjacentNodes(current_node));
  for (std::set<string>::iterator node = nodes.begin(); node != nodes.end() ; ++node) {
    if (std::find(visited.begin(), visited.end(), *node) != visited.end()) {
      cout << " !" << *node;
    }
    else {
      cout << " " << *node;
    }
  }
  cout << endl;
  */
}

void printPath(std::list<string> & visited) {
  cout << "== path == ";
  for (std::list<string>::iterator node = visited.begin(); node != visited.end() ; ++node) {
    cout << *node << " ";

  }
  cout << endl;
};

void breadthFirst(Graph & graph, std::list<string> & visited) {
  std::set<string> nodes = *(graph.adjacentNodes(visited.back()));
  // examine adjacent nodes
  for (std::set<string>::iterator node = nodes.begin(); node != nodes.end() ; ++node) {
    if ((node->compare(start) != 0) &&
        (std::find(visited.begin(), visited.end(), *node) != visited.end())) {
      continue;
    }
    if (node->compare(dest) == 0) {
      visited.push_back(*node);
      // track(graph, visited);
      cout << endl;
      printPath(visited);
      visited.pop_back();
      // track(graph, visited);
      break;
    }
  }
  // cout << endl;
  // in breadth-first, recursion needs to come after visiting adjacent nodes
  for (std::set<string>::iterator node = nodes.begin(); node != nodes.end() ; ++node) {
    if ((std::find(visited.begin(), visited.end(), *node) != visited.end()) || (node->compare(dest) == 0)) {
      continue;
    }
    visited.push_back(*node);
    // track(graph, visited);
    breadthFirst(graph, visited);
    visited.pop_back();
    // track(graph, visited);
  }
};

int main(int argc, char * argv[]) {
  Graph graph;
  graph.addEdge("S0", "S1");
  graph.addEdge("S0", "S16");
  graph.addEdge("S1", "S0");
  graph.addEdge("S1", "S35");
  graph.addEdge("S1", "S5");
  graph.addEdge("S1", "S5");
  graph.addEdge("S1", "S2");
  graph.addEdge("S2", "S0");
  graph.addEdge("S2", "S36");
  graph.addEdge("S2", "S6");
  graph.addEdge("S2", "S6");
  graph.addEdge("S2", "S1");
  graph.addEdge("S2", "S2");
  graph.addEdge("S2", "S2");
  graph.addEdge("S2", "S2");
  graph.addEdge("S3", "S0");
  graph.addEdge("S3", "S37");
  graph.addEdge("S3", "S3");
  graph.addEdge("S3", "S3");
  graph.addEdge("S3", "S5");
  graph.addEdge("S3", "S1");
  graph.addEdge("S3", "S4");
  graph.addEdge("S4", "S0");
  graph.addEdge("S4", "S38");
  graph.addEdge("S4", "S4");
  graph.addEdge("S4", "S4");
  graph.addEdge("S4", "S6");
  graph.addEdge("S4", "S2");
  graph.addEdge("S4", "S3");
  graph.addEdge("S4", "S4");
  graph.addEdge("S4", "S4");
  graph.addEdge("S4", "S4");
  graph.addEdge("S5", "S0");
  graph.addEdge("S5", "S39");
  graph.addEdge("S5", "S5");
  graph.addEdge("S5", "S5");
  graph.addEdge("S5", "S3");
  graph.addEdge("S5", "S1");
  graph.addEdge("S5", "S6");
  graph.addEdge("S6", "S0");
  graph.addEdge("S6", "S40");
  graph.addEdge("S6", "S6");
  graph.addEdge("S6", "S6");
  graph.addEdge("S6", "S4");
  graph.addEdge("S6", "S2");
  graph.addEdge("S6", "S5");
  graph.addEdge("S6", "S6");
  graph.addEdge("S6", "S6");
  graph.addEdge("S6", "S6");
  graph.addEdge("S7", "S0");
  graph.addEdge("S7", "S59");
  graph.addEdge("S7", "S11");
  graph.addEdge("S7", "S11");
  graph.addEdge("S7", "S8");
  graph.addEdge("S8", "S0");
  graph.addEdge("S8", "S60");
  graph.addEdge("S8", "S12");
  graph.addEdge("S8", "S12");
  graph.addEdge("S8", "S7");
  graph.addEdge("S8", "S8");
  graph.addEdge("S8", "S8");
  graph.addEdge("S8", "S8");
  graph.addEdge("S9", "S0");
  graph.addEdge("S9", "S61");
  graph.addEdge("S9", "S9");
  graph.addEdge("S9", "S9");
  graph.addEdge("S9", "S11");
  graph.addEdge("S9", "S7");
  graph.addEdge("S9", "S10");
  graph.addEdge("S10", "S0");
  graph.addEdge("S10", "S62");
  graph.addEdge("S10", "S10");
  graph.addEdge("S10", "S10");
  graph.addEdge("S10", "S12");
  graph.addEdge("S10", "S8");
  graph.addEdge("S10", "S9");
  graph.addEdge("S10", "S10");
  graph.addEdge("S10", "S10");
  graph.addEdge("S10", "S10");
  graph.addEdge("S11", "S0");
  graph.addEdge("S11", "S63");
  graph.addEdge("S11", "S11");
  graph.addEdge("S11", "S11");
  graph.addEdge("S11", "S9");
  graph.addEdge("S11", "S7");
  graph.addEdge("S11", "S12");
  graph.addEdge("S12", "S0");
  graph.addEdge("S12", "S64");
  graph.addEdge("S12", "S12");
  graph.addEdge("S12", "S12");
  graph.addEdge("S12", "S10");
  graph.addEdge("S12", "S8");
  graph.addEdge("S12", "S11");
  graph.addEdge("S12", "S12");
  graph.addEdge("S12", "S12");
  graph.addEdge("S12", "S12");
  graph.addEdge("S13", "S17");
  graph.addEdge("S13", "S41");
  graph.addEdge("S13", "S0");
  graph.addEdge("S13", "S15");
  graph.addEdge("S14", "S23");
  graph.addEdge("S14", "S47");
  graph.addEdge("S14", "S0");
  graph.addEdge("S14", "S16");
  graph.addEdge("S14", "S13");
  graph.addEdge("S15", "S29");
  graph.addEdge("S15", "S53");
  graph.addEdge("S15", "S0");
  graph.addEdge("S15", "S13");
  graph.addEdge("S16", "S35");
  graph.addEdge("S16", "S59");
  graph.addEdge("S16", "S0");
  graph.addEdge("S16", "S14");
  graph.addEdge("S16", "S15");
  graph.addEdge("S17", "S0");
  graph.addEdge("S17", "S1");
  graph.addEdge("S17", "S21");
  graph.addEdge("S17", "S21");
  graph.addEdge("S17", "S29");
  graph.addEdge("S17", "S18");
  graph.addEdge("S18", "S0");
  graph.addEdge("S18", "S2");
  graph.addEdge("S18", "S22");
  graph.addEdge("S18", "S22");
  graph.addEdge("S18", "S30");
  graph.addEdge("S18", "S17");
  graph.addEdge("S18", "S18");
  graph.addEdge("S18", "S18");
  graph.addEdge("S18", "S18");
  graph.addEdge("S19", "S0");
  graph.addEdge("S19", "S3");
  graph.addEdge("S19", "S19");
  graph.addEdge("S19", "S19");
  graph.addEdge("S19", "S21");
  graph.addEdge("S19", "S17");
  graph.addEdge("S19", "S31");
  graph.addEdge("S19", "S20");
  graph.addEdge("S20", "S0");
  graph.addEdge("S20", "S4");
  graph.addEdge("S20", "S20");
  graph.addEdge("S20", "S20");
  graph.addEdge("S20", "S22");
  graph.addEdge("S20", "S18");
  graph.addEdge("S20", "S32");
  graph.addEdge("S20", "S19");
  graph.addEdge("S20", "S20");
  graph.addEdge("S20", "S20");
  graph.addEdge("S20", "S20");
  graph.addEdge("S21", "S0");
  graph.addEdge("S21", "S5");
  graph.addEdge("S21", "S21");
  graph.addEdge("S21", "S21");
  graph.addEdge("S21", "S19");
  graph.addEdge("S21", "S17");
  graph.addEdge("S21", "S33");
  graph.addEdge("S21", "S22");
  graph.addEdge("S22", "S0");
  graph.addEdge("S22", "S6");
  graph.addEdge("S22", "S22");
  graph.addEdge("S22", "S22");
  graph.addEdge("S22", "S20");
  graph.addEdge("S22", "S18");
  graph.addEdge("S22", "S34");
  graph.addEdge("S22", "S21");
  graph.addEdge("S22", "S22");
  graph.addEdge("S22", "S22");
  graph.addEdge("S22", "S22");
  graph.addEdge("S23", "S0");
  graph.addEdge("S23", "S1");
  graph.addEdge("S23", "S27");
  graph.addEdge("S23", "S27");
  graph.addEdge("S23", "S35");
  graph.addEdge("S23", "S24");
  graph.addEdge("S23", "S17");
  graph.addEdge("S24", "S0");
  graph.addEdge("S24", "S2");
  graph.addEdge("S24", "S28");
  graph.addEdge("S24", "S28");
  graph.addEdge("S24", "S36");
  graph.addEdge("S24", "S23");
  graph.addEdge("S24", "S24");
  graph.addEdge("S24", "S24");
  graph.addEdge("S24", "S24");
  graph.addEdge("S24", "S18");
  graph.addEdge("S25", "S0");
  graph.addEdge("S25", "S3");
  graph.addEdge("S25", "S25");
  graph.addEdge("S25", "S25");
  graph.addEdge("S25", "S27");
  graph.addEdge("S25", "S23");
  graph.addEdge("S25", "S37");
  graph.addEdge("S25", "S26");
  graph.addEdge("S25", "S19");
  graph.addEdge("S26", "S0");
  graph.addEdge("S26", "S4");
  graph.addEdge("S26", "S26");
  graph.addEdge("S26", "S26");
  graph.addEdge("S26", "S28");
  graph.addEdge("S26", "S24");
  graph.addEdge("S26", "S38");
  graph.addEdge("S26", "S25");
  graph.addEdge("S26", "S26");
  graph.addEdge("S26", "S26");
  graph.addEdge("S26", "S26");
  graph.addEdge("S26", "S20");
  graph.addEdge("S27", "S0");
  graph.addEdge("S27", "S5");
  graph.addEdge("S27", "S27");
  graph.addEdge("S27", "S27");
  graph.addEdge("S27", "S25");
  graph.addEdge("S27", "S23");
  graph.addEdge("S27", "S39");
  graph.addEdge("S27", "S28");
  graph.addEdge("S27", "S21");
  graph.addEdge("S28", "S0");
  graph.addEdge("S28", "S6");
  graph.addEdge("S28", "S28");
  graph.addEdge("S28", "S28");
  graph.addEdge("S28", "S26");
  graph.addEdge("S28", "S24");
  graph.addEdge("S28", "S40");
  graph.addEdge("S28", "S27");
  graph.addEdge("S28", "S28");
  graph.addEdge("S28", "S28");
  graph.addEdge("S28", "S28");
  graph.addEdge("S28", "S22");
  graph.addEdge("S29", "S0");
  graph.addEdge("S29", "S1");
  graph.addEdge("S29", "S33");
  graph.addEdge("S29", "S33");
  graph.addEdge("S29", "S17");
  graph.addEdge("S29", "S30");
  graph.addEdge("S30", "S0");
  graph.addEdge("S30", "S2");
  graph.addEdge("S30", "S34");
  graph.addEdge("S30", "S34");
  graph.addEdge("S30", "S18");
  graph.addEdge("S30", "S29");
  graph.addEdge("S30", "S30");
  graph.addEdge("S30", "S30");
  graph.addEdge("S30", "S30");
  graph.addEdge("S31", "S0");
  graph.addEdge("S31", "S3");
  graph.addEdge("S31", "S31");
  graph.addEdge("S31", "S31");
  graph.addEdge("S31", "S33");
  graph.addEdge("S31", "S29");
  graph.addEdge("S31", "S19");
  graph.addEdge("S31", "S32");
  graph.addEdge("S32", "S0");
  graph.addEdge("S32", "S4");
  graph.addEdge("S32", "S32");
  graph.addEdge("S32", "S32");
  graph.addEdge("S32", "S34");
  graph.addEdge("S32", "S30");
  graph.addEdge("S32", "S20");
  graph.addEdge("S32", "S31");
  graph.addEdge("S32", "S32");
  graph.addEdge("S32", "S32");
  graph.addEdge("S32", "S32");
  graph.addEdge("S33", "S0");
  graph.addEdge("S33", "S5");
  graph.addEdge("S33", "S33");
  graph.addEdge("S33", "S33");
  graph.addEdge("S33", "S31");
  graph.addEdge("S33", "S29");
  graph.addEdge("S33", "S21");
  graph.addEdge("S33", "S34");
  graph.addEdge("S34", "S0");
  graph.addEdge("S34", "S6");
  graph.addEdge("S34", "S34");
  graph.addEdge("S34", "S34");
  graph.addEdge("S34", "S32");
  graph.addEdge("S34", "S30");
  graph.addEdge("S34", "S22");
  graph.addEdge("S34", "S33");
  graph.addEdge("S34", "S34");
  graph.addEdge("S34", "S34");
  graph.addEdge("S34", "S34");
  graph.addEdge("S35", "S0");
  graph.addEdge("S35", "S1");
  graph.addEdge("S35", "S39");
  graph.addEdge("S35", "S39");
  graph.addEdge("S35", "S23");
  graph.addEdge("S35", "S36");
  graph.addEdge("S35", "S29");
  graph.addEdge("S36", "S0");
  graph.addEdge("S36", "S2");
  graph.addEdge("S36", "S40");
  graph.addEdge("S36", "S40");
  graph.addEdge("S36", "S24");
  graph.addEdge("S36", "S35");
  graph.addEdge("S36", "S36");
  graph.addEdge("S36", "S36");
  graph.addEdge("S36", "S36");
  graph.addEdge("S36", "S30");
  graph.addEdge("S37", "S0");
  graph.addEdge("S37", "S3");
  graph.addEdge("S37", "S37");
  graph.addEdge("S37", "S37");
  graph.addEdge("S37", "S39");
  graph.addEdge("S37", "S35");
  graph.addEdge("S37", "S25");
  graph.addEdge("S37", "S38");
  graph.addEdge("S37", "S31");
  graph.addEdge("S38", "S0");
  graph.addEdge("S38", "S4");
  graph.addEdge("S38", "S38");
  graph.addEdge("S38", "S38");
  graph.addEdge("S38", "S4");
  graph.addEdge("S38", "S36");
  graph.addEdge("S38", "S26");
  graph.addEdge("S38", "S37");
  graph.addEdge("S38", "S38");
  graph.addEdge("S38", "S38");
  graph.addEdge("S38", "S38");
  graph.addEdge("S38", "S32");
  graph.addEdge("S39", "S5");
  graph.addEdge("S39", "S39");
  graph.addEdge("S39", "S39");
  graph.addEdge("S39", "S37");
  graph.addEdge("S39", "S35");
  graph.addEdge("S39", "S27");
  graph.addEdge("S39", "S40");
  graph.addEdge("S39", "S33");
  graph.addEdge("S40", "S6");
  graph.addEdge("S40", "S40");
  graph.addEdge("S40", "S40");
  graph.addEdge("S40", "S38");
  graph.addEdge("S40", "S36");
  graph.addEdge("S40", "S28");
  graph.addEdge("S40", "S39");
  graph.addEdge("S40", "S40");
  graph.addEdge("S40", "S40");
  graph.addEdge("S40", "S40");
  graph.addEdge("S40", "S34");
  graph.addEdge("S41", "S13");
  graph.addEdge("S41", "S0");
  graph.addEdge("S41", "S45");
  graph.addEdge("S41", "S45");
  graph.addEdge("S41", "S53");
  graph.addEdge("S41", "S42");
  graph.addEdge("S42", "S13");
  graph.addEdge("S42", "S0");
  graph.addEdge("S42", "S46");
  graph.addEdge("S42", "S46");
  graph.addEdge("S42", "S54");
  graph.addEdge("S42", "S41");
  graph.addEdge("S42", "S42");
  graph.addEdge("S42", "S42");
  graph.addEdge("S42", "S42");
  graph.addEdge("S43", "S13");
  graph.addEdge("S43", "S0");
  graph.addEdge("S43", "S43");
  graph.addEdge("S43", "S43");
  graph.addEdge("S43", "S45");
  graph.addEdge("S43", "S41");
  graph.addEdge("S43", "S55");
  graph.addEdge("S43", "S44");
  graph.addEdge("S44", "S13");
  graph.addEdge("S44", "S0");
  graph.addEdge("S44", "S44");
  graph.addEdge("S44", "S44");
  graph.addEdge("S44", "S46");
  graph.addEdge("S44", "S42");
  graph.addEdge("S44", "S56");
  graph.addEdge("S44", "S43");
  graph.addEdge("S44", "S44");
  graph.addEdge("S44", "S44");
  graph.addEdge("S44", "S44");
  graph.addEdge("S45", "S13");
  graph.addEdge("S45", "S0");
  graph.addEdge("S45", "S45");
  graph.addEdge("S45", "S45");
  graph.addEdge("S45", "S43");
  graph.addEdge("S45", "S41");
  graph.addEdge("S45", "S57");
  graph.addEdge("S45", "S46");
  graph.addEdge("S46", "S13");
  graph.addEdge("S46", "S0");
  graph.addEdge("S46", "S46");
  graph.addEdge("S46", "S46");
  graph.addEdge("S46", "S44");
  graph.addEdge("S46", "S42");
  graph.addEdge("S46", "S58");
  graph.addEdge("S46", "S45");
  graph.addEdge("S46", "S46");
  graph.addEdge("S46", "S46");
  graph.addEdge("S46", "S46");
  graph.addEdge("S47", "S14");
  graph.addEdge("S47", "S0");
  graph.addEdge("S47", "S51");
  graph.addEdge("S47", "S51");
  graph.addEdge("S47", "S59");
  graph.addEdge("S47", "S48");
  graph.addEdge("S47", "S41");
  graph.addEdge("S48", "S14");
  graph.addEdge("S48", "S0");
  graph.addEdge("S48", "S52");
  graph.addEdge("S48", "S52");
  graph.addEdge("S48", "S60");
  graph.addEdge("S48", "S47");
  graph.addEdge("S48", "S48");
  graph.addEdge("S48", "S48");
  graph.addEdge("S48", "S48");
  graph.addEdge("S48", "S42");
  graph.addEdge("S49", "S14");
  graph.addEdge("S49", "S0");
  graph.addEdge("S49", "S49");
  graph.addEdge("S49", "S49");
  graph.addEdge("S49", "S51");
  graph.addEdge("S49", "S47");
  graph.addEdge("S49", "S61");
  graph.addEdge("S49", "S50");
  graph.addEdge("S49", "S43");
  graph.addEdge("S50", "S14");
  graph.addEdge("S50", "S0");
  graph.addEdge("S50", "S50");
  graph.addEdge("S50", "S50");
  graph.addEdge("S50", "S52");
  graph.addEdge("S50", "S48");
  graph.addEdge("S50", "S62");
  graph.addEdge("S50", "S49");
  graph.addEdge("S50", "S50");
  graph.addEdge("S50", "S50");
  graph.addEdge("S50", "S50");
  graph.addEdge("S50", "S44");
  graph.addEdge("S51", "S14");
  graph.addEdge("S51", "S0");
  graph.addEdge("S51", "S51");
  graph.addEdge("S51", "S51");
  graph.addEdge("S51", "S49");
  graph.addEdge("S51", "S47");
  graph.addEdge("S51", "S63");
  graph.addEdge("S51", "S52");
  graph.addEdge("S51", "S45");
  graph.addEdge("S52", "S14");
  graph.addEdge("S52", "S0");
  graph.addEdge("S52", "S52");
  graph.addEdge("S52", "S52");
  graph.addEdge("S52", "S50");
  graph.addEdge("S52", "S48");
  graph.addEdge("S52", "S64");
  graph.addEdge("S52", "S51");
  graph.addEdge("S52", "S52");
  graph.addEdge("S52", "S52");
  graph.addEdge("S52", "S52");
  graph.addEdge("S52", "S46");
  graph.addEdge("S53", "S15");
  graph.addEdge("S53", "S0");
  graph.addEdge("S53", "S57");
  graph.addEdge("S53", "S57");
  graph.addEdge("S53", "S41");
  graph.addEdge("S53", "S54");
  graph.addEdge("S54", "S15");
  graph.addEdge("S54", "S0");
  graph.addEdge("S54", "S58");
  graph.addEdge("S54", "S58");
  graph.addEdge("S54", "S42");
  graph.addEdge("S54", "S53");
  graph.addEdge("S54", "S54");
  graph.addEdge("S54", "S54");
  graph.addEdge("S54", "S54");
  graph.addEdge("S55", "S15");
  graph.addEdge("S55", "S0");
  graph.addEdge("S55", "S55");
  graph.addEdge("S55", "S55");
  graph.addEdge("S55", "S57");
  graph.addEdge("S55", "S53");
  graph.addEdge("S55", "S43");
  graph.addEdge("S55", "S56");
  graph.addEdge("S56", "S15");
  graph.addEdge("S56", "S0");
  graph.addEdge("S56", "S56");
  graph.addEdge("S56", "S56");
  graph.addEdge("S56", "S58");
  graph.addEdge("S56", "S54");
  graph.addEdge("S56", "S44");
  graph.addEdge("S56", "S55");
  graph.addEdge("S56", "S56");
  graph.addEdge("S56", "S56");
  graph.addEdge("S56", "S56");
  graph.addEdge("S57", "S15");
  graph.addEdge("S57", "S0");
  graph.addEdge("S57", "S57");
  graph.addEdge("S57", "S57");
  graph.addEdge("S57", "S55");
  graph.addEdge("S57", "S53");
  graph.addEdge("S57", "S45");
  graph.addEdge("S57", "S58");
  graph.addEdge("S58", "S15");
  graph.addEdge("S58", "S0");
  graph.addEdge("S58", "S58");
  graph.addEdge("S58", "S58");
  graph.addEdge("S58", "S56");
  graph.addEdge("S58", "S54");
  graph.addEdge("S58", "S46");
  graph.addEdge("S58", "S57");
  graph.addEdge("S58", "S58");
  graph.addEdge("S58", "S58");
  graph.addEdge("S58", "S58");
  graph.addEdge("S59", "S16");
  graph.addEdge("S59", "S0");
  graph.addEdge("S59", "S63");
  graph.addEdge("S59", "S63");
  graph.addEdge("S59", "S47");
  graph.addEdge("S59", "S60");
  graph.addEdge("S59", "S53");
  graph.addEdge("S60", "S16");
  graph.addEdge("S60", "S0");
  graph.addEdge("S60", "S64");
  graph.addEdge("S60", "S64");
  graph.addEdge("S60", "S48");
  graph.addEdge("S60", "S59");
  graph.addEdge("S60", "S60");
  graph.addEdge("S60", "S60");
  graph.addEdge("S60", "S60");
  graph.addEdge("S60", "S54");
  graph.addEdge("S61", "S16");
  graph.addEdge("S61", "S0");
  graph.addEdge("S61", "S61");
  graph.addEdge("S61", "S61");
  graph.addEdge("S61", "S63");
  graph.addEdge("S61", "S59");
  graph.addEdge("S61", "S49");
  graph.addEdge("S61", "S62");
  graph.addEdge("S61", "S55");
  graph.addEdge("S62", "S16");
  graph.addEdge("S62", "S0");
  graph.addEdge("S62", "S62");
  graph.addEdge("S62", "S62");
  graph.addEdge("S62", "S64");
  graph.addEdge("S62", "S60");
  graph.addEdge("S62", "S50");
  graph.addEdge("S62", "S61");
  graph.addEdge("S62", "S62");
  graph.addEdge("S62", "S62");
  graph.addEdge("S62", "S62");
  graph.addEdge("S62", "S56");
  graph.addEdge("S63", "S16");
  graph.addEdge("S63", "S0");
  graph.addEdge("S63", "S63");
  graph.addEdge("S63", "S63");
  graph.addEdge("S63", "S61");
  graph.addEdge("S63", "S59");
  graph.addEdge("S63", "S51");
  graph.addEdge("S63", "S64");
  graph.addEdge("S63", "S57");
  graph.addEdge("S64", "S16");
  graph.addEdge("S64", "S0");
  graph.addEdge("S64", "S64");
  graph.addEdge("S64", "S64");
  graph.addEdge("S64", "S62");
  graph.addEdge("S64", "S60");
  graph.addEdge("S64", "S52");
  graph.addEdge("S64", "S63");
  graph.addEdge("S64", "S64");
  graph.addEdge("S64", "S64");
  graph.addEdge("S64", "S64");
  graph.addEdge("S64", "S58");

  if (argc > 1) {
    start = string(argv[1]);
  }

  if (argc > 2) {
    dest = string(argv[2]);
  }

  list<string> visited;
  visited.push_back(start);
  track(graph, visited);
  breadthFirst(graph, visited);

  return 0;
}
