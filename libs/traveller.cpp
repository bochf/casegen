#include "traveller.h"
#include "graph.h"

#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <cstring>
#include <stack>
#include <algorithm>

#include "bitmap.h"

using namespace std;

IGraphTraveller *
IGraphTraveller::createInstance(const GT_ALGORITHM &algorithm) {
  IGraphTraveller * pInstance = NULL;
  switch (algorithm) {
  case GT_BFS: {
    pInstance = new GraphTravellerBfs();
  }
  break;
  case GT_DFS: {
    pInstance = new GraphTravellerDfs();
  }
  break;
  case GT_BFS_ALL: {
    pInstance = new GraphTravellerBfsAll();
  }
  break;
  case GT_BFS_ONE: {
    pInstance = new GraphTravellerBfsOne();
  }
  break;
  case GT_DFS_PATH: {
    pInstance = new GraphTravellerDfsPath();
  }
  break;
  case GT_EULER: {
    pInstance = new GraphTravellerEuler();
  }
  break;
  default:
    break;
  }

  return pInstance;
}

void GraphTravellerDfs::travel(const Graph & g, string & trace) {
  if (g.size() == 0) {
    return;
  }

  startOver(g);
  vector<VERTEX_ID> endpoints;
  LinkList backtrack(g.size(), NULL);

  VERTEX_ID start = 0;
  stack<VERTEX_ID> s;
  s.push(g.getVertices()[start]->id);
  visit(0);
  backtrack[0] = g.getAdjacencies(start)[0];

  while (!s.empty()) {
    // get the top element from the stack
    VERTEX_ID v = s.top();
    s.pop();

    // mark this vertex as visited
    // visit(v);

    // exercise the vertex
    // push all the unvisited neighbor of this vertex in to stack
    LinkList adj = g.getAdjacencies(v);
    bool path_terminated = true; // if can't go further, mark the vertex as terminator
    for (size_t i = 0; i < adj.size(); ++i) {
      VERTEX_ID w = adj[i]->to->id;
      if (isVisited(w)) {
        continue;
      }
      visit(w);
      s.push(w);
      path_terminated = false;

      // track the path
      backtrack[w] = adj[i];
    }
    if (path_terminated) {
      endpoints.push_back(v);
    }
  }
  cout << "finished" << endl;
  for (size_t i = 0; i < endpoints.size(); ++i) {
    trace += print(start, endpoints[i], backtrack);
  }
}

void GraphTravellerDfs::startOver(const Graph & g) {
  // reset visit table
  if (m_visit_table) {
    delete m_visit_table;
  }
  m_visit_table = new BitMap(g.size());
}

void GraphTravellerDfs::visit(const VERTEX_ID v_id, const bool mark) {
  if (NULL == m_visit_table) {
    throw std::runtime_error("visit table is null");
  }

  if (mark) {
    m_visit_table->set(v_id);
  } else {
    m_visit_table->reset(v_id);
  }
}

bool GraphTravellerDfs::isVisited(const VERTEX_ID v_id) const {
  if (NULL == m_visit_table) {
    throw std::runtime_error("visit table is null");
  }

  return m_visit_table->get(v_id);
}

string GraphTravellerDfs::print(const VERTEX_ID start, const VERTEX_ID end, const LinkList & backtrack) const {
  string path;
  Link * link = backtrack[end];
  if (NULL == link) {
    throw std::runtime_error("trace link is null");
  }

  while (!link->circle()) {
    path = "--" + link->edge->name() + "-->" + link->to->name() + path;
    if (link->from->id == start) {
      path = "\n" + link->from->name() + path;
      break;
    }
    link = backtrack[link->from->id];
  }

	path += "\n";
  return path;
}

void GraphTravellerDfsPath::travel(const Graph & g, string & trace) {
  if (g.size() == 0) {
    return;
  }

  startOver(g);

  for (LINK_ID e = 0; e < g.getLinks().size(); ++e) {
    // get a path has not been walked
    if (isVisited(e)) {
      continue;
    }
    travel(g, e, trace);
  }
}

void GraphTravellerDfsPath::travel(const Graph & g, const LINK_ID e, string & trace) {
  LinkList backtrack;

  Link * link = g.getLink(e);
  backtrack.push_back(g.getLink(e));
  visit(e);

  VERTEX_ID start = link->from->id;
  size_t uncover = 1;
  while (uncover != 0) {
    if (NULL == link) {
      throw std::runtime_error("link is null");
    }

    // test the destination vertex of this edge
    // get one edge start from it
    LinkList adj = g.getAdjacencies(link->to->id);
    LINK_ID x;
    uncover = mostChoice(g, link->to->id, 1, x);
    if (uncover == 0) {
      break;
    }

    visit(x);
    link = g.getLink(x);
    backtrack.push_back(link);
  }
	trace += print(start, link->to->id, backtrack) + "\n";
}

void GraphTravellerDfsPath::startOver(const Graph & g) {
  // reset visit table, it is the visit bit for edges
  if (m_visit_table) {
    delete m_visit_table;
  }
  m_visit_table = new BitMap(g.getLinks().size());
}

string GraphTravellerDfsPath::print(const VERTEX_ID start, const VERTEX_ID end, const LinkList & backtrack) const {
  string path = backtrack[0]->from->name();
  for (size_t i = 0; i < backtrack.size(); ++i) {
    path += "--" + backtrack[i]->edge->name() + "-->" + backtrack[i]->to->name();
  }
  return path;
}

size_t GraphTravellerDfsPath::uncoveredBranches(const Graph & g, const VERTEX_ID v, const size_t steps) const {
  LinkList adj = g.getAdjacencies(v);

  size_t uncover = 0;
  if (steps == 1) {
    for (size_t i = 0; i < adj.size(); ++i)
      if (!isVisited(adj[i]->edge->id)) {
        ++uncover;
      }
  } else {
    for (size_t i = 0; i < adj.size(); ++i) {
      uncover += uncoveredBranches(g, adj[i]->to->id, steps - 1);
    }
  }

  return uncover;
}

size_t GraphTravellerDfsPath::mostChoice(const Graph & g, const VERTEX_ID v, const size_t steps, LINK_ID & e) const {
  LinkList adj = g.getAdjacencies(v);
  size_t possibility = 0;

  for (size_t i = 0; i < adj.size(); ++i) {
    if (isVisited(adj[i]->edge->id)) {
      continue;
    }
    size_t p = uncoveredBranches(g, adj[i]->to->id, steps);
    if (possibility < p) {
      possibility = p;
      e = adj[i]->edge->id;
    }
  }

  return possibility;
}

void GraphTravellerBfs::travel(const Graph & g, string & trace) {
}

LinkList * GraphTravellerBfs::shortestPath(const Graph * g, const Vertex * v1, const Vertex * v2) {
  if (NULL == g || NULL == v1 || NULL == v2) {
    return NULL;
  }
  if (!g->reachable(v1->id, v2->id)) {
    return NULL;
  }

  LinkList visit_trace(g->size(), NULL);
  BitMap visit_table(g->size());
  queue<VERTEX_ID> q;
  q.push(v1->id);

  bool found = false;
  while (!q.empty() && !found) {
    VERTEX_ID v = q.front();
    q.pop();

    LinkList adj(g->getAdjacencies(v).begin(), g->getAdjacencies(v).end());
    random_shuffle(adj.begin(), adj.end());

    visit_table.set(v);
    for (LinkList::iterator it = adj.begin(); it != adj.end(); ++it) {
      VERTEX_ID w = (*it)->to->id;

      if (v2->id == w) { // found, stop searching
        visit_trace[w] = (*it);
        found = true;
        break;
      }

      if (!visit_table.get(w)) {
        q.push(w);
        visit_trace[w] = (*it);
      }
    }
  }
  if (!found) {
    return NULL;  // something wrong?
  }

  LinkList * path = new LinkList();
  // set the path according to the visit trace
  Link * link = visit_trace[v2->id];
  while (v1->id != link->from->id && !link->circle()) {
    path->insert(path->begin(), link);
    link = visit_trace[link->from->id];
  }
  path->insert(path->begin(), link);

  return path;
}

void GraphTravellerBfs::startOver(const Graph & g) {
  m_nodes = g.size();
  resetVisitBits();
}

void GraphTravellerBfs::resetVisitBits() {
  m_bits.clear();
  for (size_t i = 0; i < m_nodes; i++ ) {
    m_bits.push_back(0);
  }
}

void GraphTravellerBfs::visit(const VERTEX_ID v_id, const bool mark) {
  if (v_id >= m_nodes) {
    return;
  }
  if (m_bits.empty()) {
    return;
  }

  size_t offset = v_id / MARKER_BLOCK_BITS;
  size_t bits = v_id % MARKER_BLOCK_BITS;

  char mask = 1 << bits;
  if (mark) {
    m_bits[offset] |= mask;
  } else {
    m_bits[offset] &= ~mask;
  }
}

bool GraphTravellerBfs::isVisited(const VERTEX_ID v_id) const {
  if (v_id >= m_nodes) {
    return false;
  }
  if (m_bits.empty()) {
    return false;
  }

  size_t offset = v_id / MARKER_BLOCK_BITS;
  size_t bits = v_id % MARKER_BLOCK_BITS;
  char mask = 1 << bits;
  return ((m_bits[offset] & mask) == mask);
}

void GraphTravellerBfsAll::configure(const Properties & config) {
  Properties::const_iterator it = config.find("MAX_DEPTH");
  if (it != config.end()) {
    m_max_depth = it->second;
  } else {
    m_max_depth = UINT_MAX;
  }

  it = config.find("START");
  if (it != config.end()) {
    m_start = it->second;
  } else {
    m_start = 0;
  }

  it = config.find("END");
  if (it != config.end()) {
    m_end = it->second;
  } else {
    m_end = 0;
  }

  it = config.find("MAX_CASES");
  if (it != config.end()) {
    m_max_cases = it->second;
  } else {
    m_max_cases = 0;
  }

  it = config.find("RANDOM_WALK");
  if (it != config.end()) {
    m_random = (it->second == 1) ? true : false;
  } else {
    m_random = false;
  }

}

void GraphTravellerBfsAll::travel(const Graph & g, string & trace) {
  if (!g.reachable(m_start, m_end)) {
    cerr << "no connectivity from node " << m_start << " to node " << m_end << endl;
    return;
  }

  startOver(g);
  m_path.push_back(m_start);
  if (m_start != m_end) {
    visit(m_start, true);
  }
  travel(g, m_start);
}

void GraphTravellerBfsAll::travel(const Graph & g, const VERTEX_ID current_node_id) {
  // continue search if not reach the limit
  if (!searchFurther()) {
    return;
  }

  // get adjacencies of current node
  LinkList adj(g.getAdjacencies(current_node_id).begin(), g.getAdjacencies(current_node_id).end());
  if (m_random) {
    random_shuffle(adj.begin(), adj.end());
  }

  // examine adjacent nodes
  VERTEX_ID neighbor;
  for (LinkList::iterator link = adj.begin(); link != adj.end(); ++link) {
    neighbor = (*link)->to->id;
    if (neighbor != m_start && isVisited(neighbor)) {
      // the node has been visited in this path
      continue;
    }

    if (neighbor == m_end) {
      // reach the destination
      m_path.push_back((*link)->edge->id);
      m_path.push_back(neighbor);
      visit(neighbor, true);
      ++m_found;
      // cout << print() << endl;
      visit(neighbor, false);
      m_path.pop_back();
      m_path.pop_back();
    }
  }

  // recursively visit adjacent nodes
  for (LinkList::iterator link = adj.begin(); link != adj.end(); ++link) {
    neighbor = (*link)->to->id;
    if (neighbor == m_end || isVisited(neighbor)) {
      // it is destination or has bee visited
      continue;
    }

    m_path.push_back((*link)->edge->id);
    m_path.push_back(neighbor);
    visit(neighbor, true);
    travel(g, neighbor);
    visit(neighbor, false);
    m_path.pop_back();
    m_path.pop_back();
  }
}

void GraphTravellerBfsAll::startOver(const Graph & g) {
  GraphTravellerBfs::startOver(g);
  m_found = 0;
  m_path.clear();
}

string GraphTravellerBfsAll::print() const {
  if (m_path.empty()) {
    return "";
  }

  if (m_path.size() % 2 == 0) {
    // there should be odd number of elements in the path
    // something wrong
    cout << "error there are " << m_path.size() << " elements in the path" << endl;
    return "";
  }

  stringstream path;
  path <<  "S" << m_path[0];
  size_t i = 1;
  while (i < m_path.size()) {
    path << "--E";
    path << m_path[i++];
    path << "-->S";
    path << m_path[i++];
  }

  return path.str();
}

bool GraphTravellerBfsAll::searchFurther() const {
  // if already found at least one valid path
  // and current search exceeds max search steps
  // terminate current search
  if (m_found >= m_max_cases) {
    return false;
  }
  return (m_found == 0 || (m_path.size() / 2) < m_max_depth);
}

void GraphTravellerBfsOne::travel(const Graph & g, string & trace) {
  if (!g.reachable(m_start, m_end)) {
    cerr << "no connectivity from node " << m_start << " to node " << m_end << endl;
    return;
  }

	m_backtrack.resize(g.size(), NULL);
	BitMap visit_table(g.size());
	queue<VERTEX_ID> q;
	q.push(m_start);

	bool found = false;
	while (!q.empty() && !found) {
		VERTEX_ID v = q.front();
		q.pop();

		LinkList adj(g.getAdjacencies(v).begin(), g.getAdjacencies(v).end());
		if (m_random)
			random_shuffle(adj.begin(), adj.end());

		visit_table.set(v);
		for (LinkList::iterator it = adj.begin(); it!= adj.end(); ++it) {
			VERTEX_ID w = (*it)->to->id;
			if (m_end == w) { // found, stop searching
				m_backtrack[w] = *it;
				found = true;
				break;
			}

			if (!visit_table.get(w)) {
				q.push(w);
				m_backtrack[w] = *it;
			}
		}
	}
	trace = print();
}

void GraphTravellerBfsOne::configure(const Properties & config) {
  Properties::const_iterator it = config.find("MAX_DEPTH");

  it = config.find("START");
  if (it != config.end()) {
    m_start = it->second;
  } else {
    m_start = 0;
  }

  it = config.find("END");
  if (it != config.end()) {
    m_end = it->second;
  } else {
    m_end = 0;
  }

  it = config.find("RANDOM_WALK");
  if (it != config.end()) {
    m_random = (it->second == 1) ? true : false;
  } else {
    m_random = false;
  }
}

void GraphTravellerBfsOne::startOver(const Graph & g) {
  m_nodes = g.size();
  resetVisitBits();
  // initialize back track
  m_backtrack.clear();
  m_backtrack.resize(g.size(), NULL);
}

string GraphTravellerBfsOne::print() const {
  Link * link = m_backtrack[m_end];
  string path;

  while (!link->circle() && link->from->id != m_start) {
    path = "--" + link->edge->name() + "-->" + link->to->name() + path;
    link = m_backtrack[link->from->id];
  }

  path = link->from->name() + "--" + link->edge->name() + "-->" + link->to->name() + path;
  return path;
}

void GraphTravellerEuler::travel(const Graph & g, string & trace) {
  if (!g.eulerian()) {
    cout << "the graph is not Eulerian graph" << endl;
    return;
  }

  startOver(g);

  LinkList adj = g.getAdjacencies(m_start);
  if (adj.empty()) {
    return;
  }

  while (!m_visit_table->all1()) {
    walk(g);
    freeVertex();
  }

  trace = print();
}

void GraphTravellerEuler::configure(const Properties & config) {
  Properties::const_iterator it = config.find("START");
  if (it != config.end()) {
    m_start = it->second;
  } else {
    m_start = 0;
  }
}

string GraphTravellerEuler::print() {
  if (m_euler_cycle.empty()) {
    return "";
  }

  // shift start point on top
  while(m_start != m_euler_cycle.front()->from->id) {
    Link * link = m_euler_cycle.front();
    m_euler_cycle.erase(m_euler_cycle.begin());
    m_euler_cycle.push_back(link);
  }

  LinkList::const_iterator it = m_euler_cycle.begin();
  string path = (*it)->from->name();
  while (it != m_euler_cycle.end()) {
    path += "--" + (*it)->edge->name() + "-->" + (*it)->to->name();
    ++it;
  }
  return path;
}

void GraphTravellerEuler::startOver(const Graph & g) {
  m_euler_cycle.clear();
  /*
  while (!m_euler_cycle.empty()) {
    m_euler_cycle.pop();  // clean up the tour
  }
  */

  if (m_visit_table) {
    delete m_visit_table;
  }
  m_visit_table = new BitMap(g.getLinks().size());  // reset the visit trace

  m_vertices.clear(); // reset the vertices set
  for (size_t i = 0; i < g.size(); ++i) {
    m_vertices.push_back(*g.getVertex(i));
  }
}

// find a Euler cycle in graph
void GraphTravellerEuler::walk(const Graph & g) {
  // this finction is to find a circuit in directed Eulerian graph
  // based on Hierholzer's algorithm
  // each walk will start from an available vertex,
  // available vertex means the vertex has at least one incoming edge and
  // one outgoing edge, since it is directed Eulerian graph, the in degree
  // and out degree of any vertex are always same, so following randomly
  // selected unvisited edge will always lead the walking arriving the
  // origination (destination) vertex.
  //
  // assuming the previous trail already has a circle in m_euler_cycle, and
  // the last vertex in the trail is available vertex, the function will find
  // a circuit with exploring unvisited edges and back to the originate vertex
  //

  if (!g.eulerian()) {
    throw std::logic_error("graph is not Eulerian");
  }

  VERTEX_ID v = m_start;
  if (!m_euler_cycle.empty()) { // if there is previous cycle, use the last vertex as the new start point
    v = m_euler_cycle.back()->to->id;
  }

  VERTEX_ID dest = v;

  bool found = false;
  while (m_vertices[v].out_degree > 0 && !found) {
    if (m_vertices[v].out_degree == 0) {
      cout << "can't go further from " << v << endl;
      throw std::logic_error("Euler walk terminated at " + m_vertices[v].name());
    }

    LinkList adj = g.getAdjacencies(v);
    for (LinkList::iterator it = adj.begin(); it != adj.end(); ++it) {
      if (m_visit_table->get((*it)->edge->id)) {
        continue;  // if the edge was visited, skip
      }

      // found an available out edge
      m_euler_cycle.push_back(*it); // added in the trail
      m_visit_table->set((*it)->edge->id); // mark the edge as visited
      m_vertices[v].out_degree--; // reduce the out degree of v
      v = (*it)->to->id;
      m_vertices[v].in_degree--; // and the in degree of next vertex

      if (v == dest) { // found, stop searching
        found = true;
      }
      break;
    }
  }
}

// get the vertex in the path which has out going link
// this function also shifts the cycle to keep the available vertex at the rear of the path
// in order to start a new circle
VERTEX_ID GraphTravellerEuler::freeVertex() {
  if (m_euler_cycle.empty()) {
    throw std::logic_error("euler cycle is empty");
  }

  // make sure the trail is a circuit
  if (m_euler_cycle.front()->from->id != m_euler_cycle.back()->to->id) {
    throw std::logic_error("euler cycle is not a circuit");
  }

  // turn the circle around until the rear vertex has uncovered edge
  // i.e. the last link->to->out_degree > 0
  // while went through all the vertices in the path but didn't found any uncovered edge
  // i.e. all vertex->out_degree == 0
  // stop the loop
  // use a counter to record how many steps has been taken
  size_t steps = 0;
  Link * link = m_euler_cycle.back();
  while ((m_vertices[link->to->id].out_degree == 0)
         && (steps < m_euler_cycle.size())) {
    link = m_euler_cycle.front();
    // each vertex in the cycle should have equal in and out degree
    if (m_vertices[link->from->id].balance() != 0) {
      throw std::logic_error("vertex in the euler cycle is not balanced");
    }
    m_euler_cycle.erase(m_euler_cycle.begin());
    m_euler_cycle.push_back(link);
    ++steps;
  }

  // if all the vertices explored, the graph should be fully covered
  if (steps == m_euler_cycle.size()) {
    if (!m_visit_table->all1()) {
      throw std::logic_error("the graph is not eulerian graph");
    }
  }
  return link->to->id;
}
