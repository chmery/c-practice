#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 256
#define TABLE_SIZE 3

struct book {
  char title[MAX_TITLE];
  int year;
};

struct book_node {
  struct book_node *prev;
  struct book_node *next;
  struct book *data;
};

unsigned hash_title(char *title) {
  int title_len = strlen(title);
  int sum = 0;

  for (int i = 0; i < title_len; i++) {
    sum += title[i];
  }

  return sum % TABLE_SIZE;
}

void insert(struct book_node *table[], struct book *book) {
  unsigned idx = hash_title(book->title);
  struct book_node *new_node = malloc(sizeof(*new_node));

  new_node->next = NULL;
  new_node->prev = NULL;
  new_node->data = book;

  if (!table[idx]) {
    table[idx] = new_node;
  } else {
    struct book_node *node = table[idx];
    while (node->next) {
      node = node->next;
    }
    node->next = new_node;
    new_node->prev = node;
  }
}

void free_table(struct book_node *table[]) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (table[i]) {
      struct book_node *node = table[i];
      do {
        struct book_node *node_to_free = node;
        node = node->next;
        free(node_to_free);
      } while (node);
    }
  }
}

struct book_node *search(struct book_node *table[], char *title) {
  unsigned idx = hash_title(title);
  struct book_node *node = table[idx];

  while (node) {
    if (strcmp(node->data->title, title) == 0) {
      return node;
    }
    node = node->next;
  }

  return NULL;
}

void delete(struct book_node *table[], char *title) {
  unsigned idx = hash_title(title);
  struct book_node *node = search(table, title);

  if (!node) {
    return;
  }

  int not_chained = !node->prev && !node->next;
  if (not_chained) {
    table[idx] = 0;
    free(node);
    return;
  }

  if (node->prev) {
    node->prev->next = node->next;
  }

  if (node->next) {
    node->next->prev = node->prev;
  }

  int is_head_node = table[idx] == node;
  if (is_head_node) {
    table[idx] = node->next;
  }

  free(node);
}

int main(void) {
  struct book_node *books_table[TABLE_SIZE] = {0};

  struct book one = {"kotek", 2011};
  struct book two = {"gangam style", 300};
  struct book three = {"expanse", 3333};
  struct book four = {"potop1", 43};
  struct book five = {"potop2", 1999};

  insert(books_table, &one);
  insert(books_table, &two);
  insert(books_table, &three);
  insert(books_table, &four);
  insert(books_table, &five);

  struct book_node *result = search(books_table, "potop2");
  if (result) {
    printf("found: %s %i\n\n", result->data->title, result->data->year);
  }

  delete(books_table, one.title);
  delete(books_table, "expanse");
  delete(books_table, four.title);

  for (int i = 0; i < TABLE_SIZE; i++) {
    struct book_node *node = books_table[i];
    printf("%i: ", i);

    if (node) {
      printf("%p (%s)", (void *)node, node->data->title);
      while (node->next) {
        printf(" -> %p (%s)", (void *)node->next, node->next->data->title);
        node = node->next;
      }
    } else {
      printf("(nil)");
    }

    printf("\n");
  };

  free_table(books_table);
}