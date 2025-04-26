
## 1. 线程间工作划分（工作窃取）

**概念**：使用工作窃取（Work Stealing）策略平衡负载。空闲线程从其他线程的任务队列尾部“偷”任务执行，减少闲置线程。
**代码示例**：线程池实现工作窃取队列

```cpp
#include <deque>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>
#include <iostream>
#include <random>

class WorkStealingQueue {
private:
    std::deque<std::function<void()>> tasks;
    mutable std::mutex mtx;

public:
    void push(std::function<void()> task) {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push_front(std::move(task));
    }

    bool try_pop(std::function<void()>& task) {
        std::lock_guard<std::mutex> lock(mtx);
        if (tasks.empty()) return false;
        task = std::move(tasks.front());
        tasks.pop_front();
        return true;
    }

    bool try_steal(std::function<void()>& task) {
        std::lock_guard<std::mutex> lock(mtx);
        if (tasks.empty()) return false;
        task = std::move(tasks.back());
        tasks.pop_back();
        return true;
    }
};

class ThreadPool {
private:
    std::vector<WorkStealingQueue> queues;
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};
    std::random_device rd;
    std::mt19937 gen;

public:
    ThreadPool(size_t threads_count) : queues(threads_count), gen(rd()) {
        for (size_t i = 0; i < threads_count; ++i) {
            threads.emplace_back([this, i] {
                while (!stop) {
                    std::function<void()> task;
                    if (queues[i].try_pop(task)) {
                        task();
                    } else {
                        // 随机选择队列进行窃取
                        std::uniform_int_distribution<> dis(0, queues.size() - 1);
                        for (size_t j = 0; j < queues.size(); ++j) {
                            size_t index = (i + dis(gen)) % queues.size();
                            if (index != i && queues[index].try_steal(task)) {
                                task();
                                break;
                            }
                        }
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        stop = true;
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    template<typename Func>
    void submit(Func&& func) {
        static thread_local size_t currentQueue = 0;
        queues[currentQueue].push(std::forward<Func>(func));
        currentQueue = (currentQueue + 1) % queues.size();
    }
};

int main() {
    ThreadPool pool(4);
    for (int i = 0; i < 100; ++i) {
        pool.submit([i] {
            std::cout << "Task " << i << " executed by thread "
                      << std::this_thread::get_id() << std::endl;
        });
    }
    // 让主线程等待一段时间，确保任务执行完成
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}

```

## 2. 性能优化（伪共享与缓存行对齐）

- 难点：伪共享（False Sharing）是由于不同线程频繁访问同一缓存行的不同变量，导致缓存行无效化。
- 解决方案：填充数据使变量独占缓存行。

```cpp
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

// 定义使用缓存行对齐的计数器结构体
struct alignas(64) CacheLineAlignedCounter {
    std::atomic<int> value{0};
    char padding[64 - sizeof(std::atomic<int>)];
};

// 不使用缓存行对齐的计数器结构体
struct UnalignedCounter {
    std::atomic<int> value{0};
};

// 测试使用缓存行对齐的计数器
void test_false_sharing_aligned() {
    CacheLineAlignedCounter counter1, counter2;
    auto start = std::chrono::high_resolution_clock::now();
    std::thread t1([&] { for(int i = 0; i < 1e6; ++i) ++counter1.value; });
    std::thread t2([&] { for(int i = 0; i < 1e6; ++i) ++counter2.value; });
    t1.join();
    t2.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Time taken with cache line alignment: " << duration << " microseconds" << std::endl;
}

// 测试不使用缓存行对齐的计数器
void test_false_sharing_unaligned() {
    UnalignedCounter counter1, counter2;
    auto start = std::chrono::high_resolution_clock::now();
    std::thread t1([&] { for(int i = 0; i < 1e6; ++i) ++counter1.value; });
    std::thread t2([&] { for(int i = 0; i < 1e6; ++i) ++counter2.value; });
    t1.join();
    t2.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Time taken without cache line alignment: " << duration << " microseconds" << std::endl;
}

int main() {
    // 测试使用缓存行对齐的情况
    test_false_sharing_aligned();
    // 测试不使用缓存行对齐的情况
    test_false_sharing_unaligned();
    return 0;
}
```

## 3. 设计并发数据结构（无锁队列）

示例：基于原子操作的简单无锁队列（Michael-Scott算法）。

```cpp
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

template<typename T>
class LockFreeQueue {
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node(T val) : data(std::move(val)), next(nullptr) {}
    };

    std::atomic<Node*> head, tail;

public:
    LockFreeQueue() : head(new Node(T{})), tail(head.load()) {}

    void enqueue(T value) {
        Node* newNode = new Node(std::move(value));
        Node* oldTail = tail.load();
        while (!tail.compare_exchange_weak(oldTail, newNode)) {}
        oldTail->next.store(newNode);
    }

    bool dequeue(T& result) {
        Node* oldHead = head.load();
        Node* nextNode = oldHead->next.load();
        if (!nextNode) return false;
        result = std::move(nextNode->data);
        head.store(nextNode);
        delete oldHead;
        return true;
    }
};

void testLockFreeQueue() {
    LockFreeQueue<int> queue;
    const int numThreads = 4;
    const int numOperations = 1000;

    auto enqueueTask = [&queue]() {
        for (int i = 0; i < numOperations; ++i) {
            queue.enqueue(i);
        }
    };

    auto dequeueTask = [&queue]() {
        int result;
        for (int i = 0; i < numOperations; ++i) {
            if (queue.dequeue(result)) {
                // 可以在这里添加输出，查看出队元素
                // std::cout << "Dequeued: " << result << std::endl;
                ;
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads / 2; ++i) {
        threads.emplace_back(enqueueTask);
    }
    for (int i = 0; i < numThreads / 2; ++i) {
        threads.emplace_back(dequeueTask);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Test completed." << std::endl;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    testLockFreeQueue();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Test took " << duration << " milliseconds." << std::endl;
    return 0;
}
```

## 4. 多选题目

1. 在并发设计中，工作窃取（Work Stealing）的优势包括：
   A. 减少线程切换开销B. 提高任务分配的负载均衡C. 完全避免锁的使用D. 降低任务队列的竞争
2. 伪共享（False Sharing）是因为：A. 多线程同时修改同一变量B. 多个变量位于同一缓存行被频繁访问
   C. 锁的争用导致线程阻塞
   D. 内存分配不连续
3. 无锁队列的设计通常依赖于：A. std::mutexB. 原子操作（Atomic Operations）
   C. 条件变量（Condition Variables）D. RAII锁管理
4. 以下哪些是设计并发代码的正确实践：
   A. 优先使用细粒度锁
   B. 避免所有全局变量C. 最小化临界区范围
   D. 频繁创建销毁线程
5. 影响多线程性能的关键因素包括：
   A. 缓存局部性（Cache Locality）
   B. 线程数量与内核数的匹配
   C. 内存带宽
   D. 使用递归锁

## 5. 多选题目答案

1. 答案：B、D
   解析：工作窃取（Work Stealing）的核心优势是提高负载均衡（B）。当某些线程完成任务后，可以从其他线程的任务队列中“窃取”任务，避免空闲，从而提升整体效率。同时，由于每个线程通常维护自己的本地任务队列，只有在需要窃取时才访问其他队列，因此减少了全局任务队列的竞争（D）。工作窃取并不能完全消除线程切换开销（A），也仍可能涉及锁或无锁结构，但并非完全避免锁使用（C错误）。
2. 答案：B
   解析：伪共享（False Sharing）是由于多个变量实际位于同一缓存行（CPU缓存的最小单元），当不同线程频繁修改同一缓存行中的不同变量时，会触发缓存行的同步机制（如MESI协议），导致性能下降。关键在于缓存行的共享，而非直接修改同一变量（A错误）或锁的争用（C错误）。
3. 答案：B

解析：无锁队列的核心是依赖原子操作（如std::atomic）来确保操作的原子性，无需使用传统锁（如std::mutex），从而避免线程阻塞。条件变量（C）和RAII锁（D）是传统同步机制，与无锁设计无关。
4. 答案：C

解析：最小化临界区范围（如缩短锁的持有时间）是并发设计的最佳实践之一（C），可减少竞争和阻塞。细粒度锁（A）可能导致更复杂的死锁问题，需谨慎；“避免所有全局变量”（B）不现实，正确的做法是合理保护共享数据；频繁创建销毁线程（D）会引入性能开销，通常应使用线程池。
5. 答案：A、B、C

解析：
缓存局部性（A）：良好的缓存利用率（如减少伪共享）可显著提升性能。
线程与内核数的匹配（B）：过多线程会导致上下文切换开销，过少则无法利用多核。
内存带宽（C）：多线程频繁访问内存时，带宽可能成为瓶颈。递归锁（D）容易引入死锁，与性能无关。

## 4. 设计题目

1. 设计一个线程安全的环形缓冲区（Ring Buffer）
   要求：支持多生产者多消费者，使用无锁或细粒度锁。
   答案要点：基于原子变量索引管理，检查头和尾的CAS操作。
2. 优化快速排序算法的并行版本
   思路：递归分治时，当子任务小于阈值时切到串行，并用线程池并行处理大任务。
3. 实现一个支持超时获取的阻塞队列
   代码框架：

template`<typename T>`
class BlockingQueue {
    std::queue`<T>` queue;
    std::mutex mtx;
    std::condition_variable cv;
public:
    bool push(T item, std::chrono::milliseconds timeout) { /* ... */ }
    bool pop(T& item, std::chrono::milliseconds timeout) { /* ... */ }
};

4. 设计一个可动态调整线程数量的线程池
   策略：根据队列中任务数量增减线程，避免空闲线程资源浪费。
5. 实现一个读写锁（Read-Write Lock）
   要求：允许并发读，排他写，优先写者。
   核心：使用两个互斥量和一个计数器，写锁获取时阻塞后续读锁。 把答案放到最后， 并详解，  涉及到的代码在main函数中生成测试用例进行输出测试

## 5. 设计题目参考答案

```cpp
// 1. 线程安全环形缓冲区（无锁实现）
#include <thread>
#include <iostream>
#include <atomic>
#include <vector>
#include <condition_variable>
#include <mutex>

template<typename T, size_t Capacity>
class RingBuffer {
private:
    std::vector<T> buffer;
    std::atomic<size_t> head{0}, tail{0};
    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;

public:
    RingBuffer() : buffer(Capacity) {}

    // 向缓冲区中插入元素
    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        // 等待缓冲区有空闲位置
        not_full.wait(lock, [this] {
            size_t current_tail = tail.load(std::memory_order_relaxed);
            size_t next_tail = (current_tail + 1) % Capacity;
            return next_tail != head.load(std::memory_order_acquire);
        });
        size_t current_tail = tail.load(std::memory_order_relaxed);
        buffer[current_tail] = item;
        tail.store((current_tail + 1) % Capacity, std::memory_order_release);
        // 通知消费者缓冲区有新元素
        not_empty.notify_one();
    }

    // 从缓冲区中取出元素
    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        // 等待缓冲区中有元素
        not_empty.wait(lock, [this] {
            return head.load(std::memory_order_relaxed) != tail.load(std::memory_order_acquire);
        });
        size_t current_head = head.load(std::memory_order_relaxed);
        T item = buffer[current_head];
        head.store((current_head + 1) % Capacity, std::memory_order_release);
        // 通知生产者缓冲区有空闲位置
        not_full.notify_one();
        return item;
    }
};

// 测试环形缓冲区的函数
void test_ring_buffer() {
    RingBuffer<int, 8> rb;
    std::thread producer([&] {
        for (int i = 0; i < 10; ++i) {
            rb.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    std::thread consumer([&] {
        for (int i = 0; i < 10; ++i) {
            int val = rb.pop();
            std::cout << "Consumed: " << val << std::endl;
        }
    });
    producer.join();
    consumer.join();
}

int main() {
    test_ring_buffer();
    return 0;
}

// 2. 优化并行快速排序
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <atomic>
#include <iostream>
#include <chrono>
#include <cstdlib>

const size_t PARALLEL_THRESHOLD = 1000;

template<typename Iter>
void parallel_quick_sort(Iter begin, Iter end) {
    if (end - begin <= PARALLEL_THRESHOLD) {
        std::sort(begin, end);
        return;
    }

    auto pivot = *(begin + (end - begin) / 2);
    Iter middle = std::partition(begin, end, [pivot](const auto& val) {
        return val < pivot;
    });
    std::thread left(parallel_quick_sort<Iter>, begin, middle);
    parallel_quick_sort(middle, end); // 当前线程处理右半部分
    left.join();
}

// 测试代码
void test_parallel_sort() {
    std::vector<int> data(10000);
    std::generate(data.begin(), data.end(), [] { return rand() % 1000; });
    auto start = std::chrono::high_resolution_clock::now();
    parallel_quick_sort(data.begin(), data.end());
    auto end = std::chrono::high_resolution_clock::now();
    auto parallel_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Parallel sorted: " << std::is_sorted(data.begin(), data.end()) << std::endl;
    std::cout << "Parallel sort took " << parallel_duration << " ms" << std::endl;

    // 测试普通 std::sort
    std::vector<int> data2(10000);
    std::generate(data2.begin(), data2.end(), [] { return rand() % 1000; });
    start = std::chrono::high_resolution_clock::now();
    std::sort(data2.begin(), data2.end());
    end = std::chrono::high_resolution_clock::now();
    auto std_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Std sorted: " << std::is_sorted(data2.begin(), data2.end()) << std::endl;
    std::cout << "Std sort took " << std_duration << " ms" << std::endl;
}

int main() {
    test_parallel_sort();
    return 0;
}

// 3. 支持超时的阻塞队列
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>

template<typename T>
class BlockingQueue {
    std::queue<T> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
public:
    bool push(T item, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, timeout, [this]{ return true; })) { // 永真条件，表示可插入
            queue.push(std::move(item));
            cv.notify_one();
            return true;
        }
        return false;
    }

    bool pop(T& item, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, timeout, [this]{ return !queue.empty(); })) {
            item = std::move(queue.front());
            queue.pop();
            return true;
        }
        return false;
    }
};

// 测试代码
void test_blocking_queue() {
    BlockingQueue<int> q;
    std::thread producer([&]{
        q.push(42, std::chrono::seconds(1));
    });
    int val;
    std::thread consumer([&]{
        q.pop(val, std::chrono::seconds(1));
        std::cout << "Received: " << val << std::endl;
    });
    producer.join();
    consumer.join();
}

// 测试超时情况
void test_timeout() {
    BlockingQueue<int> q;
    int val;
    // 尝试在队列为空时弹出元素，设置超时时间
    bool result = q.pop(val, std::chrono::milliseconds(500));
    if (result) {
        std::cout << "Popped value: " << val << std::endl;
    } else {
        std::cout << "Pop operation timed out." << std::endl;
    }
}

// 多生产者多消费者测试
void test_multi_producer_consumer() {
    BlockingQueue<int> q;
    const int numProducers = 2;
    const int numConsumers = 2;
    const int numItems = 10;

    auto producerTask = [&](int id) {
        for (int i = 0; i < numItems; ++i) {
            if (q.push(i, std::chrono::seconds(1))) {
                std::cout << "Producer " << id << " pushed: " << i << std::endl;
            }
        }
    };

    auto consumerTask = [&](int id) {
        int val;
        for (int i = 0; i < numItems; ++i) {
            if (q.pop(val, std::chrono::seconds(1))) {
                std::cout << "Consumer " << id << " popped: " << val << std::endl;
            }
        }
    };

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // 创建生产者线程
    for (int i = 0; i < numProducers; ++i) {
        producers.emplace_back(producerTask, i);
    }
    // 创建消费者线程
    for (int i = 0; i < numConsumers; ++i) {
        consumers.emplace_back(consumerTask, i);
    }

    // 等待生产者线程完成
    for (auto& p : producers) {
        p.join();
    }
    // 等待消费者线程完成
    for (auto& c : consumers) {
        c.join();
    }
}

int main() {
    std::cout << "Running basic test..." << std::endl;
    test_blocking_queue();

    std::cout << "\nRunning timeout test..." << std::endl;
    test_timeout();

    std::cout << "\nRunning multi-producer multi-consumer test..." << std::endl;
    test_multi_producer_consumer();

    return 0;
}

// 4. 动态线程池
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <chrono>
#include <functional> // 引入正确的头文件

class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop{false};
    size_t max_threads;

    void worker() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this]{ return stop || !tasks.empty(); });
                if (stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

public:
    explicit ThreadPool(size_t initial = 4, size_t max = 16) : max_threads(max) {
        for (size_t i = 0; i < initial; ++i) {
            workers.emplace_back([this] { worker(); });
        }
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.emplace(std::forward<F>(f));
            if (workers.size() < max_threads && tasks.size() > workers.size()) {
                workers.emplace_back([this] { worker(); });
            }
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        stop = true;
        cv.notify_all();
        for (auto& worker : workers) worker.join();
    }
};

// 测试代码
void test_thread_pool() {
    ThreadPool pool;
    std::cout << "Starting to enqueue tasks..." << std::endl;
    for (int i = 0; i < 20; ++i) {
        pool.enqueue([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "Task " << i << " executed by thread " << std::this_thread::get_id() << std::endl;
        });
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "All tasks should be completed." << std::endl;
}

int main() {
    std::cout << "Running thread pool test..." << std::endl;
    test_thread_pool();
    std::cout << "Thread pool test completed." << std::endl;
    return 0;
}

// 5. 
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <chrono>

class ReadWriteLock {
private:
    std::mutex mtx;
    std::condition_variable read_cv;   // 用于读操作的等待
    std::condition_variable write_cv;  // 用于写操作的等待
    int readers = 0;                   // 当前活跃的读者数量
    int writers_waiting = 0;           // 正在等待的写者数量
    bool is_writing = false;           // 是否正在写操作

public:
    // 获取读锁：如果允许读（没有写者正在写或等待），则立即读；否则等待
    void read_lock() {
        std::unique_lock<std::mutex> lock(mtx);
        // 等待条件：没有写者正在写，且没有写者在等待（优先写者）
        read_cv.wait(lock, [this]() {
            return !is_writing && writers_waiting == 0;
        });
        readers++;
    }

    // 释放读锁：减少读者数量，若全部读完且有待处理的写者，则唤醒一个写者
    void read_unlock() {
        std::unique_lock<std::mutex> lock(mtx);
        readers--;
        if (readers == 0 && writers_waiting > 0) {
            write_cv.notify_one();
        }
    }

    // 获取写锁：增加等待的写者数量，直到无读者和写者运行时进入
    void write_lock() {
        std::unique_lock<std::mutex> lock(mtx);
        writers_waiting++;
        // 等待条件：无读者和写者正在执行
        write_cv.wait(lock, [this]() {
            return readers == 0 && !is_writing;
        });
        writers_waiting--;
        is_writing = true;
    }

    // 释放写锁：恢复非写状态，优先唤醒等待的写者，其次唤醒读者（避免写者饥饿）
    void write_unlock() {
        std::unique_lock<std::mutex> lock(mtx);
        is_writing = false;
        if (writers_waiting > 0) {
            write_cv.notify_one();
        } else {
            read_cv.notify_all();  // 无写等待时唤醒所有读者
        }
    }
};

// 全局读写锁和共享数据
ReadWriteLock rw_lock;
int shared_value = 0;

// 读者线程：读取共享数据
void reader(int id) {
    rw_lock.read_lock();
    std::cout << "[Reader " << id << "] reads: " << shared_value << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 模拟读耗时
    rw_lock.read_unlock();
}

// 写者线程：修改共享数据
void writer(int id) {
    rw_lock.write_lock();
    shared_value++;
    std::cout << "[Writer " << id << "] writes: " << shared_value << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 模拟写耗时
    rw_lock.write_unlock();
}

// 测试函数
int main() {
    std::vector<std::thread> threads;
  
    // 启动3个写者和5个读者，验证写优先逻辑
    threads.emplace_back(writer, 1);
    threads.emplace_back(reader, 1);
    threads.emplace_back(reader, 2);
    threads.emplace_back(writer, 2);
    threads.emplace_back(reader, 3);
    threads.emplace_back(writer, 3);
    threads.emplace_back(reader, 4);
    threads.emplace_back(reader, 5);

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}

/*
关键设计说明
1.
写者优先：

写者请求锁时，writers_waiting 递增，新的读者需等待写者完成（通过 read_cv.wait 的条件判断）。
释放写锁时，若存在等待的写者，优先唤醒一个写者（保证连续写操作优先级高于读）。
2.
避免读者饥饿：

若没有等待的写者，写锁释放后会唤醒所有读者，允许并发读。
3.
资源安全释放：

使用 std::unique_lock 管理互斥锁，确保异常安全性。
条件变量的通知逻辑明确，防止无效唤醒。

改进方向
为确保更高性能或适应更复杂场景，可扩展以下功能：

1.超时等待：在 read_lock/write_lock 中添加超时机制，防止死锁。
2.公平调度：允许读/写请求按到达顺序排队。
3.可重入支持：允许同一线程多次获取读锁。
*/
```
