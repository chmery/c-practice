#include "array.h"
#include <stdio.h>

void print_array(struct array *array) {
  for (unsigned i = 0; i < array->length; i++) {
    printf("%i: %i\n", i, *(int *)array_at(array, i));
  }
}

int main(void) {
  struct array ints = create_array(sizeof(int) * 10, sizeof(int));

  for (int i = 0; i < 10; i++) {
    push_to_array(&ints, &i);
  }

  remove_from_array(&ints, 2);
  remove_from_array(&ints, 5);
  remove_from_array(&ints, 5);

  int a = 100;
  push_to_array(&ints, &a);
  push_to_array(&ints, &a);

  remove_from_array(&ints, 0);

  print_array(&ints);
  printf("len: %zu, cap: %zu\n", ints.length, ints.capacity);

  free_array(&ints);
}