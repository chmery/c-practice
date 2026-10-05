#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE_LENGTH 3

struct request {
  int id;
  char *content;
};

struct queue {
  int length;
  int head;
  int tail;
  struct request **requests;
};

void remove_trailing_new_line(char *str) {
  str[strcspn(str, "\n")] = 0;
}

struct request *new_request(int id, FILE *fp) {
  struct request *r = malloc(sizeof(*r));
  r->id = id;
  r->content = malloc(256);
  fgets(r->content, 256, fp);
  remove_trailing_new_line(r->content);
  return r;
}

void free_request(struct request *r) {
  free(r->content);
  free(r);
}

struct queue create_queue() {
  struct queue queue = {0, 0, 0, calloc(MAX_QUEUE_LENGTH, sizeof(struct request *))};
  return queue;
}

void free_queue(struct queue *q) {
  for (int i = 0; i < MAX_QUEUE_LENGTH; i++) {
    struct request *req = q->requests[i];
    if (req) {
      free_request(req);
      q->requests[i] = NULL;
    }
  }

  free(q->requests);
}

int enqueue(struct queue *queue, struct request *request) {
  if (!queue->length) {
    queue->requests[queue->head] = request;
  } else if (queue->length >= MAX_QUEUE_LENGTH) {
    printf("cannot enqueue, queue full\n");
    return 1;
  } else {
    queue->tail = (queue->tail + 1) % MAX_QUEUE_LENGTH;
    queue->requests[queue->tail] = request;
  }

  queue->length++;
  return 0;
}

void dequeue(struct queue *queue) {
  if (!queue->length) {
    return;
  }

  free_request(queue->requests[queue->head]);
  queue->requests[queue->head] = NULL;
  queue->length--;

  if (queue->head == queue->tail) {
    queue->head = 0;
    queue->tail = 0;
  } else {
    queue->head = (queue->head + 1) % MAX_QUEUE_LENGTH;
  }
}

void print_queue(struct queue *q) {
  printf("\n");

  for (int i = 0; i < MAX_QUEUE_LENGTH; i++) {
    int queue_position = (q->head + i) % MAX_QUEUE_LENGTH;
    struct request *req = q->requests[queue_position];

    if (!req) {
      printf("[ ] ");
      continue;
    }

    printf("[%s] ", req->content);
  }

  printf("\n");
}

int main(void) {
  struct queue requests_queue = create_queue();
  FILE *fp = fopen("text.txt", "r");

  dequeue(&requests_queue);

  // queue overflow, should ignore last request
  for (int i = 0; i < 4; i++) {
    struct request *req = new_request(i, fp);
    int error = enqueue(&requests_queue, req);
    if (error) {
      free_request(req);
    }
  }

  dequeue(&requests_queue);
  enqueue(&requests_queue, new_request(5, fp));
  dequeue(&requests_queue);

  print_queue(&requests_queue);
  free_queue(&requests_queue);
  fclose(fp);
}
