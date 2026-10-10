#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

#define CHUNK_SIZE 1024 * 64
#define ASCII_SIZE 128

void aggregate(char *buf, size_t size, int frequencies[]) {
  for (size_t i = 0; i < size; i++) {
    frequencies[(int)buf[i]]++;
  }
}

void print_frequencies(int frequencies[]) {
  for (int i = 0; i < ASCII_SIZE; i++) {
    if (frequencies[i]) {
      printf("\"%c\": %i\n", i, frequencies[i]);
    }
  }
}

int main(void) {
  int frequencies[ASCII_SIZE] = {0};
  FILE *fptr = fopen("raw.txt", "rb");

  char buf[CHUNK_SIZE] = {0};
  while (!feof(fptr)) {
    size_t bytes_read = fread(buf, 1, CHUNK_SIZE, fptr);
    aggregate(buf, bytes_read, frequencies);
  }

  if (ferror(fptr)) {
    printf("errorr\n");
  }

  print_frequencies(frequencies);
  return 0;
}