#include "shared.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct encoded_chunk {
  uint8_t *data;
  size_t size;
};

struct decoder_state {
  struct huffman_node *root;
  struct huffman_node *curr;
  uint32_t insert_idx;
  uint32_t output_size;
  char *output;
};

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
      ds->output_size++;
    }
  }
}

void decode_chunk(struct decoder_state *ds, struct encoded_chunk *ec, struct encoded_size *es) {
  uint8_t is_last_chunk = ec->size < CHUNK_SIZE;

  for (size_t i = 0; i < (is_last_chunk ? ec->size - 1 : ec->size); i++) {
    uint8_t byte = ec->data[i];
    decode_byte(ds, byte, 0);
  }

  if (is_last_chunk) {
    uint8_t last_byte = ec->data[ec->size - 1];
    decode_byte(ds, last_byte, es->padding_bits);
    ds->output[ds->insert_idx] = 0;
  }
}

void write_decoded_text(FILE *input_fptr, char *output_path, struct huffman_node *root_node, struct encoded_size *es) {
  FILE *output_fptr = fopen(output_path, "w");

  struct decoder_state ds = {root_node, root_node, 0, 0, malloc(CHUNK_SIZE)};
  struct encoded_chunk ec = {malloc(CHUNK_SIZE), 0};
  int chunks_count = ceil((double)es->bytes / CHUNK_SIZE);

  for (int i = 0; i < chunks_count; i++) {
    ds.output_size = 0;
    ds.insert_idx = 0;
    ec.size = fread(ec.data, 1, CHUNK_SIZE, input_fptr);
    decode_chunk(&ds, &ec, es);
    fwrite(ds.output, ds.output_size, 1, output_fptr);
  }

  fclose(output_fptr);
  free(ds.output);
  free(ec.data);
}

void decode_ascii_text(char *input_path, char *output_path) {
  FILE *input_fptr = fopen(input_path, "rb");

  struct huffman_node *root_node = recreate_huffman_tree(input_fptr);
  struct encoded_size es = get_encoded_size(input_fptr);

  write_decoded_text(input_fptr, output_path, root_node, &es);

  fclose(input_fptr);
  free_huffman_tree(root_node);
}