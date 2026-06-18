/* queue.h */
#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdbool.h>

/* Buffer size of 64 samples provides 1 280 µs of tolerance at 50 kHz */
#define QUEUE_SIZE 64

typedef struct {
    int16_t data[QUEUE_SIZE];
    volatile uint32_t head;  /* dequeue position (ISR reads here) */
    volatile uint32_t tail;  /* enqueue position (main writes here) */
    volatile uint32_t count; /* number of valid samples in queue */
} Queue_t;

/* Initialise queue to empty state */
void Queue_Init(Queue_t *q);

/* Returns true if value was added; false if queue was full (OVERFLOW) */
bool Queue_Enqueue(Queue_t *q, int16_t value);

/* Returns true and writes *value if queue was non-empty; false on UNDERFLOW */
bool Queue_Dequeue(Queue_t *q, int16_t *value);

bool Queue_IsFull(const Queue_t *q);
bool Queue_IsEmpty(const Queue_t *q);
uint32_t Queue_Count(const Queue_t *q);

#endif /* QUEUE_H */
