#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct heap {
  int size;
  int *elements;
};

int sift_up(struct heap *h, int child_idx) {
  int parent_idx = child_idx / 2;
  if (parent_idx == 0) {
    return 0;
  }

  int parent_value = h->elements[parent_idx];
  int child_value = h->elements[child_idx];

  if (child_value < parent_value) {
    h->elements[parent_idx] = child_value;
    h->elements[child_idx] = parent_value;
    return sift_up(h, parent_idx);
  }

  return 0;
}

int sift_down(struct heap *h, int parent_idx) {
  int is_leaf_node = parent_idx > h->size / 2;
  if (is_leaf_node) {
    return 0;
  }

  int left_child_idx = 2 * parent_idx;
  int right_child_idx = 2 * parent_idx + 1;

  if (right_child_idx > h->size) {
    right_child_idx = left_child_idx;
  }

  int parent_value = h->elements[parent_idx];
  int left_child_value = h->elements[left_child_idx];
  int right_child_value = h->elements[right_child_idx];

  int smaller_child_idx = left_child_value < right_child_value ? left_child_idx : right_child_idx;
  int smaller_child_value = h->elements[smaller_child_idx];

  if (smaller_child_value < parent_value) {
    h->elements[parent_idx] = smaller_child_value;
    h->elements[smaller_child_idx] = parent_value;
    sift_down(h, smaller_child_idx);
  }

  return 0;
}

void heapify(struct heap *h) {
  int first_non_leaf_idx = h->size / 2;
  for (int i = first_non_leaf_idx; i > 0; i--) {
    sift_down(h, i);
  }
}

struct heap init_heap(int arr[], int size) {
  struct heap h = {size, malloc(sizeof(int) * (size + 1))};
  h.elements[0] = 0;
  memcpy(&h.elements[1], arr, sizeof(int) * size);
  heapify(&h);
  return h;
}

void insert(struct heap *h, int value) {
  h->size++;
  h->elements[h->size] = value;
  sift_up(h, h->size);
}

int delete(struct heap *h) {
  int deleted_element = h->elements[1];

  h->elements[1] = h->elements[h->size];
  h->size--;
  sift_down(h, 1);

  return deleted_element;
}

void heap_sort(struct heap *h) {
  int init_size = h->size;
  for (int i = 0; i < init_size; i++) {
    int deleted = delete(h);
    h->elements[h->size + 1] = deleted;
  }
}

void print_heap(struct heap *h) {
  printf("\n");
  printf("hsize: %i\n", h->size);
  for (int i = 1; i <= h->size; i++) {
    printf("[%i] ", h->elements[i]);
  }
  printf("\n");
}

int main(void) {
  int arr[] = {5, 8, 9, 10, 2, 3};
  struct heap h = init_heap(arr, 6);
  print_heap(&h);
  heap_sort(&h);

  for (int i = 1; i < 7; i++) {
    printf("[%i] ", h.elements[i]);
  }
  printf("\n");

  free(h.elements);
  return 0;
}
