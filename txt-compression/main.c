#include "../binary-tree/heap.h"
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

void print_huffman_tree(struct huffman_node *root) {
  printf("%i ", root->ascii_code);

  if (!root->left && !root->right) {
    free(root);
    return;
  }

  if (root->left) {
    print_huffman_tree(root->left);
  }

  if (root->right) {
    print_huffman_tree(root->right);
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
  print_huffman_tree(root_node);

  heap_free(&priority_queue);

  return 0;
}