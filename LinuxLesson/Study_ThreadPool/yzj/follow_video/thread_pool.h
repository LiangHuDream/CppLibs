#pragma once

typedef struct thread_pool_s thread_pool_t;

typedef void (*handler_ptr)(void * /* ctx */); // 上下文，


thread_pool_t *thread_pool_create(int thread_num);
void thread_pool_terminate(thread_pool_t *pool);
int thread_pool_post(thread_pool_t *pool, handler_ptr func, void *args);
void thread_pool_wait(thread_pool_t *pool);

