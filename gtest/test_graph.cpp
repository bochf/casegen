#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include <graph.h>
#include <traveller.h>

class GraphTest : public ::testing::Test {
private:
  Graph m_graph;

protected:
  Graph &graph() { return m_graph; };

  void SetUp() override { m_graph.loadFromFile("test_matrix.txt"); }
};

TEST_F(GraphTest, create) {
  EXPECT_TRUE(graph().good());
  graph().dump();
  auto pTraveller =
      IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ALL);
  EXPECT_FALSE(nullptr == pTraveller);
}

/*
TEST_F(GraphTest, visit) {
  IGraphTraveller * pTraveller =
IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ALL);
  pTraveller->setGoal(&g, 0, 0);
  for (size_t i = 0; i < g.size(); ++i) {
    pTraveller->visit(i, true);
    EXPECT_TRUE(pTraveller->isVisited(i));
  }

  for (size_t i = 0; i < g.size(); ++i) {
    pTraveller->visit(i, false);
    EXPECT_FALSE(pTraveller->isVisited(i));
  }

  delete pTraveller;
}
*/

TEST_F(GraphTest, Dump) { graph().dump(); }

TEST_F(GraphTest, Scan) {
  EXPECT_TRUE(graph().reachable(0, 1));
  EXPECT_TRUE(graph().reachable(0, 0));
  EXPECT_TRUE(graph().reachable(10, 53));
  EXPECT_TRUE(graph().reachable(52, 1));
  EXPECT_TRUE(graph().reachable(11, 18));
  EXPECT_TRUE(graph().reachable(1, 1));
}

/*
TEST_F(GraphTest, SearchAll) {
  IGraphTraveller * pTraveller =
IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ALL); Properties config;
  config["START"] = 0;
  config["END"] = 13;
  config["MAX_DEPTH"] = 8;
  pTraveller->configure(config);
        string trace;
  pTraveller->travel(g, trace);
        cout << trace << endl;
}
*/

TEST_F(GraphTest, SearchDfs) {
  // g.dump();
  auto pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_DFS);
  string trace;
  pTraveller->travel(graph(), trace);
  cout << trace << '\n';
}

TEST_F(GraphTest, SearchDfsPath) {
  // g.dump();
  auto pTraveller =
      IGraphTraveller::createInstance(IGraphTraveller::GT_DFS_PATH);
  string trace;
  pTraveller->travel(graph(), trace);
  cout << trace << '\n';
}

TEST_F(GraphTest, Shortest) {
  LinkList *path = nullptr;
  for (VERTEX_ID i = 0; i < graph().size(); ++i)
    for (VERTEX_ID j = 11; j < graph().size(); ++j) {
      path = GraphTravellerBfs::shortestPath(&graph, graph().getVertex(i),
                                             graph().getVertex(j));
      if (graph().reachable(i, j)) {
        EXPECT_FALSE(nullptr == path);
      } else {
        EXPECT_TRUE(nullptr == path);
      }
      if (path) {
        delete path;
      }
    }
}

TEST_F(GraphTest, SearchOne) {
  auto pTraveller =
      IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ONE);
  Properties config;
  for (size_t start = 0; start < graph.size(); ++start) {
    for (size_t end = 0; end < graph.size(); ++end) {
      config["START"] = start;
      config["END"] = end;
      pTraveller->configure(config);
      string trace;
      pTraveller->travel(graph, trace);
      cout << trace << '\n';
    }
  }
}

TEST_F(GraphTest, eulerization) { graph.eulerize(); }

TEST_F(GraphTest, eulerwalk) {
  graph.eulerize();
  auto pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_EULER);
  string trace;
  pTraveller->travel(graph, trace);
  cout << trace << '\n';
}
