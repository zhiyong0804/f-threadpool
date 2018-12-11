
#ifndef _THREAD_POOL_QUEUE_H_
#define _THREAD_POOL_QUEUE_H_

typedef struct queue
{
    int header;
    int tail;
    int size;
    int capcity;
    void **_buf;
} queue_t;

// API

/**
 * @function threadpool_create
 * @brief create queue
 * @param size the size of queue
 * @return created queue
 */
queue_t *queue_create(int size);

bool queue_is_full(queue_t *q);

bool queue_is_empty(queue_t *q);

bool queue_push_tail(queue_t *q, void *data);

void *queue_pop_head(queue_t *q);

int *queue_free(queue_t *q);



#endif
