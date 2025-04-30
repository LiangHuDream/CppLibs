/**
 * Copyright (c) 2025-2025 LiangHuDream
 * MIT License. See LICENSE for details.
 * 
 * Module:池式结构
 * Description:线程池实现测试用例
 * 
 * Date:2025-04-30
 * Author:LiangHuDream
 */


#include <iostream>
#include <vector>
#include <future>
#include <chrono>
#include <atomic>
#include "thread_pool.h"

using namespace PoolTypeStructure;
using namespace std::chrono_literals;

// 全局输出互斥量（生产环境应封装为日志模块）
namespace {
    std::mutex io_mutex;  // 互斥量定义
}

 /**
 * @brief 计算斐波那契数列（递归实现）
 * @param n 数列位置
 * @return 第n项的值
 * @note 用于演示计算密集型任务
 */
uint64_t fibonacci(uint8_t n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

 /**
 * @brief 模拟I/O密集型任务
 * @param duration 模拟的I/O延迟
 */
void simulate_io_task(std::chrono::milliseconds duration) {
    std::this_thread::sleep_for(duration);
}

int main() {
    try {
        // 初始化4工作线程的线程池
        ThreadPool pool(4);
        std::atomic<uint32_t> completed_tasks{0};
        constexpr size_t total_tasks = 20;
        std::vector<std::future<void>> futures;

        // 第一阶段：提交混合型任务
        std::cout << "=== 提交初始任务 ===" << std::endl;
        for (size_t i = 0; i < total_tasks; ++i) {
            futures.emplace_back(pool.SubmitTask([i, &completed_tasks]() {
                // 交替执行计算型和I/O型任务
                if (i % 2 == 0) {
                    auto start = std::chrono::high_resolution_clock::now();
                    uint64_t result = fibonacci(30 + (i % 10));
                    auto end = std::chrono::high_resolution_clock::now();
                    
                    std::lock_guard<std::mutex> lock(io_mutex);
                    std::cout << "计算任务" << i << "完成, 耗时: " 
                            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                            << "ms\n";
                } else {
                    simulate_io_task(100ms * (i % 3 + 1));
                    
                    std::lock_guard<std::mutex> lock(io_mutex);
                    std::cout << "I/O任务" << i << "完成\n";
                }
                completed_tasks.fetch_add(1);
            }));
        }

        // 第二阶段：动态添加紧急任务
        std::cout << "\n=== 添加紧急任务 ===" << std::endl;
        auto urgent_future = pool.SubmitTask([]() {
            std::lock_guard<std::mutex> lock(io_mutex);
            std::cout << "[紧急] 系统监控任务执行\n";
        });

        // 等待所有任务完成
        for (auto& future : futures) {
            future.wait();
        }
        urgent_future.wait();

        // 第三阶段：结果验证
        std::cout << "\n=== 执行结果验证 ===" << std::endl;
        if (completed_tasks.load() != total_tasks) {
            std::cerr << "错误: 完成任务数(" << completed_tasks 
                     << ") 不等于总任务数(" << total_tasks << ")\n";
            return EXIT_FAILURE;
        }

        std::cout << "\n=== 所有任务成功完成 ===" << std::endl;
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "系统错误: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}