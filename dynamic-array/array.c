#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

struct int_array new_ia(size_t length) {
  struct int_array a = {.item_size = sizeof(int),
                        .length = length,
                        .capacity = length * sizeof(int),
                        .items = calloc(length, sizeof(int))};
  return a;
}

void free_ia(struct int_array *array) {
  free(array->items);
}

void resize_ia(struct int_array *array, size_t n, enum resize_action action) {
  size_t delta_size = n * sizeof(int);
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

void remove_from_ia(struct int_array *array, unsigned idx) {
  int is_last = array->length == idx + 1;
  if (is_last) {
    resize_ia(array, 1, SHRINK);
  } else {
    int *dest = &array->items[idx];
    memmove(dest, dest + 1, (array->length - idx - 1) * sizeof(*dest));
    resize_ia(array, 1, SHRINK);
  }
}

void push_to_ia(struct int_array *array, int new_int) {
  resize_ia(array, 1, EXPAND);
  array->items[array->length - 1] = new_int;
}

void print_ia(struct int_array *array) {
  for (unsigned i = 0; i < array->length; i++) {
    printf("%i: %i\n", i, array->items[i]);
  }
}