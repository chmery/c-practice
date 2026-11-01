#include "decoder.h"
#include "encoder.h"
#include <stdio.h>
#include <string.h>

#define COMPRESS_FLAG "-c"
#define DECOMPRESS_FLAG "-d"

void print_usage() {
  printf("usage: ./main <-c | -d> <filename>\n -c  compress file\n -d  decompress file\n");
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    print_usage();
    return 1;
  }

  if (strcmp(argv[1], COMPRESS_FLAG) == 0) {
    compress_ascii_text(argv[2], "out");
    return 0;
  } else if (strcmp(argv[1], DECOMPRESS_FLAG) == 0) {
    decode_ascii_text(argv[2], "out.txt");
    return 0;
  } else {
    print_usage();
    return 1;
  }
}