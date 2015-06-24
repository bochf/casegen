#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/md5.h>   //mmmm
#include <openssl/hmac.h>
#include <openssl/evp.h>


#include "OneWayHashHandler.h"
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
//#include "safe_lib.h"
#include "safe_mem_lib.h"


const int SHA256_LENGTH = 32;//HMAC length
const int NUMBER_TO_GENERATE = 32;//Salt length
const int RAND_SEED_LENGTH = 16;//Seed length to generate Salt
const int CRYPTO_SUCCESS = 1;
const int CRYPTO_ERROR = -1;


/* OneWayHashGeneration
G1(2 bytes)|salt(32 bytes)|sha256(G1|salt|data)(32 bytes)
*/
int GenerateOneWayHash(const unsigned char * content_data, unsigned long content_data_length,
                       unsigned char * hashed_buffer, unsigned long hashed_length) {

  if(  hashed_buffer == NULL || hashed_length < (2 + NUMBER_TO_GENERATE + SHA256_LENGTH)) {
    return CRYPTO_ERROR;
  }
  if(  content_data == NULL || content_data_length == 0) {
    printf("No content data to be hashed\n");
    return CRYPTO_ERROR;
  }

  unsigned char * p = hashed_buffer;
  unsigned char * temp_buffer = (unsigned char *) malloc(content_data_length + NUMBER_TO_GENERATE + 2);
  unsigned long len = content_data_length + NUMBER_TO_GENERATE + 2;
  unsigned char * p1 = temp_buffer;
  const char * algorithm_id = "G1";
  memcpy_s(p, hashed_length, algorithm_id, 2);
  p += 2;
  memcpy_s(p1, len, algorithm_id, 2);
  p1 += 2;

  int salt_length = NUMBER_TO_GENERATE;
  unsigned char salt[NUMBER_TO_GENERATE + 1];

  memset_s(salt, NUMBER_TO_GENERATE, 0);

  if (!GenerateSalt(salt, salt_length)) {
    printf("Failed to generate Salt. Initialize the Salt Bytes\n");
  }
  //printf("Salt Value : %s\n",salt);

  memcpy_s(p, hashed_length - 2, &salt, NUMBER_TO_GENERATE);
  p += NUMBER_TO_GENERATE;

  memcpy_s(p1, len - 2, &salt, NUMBER_TO_GENERATE);
  p1 += NUMBER_TO_GENERATE;

  memcpy_s(p1, len - 2 - NUMBER_TO_GENERATE, content_data, content_data_length);


  unsigned int sha256_len = 0;

  //Calculate the MessageDigest

  EVP_MD_CTX    messageDigestContext;

  unsigned char messageDigest[EVP_MAX_MD_SIZE];

  EVP_DigestInit(&messageDigestContext, EVP_sha256());
  EVP_DigestUpdate(&messageDigestContext, temp_buffer, (content_data_length + NUMBER_TO_GENERATE + 2));
  EVP_DigestFinal(&messageDigestContext, messageDigest, &sha256_len);

  memcpy_s(p, hashed_length - 2 - NUMBER_TO_GENERATE, messageDigest, sha256_len);

  //printf("MessageDigest : %s\n",p);
  printf("Algo+Salt+HMAC : %s\n", hashed_buffer);
  free(temp_buffer);

  sha256_len = sha256_len + NUMBER_TO_GENERATE + 2;
  return sha256_len;

}



/* OneWayHashGeneration
G1(2 bytes)|salt(32 bytes)|sha256(G1|salt|data)(32 bytes)
*/
int GenerateOneWayHashWithSalt(const unsigned char * content_data, unsigned long content_data_length,
                               unsigned char * hashed_buffer, unsigned long hashed_length, const char * salt) {

  if(  hashed_buffer == NULL || hashed_length < (2 + NUMBER_TO_GENERATE + SHA256_LENGTH)) {
    return -1;
  }
  if(  content_data == NULL || content_data_length == 0) {
    return -2;
  }

  unsigned char * p = hashed_buffer;
  unsigned char * temp_buffer = (unsigned char *) malloc(content_data_length + NUMBER_TO_GENERATE + 2);
  unsigned long len = content_data_length + NUMBER_TO_GENERATE + 2;
  unsigned char * p1 = temp_buffer;
  const char * algorithm_id = "G1";
  memcpy_s(p, hashed_length, algorithm_id, 2);
  p += 2;
  memcpy_s(p1, len, algorithm_id, 2);
  p1 += 2;

  int salt_length = NUMBER_TO_GENERATE;
  //printf("Salt Value : %s\n",salt);

  memcpy_s(p, hashed_length - 2, salt, NUMBER_TO_GENERATE);
  p += NUMBER_TO_GENERATE;

  memcpy_s(p1, len - 2, &salt, NUMBER_TO_GENERATE);
  p1 += NUMBER_TO_GENERATE;

  memcpy_s(p1, len - 2 - NUMBER_TO_GENERATE, content_data, content_data_length);


  unsigned int sha256_len = 0;
  //Calculate the MessageDigest

  EVP_MD_CTX    messageDigestContext;

  unsigned char messageDigest[EVP_MAX_MD_SIZE];

  EVP_DigestInit(&messageDigestContext, EVP_sha256());
  EVP_DigestUpdate(&messageDigestContext, temp_buffer, (content_data_length + NUMBER_TO_GENERATE + 2));
  EVP_DigestFinal(&messageDigestContext, messageDigest, &sha256_len);

  memcpy_s(p, hashed_length - 2 - NUMBER_TO_GENERATE, messageDigest, sha256_len);

  //printf("MessageDigest : %s\n",p);

  //printf("Algo+Salt+HMAC : %s\n",hashed_buffer);
  free(temp_buffer);

  sha256_len = sha256_len + NUMBER_TO_GENERATE + 2;
  return sha256_len;

}

int ValidateHash(const unsigned char * content_data, unsigned long content_data_length,
                 const unsigned char * hashed_buffer, unsigned long hashed_buffer_length) {
  //hashed_buffer is the onewayhash value
  //content_buffer is the plaintext
  //ValidateHash function will Generate the onewayhash for the content-buffer and then compare the value with the hashed_buffer
  //If the result matched it will return SUCCESS
  if(  hashed_buffer == NULL || hashed_buffer_length < (2 + NUMBER_TO_GENERATE + SHA256_LENGTH)) {
    return CRYPTO_ERROR;
  }
  if(  content_data == NULL || content_data_length == 0) {
    return CRYPTO_ERROR;
  }

  //0.hashed_buffer should be base64decoded as retrieve from DB to character string before calling this method

  unsigned long decode_len = owh_xbase64_calc_decode_buf_size(hashed_buffer_length);
  if(!decode_len) {
    return CRYPTO_ERROR;
  }
  unsigned char * decode_data = NULL;
  decode_data = (unsigned char *) malloc(decode_len);
  decode_len = owh_xbase64_decode((unsigned char *)hashed_buffer, hashed_buffer_length, decode_data, decode_len);
  printf("Decoded String and Length: %d\n", decode_len);
  int i = 0;
  for(i = 0; i < decode_len; i++) {
    printf("%c", decode_data[i]);
  }
  printf("\n");
  decode_len = 2 + NUMBER_TO_GENERATE + SHA256_LENGTH;

  //1. Retrieve Salt from hashed_buffer
  int salt_length = NUMBER_TO_GENERATE;
  unsigned char salt_bytes[33];
  memset_s(salt_bytes, 33, 0);

  int crypto_Result = RetrieveSalt(decode_data, decode_len, salt_bytes, salt_length);
  free(decode_data);

  if(crypto_Result == CRYPTO_SUCCESS) {
    //2. Generate the hash with the Salt value
    int length = 2 + NUMBER_TO_GENERATE + SHA256_LENGTH;
    unsigned char buffer[length + 1];
    memset_s(buffer, length + 1, 0);

    if(!GenerateOneWayHashWithSalt(content_data, content_data_length, buffer, length, (char *)salt_bytes)) {
      printf("Failed to GenerateHashWithSalt\n");
      return CRYPTO_ERROR;
    }


    //3. Encode to Base64

    printf("OWH is Length is %d\n", length);
    for(i = 0; i < length; i++) {
      printf("%c", buffer[i]);
    }
    printf("\n");
    unsigned long encoded_len = owh_xbase64_calc_encode_buf_size((unsigned long)length);

    if(encoded_len > 0) {
      unsigned char * encoded_buffer = NULL;
      encoded_buffer  = (unsigned char *) malloc(encoded_len + 1);
      owh_xbase64_encode(buffer, length, encoded_buffer, encoded_len);
      encoded_buffer[encoded_len] = 0;
      printf("Encoded Length:%d \n", encoded_len);
      printf("Encoded Buffer:");
      for(i = 0; i < encoded_len; i++) {
        printf("%c", encoded_buffer[i]);
      }
      printf("\n");

      //4. Compare the hash values
      int indicator = 1;
      if(memcmp_s(hashed_buffer, hashed_buffer_length, encoded_buffer, encoded_len, &indicator)) {
        printf("The Hash value Matched \n");
        free(encoded_buffer);
        return CRYPTO_SUCCESS;

      } else {
        printf("Failed to retrieve Salt from the stored password\n");
        free(encoded_buffer);
        return CRYPTO_ERROR;
      }
    }


  } else {
    printf("Failed to retrieve Salt from the stored password\n");
    return CRYPTO_ERROR;

  }

  return CRYPTO_SUCCESS;

}

int GenerateSalt(unsigned char * salt, int length) {
  if(length < NUMBER_TO_GENERATE) {
    return CRYPTO_ERROR;
  }

  unsigned char rand_bytes[33];
  memset_s(rand_bytes, 33, 0);

  unsigned char seed[17];
  memset_s(seed, 17, 0);

  RAND_seed(seed, length);
  RAND_bytes(rand_bytes, NUMBER_TO_GENERATE);

  MD5(rand_bytes, 32, seed);
  memcpy_s(salt, (NUMBER_TO_GENERATE - 16), seed, 16);
  RAND_bytes(rand_bytes, 32);
  MD5(rand_bytes, 32, seed);
  memcpy_s(salt + 16, (NUMBER_TO_GENERATE - 16), rand_bytes, 16);


  return CRYPTO_SUCCESS;


}

int RetrieveSalt(const unsigned char * hashed_buffer, int hashed_length, unsigned char * salt, int length) {
  if(hashed_length < length) {
    return CRYPTO_ERROR;
  }

  memcpy_s(salt, hashed_length, hashed_buffer + 2, length);

  return CRYPTO_SUCCESS;
}


int seed_rand_generator(const void * buffer, int length) {
  if (length < RAND_SEED_LENGTH) {
    return CRYPTO_ERROR;
  }

  RAND_seed(buffer, length);
  return CRYPTO_SUCCESS;  // by Elton
}

int generate_rand_bytes(unsigned char * buffer, int length) {
  if (RAND_bytes(buffer, length) <= 0 ) {
    return CRYPTO_ERROR;
  }

  return CRYPTO_SUCCESS;
}


#if 0
unsigned char * wbx_hex_to_string(char * str, long * len) {
  unsigned char * hexbuf, *q;
  unsigned char ch, cl, *p;
  if (!str) {
    return NULL;
  }
  if (!(hexbuf = malloc(strlen(str) >> 1))) {
    goto err;
  }
  for (p = (unsigned char *)str, q = hexbuf; *p;) {
    ch = *p++;
    if (ch == ':') {
      continue;
    }
    cl = *p++;
    if (!cl) {
      free(hexbuf);
      return NULL;
    }
    if (isupper(ch)) {
      ch = tolower(ch);
    }
    if (isupper(cl)) {
      cl = tolower(cl);
    }
    if ((ch >= '0') && (ch <= '9')) {
      ch -= '0';
    } else if ((ch >= 'a') && (ch <= 'f')) {
      ch -= 'a' - 10;
    } else {
      goto badhex;
    }
    if ((cl >= '0') && (cl <= '9')) {
      cl -= '0';
    } else if ((cl >= 'a') && (cl <= 'f')) {
      cl -= 'a' - 10;
    } else {
      goto badhex;
    }
    *q++ = (ch << 4) | cl;
  }
  if (len) {
    *len = q - hexbuf;
  }
  return hexbuf;

err:
  if (hexbuf) {
    free(hexbuf);
  }
  return NULL;
badhex:
  free(hexbuf);
  return NULL;

}
#endif


void wbx_string_to_hex(char * buffer, int bufferSize, char * output) {
  static char hexmap[] = "0123456789abcdef";

  const char * p = buffer;
  char * r = output;
  int i = 0;
  for (i = 0; i < bufferSize; i++) {
    unsigned char temp = *p++;

    int hi = (temp & 0xf0) >> 4;
    int low = (temp & 0xf);

    *r++ = hexmap[hi];
    *r++ = hexmap[low];
  }
  *r = 0;
}


// Base64 encode/decode
//
static char s_base64[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static unsigned char s_base64_decode_table[256] = { 0 };

void owh_xbase64_init_decode_table() {
  int i;
  for(i = 0; i < 256; i++) {
    s_base64_decode_table[i] = 0;
  }

  unsigned char code = 0;
  for(i = (int)'A'; i <= (int)'Z'; i++) {
    s_base64_decode_table[i] = code;
    code++;
  }

  for(i = (int)'a'; i <= (int)'z'; i++) {
    s_base64_decode_table[i] = code;
    code++;
  }

  for(i = '0'; i <= '9'; i++) {
    s_base64_decode_table[i] = code;
    code++;
  }
  s_base64_decode_table[(int)'+'] = code++;
  s_base64_decode_table[(int)'/'] = code++;

  if(code == 64) {
    printf("Initialize table Done\n");
  }
}

unsigned long owh_xbase64_calc_encode_buf_size(unsigned long data_len) {
  //return ((data_len / 3) + 1) * 4;
  return ((data_len / 3) + 1) << 2;
}

unsigned long owh_xbase64_calc_decode_buf_size(unsigned long data_len) {
  return (data_len >> 2) * 3;
}

unsigned long owh_xbase64_encode(unsigned char * data, unsigned long data_len,
                                 unsigned char * buf, unsigned long buf_len) {
  if(buf == NULL || data == NULL || (buf_len < owh_xbase64_calc_encode_buf_size(data_len))) {
    return 0;
  }

  unsigned char * cur_data = data;
  unsigned char * cur_buf = buf;
  unsigned long i;
  for(i = 0; i < data_len / 3; i++) {
    cur_buf[0] = (unsigned char)s_base64[(cur_data[0] & 0xFC) >> 2];
    cur_buf[1] = (unsigned char)s_base64[((cur_data[0] & 0x03) << 4) | ((cur_data[1] & 0xF0) >> 4)];
    cur_buf[2] = (unsigned char)s_base64[((cur_data[1] & 0x0F) << 2) | ((cur_data[2] & 0xC0) >> 6)];
    cur_buf[3] = (unsigned char)s_base64[cur_data[2] & 0x3F];

    cur_buf  += 4;
    cur_data += 3;
  }

  switch(data_len % 3) {
  case 0 :
    break;

  case 1 :
    cur_buf[0] = (unsigned char)s_base64[(cur_data[0] & 0xFC) >> 2];
    cur_buf[1] = (unsigned char)s_base64[(cur_data[0] & 0x03) << 4];
    cur_buf[2] = (unsigned char)'=';
    cur_buf[3] = (unsigned char)'=';

    cur_buf += 4;
    break;

  case 2 :
    cur_buf[0] = (unsigned char)s_base64[(cur_data[0] & 0xFC) >> 2];
    cur_buf[1] = (unsigned char)s_base64[((cur_data[0] & 0x03) << 4) | ((cur_data[1] & 0xF0) >> 4)];
    cur_buf[2] = (unsigned char)s_base64[(cur_data[1] & 0x0F) << 2];
    cur_buf[3] = (unsigned char)'=';

    cur_buf += 4;
    break;
  }

  return cur_buf - buf;
}

unsigned long owh_xbase64_decode(unsigned char * buf, unsigned long buf_len,
                                 unsigned char * data, unsigned long data_len) {
  if(buf == NULL || data == NULL || (buf_len % 4 == 0)) {
    return 0;
  }

  unsigned char * cur_buf = buf;
  unsigned char * cur_data = data;
  unsigned long i = 0;
  for(i = 0; i < buf_len / 4; i++) {
    unsigned char bA, bB, bC, bD;
    bA = s_base64_decode_table[cur_buf[0]];
    bB = s_base64_decode_table[cur_buf[1]];
    bC = s_base64_decode_table[cur_buf[2]];
    bD = s_base64_decode_table[cur_buf[3]];

    cur_data[0] = (bA << 2) | (bB >> 4);
    cur_data[1] = ((bB & 0x0F) << 4) | (bC >> 2);
    cur_data[2] = ((bC & 0x03) << 6) | bD;

    if(cur_buf[2] != '=' && cur_buf[3] != '=') {
      cur_data += 3;
    } else {
      if(cur_buf[2] != '=') {
        cur_data[2] = 0;
        cur_data += 2;
      } else {
        cur_data[2] = 0;
        cur_data[1] = 0;
        cur_data += 1;
      }
    }

    cur_buf += 4;
  }

  return cur_data - data;
}

