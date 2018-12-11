#include <stdlib.h>

#include "queue.h"

#define DEFAULT_QUEUE_SIZE 65535


queue_t *queue_create(int size)
{
    queue_t *q = (queue_t*)malloc(sizeof (queue_t));
    if (q != NULL)
    {
        if (size > 0)
        {
            q->_buf = (void**)malloc(size * sizeof(void*));
            q->capcity = size;
        }
        else
        {
            int default_size = DEFAULT_QUEUE_SIZE;
            q->_buf =(void**)malloc(default_size * sizeof (void *));
            q->capcity = default_size;
        }
        q->header = q->tail = q->size = 0;
    }

    return q;
}

bool queue_is_full(queue_t *q)
{
    return q->size == q->capcity;
}

bool queue_is_empty(queue_t *q)
{
    return q->size == 0;
}

bool queue_push_tail(queue_t *q, void *data)
{
    if (!queue_is_full(q))
    {
        q->_buf[q->tail] = data;
        q->tail = (q->tail + 1) % q->capcity;
        q->size++;
        return true;
    }
    
    return false;
}

void *queue_pop_head(queue_t *q)
{
    void *data = NULL;
    if (!queue_is_empty(q))
    {
        data = q->_buf[(q->header)];
        q->header = (q->header + 1) % q->capcity;
        q->size--;
    }
    return data;
}

int *queue_free(queue_t *q)
{
    free(q->_buf);
    free(q);
}
