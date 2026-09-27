#include <stdio.h>
#include <stdlib.h>

#define SIGNATURE_BYTES_COUNT 8
#define FILE_NAME "image.png"

int is_png(FILE *fptr) {
  unsigned char bytes[SIGNATURE_BYTES_COUNT] = {0};
  int bytes_read = fread(bytes, 1, SIGNATURE_BYTES_COUNT, fptr);

  return bytes_read == SIGNATURE_BYTES_COUNT && bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4E &&
         bytes[3] == 0x47 && bytes[4] == 0x0D && bytes[5] == 0x0A && bytes[6] == 0x1A && bytes[7] == 0x0A;
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

  // unsigned char bytes[SIGNATURE_BYTES_COUNT] = {0};

  // int bytes_read = fread(bytes, 1, SIGNATURE_BYTES_COUNT, fptr);
  // if (bytes_read == SIGNATURE_BYTES_COUNT) {
  //   for (int i = 0; i < SIGNATURE_BYTES_COUNT; i++) {
  //     printf("%02x ", bytes[i]);
  //   }
  //   printf("\n");
  // }

  return 0;
}