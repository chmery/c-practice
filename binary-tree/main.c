#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct node {
  int data;
  struct node *left;
  struct node *right;
};

// stack behavior can be achieved by recursion
// int tree[10] = {50, 40, 30, 17, 13, 12, 11, 7, 4, 6};

struct stack {
  int top;
  struct node **nodes;
};

struct stack init_stack(size_t n) {
  struct stack stack = {-1, malloc(sizeof(*stack.nodes) * n)};
  return stack;
}

void push(struct stack *stack, struct node *node) {
  stack->top++;
  stack->nodes[stack->top] = node;
}

void pop(struct stack *stack) {
  if (stack->top > -1) {
    stack->top--;
  }
}

void free_stack(struct stack *stack) {
  free(stack->nodes);
}

struct node *dfs(struct node *root, int value) {
  struct stack s = init_stack(6);
  push(&s, root);

  while (s.top > -1) {
    struct node *node = s.nodes[s.top];
    printf("visited: %i\n", node->data);

    if (node->data == value) {
      printf("found value\n");
      free_stack(&s);
      return node;
    }

    pop(&s);

    if (node->right) {
      push(&s, node->right);
    }

    if (node->left) {
      push(&s, node->left);
    }
  }

  free_stack(&s);
  return NULL;
}

int main(void) {
  struct node l2_child1 = {5, 0, 0};
  struct node l2_child2 = {6, 0, 0};
  struct node l2_child4 = {7, 0, 0};
  struct node l1_child1 = {3, &l2_child1, &l2_child2};
  struct node l1_child2 = {4, 0, &l2_child4};
  struct node root = {2, &l1_child1, &l1_child2};

  struct node *n = dfs(&root, 33);
}