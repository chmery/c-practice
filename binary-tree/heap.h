
// i need to be able to store custom objects in heap elements array
// caller should pass array of heap elements
struct heap_node {
  int weight;
  void *data; // ptr to caller custom object
};

struct heap {
  int len;
  struct heap_node *nodes;
};

struct heap min_heap_init(struct heap_node nodes[], int len);
void min_heap_insert(struct heap *h, struct heap_node node);
struct heap_node min_heap_delete(struct heap *h);
void min_heap_sort(struct heap *h);
void print_heap(struct heap *h);