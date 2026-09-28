#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// IDAT chunks can appear in any order

// todo:
// print image metadata
// read IDAT

//       to ensure early detection of common file-transfer problems,
//       decoders should verify that all eight bytes of the PNG file
//       signature are correct.  (See Rationale: PNG file signature,
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

#define SIGNATURE_BYTES_COUNT 8
#define IHDR_BYTES_COUNT 21
#define FILE_NAME "image.png"
#define EOF_CHUNK "IEND"

struct chunk {
  int data_len;
  char *type;
  unsigned char *data;
  int crc;
};

// todo: compare using AND

int signature_valid(FILE *fptr) {
  unsigned char bytes[SIGNATURE_BYTES_COUNT] = {0};
  int bytes_read = fread(bytes, 1, SIGNATURE_BYTES_COUNT, fptr);

  return bytes_read == SIGNATURE_BYTES_COUNT && bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4E &&
         bytes[3] == 0x47 && bytes[4] == 0x0D && bytes[5] == 0x0A && bytes[6] == 0x1A && bytes[7] == 0x0A;
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

  // IHDR_chunk_valid

  if (!signature_valid(fptr)) {
    printf("specified file is not a valid png\n");
    return 0;
  }

  // struct chunk *IHDR_chunk = read_chunk(fptr);

  int chunks_count = 0;

  do {
    chunks_count++;
  } while (strcmp(EOF_CHUNK, read_chunk(fptr)->type));

  printf("chunks count: %i\n", chunks_count);

  // int width = (bytes[8] << 24) + (bytes[9] << 16) + (bytes[10] << 8) + bytes[11];
  // int height = (bytes[12] << 24) + (bytes[13] << 16) + (bytes[14] << 8) + bytes[15];
  // printf("%i x %i\n", width, height); // 591 0x024F x 334 014E

  return 0;
}