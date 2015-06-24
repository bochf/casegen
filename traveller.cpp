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

IGraphTraveller * IGraphTraveller::createInstance(const GT_ALGORITHM algorithm) {
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
  default:
    break;
  }

  return pInstance;
}

void GraphTravellerDfs::travel(const Graph & g) {
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
	for (size_t i = 0; i < endpoints.size(); ++i)
		cout << print(start, endpoints[i], backtrack);
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

  return path;
}

void GraphTravellerDfsPath::travel(const Graph & g) {
  if (g.size() == 0) {
    return;
  }

  startOver(g);

  for (LINK_ID e = 0; e < g.getLinks().size(); ++e) {
    // get a path has not been walked
    if (isVisited(e)) {
      continue;
    }
    travel(g, e);
  }
}

void GraphTravellerDfsPath::travel(const Graph & g, const LINK_ID e) {
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
		if (uncover == 0)
			break;

    visit(x);
    link = g.getLink(x);
    backtrack.push_back(link);
  }
	VERTEX_ID end = link->to->id;

  // cout << "finished" << endl;
  cout << print(start, end, backtrack) << endl;
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

void GraphTravellerBfs::travel(const Graph & g) {
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
    m_depth = it->second;
  } else {
    m_depth = UINT_MAX;
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

}

void GraphTravellerBfsAll::travel(const Graph & g) {
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
  LinkList adj =  g.getAdjacencies(current_node_id);

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
      m_found = true;
      cout << print() << endl;
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
  m_found = false;
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
  return (!m_found || (m_path.size() / 2) < m_depth);
}

void GraphTravellerBfsOne::travel(const Graph & g) {
  if (!g.reachable(m_start, m_end)) {
    cerr << "no connectivity from node " << m_start << " to node " << m_end << endl;
    return;
  }

  startOver(g);

  queue<VERTEX_ID> q;
  q.push(m_start);
  if (m_start != m_end) {
    visit(m_start);
  }

  while (!q.empty()) {
    VERTEX_ID v = q.front();
    q.pop();

    // stop when found
    /*
    if (m_end == v) {
      cout << print() << endl;
      break;
    }
    */

    LinkList adj(g.getAdjacencies(v).begin(), g.getAdjacencies(v).end());
		if (m_random)
			random_shuffle(adj.begin(), adj.end());

    for (LinkList::iterator it = adj.begin(); it != adj.end(); ++it) {
      VERTEX_ID w = (*it)->to->id;
      if (isVisited(w)) {
        continue;
      }

      // set back track
      m_backtrack[w] = **it;

      // set visit flag
      visit(w);

      // add into queue
      q.push(w);

      // stop when found
      if (m_end == w) {
        cout << print() << endl;
        // clean up the queue
        while (!q.empty()) {
          q.pop();
        }
        break;
      }
    }
  }
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
  m_backtrack.resize(g.size(), Link(NULL, NULL, NULL));
}

string GraphTravellerBfsOne::print() const {
  Link link = m_backtrack[m_end];
  string path;

  while (!link.circle() && link.from->id != m_start) {
    path = "--" + link.edge->name() + "-->" + link.to->name() + path;
    link = m_backtrack[link.from->id];
  }

  path = link.from->name() + "--" + link.edge->name() + "-->" + link.to->name() + path;
  return path;
}
