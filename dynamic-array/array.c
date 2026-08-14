#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

struct array create_array(size_t capacity, size_t item_size) {
  struct array a = {.item_size = item_size, .length = 0, .capacity = capacity, .items = malloc(capacity)};
  return a;
}

void free_array(struct array *array) {
  free(array->items);
}

void resize_array(struct array *array, size_t n, enum resize_action action) {
  size_t delta_size = n * array->item_size;
  size_t new_size = action == SHRINK ? array->capacity - delta_size : array->capacity + delta_size;

  int *np = realloc(array->items, new_size);
  if (!np) {
    exit(EXIT_FAILURE);
    return;
  }

  array->items = np;

  if (action == SHRINK) {
    array->length -= n;
    array->capacity -= delta_size;
  } else {
    array->length += n;
    array->capacity += delta_size;
  }
}

void *array_at(struct array *array, unsigned idx) {
  int invalid_idx = idx > array->capacity / array->item_size - 1;
  if (invalid_idx) {
    return NULL;
  }

  return (char *)array->items + (array->item_size * idx);
}

void remove_from_array(struct array *array, unsigned idx) {
  if (!array->length || !array_at(array, idx)) {
    exit(EXIT_FAILURE);
  }

  int is_last = array->length == idx + 1;
  if (is_last) {
    resize_array(array, 1, SHRINK);
  } else {
    void *dest = array_at(array, idx);
    void *src = array_at(array, idx + 1);
    size_t bytes_to_move = (array->length - idx - 1) * array->item_size;
    memmove(dest, src, bytes_to_move);
    resize_array(array, 1, SHRINK);
  }
}

// https://stackoverflow.com/questions/9410/how-do-you-pass-a-function-as-a-parameter-in-c
// void filter_array(struct array *array, int (*filter_fn)(void *)) {
//   struct array new_array = create_array(array->length, array->item_size);

//   for (int i = 0; i < array->length; i++) {
//     void *item = array_at(array, i);
//     int matches_condidtion = filter_fn(item);
//     if (matches_condidtion) {
//       push_to_array(&new_array, &item);
//       // push to new
//     }
//   }
// }

void push_to_array(struct array *array, void *item) {
  unsigned space_left = array->capacity - array->length * array->item_size;

  if (space_left) {
    void *dest = array_at(array, array->length);
    memcpy(dest, item, array->item_size);
    array->length++;
  } else {
    resize_array(array, 1, EXPAND);
    void *dest = array_at(array, array->length - 1);
    memcpy(dest, item, array->item_size);
  }
}