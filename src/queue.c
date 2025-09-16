// queue.c - Simple generic pointer queue implementation
#include "../include/queue.h"
#include <stdlib.h>

// Internal node structure
typedef struct QueueNode {
  void *data;
  struct QueueNode *next;
} QueueNode;

// Queue structure
struct Queue {
  QueueNode *head;
  QueueNode *tail;
};

Queue *queue_init(void) {
  Queue *q = (Queue *)malloc(sizeof(Queue));
  if (!q)
    return NULL;
  q->head = NULL;
  q->tail = NULL;
  return q;
}

void queue_destroy(Queue *q) {
  if (!q)
    return;
  QueueNode *node = q->head;
  while (node) {
    QueueNode *next = node->next;
    free(node);
    node = next;
  }
  free(q);
}

int queue_enqueue(Queue *q, void *item) {
  if (!q)
    return -1;
  QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
  if (!node)
    return -1;
  node->data = item;
  node->next = NULL;
  if (q->tail) {
    q->tail->next = node;
  } else {
    q->head = node;
  }
  q->tail = node;
  return 0;
}

void *queue_dequeue(Queue *q) {
  if (!q || !q->head)
    return NULL;
  QueueNode *node = q->head;
  void *data = node->data;
  q->head = node->next;
  if (!q->head)
    q->tail = NULL;
  free(node);
  return data;
}

void *queue_peek(Queue *q) {
  if (!q || !q->head)
    return NULL;
  return q->head->data;
}

int queue_is_empty(const Queue *q) {
  return (q == NULL || q->head == NULL) ? 1 : 0;
}
