#include "shared.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int read_n_bit(int n, int value) {
  int mask = 1 << (n - 1);
  return value & mask;
}

void decode_byte(struct decoder_state *ds, uint8_t byte, int padding_bits) {
  for (int bit_num = 8; bit_num > padding_bits; bit_num--) {
    int bit = read_n_bit(bit_num, byte);

    if (bit == 0) {
      ds->curr = ds->curr->left;
    } else {
      ds->curr = ds->curr->right;
    }

    if (ds->curr->ascii_code) {
      ds->output[ds->insert_idx] = ds->curr->ascii_code;
      ds->insert_idx++;
      ds->curr = ds->root;
    }
  }
}

// TODO: gib better name
void add_tree_node(struct huffman_node *n, uint8_t *tree_arr, int *idx) {
  n->ascii_code = tree_arr[*idx];
  if (n->ascii_code) {
    return;
  }

  (*idx)++;
  n->left = malloc(sizeof(struct huffman_node));
  add_tree_node(n->left, tree_arr, idx);

  (*idx)++;
  n->right = malloc(sizeof(struct huffman_node));
  add_tree_node(n->right, tree_arr, idx);
}

struct huffman_node *recreate_huffman_tree(uint8_t *tree_arr) {
  struct huffman_node *root = calloc(1, sizeof(*root));
  int idx = 0;

  add_tree_node(root, tree_arr, &idx);
  return root;
}

// this is temp solution, it doesnt make sense to hold lets say 2gb of text in memory
// chunk should get written into file straight away
char *decode_text(struct huffman_node *root_node, uint8_t *encoded_text, struct encoded_size es) {
  struct decoder_state ds = {root_node, root_node, 0, malloc(CHUNK_SIZE)};

  for (size_t i = 0; i < es.bytes - 1; i++) {
    uint8_t byte = encoded_text[i];
    decode_byte(&ds, byte, 0);
  }

  uint8_t last_byte = encoded_text[es.bytes - 1];
  decode_byte(&ds, last_byte, es.padding_bits);
  ds.output[ds.insert_idx] = 0;

  return ds.output;
}

void decode_ascii_text(char *input_path, char *output_path) {
  FILE *input_fptr = fopen(input_path, "rb");

  uint8_t tree_nodes_count = 0;
  fread(&tree_nodes_count, 1, 1, input_fptr);

  uint8_t *tree_nodes_arr = malloc(tree_nodes_count);
  fread(tree_nodes_arr, tree_nodes_count, 1, input_fptr);
  struct huffman_node *root_node = recreate_huffman_tree(tree_nodes_arr);
  free(tree_nodes_arr);

  struct encoded_size es = {0};
  uint32_t es_buf[2] = {0};
  fread(es_buf, DATA_HEADER_SIZE, 1, input_fptr);
  es.bytes = es_buf[0];
  es.padding_bits = es_buf[1];

  uint8_t *encoded_buf = malloc(CHUNK_SIZE);
  fread(encoded_buf, CHUNK_SIZE, 1, input_fptr);
  char *str = decode_text(root_node, encoded_buf, es);
  printf("%s\n", str);

  fclose(input_fptr);
  free(encoded_buf);
  free(str);
  free_huffman_tree(root_node);
}