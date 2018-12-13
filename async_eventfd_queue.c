
#include "queue.h"
//#include "async_queue_interner.h"

#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "systime.h"
#include "queue.h"
#include "async_queue_interner.h"



#define MAX_EVENTS 1024


static async_queue_t* async_eventfd_queue_create(int size);
static BOOL async_eventfd_queue_push_tail(async_queue_t* q, task_t* data);
static task_t* async_eventfd_queue_pop_head(async_queue_t* q, int timeout);
static void async_eventfd_queue_free(async_queue_t* q);
static BOOL async_eventfd_queue_empty(async_queue_t* q);
static BOOL async_eventfd_queue_destory(async_queue_t* q);


const async_queue_op_t async_eventfd_op =
{
    "eventfd",
    async_eventfd_queue_create,
    async_eventfd_queue_push_tail,
    async_eventfd_queue_pop_head,
    async_eventfd_queue_free,
    async_eventfd_queue_empty,
    async_eventfd_queue_destory
};

static time_t start_stm = 0;

async_queue_t *async_eventfd_queue_create(int size)
{
    async_queue_t* q = (async_queue_t*)malloc(sizeof (async_queue_t));

    q->queue   = queue_create(size);
    q->epollfd = epoll_create1(0);
    q->tasked  = 0;
    if (q->epollfd == -1)
    {
        return NULL;
    }

    start_stm = get_current_timestamp();

    return q;
}

BOOL async_eventfd_queue_push_tail(async_queue_t* q, task_t *task)
{
    unsigned long long i = 0xffffffff;
    if (!queue_is_full(q->queue))
    {
        queue_push_tail(q->queue, task);

        struct epoll_event ev;
        int efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (efd == -1) printf("eventfd create: %s", strerror(errno));
        ev.events = EPOLLIN ;// | EPOLLLT;
        ev.data.fd = efd;
        if (epoll_ctl(q->epollfd, EPOLL_CTL_ADD, efd, &ev) == -1)
        {
            return NULL;
        }

        write(efd, &i, sizeof (i));

        return TRUE;
    }

    return FALSE;
}

task_t* async_eventfd_queue_pop_head(async_queue_t* q, int timeout)
{
    unsigned long long i = 0;
    struct epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(q->epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
        return NULL;
    }
    else
    {
        read(events[0].data.fd, &i, sizeof (i));
        close(events[0].data.fd); // NOTE: need to close here
        task_t* task = queue_pop_head(q->queue);

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
        return task;
    }

    return NULL;
}

void async_eventfd_queue_free(async_queue_t *q)
{
    queue_free(q->queue);
    close(q->efd);
    free(q);
}

BOOL async_eventfd_queue_empty(async_queue_t* q)
{
    return queue_is_empty(q->queue);
}

BOOL async_eventfd_queue_destory(async_queue_t* q)
{
    return TRUE;
}

