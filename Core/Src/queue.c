/* queue.c */
#include "queue.h"
#include "stm32f4xx_hal.h"

void Queue_Init(Queue_t *q) {
    q->head  = 0u;
    q->tail  = 0u;
    q->count = 0u;
}

bool Queue_Enqueue(Queue_t *q, int16_t value) {
    if (Queue_IsFull(q)) {
        return false; /* OVERFLOW caller must wait and retry */
    }

    /* Critical section: ISR may read count at any time */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    q->data[q->tail] = value;
    q->tail = (q->tail + 1u) % QUEUE_SIZE;
    q->count++;

    __set_PRIMASK(primask);
    return true;
}

bool Queue_Dequeue(Queue_t *q, int16_t *value) {
    if (Queue_IsEmpty(q)) {
        return false; /* UNDERFLOW error */
    }

    *value = q->data[q->head];
    q->head = (q->head + 1u) % QUEUE_SIZE;
    q->count--;
    return true;
}

bool Queue_IsFull(const Queue_t *q) {
    return (q->count >= QUEUE_SIZE);
}

bool Queue_IsEmpty(const Queue_t *q) {
    return (q->count == 0);
}

uint32_t Queue_Count(const Queue_t *q) {
    return q->count;
}
