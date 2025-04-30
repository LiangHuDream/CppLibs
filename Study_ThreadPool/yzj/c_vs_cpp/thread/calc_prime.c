// File: prime_pthread.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h> 

#define N 10000000
#define THREADS 4

typedef struct {
    int start;
    int end;
    int count;
} ThreadArgs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

void* calculate_primes(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int local_count = 0;
    for (int i = args->start; i <= args->end; i++) {
        if (is_prime(i)) local_count++;
    }
    
    // 同步累加结果
    pthread_mutex_lock(&mutex);
    args->count += local_count;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

int main() {

    struct timespec start, end;  // 新增时间变量
    clock_gettime(CLOCK_MONOTONIC, &start);  // 记录开始时间

    pthread_t threads[THREADS];
    ThreadArgs args[THREADS];
    int segment = N / THREADS;

    for (int i = 0; i < THREADS; i++) {
        args[i].start = i * segment + 1;
        args[i].end = (i == THREADS - 1) ? N : (i + 1) * segment;
        args[i].count = 0;
        pthread_create(&threads[i], NULL, calculate_primes, &args[i]);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);  // 记录结束时间
    double elapsed = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;

    int total = 0;
    for (int i = 0; i < THREADS; i++) {
        total += args[i].count;
    }

    printf("Total primes: %d\n", total);
    printf("Execution time: %.3f seconds\n", elapsed);  // 新增时间输出
    return 0;
}