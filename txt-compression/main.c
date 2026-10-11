#include "../binary-tree/heap.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define CHUNK_SIZE 1024 * 64
#define ASCII_SIZE 128

int create_heap_nodes(struct heap_node **nodes, int char_counts[]) {
  size_t max_size = ASCII_SIZE * sizeof(struct heap_node);
  struct heap_node *nodes_ptr = malloc(max_size);

  int skipped = 0;
  for (int i = 0; i < ASCII_SIZE; i++) {
    int char_count = char_counts[i];
    int insert_idx = i - skipped;
    char ascii_code = i;

    if (char_count) {
      nodes_ptr[insert_idx].weight = char_count;
      nodes_ptr[insert_idx].data = ascii_code;
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

int main(void) {
  int char_counts[ASCII_SIZE] = {0};
  FILE *fptr = fopen("raw.txt", "rb");
  aggregate_char_counts(char_counts, fptr);

  if (ferror(fptr)) {
    printf("error\n");
  } else {
    fclose(fptr);
  }

  struct heap_node *nodes = 0;
  int nodes_count = create_heap_nodes(&nodes, char_counts);
  struct heap h = min_heap_init(nodes, nodes_count);
  print_heap(&h);

  free(nodes);
  free(h.nodes);

  return 0;
}