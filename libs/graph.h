#ifndef CASEGEN_GRAPH_H
#define CASEGEN_GRAPH_H

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "bitmap.h"

#define ELEMENT_ID int
#define VERTEX_ID ELEMENT_ID
#define EDGE_TYPE ELEMENT_ID
#define LINK_ID ELEMENT_ID

// simple graph element
struct GraphElement {
  ELEMENT_ID id{0};
  std::string content;

  GraphElement(ELEMENT_ID _id, const std::string &_content)
      : id(_id), content(_content){};

  virtual std::string name() { return content; };
};

// graph vertex
struct Vertex : public GraphElement {
  int in_degree{0};
  int out_degree{0};

  Vertex(const ELEMENT_ID _id, const string &_content)
      : GraphElement(_id, _content){};

  std::string name() override {
    if (content.empty()) {
      std::stringstream oss;
      oss << "S" << id;
      content = oss.str();
    }
    return content;
  };

  int balance() const { return in_degree - out_degree; };
};

struct Edge : public GraphElement {
  EDGE_TYPE type;

  Edge(ELEMENT_ID _id, const std::string &_content, EDGE_TYPE _type)
      : GraphElement(_id, _content), type(_type){};

  std::string name() override {
    if (content.empty()) {
      std::stringstream oss;
      oss << "E" << type;
      content = oss.str();
    }
    return content;
  };
};

// tuple for link
struct Link {
  const Vertex &source;
  const Vertex &target;
  const Edge &edge;

  Link(const Vertex &v1, const Vertex &v2, const Edge &e)
      : source(v1), target(v2), edge(e){};

  bool circle() const { return (source.id == target.id); };

  int balance() const { return (source.balance() + target.balance()); }
};

typedef std::vector<GraphElement *> EdgeList;
typedef std::vector<Vertex *> VertexList;
typedef std::vector<Link *> LinkList;

typedef std::vector<LinkList> Net;

// graph
class Graph {
public:
  Graph();
  virtual ~Graph();

  virtual void loadFromFile(const std::string &matrix_file);

  virtual void init(size_t rows) {
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

  virtual Link *link(VERTEX_ID source, VERTEX_ID target, EDGE_TYPE type);

  virtual const bool good() const;
  virtual const size_t size() const;

  virtual bool reachable(const VERTEX_ID v1, const VERTEX_ID v2) const {
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

  // a 2D bit map saving the reachable info of 2 vertices
  std::shared_ptr<BitMap2> m_reach_table;

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
      link(l->source->id, l->target->id, l->edge->type);
    }
  }

  inline static bool compareByLength(const LinkList *path1,
                                     const LinkList *path2) {
    if (nullptr == path1 || nullptr == path2) {
      throw std::runtime_error("null pointer");
    }
    return (path1->size() < path2->size());
  }
};

#endif
