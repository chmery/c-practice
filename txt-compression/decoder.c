#include "shared.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void create_subtree(struct huffman_node *n, uint8_t *nodes_arr, int *idx) {
  n->ascii_code = nodes_arr[*idx];
  if (n->ascii_code) {
    return;
  }

  (*idx)++;
  n->left = malloc(sizeof(*n->left));
  create_subtree(n->left, nodes_arr, idx);

  (*idx)++;
  n->right = malloc(sizeof(*n->right));
  create_subtree(n->right, nodes_arr, idx);
}

struct huffman_node *recreate_huffman_tree(FILE *fptr) {
  uint8_t nodes_count = 0;
  fread(&nodes_count, 1, 1, fptr);

  uint8_t *nodes_arr = malloc(nodes_count);
  fread(nodes_arr, nodes_count, 1, fptr);

  struct huffman_node *root = calloc(1, sizeof(*root));
  int idx = 0;

  create_subtree(root, nodes_arr, &idx);
  free(nodes_arr);

  return root;
}

struct encoded_size get_encoded_size(FILE *fptr) {
  struct encoded_size es = {0};
  uint32_t buf[2] = {0};
  fread(buf, DATA_HEADER_SIZE, 1, fptr);
  es.bytes = buf[0];
  es.padding_bits = buf[1];
  return es;
}

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

  struct huffman_node *root_node = recreate_huffman_tree(input_fptr);
  struct encoded_size es = get_encoded_size(input_fptr);

  uint8_t *encoded_buf = malloc(CHUNK_SIZE);
  fread(encoded_buf, CHUNK_SIZE, 1, input_fptr);
  char *str = decode_text(root_node, encoded_buf, es);
  printf("%s\n", str);

  fclose(input_fptr);
  free(encoded_buf);
  free(str);
  free_huffman_tree(root_node);
}