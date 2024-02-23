#ifndef CASEGEN_GRAPH_H
#define CASEGEN_GRAPH_H

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "bitmap.h"

using namespace std;

#define ELEMENT_ID int
#define VERTEX_ID ELEMENT_ID
#define EDGE_TYPE ELEMENT_ID
#define LINK_ID ELEMENT_ID

// simple graph element
struct GraphElement {
  ELEMENT_ID id{0};
  string content;

  GraphElement(){};
  GraphElement(const ELEMENT_ID _id, const string &_content)
      : id(_id), content(_content){};
  GraphElement(const GraphElement &rhs) : id(rhs.id), content(rhs.content){};
  virtual ~GraphElement(){};

  virtual string name() { return content; };
};

// graph vertex
struct Vertex : public GraphElement {
  size_t in_degree{0};
  size_t out_degree{0};

  Vertex(const ELEMENT_ID _id, const string &_content)
      : GraphElement(_id, _content){};

  Vertex(const Vertex &rhs)
      : GraphElement(rhs.id, rhs.content), in_degree(rhs.in_degree),
        out_degree(rhs.out_degree){

        };

  virtual string name() {
    if (content.empty()) {
      stringstream ss;
      ss << "S" << id;
      content = ss.str();
    }
    return content;
  };

  inline int balance() const { return in_degree - out_degree; };
};

struct Edge : public GraphElement {
  EDGE_TYPE type;

  Edge(const ELEMENT_ID _id, const string _content, const EDGE_TYPE _type)
      : GraphElement(_id, _content), type(_type){};

  virtual string name() {
    if (content.empty()) {
      stringstream ss;
      ss << "E" << type;
      content = ss.str();
    }
    return content;
  };
};

// tuple for link
struct Link {
  Vertex *from;
  Vertex *to;
  Edge *edge;

  Link(Vertex *v1, Vertex *v2, Edge *e) : from(v1), to(v2), edge(e){};

  bool circle() {
    if ((from != nullptr) && (to != nullptr) && (edge != nullptr)) {
      return (from->id == to->id);
    }
    return false;
  };

  inline int balance() const { return (from->balance() + to->balance()); }
};

typedef vector<GraphElement *> EdgeList;
typedef vector<Vertex *> VertexList;
typedef vector<Link *> LinkList;

typedef vector<LinkList> Net;

// graph
class Graph {
public:
  Graph();
  virtual ~Graph();

  virtual void init(const string matrix_file);
  inline virtual void init(const size_t rows, const size_t cols) {
    m_vertices.clear();
    m_links.clear();
    m_edge_types.clear();
    m_net.clear();
    if (m_reach_table) {
      delete m_reach_table;
    }

    for (size_t i = 0; i < rows; ++i) {
      m_vertices.push_back(new Vertex(m_vertices.size(), ""));
      m_net.push_back(LinkList());
    }
  }

  virtual Link *link(const VERTEX_ID v1, const VERTEX_ID v2,
                     const EDGE_TYPE type);

  virtual const bool good() const;
  virtual const size_t size() const;

  inline virtual bool reachable(const VERTEX_ID v1, const VERTEX_ID v2) const {
    return m_reach_table->get(v1, v2);
  }

  const VertexList &getVertices() const { return m_vertices; };
  Vertex *getVertex(const VERTEX_ID v) const { return m_vertices[v]; };
  const LinkList &getLinks() const { return m_links; };
  Link *getLink(const LINK_ID e) const { return m_links[e]; };
  const virtual LinkList &getAdjacencies(const VERTEX_ID v_id) const;

  virtual void eulerize();
  bool eulerian() const;

  void scan();
  virtual void dump();

private:
  Net m_net;

  VertexList m_vertices;
  LinkList m_links;
  EdgeList m_edge_types;

  BitMap2
      *m_reach_table; // a 2D bit map saving the reachable info of 2 vertices

  VertexList m_forks; // the vertices set which all the vertices' in degree less
                      // than out degress
  VertexList m_arrows; // the vertices set which all the vertices' in degree
                       // greater than out degress
  VertexList m_equals; // the vertices set which all the vertices' in degree
                       // equals to out degress

  void divide(); // divide the vertices into 2 parts, fork vertices and arrow
                 // vertices

  // clonePath, duplicate every edge in the path
  // this is the function to be used for graph eulerization
  // to add directed edges between vertices a and b
  // which a is arrow vertex and be is fork vertex
  // a and be might be directed connected or intermedia connected through
  // multipl balanced vertices this function is to increase out degree of a and
  // in degree of b by repeating cloning the path in limited times either a or b
  // will become balanced vertex (or both get to balance at same time) because
  // we will add both in edge and out edge at same time on the intermedia vertex
  // if a and b are not directly connected, so the operation will not break the
  // balance status of the intermedia vertices
  void clonePath(const LinkList &path) {
    for (size_t i = 0; i < path.size(); ++i) {
      Link *l = path[i];
      link(l->from->id, l->to->id, l->edge->type);
    }
  }

  inline static bool compareByLength(const LinkList *path1,
                                     const LinkList *path2) {
    if (NULL == path1 || NULL == path2) {
      throw std::runtime_error("null pointer");
    }
    return (path1->size() < path2->size());
  }
};

#endif
