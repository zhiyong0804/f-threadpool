
#ifndef _THREAD_POOL_QUEUE_H_
#define _THREAD_POOL_QUEUE_H_

#include "glo_def.h"

/**
 *  @struct task_t
 *  @brief the work struct
 *
 *  @var run   Pointer to the function that will perform the task.
 *  @var argv  Argument to be passed to the run function.
 */
typedef struct queue_task
{
    void* (*run)(void *);
    void* argv;
}task_t;

typedef struct queue
{
    int      head;
    int      tail;
    int      size;
    int      capcity;
    task_t*  tasks;
} queue_t;

// API

/**
 * @function threadpool_create
 * @brief create queue
 * @param size the size of queue
 * @return created queue
 */
queue_t *queue_create(int size);

BOOL queue_is_full(queue_t* q);

BOOL queue_is_empty(queue_t* q);

BOOL queue_push_tail(queue_t* q, task_t* data);

task_t* queue_pop_head(queue_t* q);

void queue_free(queue_t* q);



#endif
