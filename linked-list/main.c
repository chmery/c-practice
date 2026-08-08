#include <stdio.h>
#include <stdlib.h>

struct point {
  int x;
  int y;
};

struct node {
  struct point point;
  struct node *prev;
  struct node *next;
};

struct linked_list {
  struct node *first_node;
  struct node *last_node;
};

struct node *new_node(int x, int y) {
  struct node *node = calloc(1, sizeof(*node));
  node->point.x = x;
  node->point.y = y;
  return node;
}

struct node *ll_find(struct linked_list *ll, struct point *point) {
  struct node *node = ll->first_node;

  while (node) {
    if (node->point.x == point->x && node->point.y == point->y) {
      return node;
    }
    node = node->next;
  };

  return NULL;
}

void ll_init(struct linked_list *ll, struct node *node) {
  ll->first_node = node;
  ll->last_node = node;
}

void ll_destroy(struct linked_list *ll) {
  struct node *node = ll->first_node;

  while (node) {
    struct node *next = node->next;
    free(node);
    node = next;
  }
}

void ll_add_at_end(struct linked_list *ll, struct node *new_node) {
  if (!ll->last_node) {
    ll_init(ll, new_node);
    return;
  }

  new_node->prev = ll->last_node;
  ll->last_node->next = new_node;
  ll->last_node = new_node;
}

void ll_insert_after(struct linked_list *ll, struct node *node, struct node *new_node) {
  if (ll->last_node == node) {
    ll_add_at_end(ll, new_node);
    return;
  }

  new_node->prev = node;
  new_node->next = node->next;
  node->next->prev = new_node;
  node->next = new_node;
}

void ll_remove(struct linked_list *ll, struct node *node) {
  if (ll->last_node == node) {
    ll->last_node = ll->last_node->prev;
    node->prev->next = node->next;
  } else if (ll->first_node == node) {
    ll->first_node = ll->first_node->next;
    node->next->prev = node->prev;
  } else {
    node->prev->next = node->next;
    node->next->prev = node->prev;
  }

  free(node);
}

void print_node(struct node *node, int i) {
  printf("%i: (%i, %i)\n", i, node->point.x, node->point.y);
}

void ll_print(struct linked_list *ll) {
  struct node *node = ll->first_node;
  int i = 0;
  while (node) {
    print_node(node, i);
    node = node->next;
    i++;
  }

  printf("\nfirst node: ");
  print_node(ll->first_node, 0);
  printf("last node: ");
  print_node(ll->last_node, i - 1);
}

int main(void) {
  struct linked_list ll = {0};

  for (int i = 0; i < 10; i++) {
    struct node *node = new_node(i, i * 2);
    ll_add_at_end(&ll, node);
  }

  struct point p1 = {0, 0};
  struct point p2 = {3, 6};
  struct node *n1 = ll_find(&ll, &p1);
  struct node *n2 = ll_find(&ll, &p2);

  struct node *n3 = new_node(20, 20);
  struct node *n4 = new_node(30, 30);
  struct node *n5 = new_node(40, 40);
  struct node *n6 = new_node(50, 50);

  if (n1 && n2) {
    ll_insert_after(&ll, n1, n3);
    ll_remove(&ll, n1);
    ll_remove(&ll, n2);
    ll_insert_after(&ll, ll.last_node, n4);
    ll_insert_after(&ll, ll.last_node, n5);
    ll_remove(&ll, ll.last_node);
    ll_insert_after(&ll, ll.last_node, n6);
  }

  struct point p3 = {8, 16};
  struct node *n7 = ll_find(&ll, &p3);

  if (n7) {
    ll_remove(&ll, n7);
  }

  ll_print(&ll);
  ll_destroy(&ll);
}