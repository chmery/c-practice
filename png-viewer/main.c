#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// IDAT chunks can appear in any order

// todo:
// print image metadata
// read IDAT

//       Section 12.11.) A decoder can have additional confidence in the
//       file's integrity if the next eight bytes are an IHDR chunk header
//       with the correct chunk length.

// It is strongly recommended that decoders should verify the CRC on
// each chunk.

// chunk build:
// 4 bytes uint meaning num of bytes in data
// 4 bytes chunk type
// x bytes of data
// 4 bytes of crc

#define FILE_NAME "image.png"
#define EOF_CHUNK "IEND"
#define IHDR_CHUNK_DATA_LEN 13

struct chunk {
  int data_len;
  char *type;
  unsigned char *data;
  int crc;
};

int signature_valid(FILE *fptr) {
  const int signature_bytes_count = 8;
  const u_int32_t signature_l_bytes = be32toh(0x89504E47);
  const u_int32_t signature_r_bytes = be32toh(0x0D0A100A);

  u_int32_t bytes[2] = {0};
  fread(bytes, signature_bytes_count, 1, fptr);

  // return bytes[0] == signature_l_bytes && bytes[1] == signature_r_bytes;

  return !((bytes[0] ^ (signature_l_bytes)) & (bytes[1] ^ (signature_r_bytes)));
}

int ihdr_chunk_valid(struct chunk *ihdr_chunk) {
  int valid_type = strcmp(ihdr_chunk->type, "IHDR") == 0;
  int valid_len = ihdr_chunk->data_len == IHDR_CHUNK_DATA_LEN;
  return valid_type && valid_len;
}

char *type_bytes_to_string(u_int32_t chunk_type_bytes) {
  char *type = malloc(4 * sizeof(*type));

  for (int i = 0; i < 4; i++) {
    type[i] = ((unsigned char *)&chunk_type_bytes)[i];
  }

  return type;
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

  int chunks_count = 1;

  do {
    chunks_count++;
  } while (strcmp(EOF_CHUNK, read_chunk(fptr)->type));

  printf("chunks count: %i\n", chunks_count);

  // int width = (bytes[8] << 24) + (bytes[9] << 16) + (bytes[10] << 8) + bytes[11];
  // int height = (bytes[12] << 24) + (bytes[13] << 16) + (bytes[14] << 8) + bytes[15];
  // printf("%i x %i\n", width, height); // 591 0x024F x 334 014E

  return 0;
}