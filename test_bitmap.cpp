#include <iostream>

#include "gtest/gtest.h"
#include "bitmap.h"

TEST(BitMap, create) {
  BitMap bitmap(10);

  EXPECT_EQ(10, bitmap.size());
  EXPECT_TRUE(bitmap.all0());
}

TEST(BitMap, create_big) {
  BitMap bitmap(0x7fff);

  EXPECT_EQ(0x7fff, bitmap.size());
  EXPECT_TRUE(bitmap.all0());
}

TEST(BitMap, set_get) {
  const size_t size = 999;
  BitMap bm(size);
  EXPECT_EQ(size, bm.size());
  EXPECT_TRUE(bm.all0());

  bm.set();
  EXPECT_TRUE(bm.all1());
  for (size_t i = 0; i < size; ++i) {
    EXPECT_TRUE(bm.get(i));
    EXPECT_TRUE(bm[i]);
  }

  bm.reset();
  EXPECT_TRUE(bm.all0());
  for (size_t i = 0; i < size; ++i) {
    EXPECT_FALSE(bm.get(i));
    EXPECT_FALSE(bm[i]);
  }

  for (size_t i = 0; i < size; ++i) {
    bm.set(i);
    EXPECT_TRUE(bm.get(i));
    EXPECT_TRUE(bm[i]);
  }

  for (size_t i = 0; i < size; ++i) {
    bm.reset(i);
    EXPECT_FALSE(bm.get(i));
    EXPECT_FALSE(bm[i]);
  }

  bm.reset();
  for (size_t i = 0; i < size; ++i) {
    bm.set(i);
  }
  EXPECT_TRUE(bm.all1());

  bm.set();
  for (size_t i = 0; i < size; ++i) {
    bm.reset(i);
  }
  EXPECT_TRUE(bm.all0());
}

TEST(BitMap, manipulate) {
  const size_t size = 999;
  BitMap bm1(size);
  BitMap bm2(size);
  BitMap bm3(size);
  EXPECT_EQ(bm1.size(), bm2.size());
  EXPECT_TRUE(bm1.all0());
  EXPECT_TRUE(bm2.all0());

  bm1.set(10);
  bm2.set(20);
  bm3 = bm1 & bm2;
  EXPECT_TRUE(bm1.get(10));
  EXPECT_TRUE(bm2.get(20));
  EXPECT_TRUE(bm3.all0());

  bm1 &= bm2;
  EXPECT_TRUE(bm1.all0());
  EXPECT_TRUE(bm2.get(20));

  bm1.set(10);
  bm2.set(20);
  bm3 = bm1 | bm2;
  EXPECT_TRUE(bm1.get(10));
  EXPECT_TRUE(bm2.get(20));
  EXPECT_TRUE(bm3.get(10));
  EXPECT_TRUE(bm3.get(20));

  bm1 |= bm2;
  EXPECT_TRUE(bm1.get(10));
  EXPECT_TRUE(bm2.get(20));
  EXPECT_TRUE(bm1.get(20));
}

TEST(BitMap, compare) {
  const size_t size = 999;
  BitMap bm1(size);
  BitMap bm2(size);

  bm1.set(1);
  bm2.set(100);
  EXPECT_TRUE(bm1 != bm2);
  bm1 = bm2;
  EXPECT_TRUE(bm1 == bm2);

  bm1.set(2);
  EXPECT_TRUE(bm1 > bm2);
  EXPECT_TRUE(bm1 >= bm2);
  EXPECT_TRUE(bm2 < bm1);
  EXPECT_TRUE(bm2 <= bm1);
}

TEST(BitMap2, create) {
  BitMap2 bitmap(10, 10);

  EXPECT_EQ(100, bitmap.size());
  EXPECT_TRUE(bitmap.all0());
}

TEST(BitMap2, create_big) {
  BitMap2 bitmap(0x7fff, 0x0fff);

  EXPECT_EQ(0x7fff * 0x0fff, bitmap.size());
  EXPECT_TRUE(bitmap.all0());
}

TEST(BitMap2, set_get) {
  const size_t row = 77;
  const size_t col = 99;
  const size_t size = row * col;
  BitMap2 bm(row, col);
  EXPECT_EQ(size, bm.size());
  EXPECT_TRUE(bm.all0());

  bm.set();
  EXPECT_TRUE(bm.all1());
  for (size_t i = 0; i < row; ++i) {
    EXPECT_TRUE(bm[i].all1());
    for (size_t j = 0; j < col; ++j) {
      EXPECT_TRUE(bm.get(i, j));
    }
  }

  bm.reset();
  EXPECT_TRUE(bm.all0());
  for (size_t i = 0; i < row; ++i) {
    EXPECT_TRUE(bm[i].all0());
    for (size_t j = 0; j < col; ++j) {
      EXPECT_FALSE(bm.get(i, j));
    }
  }

  for (size_t i = 0; i < row; ++i) {
    for (size_t j = 0; j < col; ++j) {
      bm.set(i, j);
      EXPECT_TRUE(bm.get(i, j));
    }
  }

  for (size_t i = 0; i < row; ++i) {
    for (size_t j = 0; j < col; ++j) {
      bm.reset(i, j);
      EXPECT_FALSE(bm.get(i, j));
    }
  }

  bm.reset();
  for (size_t i = 0; i < row; ++i) {
    for (size_t j = 0; j < col; ++j) {
      bm.set(i, j);
    }
  }
  EXPECT_TRUE(bm.all1());

  bm.set();
  for (size_t i = 0; i < row; ++i) {
    for (size_t j = 0; j < col; ++j) {
      bm.reset(i, j);
    }
  }
  EXPECT_TRUE(bm.all0());
}

TEST(BitMap2, manipulate) {
  const size_t row = 77;
  const size_t col = 99;
  const size_t size = row * col;
  BitMap2 bm1(row, col);
  BitMap2 bm2(row, col);
  BitMap2 bm3(row, col);
  EXPECT_EQ(bm1.size(), bm2.size());
  EXPECT_TRUE(bm1.all0());
  EXPECT_TRUE(bm2.all0());

  bm1.set(10, 30);
  bm2.set(20, 40);
  bm3 = bm1 & bm2;
  EXPECT_TRUE(bm1.get(10, 30));
  EXPECT_TRUE(bm2.get(20, 40));
  EXPECT_TRUE(bm3.all0());

  bm1 &= bm2;
  EXPECT_TRUE(bm1.all0());
  EXPECT_TRUE(bm2.get(20, 40));

  bm1.set(10, 30);
  bm2.set(20, 40);
  bm3 = bm1 | bm2;
  EXPECT_TRUE(bm1.get(10, 30));
  EXPECT_TRUE(bm2.get(20, 40));
  EXPECT_TRUE(bm3.get(10, 30));
  EXPECT_TRUE(bm3.get(20, 40));

  bm1 |= bm2;
  EXPECT_TRUE(bm1.get(10, 30));
  EXPECT_TRUE(bm2.get(20, 40));
  EXPECT_TRUE(bm1.get(20, 40));
}

TEST(BitMap2, compare) {
  const size_t row = 77;
  const size_t col = 99;
  const size_t size = row * col;
  BitMap2 bm1(row, col);
  BitMap2 bm2(row, col);

  bm1.set(1, 1);
  bm2.set(33, 11);
  EXPECT_TRUE(bm1 != bm2);
  bm1 = bm2;
  EXPECT_TRUE(bm1 == bm2);

  bm1.set(2, 25);
  EXPECT_TRUE(bm1 > bm2);
  EXPECT_TRUE(bm1 >= bm2);
  EXPECT_TRUE(bm2 < bm1);
  EXPECT_TRUE(bm2 <= bm1);
}

GTEST_API_ int main(int argc, char ** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
