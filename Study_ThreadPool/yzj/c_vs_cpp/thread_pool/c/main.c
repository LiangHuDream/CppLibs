#include "thread_pool.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define TASK_COUNT 10000
#define THREADS 4

typedef struct {
    int num;
    long* result;
} SquareTask;

void square_task(void* arg) {
    SquareTask* task = (SquareTask*)arg;
    *(task->result) += task->num * task->num;
    free(task);
}

int main() {
    ThreadPool* pool = threadpool_create(THREADS);
    long total = 0;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 1; i <= TASK_COUNT; i++) {
        SquareTask* task = (SquareTask*)malloc(sizeof(SquareTask));
        task->num = i;
        task->result = &total;
        threadpool_add_task(pool, square_task, task);
    }

    threadpool_destroy(pool); // 等待所有任务完成
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("C ThreadPool Result: %ld\n", total);
    printf("Execution time: %.3f seconds\n", elapsed);
    return 0;
}