// max heap value of i <= value of parent used for heapsort
// min heap value of i >= value of parent used for priority queues

// left(i) = 2*i
// right(i) = 2*i + 1
// parent(i) = floor(i/2)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct heap {
  int size;
  int *elements;
};

struct heap init_heap() {
  struct heap h = {8, malloc(sizeof(int) * 50)};

  // example valid heap of size 8
  // pad first idx with 0 to start from index 1
  int valid_heap[] = {0, 1, 4, 2, 9, 10, 3, 7, 10};
  memcpy(h.elements, valid_heap, sizeof(valid_heap));

  return h;
}

int min_heapify(struct heap *h, int value_idx) {
  int parent_idx = value_idx / 2;
  if (parent_idx == 0) {
    return 0;
  }

  int parent_value = h->elements[parent_idx];
  int value = h->elements[value_idx];

  if (value < parent_value) {
    h->elements[parent_idx] = value;
    h->elements[value_idx] = parent_value;
    return min_heapify(h, parent_idx);
  }

  return 0;
}

void insert(struct heap *h, int value) {
  h->size++;
  h->elements[h->size] = value;
  min_heapify(h, h->size);
}

void print_heap(struct heap *h) {
  for (int i = 1; i <= h->size; i++) {
    printf("[%i] ", h->elements[i]);
  }
  printf("\n");
}

int main(void) {
  struct heap h = init_heap();
  insert(&h, 3);

  print_heap(&h);
  free(h.elements);
  return 0;
}
