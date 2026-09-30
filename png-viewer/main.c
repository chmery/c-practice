#include <endian.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// IDAT chunks can appear in any order

// IHDR
// 4b width
// 4b height

// 1b bit depth
// number of bits used to encode color of a single pixel OR
// number of bits used for each color component of a single pixel
// but in png it describes sample or pallete index not single pixel

// Valid values are 1, 2, 4, 8, and 16

// 1b color type
//  represent sums of the following values: 1 (palette used), 2 (color used),
//  and 4 (alpha channel used). Valid values are 0, 2, 3, 4, and 6.

// 1b comp // always 0
// 1b filter // always 0
// 1b interlace // 0 no 1 yes adam7

// todo:
// print image metadata
// read IDAT
// verify CRC

//       Section 12.11.) A decoder can have additional confidence in the
//       file's integrity if the next eight bytes are an IHDR chunk header
//       with the correct chunk length.

// It is strongly recommended that decoders should verify the CRC on
// each chunk.

#define FILE_NAME "image.png"
#define EOF_CHUNK "IEND"
#define IHDR_CHUNK "IHDR"
#define IHDR_CHUNK_DATA_LEN 13

struct chunk {
  u_int32_t data_len;
  char *type;
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

char *type_bytes_to_string(u_int32_t chunk_type_bytes) {
  char *type = malloc(5 * sizeof(*type));

  for (int i = 0; i < 4; i++) {
    type[i] = ((unsigned char *)&chunk_type_bytes)[i];
  }
  type[4] = '\0';

  return type;
}

struct ihdr_data extract_ihdr_data(struct chunk *chunk) {
  struct ihdr_data data = {0};

  data.width = be32toh(((u_int32_t *)chunk->data)[0]);
  data.height = be32toh(((u_int32_t *)chunk->data)[1]);
  memcpy(&data.bit_depth, &chunk->data[8], 5);

  return data;
}

void free_chunk(struct chunk *chunk) {
  free(chunk->type);
  free(chunk->data);
  free(chunk);
}

struct chunk *read_chunk(FILE *fptr) {
  struct chunk *chunk_ptr = malloc(sizeof(*chunk_ptr));
  u_int32_t chunk_buf[2] = {0}; // store chunk length and type bytes

  fread(chunk_buf, 8, 1, fptr);
  chunk_ptr->data_len = be32toh(chunk_buf[0]);
  chunk_ptr->type = type_bytes_to_string(chunk_buf[1]);
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

  fclose(fptr);

  // int chunks_count = 1;

  // do {
  //   chunks_count++;
  // } while (strcmp(EOF_CHUNK, read_chunk(fptr)->type));

  return 0;
}