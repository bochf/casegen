#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "gtest/gtest.h"
#include "OneWayHashHandler.h"

using namespace std;

TEST(TestOneWayHash, ValidateHash) {
  owh_xbase64_init_decode_table();
  printf("ValidateHash\n");

  unsigned char content_data[4096] = {0};
  unsigned char hashed_buffer[4096] = {0};
  string password = "53114605";
  string owhPwd = "RzFUJOJZmR5PHjsq9ZNjTr3VH2FhVZGCWCAYpLpG--uBM862_oBkh_rnZb889iXWA_3eMho0d7bnbMxAL4Lc632z0";

  unsigned long content_data_length = password.length();
  memcpy(content_data, password.c_str(), content_data_length);
  unsigned long hashed_buffer_length = owhPwd.length();
  memcpy(hashed_buffer, owhPwd.c_str(), hashed_buffer_length);

  printf("Encoded String and Length: %s %d\n", owhPwd.c_str(), hashed_buffer_length);
  printf("Content String and Length: %s %d\n", password.c_str(), content_data_length);

  ValidateHash(content_data, content_data_length, hashed_buffer, hashed_buffer_length);

}

GTEST_API_ int main(int argc, char ** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
