#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sift_up(struct heap *h, int child_idx) {
  int parent_idx = child_idx / 2;
  if (parent_idx == 0) {
    return 0;
  }

  struct heap_node parent = h->nodes[parent_idx];
  struct heap_node child = h->nodes[child_idx];

  if (child.weight < parent.weight) {
    h->nodes[parent_idx] = child;
    h->nodes[child_idx] = parent;
    return sift_up(h, parent_idx);
  }

  return 0;
}

int sift_down(struct heap *h, int parent_idx) {
  int is_leaf_node = parent_idx > h->len / 2;
  if (is_leaf_node) {
    return 0;
  }

  int left_child_idx = 2 * parent_idx;
  int right_child_idx = 2 * parent_idx + 1;

  if (right_child_idx > h->len) {
    right_child_idx = left_child_idx;
  }

  struct heap_node parent = h->nodes[parent_idx];
  struct heap_node left_child = h->nodes[left_child_idx];
  struct heap_node right_child = h->nodes[right_child_idx];

  int smaller_child_idx = left_child.weight < right_child.weight ? left_child_idx : right_child_idx;
  struct heap_node smaller_child_node = h->nodes[smaller_child_idx];

  if (smaller_child_node.weight < parent.weight) {
    h->nodes[parent_idx] = smaller_child_node;
    h->nodes[smaller_child_idx] = parent;
    sift_down(h, smaller_child_idx);
  }

  return 0;
}

void heapify(struct heap *h) {
  int first_non_leaf_idx = h->len / 2;
  for (int i = first_non_leaf_idx; i > 0; i--) {
    sift_down(h, i);
  }
}

struct heap min_heap_init(struct heap_node nodes[], int len) {
  struct heap h = {len, malloc(sizeof(struct heap_node) * (len + 1))};
  memcpy(&h.nodes[1], nodes, sizeof(struct heap_node) * len);
  heapify(&h);
  return h;
}

void min_heap_insert(struct heap *h, struct heap_node node) {
  h->len++;
  h->nodes[h->len] = node;
  sift_up(h, h->len);
}

struct heap_node min_heap_delete(struct heap *h) {
  struct heap_node deleted_node = h->nodes[1];

  h->nodes[1] = h->nodes[h->len];
  h->len--;
  sift_down(h, 1);

  return deleted_node;
}

void min_heap_sort(struct heap *h) {
  int init_size = h->len;
  for (int i = 0; i < init_size; i++) {
    struct heap_node deleted = min_heap_delete(h);
    h->nodes[h->len + 1] = deleted;
  }
}

void print_heap(struct heap *h) {
  printf("\n");
  printf("hsize: %i\n", h->len);
  for (int i = 1; i <= h->len; i++) {
    printf("[w:%i d:%i] ", h->nodes[i].weight, h->nodes[i].data);
  }
  printf("\n");
}

// int main(void) {
//   struct heap_node nodes[] = {5, NULL, 8, NULL, 9, NULL, 10, NULL, 2, NULL, 3, NULL};
//   struct heap h = init_min_heap(nodes, 6);
//   print_heap(&h);
//   min_heap_sort(&h);

//   for (int i = 1; i < 7; i++) {
//     printf("[%i] ", h.nodes[i].weight);
//   }
//   printf("\n");

//   free(h.nodes);
//   return 0;
// }
