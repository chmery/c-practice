#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct request {
  int id;
  char *content;
};

struct request_node {
  struct request_node *next;
  struct request data;
};

struct queue_array {
  int length;
  int head;
  int tail;
  struct request **requests;
};

#define MAX_QUEUE_LENGTH 3

struct queue_array create_queue_array() {
  struct queue_array queue = {0, 0, 0, malloc(sizeof(struct request_node *) * MAX_QUEUE_LENGTH)};
  return queue;
}

// circular queue
void enqueue_array(struct queue_array *queue, struct request *request) {
  if (!queue->length) {
    queue->requests[queue->head] = request;
  } else if (queue->length >= MAX_QUEUE_LENGTH) {
    // queue is full!
    return;
  } else {
    queue->tail = (queue->tail + 1) % MAX_QUEUE_LENGTH; // TODO: macro define this value
    queue->requests[queue->tail] = request;
  }

  queue->length++;
}

void dequeue_array(struct queue_array *queue) {
  if (!queue->length) {
    return;
  }

  if (queue->head == queue->tail) {
    queue->head = 0;
    queue->tail = 0;
  } else {
    queue->head = (queue->head + 1) % MAX_QUEUE_LENGTH;
  }

  queue->length--;
}

struct queue {
  struct request_node *head;
  struct request_node *tail;
  size_t length;
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

struct request *create_request_arr(int id, FILE *fp) {
  struct request *r = malloc(sizeof(*r));
  r->id = id;
  r->content = malloc(256);
  fgets(r->content, 256, fp);
  return r;
}

int main(void) {
  // array queue
  // TODO: test if modulo works nicely
  // 6 elements
  // do max queue 3
  // queue 3 elements
  // dequeue 1
  // queue 1
  // tail should overflow

  struct queue_array requests_queue = create_queue_array();
  FILE *fp = fopen("text.txt", "r");

  dequeue_array(&requests_queue); // should do nothing

  // char content[256] = {0};
  // int id = 0;

  enqueue_array(&requests_queue, create_request_arr(0, fp));
  enqueue_array(&requests_queue, create_request_arr(1, fp));
  enqueue_array(&requests_queue, create_request_arr(2, fp));

  dequeue_array(&requests_queue);

  enqueue_array(&requests_queue, create_request_arr(3, fp));
  enqueue_array(&requests_queue, create_request_arr(4, fp)); // should skip

  dequeue_array(&requests_queue);
  dequeue_array(&requests_queue);
  dequeue_array(&requests_queue);

  // while (fgets(content, sizeof(content), fp) != NULL) {
  //   struct request *r = malloc(sizeof(*r));
  //   r->id = id;
  //   r->content = malloc(256);
  //   memcpy(r->content, content, 256);
  //   enqueue_array(&requests_queue, r);
  //   id++;
  // }

  // dequeue_array(&requests_queue);
  // dequeue_array(&requests_queue);

  struct request *current_node = requests_queue.requests[requests_queue.head];

  printf("%i: %s\n", current_node->id, current_node->content);
  printf("%i %i", requests_queue.head, requests_queue.tail);

  // int idx = requests_queue.head;
  // do {
  //   printf("%i: %s", current_node->id, current_node->content);
  //   idx = (requests_queue.head + 1) % 10;
  //   current_node = requests_queue.requests[idx];
  // } while (idx != requests_queue.tail);
  // printf("\n");

  // free_queue(&requests_queue);
  fclose(fp);

  // nodes queue
  // struct queue requests_queue = create_queue();

  // FILE *fp = fopen("text.txt", "r");

  // dequeue(&requests_queue); // should do nothing

  // char content[256] = {0};
  // int id = 0;

  // while (fgets(content, sizeof(content), fp) != NULL) {
  //   enqueue(&requests_queue, new_request(id, content));
  //   id++;
  // }

  // dequeue(&requests_queue);
  // dequeue(&requests_queue);

  // struct request_node *current_node = requests_queue.head;
  // while (current_node) {
  //   printf("%i: %s", current_node->data.id, current_node->data.content);
  //   current_node = current_node->next;
  // }
  // printf("\n");

  // free_queue(&requests_queue);
  // fclose(fp);
}
