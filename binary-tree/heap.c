#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sift_up(struct heap *h, int child_idx, enum heap_type heap_type) {
  int parent_idx = child_idx / 2;
  if (parent_idx == 0) {
    return 0;
  }

  struct heap_node parent = h->nodes[parent_idx];
  struct heap_node child = h->nodes[child_idx];

  int swap_required =
      (heap_type == MIN && (child.weight < parent.weight)) || (heap_type == MAX && (child.weight > parent.weight));

  if (swap_required) {
    h->nodes[parent_idx] = child;
    h->nodes[child_idx] = parent;
    return sift_up(h, parent_idx, heap_type);
  }

  return 0;
}

int sift_down(struct heap *h, int parent_idx, enum heap_type heap_type) {
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

  int significant_child_idx = 0;

  if (heap_type == MIN) {
    significant_child_idx = left_child.weight < right_child.weight ? left_child_idx : right_child_idx;
  } else {
    significant_child_idx = left_child.weight > right_child.weight ? left_child_idx : right_child_idx;
  }

  struct heap_node significant_child = h->nodes[significant_child_idx];

  int swap_required = (heap_type == MIN && (significant_child.weight < parent.weight)) ||
                      (heap_type == MAX && (significant_child.weight > parent.weight));

  if (swap_required) {
    h->nodes[parent_idx] = significant_child;
    h->nodes[significant_child_idx] = parent;
    sift_down(h, significant_child_idx, heap_type);
  }

  return 0;
}

void heapify(struct heap *h, enum heap_type ht) {
  int first_non_leaf_idx = h->len / 2;
  for (int i = first_non_leaf_idx; i > 0; i--) {
    sift_down(h, i, ht);
  }
}

struct heap heap_init(struct heap_node nodes[], int len, enum heap_type ht) {
  struct heap h = {len, malloc(sizeof(struct heap_node) * (len + 1))};
  memcpy(&h.nodes[1], nodes, sizeof(struct heap_node) * len);
  heapify(&h, ht);
  return h;
}

void heap_insert(struct heap *h, struct heap_node node, enum heap_type ht) {
  h->len++;
  h->nodes[h->len] = node;
  sift_up(h, h->len, ht);
}

struct heap_node heap_delete(struct heap *h, enum heap_type ht) {
  struct heap_node deleted_node = h->nodes[1];

  h->nodes[1] = h->nodes[h->len];
  h->len--;
  sift_down(h, 1, ht);

  return deleted_node;
}

void heap_sort(struct heap *h, enum heap_type ht) {
  int init_size = h->len;
  for (int i = 0; i < init_size; i++) {
    struct heap_node deleted = heap_delete(h, ht);
    h->nodes[h->len + 1] = deleted;
  }
}

void heap_print(struct heap *h) {
  printf("\n");
  printf("hsize: %i\n", h->len);
  for (int i = 1; i <= h->len; i++) {
    printf("[w:%i d:%i] ", h->nodes[i].weight, h->nodes[i].data);
  }
  printf("\n");
}

// int main(void) {
//   struct heap_node nodes[] = {5, 0, 8, 0, 9, 0, 10, 0, 2, 0, 3, 0};
//   struct heap h = heap_init(nodes, 6, MIN);
//   heap_print(&h);
//   heap_sort(&h, MIN);

//   for (int i = 1; i < 7; i++) {
//     printf("[%i] ", h.nodes[i].weight);
//   }
//   printf("\n");

//   free(h.nodes);
//   return 0;
// }
