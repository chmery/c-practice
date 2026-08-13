#include <stddef.h>

struct int_array {
  size_t length;
  size_t item_size;
  size_t capacity;
  int *items;
};

enum resize_action {
  SHRINK,
  EXPAND,
};

struct int_array new_ia(size_t length);
void free_ia(struct int_array *array);
void remove_from_ia(struct int_array *array, unsigned idx);
void push_to_ia(struct int_array *array, int new_int);
void print_ia(struct int_array *array);