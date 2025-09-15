// queue.h - A simple queue implementation
// Simple queue API for generic pointers

#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

// Opaque queue type
typedef struct Queue Queue;

// Create a new queue, returns pointer or NULL on error
Queue *queue_init(void);
// Destroy and free queue
void queue_destroy(Queue *q);
// Enqueue an item (void pointer)
int queue_enqueue(Queue *q, void *item);
// Dequeue an item, returns NULL if queue is empty
void *queue_dequeue(Queue *q);
// Peek at the front item without removing, returns NULL if queue is empty
void *queue_peek(Queue *q);
// Returns 1 if empty, 0 otherwise
int queue_is_empty(const Queue *q);

#endif // QUEUE_H
