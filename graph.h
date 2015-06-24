#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <string>
#include <map>
#include <vector>
#include <sstream>

#include "bitmap.h"

using namespace std;

#define ELEMENT_ID size_t
#define VERTEX_ID ELEMENT_ID
#define EDGE_TYPE ELEMENT_ID
#define LINK_ID   ELEMENT_ID

// simple graph element
struct GraphElement {
  ELEMENT_ID id;
  string content;

  GraphElement(const ELEMENT_ID _id, const string _content) :
    id(_id), content(_content) {
  };
  virtual string name() {};
};

// graph vertex
struct Vertex : public GraphElement {
  size_t in_degree;
  size_t out_degree;

  Vertex(const ELEMENT_ID _id, const string _content) :
    GraphElement(_id, _content), in_degree(0), out_degree(0) {
  };
  virtual string name() {
    stringstream ss;
    ss << "S" << id;
    return ss.str();
  };

  inline int balance() const {
    return in_degree - out_degree;
  };
};

struct Edge : public GraphElement {
  EDGE_TYPE type;

  Edge(const ELEMENT_ID _id, const string _content, const EDGE_TYPE _type) :
    GraphElement(_id, _content), type(_type) {
  };

  virtual string name() {
    stringstream ss;
    ss << "E" << type;
    return ss.str();
  };
};

// tuple for link
struct Link {
  Vertex * from;
  Vertex * to;
  Edge * edge;

  Link(Vertex * v1, Vertex * v2, Edge * e) :
    from(v1), to(v2), edge(e) {
  };

  bool circle() {
    if (from && to && edge) {
      return (from->id == to->id);
    } else {
      return false;
    }
  };

  inline int balance() const {
    return (from->balance() + to->balance());
  }
};

typedef vector<GraphElement *> EdgeTypeList;
typedef vector<Vertex *> VertexList;
typedef vector<Link *> LinkList;
typedef vector<LinkList> Net;

// graph
class Graph {
 public:
  Graph();
  virtual ~Graph();

  virtual void init(const string matrix_file);
  virtual Link * link(const VERTEX_ID v1, const VERTEX_ID v2, const EDGE_TYPE type);

  virtual const bool good() const;
  virtual const size_t size() const;

  virtual const bool reachable(const VERTEX_ID v1, const VERTEX_ID v2) const;

  const VertexList & getVertices() const {
    return m_vertices;
  };
  Vertex * getVertex(const VERTEX_ID v) const {
    return m_vertices[v];
  };
  const LinkList & getLinks() const {
    return m_links;
  };
  Link * getLink(const LINK_ID e) const {
    return m_links[e];
  };
  const virtual LinkList & getAdjacencies(const VERTEX_ID v_id) const;

  virtual void eulerize();

  virtual void dump();
 private:
  Net m_net;

  VertexList m_vertices;
  LinkList m_links;
  EdgeTypeList m_edge_types;

  BitMap2 * m_reach_table;  // a 2D bit map saving the reachable info of 2 vertices

  void scan();
  bool isEularian() const;
	void merge();
	void disconnect(const VERTEX_ID v1, const VERTEX_ID v2, LinkList * track=NULL);
	void reconnect(const VERTEX_ID a, const VERTEX_ID b, const VERTEX_ID c, LinkList * track=NULL);
};

#endif
