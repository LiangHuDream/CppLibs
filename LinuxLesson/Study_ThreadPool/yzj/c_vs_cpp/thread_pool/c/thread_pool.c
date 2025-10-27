#include "thread_pool.h"
#include <stdlib.h>

static void* worker_thread(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    while (1) {
        pthread_mutex_lock(&pool->lock);
        while (pool->task_count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->cond, &pool->lock);
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }

        Task* task = pool->task_queue;
        if (task) {
            pool->task_queue = task->next;
            pool->task_count--;
        }
        pthread_mutex_unlock(&pool->lock);

        if (task) {
            task->func(task->arg);
            free(task);
        }
    }
    return NULL;
}

ThreadPool* threadpool_create(int num_threads) {
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);
    pool->threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    pool->task_queue = NULL;
    pool->thread_count = num_threads;
    pool->task_count = 0;
    pool->shutdown = 0;

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, worker_thread, pool);
    }
    return pool;
}

void threadpool_add_task(ThreadPool* pool, void (*func)(void*), void* arg) {
    Task* task = (Task*)malloc(sizeof(Task));
    task->func = func;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&pool->lock);
    if (pool->task_queue) {
        Task* last = pool->task_queue;
        while (last->next) last = last->next;
        last->next = task;
    } else {
        pool->task_queue = task;
    }
    pool->task_count++;
    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->lock);
}

void threadpool_destroy(ThreadPool* pool) {
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->lock);

    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    free(pool->threads);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    free(pool);
}