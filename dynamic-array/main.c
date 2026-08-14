#include "array.h"
#include <stdio.h>

void print_array(struct array *array) {
  for (unsigned i = 0; i < array->length; i++) {
    printf("%i: %i\n", i, *(int *)array_at(array, i));
  }
}

int is_two_digit_number(void *item) {
  if (*(int *)item > 99) {
    return 0;
  }

  return 1;
}

int main(void) {
  struct array ints = create_array(sizeof(int) * 10, sizeof(int));

  for (int i = 0; i < 10; i++) {
    push_to_array(&ints, &i);
  }

  int a = 100;
  push_to_array(&ints, &a);
  push_to_array(&ints, &a);

  remove_from_array(&ints, 2);
  remove_from_array(&ints, 5);
  remove_from_array(&ints, 5);

  int b = 200;
  push_to_array(&ints, &b);
  remove_from_array(&ints, 0);
  filter_array(&ints, &is_two_digit_number);

  print_array(&ints);
  printf("len: %zu, cap: %zu\n", ints.length, ints.capacity);

  free_array(&ints);
}