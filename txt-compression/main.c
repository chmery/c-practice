#include "../binary-tree/heap.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define CHUNK_SIZE 1024 * 64
#define ASCII_SIZE 128

struct huffman_node {
  int ascii_code;
  struct huffman_node *left;
  struct huffman_node *right;
};

struct huffman_table_item {
  int ascii_code;
  int huffman_code;
  int code_len;
};

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

void print_encoded_text(uint8_t *text, size_t size) {
  for (size_t i = 0; i < size; i++) {
    printf("%x ", text[i]);
  }
  printf("\n");
}

size_t calc_encoded_text_size(int char_counts[], struct huffman_table_item huffman_table[]) {
  float bits = 0;

  for (int i = 0; i < ASCII_SIZE; i++) {
    if (!char_counts[i]) {
      continue;
    }

    bits += huffman_table[i].code_len * char_counts[i];
  }

  return ceilf(bits / 8);
}

void fill_huffman_table(struct huffman_node *root, struct huffman_table_item table[], int depth, int code) {
  if (root->ascii_code) {
    struct huffman_table_item table_item = {root->ascii_code, code, depth};
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

void free_huffman_tree(struct huffman_node *root) {
  if (root->ascii_code) {
    free(root);
    return;
  }

  if (root->left) {
    free_huffman_tree(root->left);
  }

  if (root->right) {
    free_huffman_tree(root->right);
  }

  free(root);
}

int counts_to_heap_nodes(struct heap_node **nodes, int char_counts[]) {
  size_t max_size = ASCII_SIZE * sizeof(struct heap_node);
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

// TODO: get file size
void aggregate_char_counts(int char_counts[], FILE *fptr) {
  uint8_t buf[CHUNK_SIZE] = {0};

  while (!feof(fptr)) {
    size_t bytes_read = fread(buf, 1, CHUNK_SIZE, fptr);
    for (size_t i = 0; i < bytes_read; i++) {
      char_counts[buf[i]]++;
    }
  }
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

void encode_text(FILE *fptr, uint8_t *encoded_text, struct huffman_table_item table[]) {
  uint8_t buf[CHUNK_SIZE] = {0};

  int byte_idx = 0;
  int start_bit = 8;

  while (!feof(fptr)) {
    size_t bytes_read = fread(buf, 1, CHUNK_SIZE, fptr);
    for (size_t i = 0; i < bytes_read; i++) {
      int code = table[buf[i]].huffman_code;
      int code_len = table[buf[i]].code_len;
      write_code_into_bytes(encoded_text, &byte_idx, &start_bit, code, code_len);
    }
  }
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

int main(void) {
  int char_counts[ASCII_SIZE] = {0};
  FILE *fptr = fopen("raw.txt", "rb");
  aggregate_char_counts(char_counts, fptr);

  if (ferror(fptr)) {
    printf("file error\n");
    return 1;
  }

  struct heap_node *queue_nodes = 0;
  int queue_nodes_count = counts_to_heap_nodes(&queue_nodes, char_counts);
  struct heap priority_queue = heap_init(queue_nodes, queue_nodes_count, MIN);
  free(queue_nodes);

  struct huffman_node *root_node = create_huffman_tree(&priority_queue);
  heap_free(&priority_queue);

  struct huffman_table_item huffman_table[ASCII_SIZE] = {0};
  fill_huffman_table(root_node, huffman_table, 0, 0);
  free_huffman_tree(root_node);
  print_huffman_table(huffman_table);

  size_t encoded_text_size = calc_encoded_text_size(char_counts, huffman_table);
  printf("encoded size: %zu bytes\n", encoded_text_size);

  rewind(fptr);
  uint8_t *encoded_text = calloc(encoded_text_size, sizeof(*encoded_text));
  encode_text(fptr, encoded_text, huffman_table);
  print_encoded_text(encoded_text, encoded_text_size);

  free(encoded_text);
  fclose(fptr);

  return 0;
}