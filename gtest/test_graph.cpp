#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include <graph.h>
#include <traveller.h>

Graph g;

class GraphTest : public ::testing::Test {
 protected:
   static void SetUpTestCase() { g.init("test_matrix.txt"); }
};

TEST_F(GraphTest, create) {
  EXPECT_TRUE(g.good());
  g.dump();
  auto pTraveller =
      IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ALL);
  EXPECT_FALSE(NULL == pTraveller);
}

/*
TEST_F(GraphTest, visit) {
  IGraphTraveller * pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ALL);
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

TEST_F(GraphTest, Dump) {
  g.dump();
}

TEST_F(GraphTest, Scan) {
  EXPECT_TRUE(g.reachable(0, 1));
  EXPECT_TRUE(g.reachable(0, 0));
  EXPECT_TRUE(g.reachable(10, 53));
  EXPECT_TRUE(g.reachable(52, 1));
  EXPECT_TRUE(g.reachable(11, 18));
  EXPECT_TRUE(g.reachable(1, 1));
}

/*
TEST_F(GraphTest, SearchAll) {
  IGraphTraveller * pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ALL);
  Properties config;
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
  pTraveller->travel(g, trace);
	cout << trace << endl;
}

TEST_F(GraphTest, SearchDfsPath) {
  // g.dump();
  auto pTraveller =
      IGraphTraveller::createInstance(IGraphTraveller::GT_DFS_PATH);
  string trace;
  pTraveller->travel(g, trace);
	cout << trace << endl;
}

TEST_F(GraphTest, Shortest) {
  LinkList * path = NULL;
  for (VERTEX_ID i = 0; i < g.size(); ++i)
    for (VERTEX_ID j = 11; j < g.size(); ++j) {
      path = GraphTravellerBfs::shortestPath(&g, g.getVertex(i), g.getVertex(j));
      if (g.reachable(i, j)) {
        EXPECT_FALSE(NULL == path);
      } else {
        EXPECT_TRUE(NULL == path);
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
  for (size_t start = 0; start < g.size(); ++start) {
    for (size_t end = 0; end < g.size(); ++end) {
      config["START"] = start;
      config["END"] = end;
      pTraveller->configure(config);
			string trace;
      pTraveller->travel(g, trace);
			cout << trace << endl;
    }
  }
}

TEST_F(GraphTest, eulerization) {
  g.eulerize();
}

TEST_F(GraphTest, eulerwalk) {
  g.eulerize();
  auto pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_EULER);
  string trace;
  pTraveller->travel(g, trace);
	cout << trace << endl;
}
