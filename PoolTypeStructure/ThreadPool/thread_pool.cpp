/**
 * Copyright (c) 2025-2025 LiangHuDream
 * MIT License. See LICENSE for details.
 *
 * Module:池式结构
 * Description:线程池实现
 *
 * Date:2025-04-30
 * Author:LiangHuDream
 */

#include <iostream>

#include "thread_pool.h"

namespace PoolTypeStructure {

ThreadPool::ThreadPool(size_t threadNum)
    : m_taskQueue(std::make_unique<TaskQueue>()), m_isShutdown(false),
      m_activeThreadCount(0) {
  if (threadNum == 0) {
    throw std::invalid_argument("Thread number cannot be zero");
  }

  try {
    m_workerThreads.reserve(threadNum);
    for (size_t i = 0; i < threadNum; ++i) {
      m_workerThreads.emplace_back(&ThreadPool::WorkerThreadProc, this);
    }
  } catch (...) {
    Shutdown();
    throw;
  }
}

ThreadPool::~ThreadPool() { Shutdown(); }

void ThreadPool::Shutdown() noexcept {
    bool expected = false;
    if (m_isShutdown.compare_exchange_strong(expected, true)) {
        // 通知所有线程处理剩余任务
        m_taskQueue->condition.notify_all();

        // 等待所有工作线程退出
        for (auto& thread : m_workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
}

void ThreadPool::WorkerThreadProc() {
    m_activeThreadCount.fetch_add(1, std::memory_order_relaxed);
    
    while (true) {
        auto task = FetchTask();
        if (!task) break;

        try {
            task();
        } catch (const std::exception& ex) {
            // 异常处理策略：
            // 1. 记录异常日志
            // 2. 维持线程继续运行
            // 3. 异常信息通过future传递
            std::cerr << "[ERROR] Worker thread caught exception: " 
                      << ex.what() << std::endl;
        } catch (...) {
            std::cerr << "[ERROR] Worker thread caught unknown exception" 
                      << std::endl;
        }
    }

    m_activeThreadCount.fetch_sub(1, std::memory_order_relaxed);
}

std::function<void()> ThreadPool::FetchTask() {
    std::unique_lock<std::mutex> lock(m_taskQueue->mutex);
    
    // 修改等待条件：队列非空或关闭触发
    m_taskQueue->condition.wait(lock, [this]() {
        return !m_taskQueue->queue.empty() || m_isShutdown.load();
    });

    // 关闭且队列空时返回空任务
    if (m_isShutdown.load() && m_taskQueue->queue.empty()) {
        return nullptr;
    }

    // 提取任务
    auto task = std::move(m_taskQueue->queue.front());
    m_taskQueue->queue.pop();
    return task;
}

} // namespace PoolTypeStructure
