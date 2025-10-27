#include "thread_pool.h"

#include <bits/types/time_t.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <thread>
#include <iostream>
#include <unistd.h>

time_t GetTick() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

std::atomic<int64_t> g_count{};
void JustTask(void *ctx)
{
    ++g_count;
}

constexpr int64_t n = 1000000;

void producer(thread_pool_t *pool)
{
    for (int64_t i = 0; i < n; ++i) {
        thread_pool_post(pool, JustTask, nullptr);
    }
}

void test_thread_pool(int producer_num, int consumer_num)
{
    auto pool = thread_pool_create(consumer_num);
    for (int i = 0; i < producer_num; ++i) {
        std::thread(&producer, pool).detach();
    }

    time_t t1 = GetTick();
    while (g_count.load() != n * producer_num) {
        usleep(100000);
    }

    time_t t2 = GetTick();

    std::cout << t2 << " " << t1  << " used: " << t2 - t1 << " exec pre sec: " << (double)g_count.load() * 1000 / (t2 - t1) << std::endl;

    thread_pool_terminate(pool);
    thread_pool_wait(pool);
}

int main()
{
    test_thread_pool(4, 4);
    return 0;
}