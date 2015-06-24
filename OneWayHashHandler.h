#ifndef _OneWayHashHandler_H__
#define _OneWayHashHandler_H__



#ifdef __cplusplus
extern "C" {
#endif

int GenerateOneWayHash(const unsigned char * content_data, unsigned long content_data_length,
                       unsigned char * hashed_buffer, unsigned long hashed_buffer_length);
int GenerateOneWayHashWithSalt(const unsigned char * content_data, unsigned long content_data_length,
                               unsigned char * hashed_buffer, unsigned long hashed_length, const char * salt);
int ValidateHash(const unsigned char * content_data, unsigned long content_data_length,
                 unsigned const char * hashed_buffer, unsigned long hashed_buffer_length);
int GenerateSalt(unsigned char * salt, int length);
int RetrieveSalt(const unsigned char * hashed_buffer, int hashed_length, unsigned char * salt, int length);
int wbx_seed_rand_generator(const void * buffer, int length);
int wbx_generate_rand_bytes(unsigned char * buffer, int length);
unsigned char * wbx_hex_to_string(char * str, long * len);
void wbx_string_to_hex(char * buffer, int bufferSize, char * output);

//base64 helper functions
void owh_xbase64_init_decode_table();
unsigned long owh_xbase64_calc_encode_buf_size(unsigned long len);
unsigned long owh_xbase64_calc_decode_buf_size(unsigned long len);
unsigned long owh_xbase64_encode( unsigned char * data, unsigned long data_len, unsigned char * buf,
                                  unsigned long buf_len);
unsigned long owh_xbase64_decode( unsigned char * buf, unsigned long buf_len,  unsigned char * data,
                                  unsigned long data_len);
unsigned long owh_xbase64_n_url_decode( unsigned char ** buf, unsigned long buf_len,  unsigned char ** data,
                                        unsigned long);


#ifdef __cplusplus
}
#endif

#endif // _OneWayHashHandler_H__

