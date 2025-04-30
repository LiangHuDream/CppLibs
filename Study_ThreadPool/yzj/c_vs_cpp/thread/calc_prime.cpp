// File: prime_std_thread.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

constexpr int N = 10000000;
constexpr int THREADS = 4;

std::mutex g_mutex;
int g_total = 0;

bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

void calculate_primes(int start, int end) {
    int local_count = 0;
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) local_count++;
    }
    
    // 自动锁管理（RAII）
    std::lock_guard<std::mutex> lock(g_mutex);
    g_total += local_count;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();  // 记录开始时间

    std::vector<std::thread> threads;
    int segment = N / THREADS;

    for (int i = 0; i < THREADS; i++) {
        int start = i * segment + 1;
        int end = (i == THREADS - 1) ? N : (i + 1) * segment;
        threads.emplace_back(calculate_primes, start, end);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();  // 记录结束时间
    std::chrono::duration<double> elapsed = end - start;


    std::cout << "Total primes: " << g_total << std::endl;
    std::cout << "Execution time: " << elapsed.count() << " seconds\n";  // 新增时间输出
    return 0;
}