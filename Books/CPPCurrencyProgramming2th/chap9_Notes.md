@[TOC](高级线程管理)
## 1. 线程池（Thread Pool）
核心目的：避免频繁创建/销毁线程，复用固定数量的线程处理任务队列。

### 1.1 线程池结构要素
- 任务队列：存储待执行的任务（函数对象）
- 工作线程集合：执行任务的线程
- 同步机制：互斥锁（std::mutex）、条件变量（std::condition_variable）
- 停止标志：安全终止池的开关（std::atomic<bool>）
### 1.2 线程池实现步骤
- 步骤1：定义任务队列和工作线程
- 步骤2：初始化工作线程，循环等待任务
- 步骤3：提交任务到队列，唤醒线程
- 步骤4：安全关闭池，等待剩余任务完成
代码示例:
```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads) : stop(false) {
        for(size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using return_type = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if(stop) throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker : workers)
            worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

// 测试用例
int main() {
    ThreadPool pool(4);
    std::vector<std::future<int>> results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(pool.enqueue([i] {
            std::cout << "Task " << i << " executed by thread " 
                      << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return i * i;
        }));
    }

    for(auto&& result : results)
        std::cout << "Result: " << result.get() << std::endl;

    return 0;
}

```
## 2. 线程中断（Interruptible Threads）
核心目标：安全地请求线程终止，而非强制终止（避免资源泄露）。
### 2.1 中断机制实现
- 检查点：线程定期检查中断标志
- 原子标志：std::atomic<bool> 作为中断信号
- 异常传递：通过抛出特定异常跳出任务
代码:
```cpp
#include <iostream>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <mutex>

// 线程安全输出的互斥锁
std::mutex cout_mutex;

class InterruptFlag {
public:
    void set() { flag.store(true); }
    bool is_set() const { return flag.load(); }
private:
    std::atomic<bool> flag{false};
};

class InterruptibleThread {
public:
    template<typename FunctionType>
    InterruptibleThread(FunctionType f, InterruptFlag& flag) 
        : flag(flag), thread([this, f, &flag] {
            try {
                f(flag);
            } catch (...) {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cerr << "Thread interrupted by an unknown exception." << std::endl;
            }
        }) 
    {}

    void join() { thread.join(); }
    void interrupt() { flag.set(); }

private:
    InterruptFlag& flag;
    std::thread thread;
};

// 测试用例
void task(InterruptFlag& flag) {
    while (!flag.is_set()) {
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Working..." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    throw std::runtime_error("Exit by interrupt");
}

int main() {
    InterruptFlag flag;
    InterruptibleThread t(task, flag);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Sending interrupt..." << std::endl;
    }
    t.interrupt();
    t.join();

    return 0;
}

/*
- 协作式中断：线程在安全点检查标志，决定是否退出。
- 异常处理：通过抛出异常跳出深层嵌套调用栈。
- 资源释放：确保线程退出前正确释放持有资源。
*/

```

## 多选题
1.（多选）在线程池设计中，以下哪种情况会导致任务无法被正确执行？  
A. 工作线程在pop_task时未正确处理条件变量的虚假唤醒  
B. 使用shared_ptr保存任务队列的锁，导致死锁  
C. 提交任务时未检查线程池是否已关闭，直接放入队列  
D. 任务队列的互斥锁粒度设计过大，影响并发性能  
E. 在析构线程池时未优雅关闭所有线程  

2.（多选）关于线程中断的正确设计，以下描述错误的是？  
A. 可以通过atomic<bool>标志位实现线程的强制终止  
B. C++标准库中的std::thread原生支持中断操作  
C. 线程池中的任务应包含中断检查点以实现协作式中断  
D. 中断一个阻塞在I/O操作上的线程可能导致资源泄漏  
E. 使用条件变量等待时应结合谓词避免无法唤醒的阻塞  

3.（多选）在线程池的任务调度中，以下哪些操作需要同步？  
A. 检查任务队列是否为空  
B. 多个工作线程同时从队列头部提取任务  
C. 提交新任务到队列尾部  
D. 调整线程池中的线程数量  
E. 修改线程池的运行状态标志  

4.（多选）线程池的优雅关闭应包含哪些步骤？  
A. 停止接受新任务  
B. 等待所有已提交任务执行完成  
C. 强制终止所有阻塞的工作线程  
D. 通过条件变量通知所有工作线程退出循环  
E. 重置任务队列中的未完成任务  

5.（多选）以下哪些设计可能导致线程池的性能瓶颈？  
A. 使用无锁队列实现任务提交  
B. 工作线程频繁竞争同一互斥锁访问队列  
C. 动态调整线程数量时未考虑负载均衡  
D. 任务分发采用简单的轮询策略  
E. 使用线程本地存储缓存高频访问的数据  

## 多选题答案
1. 答案：A、C、E

解析：

A. 工作线程在弹出任务时未处理条件变量的虚假唤醒，可能导致线程错误地尝试处理空队列中的任务，从而崩溃或逻辑错误。
C. 提交任务时不检查线程池是否关闭，可能导致任务被添加到已关闭的队列中，无法执行或引发异常。
E. 析构线程池时未优雅关闭线程，可能导致资源泄漏或线程访问已销毁的数据。
B错误：shared_ptr保存锁不会直接导致死锁，锁本身设计或使用顺序错误才会导致死锁。shared_ptr管理锁可能导致所有权混乱，但题目中“导致死锁”的描述不准确。
D错误：互斥锁粒度过大会降低并发性能，但不会直接导致任务无法执行。

2. 答案：A、B

解析：

A错误：atomic<bool>标志位无法强制终止阻塞中的线程（如等待条件变量或I/O），此为协作式中断，选项描述错误。
B错误：C++标准库的std::thread不原生支持中断，需通过标志位实现协作式中断。
C正确：任务应包含中断检查点。
D正确：中断阻塞在I/O的线程可能引发资源泄漏。
E正确：条件变量需结合谓词避免虚假唤醒。

3. 答案：A、B、C、D、E

解析：

A. 检查队列是否为空需同步，以避免数据竞争。
B. 多个线程提取任务需同步，防止并发修改队列。
C. 提交任务到队列尾部需同步，保证队列的线程安全。
D. 调整线程数量需同步，避免中间状态不一致。
E. 修改运行状态标志需同步，确保可见性与原子性。

4. 答案：A、B、D

解析：

A. 停止接受新任务是优雅关闭的第一步。
B. 等待已提交任务完成保证任务全部处理。
D. 通过条件变量通知线程退出循环是关键步骤。
C错误：强制终止线程是暴力方式，不优雅。
E错误：重置未完成任务非必要，取决于设计。

5. 答案：B、C、D

解析：

B. 频繁竞争同一锁（如任务队列锁）是典型性能瓶颈。
C. 动态调整线程数未负载均衡，可能导致资源浪费或竞争。
D. 轮询策略可能导致负载不均，影响性能。
A正确：无锁队列可提升性能。
E正确：线程本地存储优化高频数据访问，反而是优化点。
## 设计题目
1. 基础线程池实现
题目要求：设计一个固定大小的线程池，支持提交无参数任务。实现任务队列、工作线程启动和任务分发逻辑，并在main函数中测试提交10个任务。
2. 支持任务优先级的线程池
题目要求：扩展线程池，实现任务优先级（高/低）。高优先级任务优先执行。在main函数中测试混合优先级任务的执行顺序。
3. 线程池中的任务返回值
题目要求：实现线程池支持任务返回Future。使用std::packaged_task封装任务，允许调用者获取计算结果。
4. 动态调整线程池大小
题目要求：允许运行时动态增加或减少工作线程数量。需处理线程的启动和退出同步问题。
5. 线程安全的中断机制
题目要求：为线程池中的任务添加协作式中断支持，允许外部请求终止长时间运行的任务。
## 设计题目答案
```cpp
// 1. 
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<typename F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& worker : workers)
            worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

// 测试代码
int main() {
    ThreadPool pool(4);
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "Task " << i << " done by thread " 
                      << std::this_thread::get_id() << std::endl;
        });
    }
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 等待任务完成
    return 0;
}
// 2. 
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
private:
    struct Task {
        std::function<void()> func;
        int priority; // 0:低, 1:高
        bool operator<(const Task& other) const { return priority < other.priority; }
    };

    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop{ false };

    void worker() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->condition.wait(lock, [this] { return this->stop ||!this->tasks.empty(); });
                if (this->stop && this->tasks.empty())
                    return;
                task = std::move(this->tasks.top().func);
                this->tasks.pop();
            }
            task();
        }
    }

public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] { this->worker(); });
        }
    }

    void enqueue(std::function<void()> f, int priority) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push({ std::move(f), priority });
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }
};

int main() {
    ThreadPool pool(2);

    // 高优先级任务
    pool.enqueue([&] {
        std::cout << "High priority task is running." << std::endl;
    }, 1);

    // 低优先级任务
    pool.enqueue([&] {
        std::cout << "Low priority task is running." << std::endl;
    }, 0);

    // 再添加一个高优先级任务
    pool.enqueue([&] {
        std::cout << "Another high priority task is running." << std::endl;
    }, 1);

    // 让主线程休眠一段时间，确保任务有足够的时间执行
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}

// 3. 
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <memory>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop{ false };

    void worker() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->condition.wait(lock, [this] { return this->stop ||!this->tasks.empty(); });
                if (this->stop && this->tasks.empty())
                    return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
            }
            task();
        }
    }

public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] { this->worker(); });
        }
    }

    template<typename Func>
    auto enqueue(Func&& func) -> std::future<typename std::result_of<Func()>::type> {
        using return_type = typename std::result_of<Func()>::type;

        auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::forward<Func>(func)
        );
        
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }
};


int main() {
    // 创建一个包含 2 个线程的线程池
    ThreadPool pool(2);

    // 测试用例 1：返回一个整数
    auto fut1 = pool.enqueue([] { return 42; });
    std::cout << "Result 1: " << fut1.get() << std::endl;

    // 测试用例 2：返回一个字符串
    auto fut2 = pool.enqueue([] { return std::string("Hello, ThreadPool!"); });
    std::cout << "Result 2: " << fut2.get() << std::endl;

    // 测试用例 3：执行一个复杂一点的计算
    auto fut3 = pool.enqueue([] {
        int sum = 0;
        for (int i = 1; i <= 10; ++i) {
            sum += i;
        }
        return sum;
    });
    std::cout << "Result 3: " << fut3.get() << std::endl;

    return 0;
}
// 4. 
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <memory>
#include <chrono>

// 日志输出控制宏
#define ENABLE_LOGGING 1
#if ENABLE_LOGGING
#define LOG(msg) std::cout << msg << std::endl
#else
#define LOG(msg)
#endif

class ThreadPool {
private:
    struct Task {
        std::function<void()> func;
        int priority;
        bool operator<(const Task& other) const { return priority < other.priority; }
    };

    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop{ false };
    std::atomic<size_t> current_size{ 0 };
    std::atomic<size_t> threads_to_exit{ 0 };

    void worker() {
        ++current_size;
        LOG("Thread started. Current size: " << current_size.load());
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                // 等待任务或退出信号
                condition.wait(lock, [this] { return stop || threads_to_exit > 0 ||!tasks.empty(); });

                // 如果需要退出且任务队列为空，则退出线程
                if ((threads_to_exit > 0 && tasks.empty()) || stop) {
                    if (threads_to_exit > 0) {
                        --threads_to_exit;
                    }
                    --current_size;
                    LOG("Thread exited. Current size: " << current_size.load());
                    return;
                }

                task = std::move(tasks.top().func);
                tasks.pop();
            }
            try {
                task();
            } catch (const std::exception& e) {
                LOG("Exception in task: " << e.what());
            }
        }
    }

    void add_threads(size_t num) {
        for (size_t i = 0; i < num; ++i) {
            workers.emplace_back([this] { worker(); });
        }
    }

    void remove_threads(size_t num) {
        threads_to_exit = num;
        condition.notify_all(); // 通知所有线程检查是否需要退出
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            // 等待线程退出
            while (current_size > workers.size() - num) {
                condition.wait(lock);
            }
        }
        // 移除已退出的线程
        workers.erase(workers.begin() + (workers.size() - num), workers.end());
    }

public:
    explicit ThreadPool(size_t initial_size = 4) {
        resize(initial_size);
    }

    void resize(size_t new_size) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (new_size > workers.size()) {
            add_threads(new_size - workers.size());
        } else if (new_size < workers.size()) {
            remove_threads(workers.size() - new_size);
        }
        LOG("Thread pool size resized to: " << workers.size());
    }

    template<typename Func>
    auto enqueue(Func&& func, int priority = 0) -> std::future<typename std::invoke_result<Func>::type> {
        using return_type = typename std::invoke_result<Func>::type;

        auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::forward<Func>(func)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop)
                throw std::runtime_error("Cannot enqueue task on stopped thread pool.");
            tasks.push({ [task]() { (*task)(); }, priority });
        }
        condition.notify_one();
        return res;
    }

    // 添加获取当前线程数量的方法
    size_t get_current_size() const {
        return workers.size();
    }

    // 检查任务队列是否为空，去掉 const 限定符
    bool is_task_queue_empty() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        return tasks.empty();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

// 任务函数增加中断检查
void long_running_task(std::atomic<bool>& interrupt_flag) {
    int count = 0;
    while (!interrupt_flag.load()) {
        // 模拟分段执行任务
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        LOG("Task is running, step: " << count++);
    }
    LOG("Task is interrupted.");
}

int main() {
    try {
        ThreadPool pool(2);

        LOG("Initial thread pool size: " << pool.get_current_size());

        // 增大线程池大小
        pool.resize(4);
        LOG("Thread pool size after increasing: " << pool.get_current_size());

        // 缩小线程池大小
        pool.resize(1);
        LOG("Thread pool size after decreasing: " << pool.get_current_size());

        std::atomic<bool> interrupt_flag(false);
        auto fut = pool.enqueue([&interrupt_flag] { long_running_task(interrupt_flag); });

        std::this_thread::sleep_for(std::chrono::seconds(1));
        interrupt_flag = true;
        fut.wait();

        // 检查任务队列是否为空
        if (pool.is_task_queue_empty()) {
            LOG("Task queue is empty.");
        } else {
            LOG("Task queue is not empty.");
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}    
// 5. 
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <memory>
#include <chrono>

class ThreadPool {
private:
    struct Task {
        std::function<void()> func;
        int priority;
        bool operator<(const Task& other) const { return priority < other.priority; }
    };

    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop{ false };
    std::atomic<size_t> current_size{ 0 };
    std::atomic<size_t> threads_to_exit{ 0 };

    void worker() {
        ++current_size;
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                condition.wait(lock, [this] { return stop || threads_to_exit > 0 ||!tasks.empty(); });

                // 如果需要退出且任务队列为空，则退出线程
                if ((threads_to_exit > 0 && tasks.empty()) || stop) {
                    if (threads_to_exit > 0) {
                        --threads_to_exit;
                    }
                    --current_size;
                    return;
                }

                task = std::move(tasks.top().func);
                tasks.pop();
            }
            task();
        }
    }

public:
    explicit ThreadPool(size_t initial_size = 4) {
        resize(initial_size);
    }

    void resize(size_t new_size) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (new_size > current_size) {
            for (size_t i = current_size; i < new_size; ++i) {
                workers.emplace_back([this] { worker(); });
            }
        } else if (new_size < current_size) {
            threads_to_exit = current_size - new_size;
            condition.notify_all(); // 通知所有线程检查是否需要退出
            // 等待线程退出
            while (current_size > new_size) {
                condition.wait(lock);
            }
            // 移除已退出的线程
            workers.erase(workers.begin() + new_size, workers.end());
        }
    }

    template<typename R>
    std::future<R> enqueue(std::function<R()> task, int priority = 0) {
        auto packaged = std::make_shared<std::packaged_task<R()>>(task);
        auto fut = packaged->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            // 修改为使用 push 方法
            tasks.push({ [packaged] { (*packaged)(); }, priority });
        }
        condition.notify_one();
        return fut;
    }

    // 添加获取当前线程数量的方法
    size_t get_current_size() const {
        return current_size;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

// 任务函数增加中断检查
void long_running_task(std::atomic<bool>& interrupt_flag) {
    int count = 0;
    while (!interrupt_flag.load()) {
        // 模拟分段执行任务
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "Task is running, step: " << count++ << std::endl;
    }
    std::cout << "Task is interrupted." << std::endl;
}

int main() {
    ThreadPool pool(2);

    // 定义中断标志
    std::atomic<bool> flag(false);

    // 显式转换 lambda 为 std::function
    auto task = std::function<void()>([&flag] { long_running_task(flag); });
    auto fut = pool.enqueue(task);

    // 主线程休眠一段时间，让任务执行一会儿
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 外部触发中断
    std::cout << "Triggering interruption..." << std::endl;
    flag.store(true);

    // 等待任务完成
    fut.wait();

    return 0;
}

```