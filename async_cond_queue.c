
#include "async_queue_interner.h"
#include "queue.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "systime.h"


static async_queue_t* async_cond_queue_create(int size);
static BOOL async_cond_queue_push_tail(async_queue_t* q, task_t* t);
static task_t* async_cond_queue_pop_head(async_queue_t* q, int timeout);
static BOOL async_cond_queue_empty(async_queue_t* q);
static BOOL async_cond_queue_destory(async_queue_t* q);
static void async_cond_queue_free(async_queue_t* q);


//struct async_queue_op_t;

const async_queue_op_t async_cond_op =
{
    "cond",
    async_cond_queue_create,
    async_cond_queue_push_tail,
    async_cond_queue_pop_head,
    async_cond_queue_free,
    async_cond_queue_empty,
    async_cond_queue_destory
};


static time_t start_stm = 0;

async_queue_t* async_cond_queue_create(int size)
{
    async_queue_t* q = (async_queue_t*)malloc(sizeof (async_queue_t));
    q->queue           = queue_create(size);
    q->waiting_threads = 0;
    q->quit            = 0;
    pthread_mutex_init(&(q->mutex), NULL);
    pthread_cond_init(&(q->cond), NULL);

    start_stm = get_current_timestamp();

    return q;
}

BOOL async_cond_queue_push_tail(async_queue_t* q, task_t* t)
{
    if (!queue_is_full(q->queue))
    {
        pthread_mutex_lock(&(q->mutex));
        queue_push_tail(q->queue, t);
        if (q->waiting_threads > 0)
        {
            pthread_cond_signal(&(q->cond));
        }
        pthread_mutex_unlock(&(q->mutex));

        return TRUE;
    }

    return FALSE;
}

task_t* async_cond_queue_pop_head(async_queue_t* q, int timeout)
{
    task_t *task = NULL;
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&(q->mutex));
    if (queue_is_empty(q->queue))
    {
        q->waiting_threads++;
        while (queue_is_empty(q->queue) && (q->quit == 0))
        {
            gettimeofday(&now, NULL);
            if (now.tv_usec + timeout > 1000)
            {
                outtime.tv_sec = now.tv_sec + 1;
                outtime.tv_nsec = ((now.tv_usec + timeout) % 1000) * 1000;
            }
            else
            {
                outtime.tv_sec = now.tv_sec;
                outtime.tv_nsec = (now.tv_usec + timeout) * 1000;
            }
            pthread_cond_timedwait(&(q->cond), &(q->mutex), &outtime);
        }
        q->waiting_threads--;
    }

    task = queue_pop_head(q->queue);

    /* µ÷ÊÔ´úÂë */
    if (task)
    {
        q->tasked ++;
        static long long precision = 10;
        if ((q->tasked % precision ) == 0)
        {
            time_t current_stm = get_current_timestamp();
            printf("%d tasks cost : %d\n", precision, current_stm - start_stm);
            precision *= 10;
        }
    }
    pthread_mutex_unlock(&(q->mutex));

    return task;
}

BOOL async_cond_queue_destory(async_queue_t* q)
{
    pthread_mutex_lock(&(q->mutex));
    q->quit = 1;  // wake up thread to jump out from async_queue_pop_head
    pthread_cond_broadcast(&(q->cond));
    pthread_mutex_unlock(&(q->mutex));

    return TRUE;
}

void async_cond_queue_free(async_queue_t* q)
{
    queue_free(q->queue);
    pthread_cond_destroy(&(q->cond));
    pthread_mutex_destroy(&(q->mutex));
    free(q);
}

BOOL async_cond_queue_empty(async_queue_t* q)
{
    return queue_is_empty(q->queue);
}

