#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct queue {
  struct request_node *head;
  struct request_node *tail;
  size_t length;
};

struct request {
  int id;
  char *content;
};

struct request_node {
  struct request_node *next;
  struct request data;
};

struct queue create_queue() {
  struct queue q = {
      .head = NULL,
      .tail = NULL,
      .length = 0,
  };

  return q;
}

void free_queue_node(struct request_node *node) {
  free(node->data.content);
  free(node);
}

void free_queue(struct queue *queue) {
  if (!queue->length) {
    return;
  } else if (queue->head == queue->tail) {
    free_queue_node(queue->head);
  } else {
    struct request_node *n = queue->head;

    while (n) {
      struct request_node *node_to_free = n;
      n = n->next;
      free_queue_node(node_to_free);
    }
  }
}

void enqueue(struct queue *queue, struct request_node *node) {
  if (!queue->length) {
    queue->head = node;
    queue->tail = node;
  } else {
    queue->tail->next = node;
    queue->tail = node;
  }

  queue->length++;
}

void dequeue(struct queue *queue) {
  if (!queue->length) {
    return;
  } else if (queue->head == queue->tail) {
    free_queue_node(queue->head);
    queue->length = 0;
    queue->head = NULL;
    queue->tail = NULL;
  } else {
    struct request_node *old_head = queue->head;
    queue->head = queue->head->next;
    free_queue_node(old_head);
    queue->length--;
  }
}

struct request_node *new_request(int id, char *content) {
  struct request_node *n = malloc(sizeof(*n));
  struct request r = {.id = id, .content = malloc(256)};
  memcpy(r.content, content, 256);

  n->next = NULL;
  n->data = r;

  return n;
}

int main(void) {
  struct queue requests_queue = create_queue();
  FILE *fp = fopen("text.txt", "r");

  dequeue(&requests_queue); // should do nothing

  char content[256] = {0};
  int id = 0;

  while (fgets(content, sizeof(content), fp) != NULL) {
    enqueue(&requests_queue, new_request(id, content));
    id++;
  }

  dequeue(&requests_queue);
  dequeue(&requests_queue);

  struct request_node *current_node = requests_queue.head;
  while (current_node) {
    printf("%i: %s", current_node->data.id, current_node->data.content);
    current_node = current_node->next;
  }
  printf("\n");

  free_queue(&requests_queue);
  fclose(fp);
}
