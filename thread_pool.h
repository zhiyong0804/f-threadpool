#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>

#ifdef POOL_WITH_EVENT_QUEUE
     #include "async_eventfd_queue.h"
#else
     #include "async_cond_queue.h"
#endif

/**
 * @file threadpool.h
 * @brief Threadpool Header File
 */

 /**
 * Increase this constants at your own risk
 * Large values might slow down your system
 */
#define MAX_THREAD_POOL_SIZE  128
#define MAX_QUEUE_SIZE        65536

//typedef struct threadpool_t threadpool_t;

/**
 *  @struct threadpool
 *  @brief The threadpool struct
 *
 *  @var threads      Array containing worker threads ID.
 *  @var tsize        Number of threads
 *  @var queue        Array containing the task queue.
 *  @var qsize        Size of the task queue.
 *  @var shutdown     Flag indicating if the pool is shutting down
 *  @var started      Number of started threads
 */
typedef struct threadpool {
  pthread_t* threads;
  async_queue_t* queue;
  int qsize;
  int tsize;
  int shutdown;
  int started;
}threadpool_t;


typedef enum {
    threadpool_invalid        = -1,
    threadpool_lock_failure   = -2,
    threadpool_queue_full     = -3,
    threadpool_shutdown       = -4,
    threadpool_thread_failure = -5,
    threadpool_memory_error   = -6
} threadpool_error_t;

typedef enum {
    immediate_shutdown = 1,
    graceful_shutdown  = 2
} threadpool_shutdown_t;

typedef enum {
    threadpool_graceful       = 1
} threadpool_destroy_flags_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @function threadpool_create
 * @brief Creates a threadpool_t object.
 * @param thread_count Number of worker threads.
 * @param queue_size   Size of the queue.
 * @param flags        Unused parameter.
 * @return a newly created thread pool or NULL
 */
threadpool_t *threadpool_create(int thread_count, int queue_size, int flags);

/**
 * @function threadpool_add
 * @brief add a new task in the queue of a thread pool
 * @param pool     Thread pool to which add the task.
 * @param function Pointer to the function that will perform the task.
 * @param argument Argument to be passed to the function.
 * @param flags    Unused parameter.
 * @return 0 if all goes well, negative values in case of error (@see
 * threadpool_error_t for codes).
 */
int threadpool_add(threadpool_t *pool, void* (*routine)(void *), void *arg);

/**
 * @function threadpool_destroy
 * @brief Stops and destroys a thread pool.
 * @param pool  Thread pool to destroy.
 * @param flags Flags for shutdown
 *
 * Known values for flags are 0 (default) and threadpool_graceful in
 * which case the thread pool doesn't accept any new tasks but
 * processes all pending tasks before shutdown.
 */
int threadpool_destroy(threadpool_t *pool, int flags);

#ifdef __cplusplus
}
#endif


#endif

