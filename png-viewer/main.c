#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// IDAT chunks can appear in any order

// todo:
// print image metadata
// read IDAT

#define SIGNATURE_BYTES_COUNT 8
#define IHDR_BYTES_COUNT 21
#define FILE_NAME "image.png"
#define EOF_CHUNK "IEND"

// struct chunk_type_code {
//   char b3;
//   char b2;
//   char b1;
//   char b0;
// };

// struct chunk {
//   unsigned data_len;
//   struct chunk_type_code type_code;
// }

// chunk build:
// 4 bytes uint meaning num of bytes in data
// 4 bytes chunk type
// x bytes of data
// 4 bytes of crc

int is_png(FILE *fptr) {
  unsigned char bytes[SIGNATURE_BYTES_COUNT] = {0};
  int bytes_read = fread(bytes, 1, SIGNATURE_BYTES_COUNT, fptr);

  return bytes_read == SIGNATURE_BYTES_COUNT && bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4E &&
         bytes[3] == 0x47 && bytes[4] == 0x0D && bytes[5] == 0x0A && bytes[6] == 0x1A && bytes[7] == 0x0A;
}

// convert array of 4 bytes to 4 byte int
int bytes_to_int(unsigned char bytes[]) {
  return (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
}

char *get_chunk_type(unsigned char *chunk_rest) {
  char *type = malloc(4 * sizeof(*type));

  for (int i = 0; i < 4; i++) {
    type[i] = chunk_rest[i];
  }

  return type;
}

char *read_chunk(FILE *fptr) {
  unsigned char len_bytes[4] = {0};
  fread(len_bytes, 1, 4, fptr);
  int data_len = bytes_to_int(len_bytes);
  int chunk_rest_size = data_len + 4 + 4;

  unsigned char *chunk_rest = malloc(chunk_rest_size);
  fread(chunk_rest, 1, chunk_rest_size, fptr);

  char *type = get_chunk_type(chunk_rest);
  printf("readin: %s, size: %uB\n", type, 4 + chunk_rest_size);

  return type;
}

int main(void) {
  FILE *fptr = fopen(FILE_NAME, "rb");
  if (!fptr) {
    return EXIT_FAILURE;
  }

  if (!is_png(fptr)) {
    printf("specified file is not a valid png\n");
    return 0;
  }

  int chunks_count = 0;

  do {
    chunks_count++;
  } while (strcmp(EOF_CHUNK, read_chunk(fptr)));

  printf("chunks count: %i\n", chunks_count);

  // int width = (bytes[8] << 24) + (bytes[9] << 16) + (bytes[10] << 8) + bytes[11];
  // int height = (bytes[12] << 24) + (bytes[13] << 16) + (bytes[14] << 8) + bytes[15];
  // printf("%i x %i\n", width, height); // 591 0x024F x 334 014E

  return 0;
}