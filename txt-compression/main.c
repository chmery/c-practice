#include "../binary-tree/heap.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define CHUNK_SIZE 1024 * 64
#define ASCII_SIZE 128

struct huffman_node {
  char ascii_code;
  struct huffman_node *left;
  struct huffman_node *right;
};

struct huffman_table_item {
  char ascii_code;
  char huffman_code;
  char code_len;
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

    printf("idx: %i char: '%c' code: ", i, table[i].ascii_code);
    print_huffman_code(table[i]);
    printf("\n");
  }
}

size_t calc_encoded_data_size(int char_counts[], struct huffman_table_item huffman_table[]) {
  float bits = 0;

  for (int i = 0; i < ASCII_SIZE; i++) {
    if (!char_counts[i]) {
      continue;
    }

    bits += (int)huffman_table[i].code_len * char_counts[i];
  }

  return ceilf(bits / 8);
}

void fill_huffman_table(struct huffman_node *root, struct huffman_table_item table[], int depth, int code) {
  if (root->ascii_code) {
    struct huffman_table_item table_item = {root->ascii_code, code, depth};
    table[(int)root->ascii_code] = table_item;
  }

  if (!root->left && !root->right) {
    free(root);
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

  free(root);
}

int counts_to_heap_nodes(struct heap_node **nodes, int char_counts[]) {
  size_t max_size = ASCII_SIZE * sizeof(struct heap_node);
  struct heap_node *nodes_ptr = malloc(max_size);

  int skipped = 0;
  for (int i = 0; i < ASCII_SIZE; i++) {
    int char_count = char_counts[i];
    int insert_idx = i - skipped;
    char ascii_code = i;

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
  char buf[CHUNK_SIZE] = {0};

  while (!feof(fptr)) {
    size_t bytes_read = fread(buf, 1, CHUNK_SIZE, fptr);
    for (size_t i = 0; i < bytes_read; i++) {
      char_counts[(int)buf[i]]++;
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

  return heap_delete(priority_queue, MIN).data;
}

int main(void) {
  int char_counts[ASCII_SIZE] = {0};
  FILE *fptr = fopen("raw.txt", "rb");
  aggregate_char_counts(char_counts, fptr);

  if (ferror(fptr)) {
    printf("file error\n");
  } else {
    fclose(fptr);
  }

  struct heap_node *queue_nodes = 0;
  int queue_nodes_count = counts_to_heap_nodes(&queue_nodes, char_counts);
  struct heap priority_queue = heap_init(queue_nodes, queue_nodes_count, MIN);
  free(queue_nodes);

  struct huffman_node *root_node = create_huffman_tree(&priority_queue);
  struct huffman_table_item huffman_table[ASCII_SIZE] = {0};
  fill_huffman_table(root_node, huffman_table, 0, 0);
  print_huffman_table(huffman_table);

  size_t encoded_data_size = calc_encoded_data_size(char_counts, huffman_table);
  printf("%zu\n", encoded_data_size);

  heap_free(&priority_queue);

  return 0;
}