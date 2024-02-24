#ifndef __TRAVELLER_H__
#define __TRAVELLER_H__

#include "graph.h"
#include <climits>
#include <queue>

typedef map<string, int> Properties;

// graph traveller
class IGraphTraveller {
public:
  enum GT_ALGORITHM {
    GT_BFS = 0,
    GT_DFS,
    GT_BFS_ALL,
    GT_BFS_ONE,
    GT_DFS_PATH,
    GT_EULER
  };

  IGraphTraveller()          = default;
  virtual ~IGraphTraveller() = default;

  // disable copy, assignment and move constructors
  IGraphTraveller(const IGraphTraveller &rhs)            = delete;
  IGraphTraveller &operator=(const IGraphTraveller &rhs) = delete;
  IGraphTraveller(const IGraphTraveller &&rhs)           = delete;

  // interfaces
  virtual void            travel(const Graph &g, string &trace) = 0;
  virtual void            configure(const Properties &config)   = 0;
  virtual GT_ALGORITHM    algorithm()                           = 0;

  // factory
  static IGraphTraveller *createInstance(const GT_ALGORITHM &algorithm);

protected:
};

// breadth first search
class GraphTravellerBfs : public IGraphTraveller {
  friend class IGraphTraveller;

public:
  virtual void travel(const Graph &g, string &trace);
  virtual void configure(const Properties &config){};

  inline virtual GT_ALGORITHM algorithm() { return GT_BFS; };

  static LinkList *
  shortestPath(const Graph *g, const Vertex *v1, const Vertex *v2);

protected:
  GraphTravellerBfs() : m_nodes(0), m_random(false){};

  virtual void resetVisitBits();
  virtual void startOver(const Graph &g);
  virtual void visit(const VERTEX_ID v_id, const bool mark = true);
  virtual bool isVisited(const VERTEX_ID v_id) const;

  size_t       m_nodes;
  bool         m_random;
  vector<char> m_bits;
};

// depth first search
class GraphTravellerDfs : public IGraphTraveller {
public:
  virtual void travel(const Graph &g, string &trace);
  virtual void configure(const Properties &config){};

  inline virtual GT_ALGORITHM algorithm() { return GT_DFS; };

protected:
  GraphTravellerDfs() : m_visit_table(NULL){};

  virtual void   startOver(const Graph &g);
  virtual void   visit(const VERTEX_ID v_id, const bool mark = true);
  virtual bool   isVisited(const VERTEX_ID v_id) const;
  virtual string print(const VERTEX_ID start,
                       const VERTEX_ID end,
                       const LinkList &backtrack) const;

  BitMap *m_visit_table;

  friend class IGraphTraveller;
};

// depth first search on path
class GraphTravellerDfsPath : public GraphTravellerDfs {
public:
  virtual void travel(const Graph &g, string &trace);
  virtual void configure(const Properties &config){};

  inline virtual GT_ALGORITHM algorithm() { return GT_DFS_PATH; };

protected:
  GraphTravellerDfsPath(){};

  virtual void   startOver(const Graph &g);
  virtual string print(const VERTEX_ID start,
                       const VERTEX_ID end,
                       const LinkList &backtrack) const;

private:
  void travel(const Graph &g, const LINK_ID v, string &trace);
  // calculate the uncovered out path of a vertex
  size_t uncoveredBranches(const Graph    &g,
                           const VERTEX_ID v,
                           const size_t    steps) const;
  // get the new edge which has most possibility to walk on new path
  // by calculating the uncovered branches of its children
  LINK_ID mostChoice(const Graph    &g,
                     const VERTEX_ID v,
                     const size_t    steps,
                     LINK_ID        &e) const;
  friend class IGraphTraveller;
};

// search all possible paths from A to B with restriction
class GraphTravellerBfsAll : public GraphTravellerBfs {
  friend class IGraphTraveller;

public:
  virtual void travel(const Graph &g, string &trace);

  virtual void                configure(const Properties &config);
  inline virtual GT_ALGORITHM algorithm() { return GT_BFS_ALL; };

protected:
  virtual void   startOver(const Graph &g);
  virtual string print() const;

  virtual bool searchFurther() const;

  GraphTravellerBfsAll()
      : m_start(0), m_end(0), m_found(0), m_max_cases(UINT_MAX),
        m_max_depth(UINT_MAX){};

  vector<ELEMENT_ID> m_path;

  VERTEX_ID m_start, m_end;

  size_t m_found;
  size_t m_max_depth;
  size_t m_max_cases;

private:
  void travel(const Graph &g, const VERTEX_ID current_node_id);
};

class GraphTravellerBfsOne : public GraphTravellerBfs {
  friend class IGraphTraveller;

public:
  virtual void travel(const Graph &g, string &trace);
  virtual void configure(const Properties &config);

  inline virtual GT_ALGORITHM algorithm() { return GT_BFS_ONE; };

protected:
  virtual void   startOver(const Graph &g);
  virtual string print() const;

  VERTEX_ID m_start, m_end;
  LinkList  m_backtrack;
};

class GraphTravellerEuler : public IGraphTraveller {
  /**
   * Hierholzer's algorithm[edit]
   *
   * If a graph has Euler cycles
   *
   * Choose any starting vertex v, and follow a trail of edges from that vertex
   * until returning to v. It is not possible to get stuck at any vertex other
   * than v, because the even degree of all vertices ensures that, when the
   * trail enters another vertex w there must be an unused edge leaving w. The
   * tour formed in this way is a closed tour, but may not cover all the
   * vertices and edges of the initial graph.
   *
   * As long as there exists a vertex u that belongs to the current tour but
   * that has adjacent edges not part of the tour, start another trail from u,
   * following unused edges until returning to u, and join the tour formed in
   * this way to the previous tour.
   */
public:
  virtual ~GraphTravellerEuler() {
    if (m_visit_table) {
      delete m_visit_table;
    }
    m_visit_table = NULL;
    m_euler_cycle.clear();
    // while (!m_euler_cycle.empty()) m_euler_cycle.pop();
    m_vertices.clear();
  };

  virtual void travel(const Graph &g, string &trace);
  virtual void configure(const Properties &config);

  inline virtual GT_ALGORITHM algorithm() { return GT_EULER; };

protected:
  GraphTravellerEuler() : m_start(0), m_random(true), m_visit_table(NULL){};

  virtual string print();

private:
  void      startOver(const Graph &g);
  void      walk(const Graph &g);
  VERTEX_ID freeVertex();

  BitMap        *m_visit_table;
  bool           m_random;
  VERTEX_ID      m_start;
  LinkList       m_euler_cycle;
  vector<Vertex> m_vertices;

  friend class IGraphTraveller;
};

#endif
