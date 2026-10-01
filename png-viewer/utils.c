#include <stdio.h>
#include <stdlib.h>

void print_bytes(void *value, size_t n) {
  for (unsigned i = 0; i < n; i++) {
    printf("%02X ", ((unsigned char *)value)[i]);
  }
  printf("\n");
}