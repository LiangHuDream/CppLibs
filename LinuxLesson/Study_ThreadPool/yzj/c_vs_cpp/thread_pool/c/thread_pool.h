#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

typedef struct Task {
    void (*func)(void*);
    void* arg;
    struct Task* next;
} Task;

typedef struct ThreadPool {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t* threads;
    Task* task_queue;
    int thread_count;
    int task_count;
    int shutdown;
} ThreadPool;

ThreadPool* threadpool_create(int num_threads);
void threadpool_add_task(ThreadPool* pool, void (*func)(void*), void* arg);
void threadpool_destroy(ThreadPool* pool);

#endif