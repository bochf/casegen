#include <iostream>
#include <vector>

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
  for (size_t i = size; i > 0; --i) {
    bm.set(i - 1);
    EXPECT_TRUE(bm.get(i - 1));
    EXPECT_FALSE(bm.all0());
  }
  EXPECT_TRUE(bm.all1());

  bm.set();
  for (size_t i = size; i > 0; --i) {
    bm.reset(i - 1);
    EXPECT_FALSE(bm.get(i - 1));
    EXPECT_FALSE(bm.all1());
  }
  EXPECT_TRUE(bm.all0());

  bm.reset();
  bm.set(1000);
  EXPECT_TRUE(bm.all0());
  EXPECT_FALSE(bm.get(1000));
  bm.reset(size);
  EXPECT_FALSE(bm.get(1000));
}

TEST(BitMap, manipulate) {
  const size_t size = 999;
  BitMap bm1(size);
  BitMap bm2(size);
  EXPECT_EQ(bm1.size(), bm2.size());
  EXPECT_TRUE(bm1.all0());
  EXPECT_TRUE(bm2.all0());

  bm1.set(10);
  bm2.set(20);
  BitMap bm3 = bm1 & bm2;
  EXPECT_EQ(bm1.size(), bm3.size());
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

  bm1.reset();
  bm2.reset();
  bm1.set(10);
  bm2.set(20);
  bm3 = bm1 ^ bm2;
  EXPECT_TRUE(bm3.get(10));
  EXPECT_TRUE(bm3.get(20));
  bm1 ^= bm2;
  EXPECT_TRUE(bm1 == bm3);
  EXPECT_TRUE(bm1.get(10));
  EXPECT_TRUE(bm1.get(20));
  EXPECT_TRUE(bm2.get(20));
  EXPECT_FALSE(bm2.get(10));
  bm2 ^= bm1;
  EXPECT_TRUE(bm2.get(10));
  EXPECT_FALSE(bm2.get(20));
  bm1 ^= bm2;
  EXPECT_TRUE(bm1.get(20));
  EXPECT_FALSE(bm1.get(10));

}

TEST(BitMap, manipulate_different_size) {
  size_t size1 = 5 * MARKER_BLOCK_BITS;
  size_t size2 = 6 * MARKER_BLOCK_BITS - 1;
  BitMap bm1(size1);
  BitMap bm2(size2);
  EXPECT_NE(bm1.size(), bm2.size());
  EXPECT_TRUE(bm1.all0());
  EXPECT_TRUE(bm2.all0());

  bm1.set(10);
  bm2.set(20);
  BitMap bm3 = bm2 & bm1;
  EXPECT_EQ(bm2.size(), bm3.size());
  EXPECT_TRUE(bm3.all0());
  EXPECT_FALSE(bm1 > bm3);
  EXPECT_TRUE(bm2 > bm3);
  EXPECT_FALSE(bm3 > bm1);

  bm1.set(20);
  bm3 = bm1 & bm2;
  EXPECT_FALSE(bm3.all0());
  EXPECT_TRUE(bm1 > bm3);
  EXPECT_TRUE(bm2 > bm3);
  EXPECT_FALSE(bm3 > bm2);

  bm1 &= bm2;
  EXPECT_FALSE(bm1.all0());
  EXPECT_TRUE(bm2.get(20));

  bm1.reset();
  bm2.reset();
  bm1.set(10);
  bm2.set(20);
  bm3 = bm1 | bm2;
  EXPECT_TRUE(bm1.get(10));
  EXPECT_TRUE(bm2.get(20));
  EXPECT_TRUE(bm3.get(10));
  EXPECT_TRUE(bm3.get(20));
  EXPECT_FALSE(bm3 > bm2);
  EXPECT_EQ(bm1.size(), bm3.size());

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
  EXPECT_FALSE(bm1 > bm2);
  EXPECT_FALSE(bm1 < bm2);

  bm1.set(2);
  EXPECT_FALSE(bm1 == bm2);
  EXPECT_TRUE(bm1 > bm2);
  EXPECT_TRUE(bm2 < bm1);
  EXPECT_TRUE(bm1 >= bm2);
  EXPECT_TRUE(bm2 <= bm1);
  EXPECT_FALSE(bm1 <= bm2);
  EXPECT_FALSE(bm2 >= bm1);

  BitMap bm3(size / 2);
  EXPECT_FALSE(bm1 == bm3);
}

TEST(BitMap, diff) {
  const size_t size = 999;
  BitMap bm1(size);
  BitMap bm2(size);

  bm1.set(1);
  bm2.set(100);
  std::vector<size_t> bit_diff;
  bm1.diff(bm2, bit_diff);
  EXPECT_EQ(bit_diff.size(), 2);
  EXPECT_EQ(bit_diff[0], 1);
  EXPECT_EQ(bit_diff[1], 100);

  BitMap bm3(size / 2);
  EXPECT_THROW(bm1.diff(bm3, bit_diff), std::logic_error);
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
  EXPECT_EQ(row, bm.rows());
  EXPECT_EQ(col, bm.cols());
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

TEST(BitMap2, compare_different_size) {
  const size_t row = 77;
  const size_t col = 99;
  const size_t size = row * col;
  BitMap2 bm1(row, col);
  BitMap2 bm2(row / 2, col * 2);

  bm1.set(1, 1);
  bm2.set(33, 11);
  EXPECT_FALSE(bm1 == bm2);
  EXPECT_FALSE(bm1 >= bm2);
  EXPECT_FALSE(bm1 <= bm2);
  EXPECT_FALSE(bm1 > bm2);
  EXPECT_FALSE(bm1 < bm2);
  EXPECT_TRUE(bm1 != bm2);
  bm1 = bm2;
  EXPECT_TRUE(bm1 == bm2);

  bm1.set(2, 25);
  EXPECT_TRUE(bm1 > bm2);
  EXPECT_TRUE(bm1 >= bm2);
  EXPECT_TRUE(bm2 < bm1);
  EXPECT_TRUE(bm2 <= bm1);
}

TEST(BitMap2, exception) {
  const size_t row = 77;
  const size_t col = 99;
  const size_t size = row * col;
  BitMap2 bm1(row, col);
  BitMap2 bm2(row / 2, col * 2);

  EXPECT_THROW(bm2[bm1.rows()], std::out_of_range);
  EXPECT_THROW(bm1 & bm2, std::out_of_range);
  EXPECT_THROW(bm1 | bm2, std::out_of_range);
  EXPECT_THROW(bm1 &= bm2, std::out_of_range);
  EXPECT_THROW(bm1 |= bm2, std::out_of_range);
}
