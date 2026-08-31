#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 256
#define TABLE_SIZE 4

struct book {
  char title[MAX_TITLE];
  int year;
};

struct book_node {
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
  new_node->data = book;

  if (!table[idx]) {
    table[idx] = new_node;
  } else {
    struct book_node *node = table[idx];
    while (node->next) {
      node = node->next;
    }
    node->next = new_node;
  }
}

void free_table(struct book_node *table[]) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (!table[i]) {
      continue;
    } else {
      struct book_node *node = table[i];
      do {
        struct book_node *node_to_free = node;
        node = node->next;
        free(node_to_free);
      } while (node);
    }
  }
}

struct book *search(struct book_node *table[], char *title) {
  unsigned idx = hash_title(title);
  struct book_node *node = table[idx];

  while (node) {
    if (strcmp(node->data->title, title) == 0) {
      return node->data;
    }
    node = node->next;
  }

  return NULL;
}

int main(void) {
  struct book_node *books_table[TABLE_SIZE] = {0};

  struct book one = {"wlaz kotek na plotek i mruga", 2011};
  struct book two = {"wopa gangam style", 300};
  struct book three = {"expanse", 3333};
  struct book four = {"potop1", 43};
  struct book five = {"potop2", 1999};

  insert(books_table, &one);
  insert(books_table, &two);
  insert(books_table, &three);
  insert(books_table, &four);
  insert(books_table, &five);

  struct book *result = search(books_table, "potop2");
  if (result) {
    printf("found: %s %i\n\n", result->title, result->year);
  }

  for (int i = 0; i < TABLE_SIZE; i++) {
    struct book_node *node = books_table[i];
    printf("%i: ", i);

    if (node) {
      printf("%p", (void *)node);
      while (node->next) {
        printf(" -> %p", (void *)node->next);
        node = node->next;
      }
    } else {
      printf("(nil)");
    }

    printf("\n");
  };

  free_table(books_table);
}