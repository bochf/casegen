#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

#include "bitmap.h"
#include "graph.h"
#include "traveller.h"

using namespace std;

Graph::Graph() : m_reach_table(NULL) {}

Graph::~Graph() {
  for (VERTEX_ID i = 0; i < size(); i++)
    for (LINK_ID j = 0; j < m_net[i].size(); ++j) {
      delete m_net[i][j];
      m_net[i].clear();
    }
  m_net.clear();

  if (m_reach_table) {
    delete m_reach_table;
  }
}

void Graph::init(const string matrix_file) {
  m_vertices.clear();
  m_links.clear();
  m_edge_types.clear();
  m_net.clear();
  if (m_reach_table) {
    delete m_reach_table;
  }
  m_reach_table = NULL;

  // read vertex-edge adjacency file
  // it must be m*n matrix, each element is the vertex id
  // m is number of lines, which is equal to vertices number
  // n is number of column, which is equal to edges number
  // -1 means no connection
  ifstream mf(matrix_file.c_str());
  if (!mf.good()) {
    cerr << "open file " << matrix_file << " error" << endl;
    return;
  }

  vector<string> rows;
  while (mf.good()) {
    string line;
    getline(mf, line);
    if (mf.eof()) {
      mf.close();
      break;
    }

    rows.push_back(line);
    m_vertices.push_back(new Vertex(m_vertices.size(), ""));
    m_net.push_back(LinkList());
  }

  VERTEX_ID dest_v_id = 0;
  for (VERTEX_ID i = 0; i < size(); ++i) {
    EDGE_TYPE type = 0;
    stringstream ss(rows[i]);
    while (ss.good()) {
      ss >> dest_v_id;
      Link *l = link(i, dest_v_id, type++);
      /*
      if (l) {
        m_net[i].push_back(l);
      }
      */
    }
    if (m_edge_types.size() != type) {
      // matrix does not have same columns in each line
      cerr << "asymmetric matrix, line " << i - 1 << " has "
           << m_edge_types.size() << " columns, but line " << i << " has "
           << type << " columns" << endl;
      mf.close();
      return;
    }
  }

  mf.close();

  // scan the graph, get connectivity table
  scan();
}

void Graph::dump() {
  if (!m_vertices.empty()) {
    cout << "Vertices: " << m_vertices.size() << endl;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
      cout << " " << m_vertices[i]->name() << " in=" << m_vertices[i]->in_degree
           << " out=" << m_vertices[i]->out_degree;
    }
    cout << endl;
  }

  if (!m_edge_types.empty()) {
    cout << "EdgeTypes: " << m_edge_types.size() << endl;
    for (size_t i = 0; i < m_edge_types.size(); ++i) {
      cout << " " << m_edge_types[i]->name();
    }
    cout << endl;
  }

  if (!m_links.empty()) {
    cout << "Links: " << m_links.size() << endl;
    for (size_t i = 0; i < m_links.size(); ++i) {
      cout << " (" << m_links[i]->from->name() << ", "
           << m_links[i]->edge->name() << ", " << m_links[i]->to->name() << ", "
           << m_links[i]->balance() << ")";
    }
    cout << endl;
  }

  if (!m_net.empty()) {
    cout << "Adjacencies:" << m_links.size() << endl;
    for (size_t m = 0; m < m_net.size(); ++m) {
      for (size_t i = 0; i < m_net[m].size(); ++i) {
        cout << " " << m_net[m][i]->from->name() << "--"
             << m_net[m][i]->edge->name() << "-->" << m_net[m][i]->to->name();
      }
      cout << endl;
    }
    cout << endl;
  }

  // print arrow vertices, fork vertices and bridge links
  cout << "Balanced vertices:" << m_equals.size() << endl;
  for (VertexList::iterator it = m_equals.begin(); it != m_equals.end(); ++it) {
    cout << " (" << (*it)->name() << ", " << (*it)->in_degree << ", "
         << (*it)->out_degree << ")";
  }
  cout << endl;
  cout << "Arrow vertices:" << m_arrows.size() << endl;
  for (VertexList::iterator it = m_arrows.begin(); it != m_arrows.end(); ++it) {
    cout << " (" << (*it)->name() << ", " << (*it)->in_degree << ", "
         << (*it)->out_degree << ")";
  }
  cout << endl;
  cout << "Fork vertices:" << m_forks.size() << endl;
  for (VertexList::iterator it = m_forks.begin(); it != m_forks.end(); ++it) {
    cout << " (" << (*it)->name() << ", " << (*it)->in_degree << ", "
         << (*it)->out_degree << ")";
  }
  cout << endl;
}

Link *Graph::link(const VERTEX_ID v1, const VERTEX_ID v2,
                  const EDGE_TYPE type) {
  if (m_edge_types.size() == type) {
    m_edge_types.push_back(new GraphElement(type, ""));
  }

  if (v1 >= size() || v2 >= size()) {
    return NULL;
  }

  /*
  for (size_t i = 0; i < m_net[v1].size(); ++i) {
    if (type == m_net[v1][i]->edge->type) {
      throw std::invalid_argument("duplicate edge type");
    }
  }
  */

  Edge *edge = new Edge(m_links.size(), "", type);
  Link *link = new Link(m_vertices[v1], m_vertices[v2], edge);
  m_links.push_back(link);
  m_net[v1].push_back(link);
  m_vertices[v1]->out_degree++;
  m_vertices[v2]->in_degree++;
  return link;
  // m_net[v1].push_back(m_links.back());
}

const bool Graph::good() const {
  if (m_vertices.size() != m_net.size()) {
    return false;
  }

  for (VERTEX_ID i = 0; i < m_vertices.size(); ++i) {
    for (size_t j = 0; j < m_net[i].size(); ++j)
      if (m_net[i][j]->edge->type >= m_edge_types.size() ||
          m_net[i][j]->from->id >= m_vertices.size() ||
          m_net[i][j]->to->id >= m_vertices.size()) {
        return false;
      }
  }

  return true;
}

void Graph::eulerize() {
  // try to make all the vertices balance, e.g. in_degree == out_degree
  // divide the graph vertices into 2 parts,
  // the vertices in one partition are all in_degree > out_degree
  // and the other partition includes vertices in_degress < out_degree
  // the balanced vertices are ignored
  vector<LinkList *> bridges;
  do {
    divide();

    bridges.clear();
    for (VertexList::const_iterator arrow = m_arrows.begin();
         arrow != m_arrows.end(); ++arrow)
      for (VertexList::const_iterator fork = m_forks.begin();
           fork != m_forks.end(); ++fork) {
        LinkList *bridge = GraphTravellerBfs::shortestPath(this, *arrow, *fork);
        if (bridge) {
          bridges.push_back(bridge);
        }
      }

    std::sort(bridges.begin(), bridges.end(), Graph::compareByLength);
    for (vector<LinkList *>::const_iterator bridge = bridges.begin();
         bridge != bridges.end(); ++bridge) {
      Link *left = (*bridge)->front();
      Link *right = (*bridge)->back();
      while (left->from->balance() > 0 && right->to->balance() < 0) {
        clonePath(**bridge);
      }
    }
    // dump();
  } while (!bridges.empty());
  dump();
}

const size_t Graph::size() const { return m_vertices.size(); }

const LinkList &Graph::getAdjacencies(const VERTEX_ID v_id) const {
  if (v_id >= m_net.size()) {
    throw std::out_of_range("vertex id is too big");
  }
  return m_net[v_id];
}

void Graph::scan() {
  // initialize connectivity table
  // allocate enough memory block to the table
  // each bit represent connectivity of node (i->j)
  // set initial value to 0

  m_reach_table = new BitMap2(m_vertices.size(), m_vertices.size());
  BitMap *visit_table = new BitMap(m_vertices.size());
  for (VERTEX_ID v = 0; v < m_vertices.size(); ++v) {
    visit_table->reset();

    queue<VERTEX_ID> q;
    q.push(v);
    visit_table->reset();
    // m_reach_table->set(v, v);

    while (!q.empty()) {
      VERTEX_ID w = q.front();
      q.pop();

      for (LinkList::iterator it = m_net[w].begin(); it != m_net[w].end();
           ++it) {
        VERTEX_ID x = (*it)->to->id;
        if (visit_table->get(x) == 1) {
          continue;
        }
        visit_table->set(x);
        m_reach_table->set(v, x);
        q.push(x);
      }
    }
  }

  divide();
}

void Graph::divide() {
  m_forks.clear();
  m_arrows.clear();
  m_equals.clear();

  for (VertexList::iterator it = m_vertices.begin(); it != m_vertices.end();
       ++it) {
    if ((*it)->balance() == 0) { // balanced node
      m_equals.push_back(*it);
    } else if ((*it)->balance() > 0) { // in degree is greater than out degree
      m_arrows.push_back(*it);
    } else { // in degree is less than out degree
      m_forks.push_back(*it);
    }
  }
}

bool Graph::eulerian() const {
  bool balance = true;
  for (size_t v = 0; v < m_vertices.size(); ++v)
    if (m_vertices[v]->balance() != 0) {
      balance = false;
      break;
    }

  return balance;
}

/*
void Graph::merge() {
  // merge 2 vertices
  // 2 vertices can be merged if there is a circle between these 2 vertices
  // e.g. a --> b and b --> a

  LinkList merge_trace;
  // go through all the links, check whether the 2 ends can be merged
  for (size_t i = 0; i < m_links.size(); ++i) {
    Vertex * start = m_links[i]->from;
    Vertex * end = m_links[i]->to;
    LinkList  adj_start = m_net[start->id];
    LinkList  adj_end = m_net[end->id];

    // check circle
    bool circle = false;
    for (size_t j = 0; j < adj_end.size() && !circle; ++j) {
      if (adj_end[j]->to->id == start->id) {
        circle = true;
      }
    }

    // if has circle, we can merge
    if (circle) {
      // disconnect start and from
      disconnect(start->id, end->id, &merge_trace);
      disconnect(end->id, start->id, &merge_trace);
      // add all adjacencies of end to start
      for (LinkList::iterator it = m_net[end->id].begin(); it !=
m_net[end->id].end(); ++it) { link(start->id, (*it)->to->id, (*it)->edge->type);
      }
      m_net[end->id].clear();
      // move all the adjacencies point to end to start
      for (size_t v = 0; v < m_vertices.size(); ++v) {
        if (m_vertices[v] == NULL) {
          continue;
        }
        if (m_vertices[v]->id == end->id) {
          continue;
        }

        reconnect(m_vertices[v]->id, end->id, start->id);
      }
    }
  }
}

void Graph::disconnect(const VERTEX_ID v1, const VERTEX_ID v2, LinkList * track)
{ for (LinkList::iterator it = m_net[v1].begin(); it != m_net[v1].end(); ++it) {
    if (v2 == (*it)->to->id) {
      if (track) {
        track->push_back(m_links[(*it)->edge->id]);
      } else {
        delete m_links[(*it)->edge->id];
      }

      m_links[(*it)->edge->id] = NULL;
      m_net[v1].erase(it--);
    }
  }
}

void Graph::reconnect(const VERTEX_ID a, const VERTEX_ID b, const VERTEX_ID c,
LinkList * track) {
  // change the link from a-->b to a-->c
  // LinkList adj = m_net[a]
  if (b == c) {
    return;
  }
  for (LinkList::iterator it = m_net[a].begin(); it != m_net[a].end(); ++it) {
    if (b == (*it)->to->id) {
      Link * oldLink = m_links[(*it)->edge->id];
      Link * newLink = link(a, c, (*it)->edge->type);

      if (newLink) {
        if (track) {
          track->push_back(oldLink);
        } else {
          delete oldLink;
        }

        *it = newLink;
        m_links[(*it)->edge->id] = NULL;
      }
    }
  }
}
*/
