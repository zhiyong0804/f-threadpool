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
            q->tasks   = (task_t*)malloc(size * sizeof(task_t));
            q->capcity = size;
        }
        else
        {
            int default_size = DEFAULT_QUEUE_SIZE;
            q->tasks    =(task_t*)malloc(default_size * sizeof(task_t));
            q->capcity = default_size;
        }

        if (q->tasks == NULL)
        {
            free(q);
            return NULL;
        }

        q->head = q->tail = q->size = 0;
    }

    return q;
}

BOOL queue_is_full(queue_t *q)
{
    return q->size == q->capcity;
}

BOOL queue_is_empty(queue_t *q)
{
    return q->size == 0;
}

BOOL queue_push_tail(queue_t* q, task_t* t)
{
    if (!queue_is_full(q))
    {
        q->tasks[q->tail].run  = t->run;
        q->tasks[q->tail].argv = t->argv;
        q->tail = (q->tail + 1) % q->capcity;
        q->size++;
        return TRUE;
    }

    return FALSE;
}

task_t* queue_pop_head(queue_t* q)
{
    task_t* t= NULL;
    if (!queue_is_empty(q))
    {
        t = &q->tasks[(q->head)];
        q->head = (q->head + 1) % q->capcity;
        q->size--;
    }
    return t;
}

void queue_free(queue_t* q)
{
    free(q->tasks);
    free(q);
}

