#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define CHUNK_SIZE 1024 * 64
#define ASCII_SIZE 128

struct frequency_node {
  char ascii_code;
  int count;
};

int create_frequency_nodes(struct frequency_node **nodes, int frequencies[]) {
  size_t max_size = ASCII_SIZE * sizeof(struct frequency_node);
  struct frequency_node *fnptr = malloc(max_size);

  int skipped = 0;
  for (int i = 0; i < ASCII_SIZE; i++) {
    int count = frequencies[i];
    int insert_idx = i - skipped;
    if (count) {
      fnptr[insert_idx].ascii_code = i;
      fnptr[insert_idx].count = count;
    } else {
      skipped++;
    }
  }

  if (skipped == ASCII_SIZE) {
    free(fnptr);
    return 0;
  }

  int nodes_count = ASCII_SIZE - skipped;
  int new_size = max_size - nodes_count * sizeof(struct frequency_node);
  struct frequency_node *new_fnptr = realloc(fnptr, new_size);
  if (!new_fnptr) {
    return 0;
  }

  *nodes = new_fnptr;
  return nodes_count;
}

void aggregate_frequencies(int frequencies[], FILE *fptr) {
  char buf[CHUNK_SIZE] = {0};

  while (!feof(fptr)) {
    size_t bytes_read = fread(buf, 1, CHUNK_SIZE, fptr);
    for (size_t i = 0; i < bytes_read; i++) {
      frequencies[(int)buf[i]]++;
    }
  }
}

void print_frequencies(int frequencies[]) {
  for (int i = 0; i < ASCII_SIZE; i++) {
    if (frequencies[i]) {
      printf("\"%c\": %i\n", i, frequencies[i]);
    }
  }
}

void print_frequency_nodes(struct frequency_node *nodes, int len) {
  for (int i = 0; i < len; i++) {
    printf("\"%c\": %i\n", nodes[i].ascii_code, nodes[i].count);
  }
}

int main(void) {
  int frequencies[ASCII_SIZE] = {0};
  FILE *fptr = fopen("raw.txt", "rb");
  aggregate_frequencies(frequencies, fptr);

  if (ferror(fptr)) {
    printf("error\n");
  }

  struct frequency_node *nodes = 0;
  int nodes_count = create_frequency_nodes(&nodes, frequencies);
  print_frequency_nodes(nodes, nodes_count);

  free(nodes);
  fclose(fptr);

  return 0;
}