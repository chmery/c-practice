#ifndef SHARED_H
#define SHARED_H

#include <stddef.h>
#include <stdint.h>

#define CHUNK_SIZE 65536 // 1024 * 64
#define ASCII_SIZE 128
#define DATA_HEADER_SIZE 5
#define TABLE_ITEM_SIZE 6

struct huffman_node {
  uint8_t ascii_code;
  struct huffman_node *left;
  struct huffman_node *right;
};

struct huffman_table_item {
  uint8_t ascii_code;
  uint8_t code_len;
  int huffman_code;
};

struct encoded_size {
  uint32_t bytes;
  uint8_t padding_bits;
};

void free_huffman_tree(struct huffman_node *root);

#endif