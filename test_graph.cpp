#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "graph.h"
#include "traveller.h"

Graph g;

class GraphTest : public ::testing::Test {
 protected:
  static void SetUpTestCase() {
    g.init("m.txt");
  }
};

TEST_F(GraphTest, create) {
  EXPECT_TRUE(g.good());
  g.dump();
  IGraphTraveller * pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ALL);
  EXPECT_FALSE(NULL == pTraveller);
  delete pTraveller;
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
  EXPECT_TRUE(g.reachable(16, 59));
  EXPECT_TRUE(g.reachable(58, 1));
  EXPECT_TRUE(g.reachable(17, 24));
  EXPECT_TRUE(g.reachable(7, 11));
  EXPECT_TRUE(g.reachable(7, 8));
  EXPECT_TRUE(g.reachable(7, 9));
  EXPECT_TRUE(g.reachable(12, 10));
  EXPECT_FALSE(g.reachable(1, 10));
}

TEST_F(GraphTest, SearchAll) {
  IGraphTraveller * pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ALL);
  Properties config;
  config["START"] = 0;
  config["END"] = 13;
  config["MAX_DEPTH"] = 8;
  pTraveller->configure(config);
  pTraveller->travel(g);
}

TEST_F(GraphTest, SearchDfs) {
  // g.dump();
  IGraphTraveller * pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_DFS);
  pTraveller->travel(g);
}

TEST_F(GraphTest, SearchDfsPath) {
  // g.dump();
  IGraphTraveller * pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_DFS_PATH);
  pTraveller->travel(g);
}

TEST_F(GraphTest, SearchOne) {
  IGraphTraveller * pTraveller = IGraphTraveller::createInstance(IGraphTraveller::GT_BFS_ONE);
  Properties config;
  for (size_t start = 0; start < g.size(); ++start) {
    for (size_t end = 0; end < g.size(); ++end) {
      config["START"] = start;
      config["END"] = end;
      pTraveller->configure(config);
      pTraveller->travel(g);
    }
  }
}

GTEST_API_ int main(int argc, char ** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
