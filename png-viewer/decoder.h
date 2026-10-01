#include <stdio.h>
#include <sys/types.h>

// idea: #define SIG_N_HDR_BYTES_COUNT 33
#define EOF_CHUNK "IEND"
#define IHDR_CHUNK "IHDR"
#define IHDR_CHUNK_DATA_LEN 13

struct chunk {
  u_int32_t data_len;
  char type[5];
  u_int8_t *data;
  u_int32_t crc;
};

struct ihdr_data {
  u_int32_t width;
  u_int32_t height;
  u_int8_t bit_depth;
  u_int8_t color_type;
  u_int8_t compression_method;
  u_int8_t filter_method;
  u_int8_t interlace_method;
};

struct ihdr_data *read_png_header(char *path);
//int signature_valid(FILE *fptr);
//int signature_valid(u_int8_t buf[]);
//int ihdr_chunk_valid(struct chunk *ihdr_chunk);
//struct ihdr_data extract_ihdr_data(struct chunk *chunk);
//void free_chunk(struct chunk *chunk);
//struct chunk *read_chunk(FILE *fptr);

