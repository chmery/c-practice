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

void free_queue(struct queue *queue) {
  if (!queue->length) {
    return;
  } else if (queue->head == queue->tail) {
    free(queue->head);
  } else {
    struct request_node *n = queue->head;

    while (n) {
      struct request_node *node_to_free = n;
      n = n->next;
      free(node_to_free);
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
    free(queue->head);
    queue->length = 0;
    queue->head = NULL;
    queue->tail = NULL;
  } else {
    struct request_node *old_head = queue->head;
    queue->head = queue->head->next;
    free(old_head);
    queue->length--;
  }
}

struct request_node *new_request(int id, char *content) {
  struct request_node *n = malloc(sizeof(*n));
  struct request r = {.id = id, .content = content};

  n->next = NULL;
  n->data = r;

  return n;
}

// process text file into requests;

int main(void) {
  struct queue requests_queue = create_queue();

  dequeue(&requests_queue); // should do nothing

  for (int i = 0; i < 5; i++) {
    enqueue(&requests_queue, new_request(i, "zegnam"));
  }

  dequeue(&requests_queue);
  dequeue(&requests_queue);
  enqueue(&requests_queue, new_request(10, "witam"));

  struct request_node *current_node = requests_queue.head;
  while (current_node) {
    printf("%i: %s\n", current_node->data.id, current_node->data.content);
    current_node = current_node->next;
  }

  free_queue(&requests_queue);
}
