#include <stddef.h>

struct array {
  size_t length;
  size_t item_size;
  size_t capacity;
  void *items;
};

enum resize_action {
  SHRINK,
  EXPAND,
};

struct array create_array(size_t capacity, size_t item_size);
void free_array(struct array *array);
void remove_from_array(struct array *array, unsigned idx);
void push_to_array(struct array *array, void *item);
void *array_at(struct array *array, unsigned idx);