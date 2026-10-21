#include "shared.h"
#include <stdio.h>

void print_huffman_code(struct huffman_table_item table_item) {
  int code_len = table_item.code_len;
  int code = table_item.huffman_code;

  for (int i = 0; i < code_len; i++) {
    printf("%i", (code >> (code_len - 1 - i)) & 1);
  }
}

void print_huffman_table(struct huffman_table_item table[]) {
  for (int i = 0; i < ASCII_SIZE; i++) {
    if (!table[i].ascii_code) {
      continue;
    }

    printf("'%c': ", table[i].ascii_code);
    print_huffman_code(table[i]);
    printf("\n");
  }
}

void print_encoded_bytes(uint8_t *bytes, size_t n) {
  for (size_t i = 0; i < n; i++) {
    printf("%x ", bytes[i]);
  }
  printf("\n");
}