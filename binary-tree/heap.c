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

int insert_compare(struct heap *h, int value_idx) {
  int parent_idx = value_idx / 2;
  if (parent_idx == 0) {
    return 0;
  }

  int parent_value = h->elements[parent_idx];
  int value = h->elements[value_idx];

  if (value < parent_value) {
    h->elements[parent_idx] = value;
    h->elements[value_idx] = parent_value;
    return insert_compare(h, parent_idx);
  }

  return 0;
}

void insert(struct heap *h, int value) {
  h->size++;
  h->elements[h->size] = value;
  insert_compare(h, h->size);
}

void print_heap(struct heap *h) {
  for (int i = 1; i <= h->size; i++) {
    printf("[%i] ", h->elements[i]);
  }
  printf("\n");
}

int delete_compare(struct heap *h, int parent_idx) {
  int is_leaf_node = parent_idx > h->size / 2;
  if (is_leaf_node) {
    return 0;
  }

  int left_child_idx = 2 * parent_idx;
  int right_child_idx = 2 * parent_idx + 1;

  int parent_value = h->elements[parent_idx];
  int left_child_value = h->elements[left_child_idx];
  int right_child_value = h->elements[right_child_idx];

  int smaller_child_idx = left_child_value < right_child_value ? left_child_idx : right_child_idx;
  int smaller_child_value = h->elements[smaller_child_idx];

  if (smaller_child_value < parent_value) {
    h->elements[parent_idx] = smaller_child_value;
    h->elements[smaller_child_idx] = parent_value;
    delete_compare(h, smaller_child_idx);
  }

  return 0;
}

void delete(struct heap *h) {
  h->elements[1] = h->elements[h->size];
  h->size--;
  delete_compare(h, 1);
}

int main(void) {
  struct heap h = init_heap();
  insert(&h, 3);

  print_heap(&h);

  delete(&h);
  delete(&h);

  print_heap(&h);

  free(h.elements);
  return 0;
}
