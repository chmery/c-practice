#include "array.h"
#include <stdio.h>

int main(void) {
  struct int_array ints = new_ia(10);

  for (int i = 0; i < 10; i++) {
    ints.items[i] = i;
  }

  remove_from_ia(&ints, 2);
  remove_from_ia(&ints, 5);
  remove_from_ia(&ints, 5);

  push_to_ia(&ints, 99);
  push_to_ia(&ints, 100);
  push_to_ia(&ints, 99);

  print_ia(&ints);
  printf("len: %zu, cap: %zu\n", ints.length, ints.capacity);

  free_ia(&ints);
}