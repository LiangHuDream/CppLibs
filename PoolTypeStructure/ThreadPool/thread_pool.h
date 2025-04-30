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
#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <memory>

namespace PoolTypeStructure {

/**
 * @brief 线程池管理类
 * @details 提供多线程任务调度能力，支持以下特性：
 *          - 固定数量工作线程
 *          - 自动任务分发
 *          - 优雅关闭机制
 *          - 任务结果追踪
 */
class ThreadPool final {
public:
    /**
     * @brief 构造函数
     * @param[in] threadNum 初始线程数量，默认值为硬件并发数
     * @exception std::invalid_argument 当threadNum为0时抛出
     */
    explicit ThreadPool(size_t threadNum = std::thread::hardware_concurrency());

    /**
     * @brief 析构函数
     * @details 自动执行关闭操作并等待所有线程退出
     */
    ~ThreadPool();

    /**
     * @brief 提交任务到线程池
     * @tparam F 可调用对象类型
     * @tparam Args 参数类型列表
     * @param[in] task 待执行的任务函数
     * @param[in] args 任务参数列表
     * @return std::future<typename std::result_of<F(Args...)>::type> 任务执行结果句柄
     * @exception std::runtime_error 当线程池已关闭时抛出
     * @note 参数传递采用完美转发机制
     */
    template <typename F, typename... Args>
    auto SubmitTask(F&& task, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;

    /**
     * @brief 关闭线程池
     * @details 执行以下操作：
     *          - 停止接受新任务
     *          - 等待已提交任务完成
     *          - 回收所有工作线程
     */
    void Shutdown() noexcept;

private:
    // 禁止拷贝和移动操作
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    /**
     * @brief 工作线程执行循环
     * @details 主要处理逻辑：
     *          1. 等待任务到达或关闭信号
     *          2. 提取并执行任务
     *          3. 处理任务异常
     */
    void WorkerThreadProc();

    /**
     * @brief 从任务队列提取任务
     * @return std::function<void()> 待执行任务
     * @retval 空函数对象表示无任务
     */
    std::function<void()> FetchTask();

    /// 工作线程容器
    std::vector<std::thread> m_workerThreads;

    /// 任务队列及同步机制
    struct TaskQueue {
        std::queue<std::function<void()>> queue; // 任务存储队列
        std::mutex mutex;                       // 队列访问互斥量
        std::condition_variable condition;      // 任务到达条件变量
    };
    std::unique_ptr<TaskQueue> m_taskQueue;

    /// 线程池状态控制
    std::atomic<bool> m_isShutdown;            // 关闭标志
    std::atomic<size_t> m_activeThreadCount;    // 活跃线程计数
};

// 模板方法实现
template <typename F, typename... Args>
auto ThreadPool::SubmitTask(F&& task, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using ReturnType = typename std::result_of<F(Args...)>::type;

    if (m_isShutdown.load(std::memory_order_acquire)) {
        throw std::runtime_error("Submit task on stopped thread pool");
    }

    // 封装任务为共享指针，保证生命周期
    auto taskWrapper = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(task), std::forward<Args>(args)...)
    );

    // 获取future对象并存储任务
    std::future<ReturnType> result = taskWrapper->get_future();
    {
        std::unique_lock<std::mutex> lock(m_taskQueue->mutex);
        m_taskQueue->queue.emplace([taskWrapper](){ (*taskWrapper)(); });
    }

    // 通知等待线程
    m_taskQueue->condition.notify_one();
    return result;
}

} // namespace PoolTypeStructure