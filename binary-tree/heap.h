
struct heap_node {
  int weight;
  int data;
};

struct heap {
  int len;
  struct heap_node *nodes;
};

enum heap_type {
  MIN,
  MAX
};

struct heap heap_init(struct heap_node nodes[], int len, enum heap_type ht);
void heap_insert(struct heap *h, struct heap_node node, enum heap_type ht);
struct heap_node heap_delete(struct heap *h, enum heap_type ht);
void heap_sort(struct heap *h, enum heap_type ht);
void heap_print(struct heap *h);