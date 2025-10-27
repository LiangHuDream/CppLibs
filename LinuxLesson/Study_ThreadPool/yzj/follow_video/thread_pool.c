#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include "thread_pool.h"
#include "spin_lock.h"
#include "atomic.h"

typedef struct spin_lock spin_lock_t;

typedef struct task_s {
    void *next;
    handler_ptr func;
    void *arg;
} task_t;

typedef struct task_queue_s {
    void *head;
    void **tail;
    int block;
    spin_lock_t lock;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} task_queue_t;

typedef struct thread_pool_s {
    task_queue_t *task_queue;
    atomic_int quit;
    int thread_count;
    pthread_t *threads;
} thread_pool_t;

static task_queue_t *__task_queue_create() {
    int ret;
    task_queue_t *queue = (task_queue_t*)malloc(sizeof(task_queue_t));

    if (queue != NULL) {
        ret = pthread_mutex_init(&queue->mutex, NULL);
        if (ret == 0) {
            ret = pthread_cond_init(&queue->cond, NULL);
            if (ret == 0) {
                spin_lock_init(&queue->lock);
                queue->head = NULL;
                queue->tail = &queue->head;
                queue->block = 1;
                return queue;
            }
    
            pthread_mutex_destroy(&queue->mutex);
        }

        free(queue);
    }
    return NULL;
}

static void __nonblock(task_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    queue->block = 0;
    pthread_mutex_unlock(&queue->mutex);

    pthread_cond_broadcast(&queue->cond);
}

static inline void __add_task(task_queue_t *queue, void *task)
{
    void **link = (void**)task;

    *link = NULL;
}

static inline void* __pop_task(task_queue_t *queue)
{
    spin_lock_lock(&queue->lock);

    if (queue->head == NULL) {
        spin_lock_unlock(&queue->lock);
        return NULL;
    }

    task_t *task = (task_t*)queue->head;
    
    void **link = (void**)task;
    queue->head = *link;

    if (queue->head == NULL) {
        queue->tail = &queue->head;
    }

    spin_lock_unlock(&queue->lock);
    return task;
}

static inline void* __get_task(task_queue_t *queue)
{
    task_t *task;

    while ((task = (task_t *)__pop_task(queue)) == NULL) {
        pthread_mutex_lock(&queue->mutex);
        if (queue->block == 0) {
            pthread_mutex_unlock(&queue->mutex);
            return NULL;
        }

        // 1 先unlock(&mtx)
        // 2 cond 休眠， __add_task 时唤醒
        // 3 在cond唤醒
        // 4 加上lock(&mtx)

        pthread_cond_wait(&queue->cond, &queue->mutex);
        pthread_mutex_unlock(&queue->mutex);
    }

    return task;
}

static void __task_queue_destroy(task_queue_t *queue)
{
    task_t *task;
    while ((task = (task_t*)__pop_task(queue))) {
        free(task);
    }

    spin_lock_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->mutex);

    free(queue);
}

static void *__thread_pool_worker(void *arg)
{
    thread_pool_t *pool = (thread_pool_t*)arg;
    task_t *task;
    void *ctx;

    while (atomic_load(&pool->quit) == 0) {
        task = (task_t*)__get_task(pool->task_queue);
        if (!task) {
            break;
        }

        handler_ptr func = task->func;
        ctx = task->arg;
        free(task);
        func(ctx);
    }

    return NULL;
}

static void __threads_terminate(thread_pool_t *pool)
{
    atomic_store(&pool->quit, 1);
    __nonblock(pool->task_queue);

    for (int i = 0; i < pool->thread_count; ++i) {
        pthread_join(pool->threads[i], NULL);
    }
}

static int __threads_create(thread_pool_t *pool, size_t thread_count)
{
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);

    if (ret == 0) {
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
        if (pool->threads) {
            int i = 0;
            for (; i < thread_count; ++i) {
                if (pthread_create(&pool->threads[i], &attr, __thread_pool_worker, pool) != 0) {
                    break;
                }
            }

            pool->thread_count = i;

            pthread_attr_destroy(&attr);

            if (i == thread_count) {
                return 0;
            }
            
            __threads_terminate(pool);
            free(pool->threads);
        }

        ret = -1;
    }

    return ret;
}

void thread_pool_terminate(thread_pool_t *pool)
{
    atomic_store(&pool->quit, 1);
    __nonblock(pool->task_queue);
}

thread_pool_t *thread_pool_create(int thread_count)
{
    thread_pool_t *pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
    if (pool != NULL) {
        task_queue_t *queue = __task_queue_create();
        if (queue != NULL) {
            pool->task_queue = queue;

            atomic_init(&pool->quit, 0);

            if (__threads_create(pool, thread_count) == 0) {
                return pool;
            }

            __task_queue_destroy(queue);
        }

        free(pool);
    }

    return NULL;
}

int thread_pool_post(thread_pool_t *pool, handler_ptr func, void *arg)
{
    if (atomic_load(&pool->quit) == 1) {
        return -1;
    }

    task_t *task = (task_t*)malloc(sizeof(task_t));
    if (task == NULL) {
        return -1;
    }

    task->func = func;
    task->arg = arg;

    __add_task(pool->task_queue, task);

    return 0;
}

void thread_pool_wait(thread_pool_t *pool)
{
    for (int i = 0; i < pool->thread_count; ++i) {
        pthread_join(pool->threads[i], NULL);
    }

    __task_queue_destroy(pool->task_queue);

    free(pool->threads);
    free(pool);
}
