#include "../binary-tree/heap.h"
#include "debug.h"
#include "shared.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void aggregate_char_counts(int char_counts[], FILE *fptr) {
  uint8_t buf[CHUNK_SIZE] = {0};

  while (!feof(fptr)) {
    uint32_t bytes_read = fread(buf, 1, CHUNK_SIZE, fptr);
    for (uint32_t i = 0; i < bytes_read; i++) {
      char_counts[buf[i]]++;
    }
  }
}

int counts_to_heap_nodes(struct heap_node **nodes, int char_counts[]) {
  uint32_t max_size = ASCII_SIZE * sizeof(struct heap_node);
  struct heap_node *nodes_ptr = malloc(max_size);

  int skipped = 0;
  for (int i = 0; i < ASCII_SIZE; i++) {
    int char_count = char_counts[i];
    int insert_idx = i - skipped;
    int ascii_code = i;

    if (char_count) {
      nodes_ptr[insert_idx].weight = char_count;
      struct huffman_node *hn = malloc(sizeof(*hn));
      hn->ascii_code = ascii_code;
      hn->left = NULL;
      hn->right = NULL;
      nodes_ptr[insert_idx].data = hn;
    } else {
      skipped++;
    }
  }

  if (skipped == ASCII_SIZE) {
    free(nodes_ptr);
    return 0;
  }

  int nodes_count = ASCII_SIZE - skipped;
  int new_size = max_size - nodes_count * sizeof(struct heap_node);
  struct heap_node *new_nodes_ptr = realloc(nodes_ptr, new_size);
  if (!new_nodes_ptr) {
    return 0;
  }

  *nodes = new_nodes_ptr;
  return nodes_count;
}

struct heap create_priority_queue(int char_counts[]) {
  struct heap_node *nodes = 0;
  int nodes_count = counts_to_heap_nodes(&nodes, char_counts);
  struct heap queue = heap_init(nodes, nodes_count, MIN);
  free(nodes);

  return queue;
}

struct huffman_node *create_huffman_tree(struct heap *priority_queue) {
  while (priority_queue->len > 1) {
    struct heap_node smallest1 = heap_delete(priority_queue, MIN);
    struct heap_node smallest2 = heap_delete(priority_queue, MIN);

    struct huffman_node *hn = malloc(sizeof(*hn));
    hn->ascii_code = 0;
    hn->left = smallest1.data;
    hn->right = smallest2.data;
    struct heap_node sum_node = {smallest1.weight + smallest2.weight, hn};
    heap_insert(priority_queue, sum_node, MIN);
  }

  struct huffman_node *root_node = heap_delete(priority_queue, MIN).data;
  return root_node;
}

void fill_huffman_table(struct huffman_node *root, struct huffman_table_item table[], uint8_t depth, int code) {
  if (root->ascii_code) {
    struct huffman_table_item table_item = {root->ascii_code, depth, code};
    table[root->ascii_code] = table_item;
    return;
  }

  if (root->left) {
    // add 0 to code
    fill_huffman_table(root->left, table, depth + 1, code << 1);
  }

  if (root->right) {
    // add 1 to code
    fill_huffman_table(root->right, table, depth + 1, (code << 1) + 1);
  }
}

struct encoded_size calc_encoded_size(int char_counts[], struct huffman_table_item huffman_table[]) {
  float bits = 0;

  for (int i = 0; i < ASCII_SIZE; i++) {
    if (!char_counts[i]) {
      continue;
    }

    bits += huffman_table[i].code_len * char_counts[i];
  }

  uint32_t bytes = ceilf(bits / 8);
  uint8_t padding_bits = bytes * 8 - bits;
  struct encoded_size es = {bytes, padding_bits};

  return es;
}

int get_n_last_bits(int n, int value) {
  int mask = (1 << n) - 1;
  return value & mask;
}

void write_code_into_bytes(uint8_t *bytes, int *byte_idx, int *start_bit, int code, int code_len) {
  int can_fit = code_len <= *start_bit;

  if (can_fit) {
    uint8_t write_mask = code << (*start_bit - code_len);
    bytes[*byte_idx] = bytes[*byte_idx] | write_mask;

    int byte_full = *start_bit == code_len;
    if (byte_full) {
      *start_bit = 8;
      (*byte_idx)++;
    } else {
      *start_bit = *start_bit - code_len;
    }
  } else {
    int truncated_code_len = code_len - *start_bit;
    int truncated_code = get_n_last_bits(truncated_code_len, code);

    uint8_t write_mask = code >> truncated_code_len;
    bytes[*byte_idx] = bytes[*byte_idx] | write_mask;

    *start_bit = 8;
    (*byte_idx)++;

    write_code_into_bytes(bytes, byte_idx, start_bit, truncated_code, truncated_code_len);
  }
}

void encode(FILE *fptr, uint8_t *encoded_text, struct huffman_table_item table[]) {
  rewind(fptr);
  uint8_t buf[CHUNK_SIZE] = {0};

  int byte_idx = 0;
  int start_bit = 8;

  while (!feof(fptr)) {
    uint32_t bytes_read = fread(buf, 1, CHUNK_SIZE, fptr);
    for (uint32_t i = 0; i < bytes_read; i++) {
      int code = table[buf[i]].huffman_code;
      int code_len = table[buf[i]].code_len;
      write_code_into_bytes(encoded_text, &byte_idx, &start_bit, code, code_len);
    }
  }
}

void pack_huffman_tree_into_array(struct huffman_node *node, uint8_t *arr, int *idx) {
  arr[*idx] = node->ascii_code;
  if (!node->left && !node->right) {
    return;
  }

  (*idx)++;
  pack_huffman_tree_into_array(node->left, arr, idx);

  (*idx)++;
  pack_huffman_tree_into_array(node->right, arr, idx);
}

void write_tree(FILE *output, struct huffman_node *root_node, uint8_t nodes_count) {
  size_t write_size = nodes_count + 1;
  uint8_t *tree = malloc(write_size);
  int idx = 0;

  tree[idx] = nodes_count;
  idx++;

  pack_huffman_tree_into_array(root_node, tree, &idx);
  fwrite(tree, write_size, 1, output);
  free(tree);
}

void write_data(FILE *output, uint8_t *encoded_text, struct encoded_size es) {
  uint8_t *header = malloc(DATA_HEADER_SIZE);

  memcpy(header, &es.bytes, sizeof(es.bytes));
  header[4] = es.padding_bits;

  fwrite(header, DATA_HEADER_SIZE, 1, output);
  fwrite(encoded_text, es.bytes, 1, output);

  free(header);
}

void compress_ascii_text(char *input_path, char *output_path) {
  FILE *input_fptr = fopen(input_path, "rb");

  int char_counts[ASCII_SIZE] = {0};
  aggregate_char_counts(char_counts, input_fptr);

  struct heap priority_queue = create_priority_queue(char_counts);
  int unique_char_counts = priority_queue.len;

  struct huffman_node *root_node = create_huffman_tree(&priority_queue);
  heap_free(&priority_queue);

  struct huffman_table_item huffman_table[ASCII_SIZE] = {0};
  fill_huffman_table(root_node, huffman_table, 0, 0);
  print_huffman_table(huffman_table);

  struct encoded_size encoded_size = calc_encoded_size(char_counts, huffman_table);
  printf("encoded size: %u bytes\n", encoded_size.bytes);

  uint8_t *encoded_text = calloc(encoded_size.bytes, sizeof(*encoded_text));
  encode(input_fptr, encoded_text, huffman_table);
  print_encoded_bytes(encoded_text, encoded_size.bytes);

  FILE *output_fptr = fopen(output_path, "w");
  write_tree(output_fptr, root_node, unique_char_counts * 2 - 1);
  write_data(output_fptr, encoded_text, encoded_size);

  free_huffman_tree(root_node);
  free(encoded_text);
  fclose(input_fptr);
  fclose(output_fptr);
}
