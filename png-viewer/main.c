#include <endian.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// sRGB standard rgb displaying
// gAMA This chunk is ignored unless it is the highest-precedence color chunk understood by the decoder.
// pHYs specifies the intended pixel size or aspect ratio for display of the image
// it contains px per unit x, px per unit y 4B each, unit specifier 1B

// todo:
// read IDAT
// verify CRC

// IDAT chunks can appear in any order
// It is strongly recommended that decoders should verify the CRC on
// each chunk.

#define FILE_NAME "image.png"
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

void print_bytes(void *value, size_t n) {
  for (unsigned i = 0; i < n; i++) {
    printf("%02X ", ((unsigned char *)value)[i]);
  }
  printf("\n");
}

int signature_valid(FILE *fptr) {
  const int signature_bytes_count = 8;
  const u_int32_t signature_l_bytes = be32toh(0x89504E47);
  const u_int32_t signature_r_bytes = be32toh(0x0D0A1A0A);

  u_int32_t bytes[2] = {0};
  fread(bytes, signature_bytes_count, 1, fptr);

  return bytes[0] == signature_l_bytes && bytes[1] == signature_r_bytes;
}

int ihdr_chunk_valid(struct chunk *ihdr_chunk) {
  int valid_type = strcmp(ihdr_chunk->type, IHDR_CHUNK) == 0;
  int valid_len = ihdr_chunk->data_len == IHDR_CHUNK_DATA_LEN;
  return valid_type && valid_len;
}

struct ihdr_data extract_ihdr_data(struct chunk *chunk) {
  struct ihdr_data data = {0};

  data.width = be32toh(((u_int32_t *)chunk->data)[0]);
  data.height = be32toh(((u_int32_t *)chunk->data)[1]);
  memcpy(&data.bit_depth, &chunk->data[8], 5);

  return data;
}

void free_chunk(struct chunk *chunk) {
  free(chunk->data);
  free(chunk);
}

struct chunk *read_chunk(FILE *fptr) {
  struct chunk *chunk_ptr = malloc(sizeof(*chunk_ptr));
  u_int32_t chunk_buf[2] = {0}; // store chunk length and type bytes

  fread(chunk_buf, 8, 1, fptr);
  chunk_ptr->data_len = be32toh(chunk_buf[0]);
  memcpy(chunk_ptr->type, &chunk_buf[1], 4);
  chunk_ptr->type[4] = '\0';

  chunk_ptr->data = malloc(chunk_ptr->data_len);
  fread(chunk_ptr->data, chunk_ptr->data_len, 1, fptr);
  fread(&chunk_ptr->crc, 4, 1, fptr);

  printf("%s, data size: %i bytes, total size: %i bytes\n", chunk_ptr->type, chunk_ptr->data_len,
         4 + 4 + 4 + chunk_ptr->data_len);

  return chunk_ptr;
}

int main(void) {
  FILE *fptr = fopen(FILE_NAME, "rb");
  if (!fptr) {
    return EXIT_FAILURE;
  }

  if (!signature_valid(fptr)) {
    printf("specified file is not a valid png\n");
    return 0;
  }

  struct chunk *ihdr_chunk = read_chunk(fptr);
  if (!ihdr_chunk_valid(ihdr_chunk)) {
    printf("specified file is not a valid png\n");
    return 0;
  }

  struct ihdr_data header_data = extract_ihdr_data(ihdr_chunk);
  free_chunk(ihdr_chunk);

  printf("%ipx x %ipx\ndepth: %i\ncolor: %i\n", header_data.width, header_data.height, header_data.bit_depth,
         header_data.color_type);

  int chunks_count = 1;

  do {
    chunks_count++;
  } while (strcmp(EOF_CHUNK, read_chunk(fptr)->type));

  fclose(fptr);
  return 0;
}