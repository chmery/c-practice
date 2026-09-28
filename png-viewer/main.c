// #include <byteswap.h>
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

// todo: check official
u_int32_t be32toh_mine(u_int32_t big_endian_32bits) {
  unsigned short right = ((short *)&big_endian_32bits)[1];
  unsigned short left = ((short *)&big_endian_32bits)[0];

  unsigned short right_swapped = right << 8 | right >> 8;
  unsigned short left_swapped = left << 8 | left >> 8;

  return right_swapped | left_swapped << 16;
}

int signature_valid(FILE *fptr) {
  unsigned char bytes[SIGNATURE_BYTES_COUNT] = {0};
  int bytes_read = fread(bytes, 1, SIGNATURE_BYTES_COUNT, fptr);

  return bytes_read == SIGNATURE_BYTES_COUNT && bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4E &&
         bytes[3] == 0x47 && bytes[4] == 0x0D && bytes[5] == 0x0A && bytes[6] == 0x1A && bytes[7] == 0x0A;
}

char *get_chunk_type(unsigned char *chunk_rest) {
  char *type = malloc(4 * sizeof(*type));

  for (int i = 0; i < 4; i++) {
    type[i] = chunk_rest[i];
  }

  return type;
}

char *read_chunk(FILE *fptr) {
  struct chunk *chunk_ptr = malloc(sizeof(*chunk_ptr));

  int data_len_buf = 0;
  fread(&data_len_buf, 4, 1, fptr);
  chunk_ptr->data_len = be32toh_mine(data_len_buf);

  unsigned char type_buf[4] = {0};
  fread(type_buf, 4, 1, fptr);
  chunk_ptr->type = get_chunk_type(type_buf);

  int chunk_rest_size = chunk_ptr->data_len + 4;
  chunk_ptr->data = malloc(chunk_ptr->data_len);
  fread(chunk_ptr->data, chunk_ptr->data_len, 1, fptr);
  fread(&chunk_ptr->crc, 4, 1, fptr);

  printf("%s, data size: %i bytes, total size: %i bytes\n", chunk_ptr->type, chunk_ptr->data_len,
         4 + 4 + chunk_rest_size);

  return chunk_ptr->type;
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