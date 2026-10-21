#ifndef SHARED_H
#define SHARED_H

#include <stddef.h>
#include <stdint.h>

#define CHUNK_SIZE 1024 * 64
#define ASCII_SIZE 128

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

struct file_header {
  uint8_t table_size;
  uint8_t padding_bits;
  struct huffman_table_item *huffman_table;
};

struct encoded_size {
  size_t bytes;
  uint8_t padding_bits;
};

struct decoder_state {
  struct huffman_node *root;
  struct huffman_node *curr;
  uint32_t insert_idx;
  char *output;
};

#endif