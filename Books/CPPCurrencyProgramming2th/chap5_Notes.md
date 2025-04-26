## 1. 原子操作与无锁编程
- 核心概念
**原子操作**：是不可分割的操作，在执行过程中不会被其他线程中断。C++ 标准库在 <atomic> 头文件中提供了一系列原子类型，如 std::atomic<int>、std::atomic<bool> 等。原子操作可以避免数据竞争，保证线程安全。
**无锁编程**：使用原子操作和 CAS（Compare - And - Swap）等技术来实现并发数据结构，避免使用传统的锁机制，从而减少线程阻塞和上下文切换带来的开销，提高并发性能。
- 示例代码
```cpp
#include <iostream>
#include <thread>
#include <atomic>

std::atomic<int> counter(0);

void increment() {
    for (int i = 0; i < 10000; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Counter value: " << counter.load(std::memory_order_relaxed) << std::endl;
    return 0;
}
/*
- std::atomic<int> counter(0) 定义了一个原子整数 counter 并初始化为 0。
- counter.fetch_add(1, std::memory_order_relaxed) 以原子方式将 counter 的值加 1，std::memory_order_relaxed 是一种内存顺序，只保证原子性，不保证顺序一致性。
- counter.load(std::memory_order_relaxed) 以原子方式读取 counter 的值。
*/
```
## 2. 内存顺序
###  核心概念
C++ 提供了六种内存顺序，用于控制原子操作之间的同步和可见性，不同的内存顺序会影响程序的性能和正确性。
- **顺序一致性（std::memory_order_seq_cst）**：是最严格的内存顺序，保证所有线程看到的所有原子操作的顺序是一致的，但性能开销较大。
- **松散顺序（std::memory_order_relaxed）**：只保证原子性，不保证操作的顺序，性能开销最小。
- **获取 - 释放顺序（std::memory_order_acquire、std::memory_order_release）**：用于同步不同线程之间的操作，std::memory_order_release 操作会将之前的所有写操作同步到其他线程，std::memory_order_acquire 操作会保证后续的读操作能看到之前 std::memory_order_release 操作的结果。
### 示例代码
```cpp
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

std::atomic<int> x(0);
std::atomic<int> y(0);
int r1, r2;

// 使用 std::memory_order_relaxed 的测试函数
void test_relaxed() {
    x = 1;
    y.store(2, std::memory_order_relaxed);

    std::thread t1([&] {
        r1 = y.load(std::memory_order_relaxed);
    });

    std::thread t2([&] {
        r2 = x.load(std::memory_order_relaxed);
    });

    t1.join();
    t2.join();

    std::cout << "Relaxed: r1 = " << r1 << ", r2 = " << r2 << std::endl;
}

// 使用 std::memory_order_consume 的测试函数
void test_consume() {
    x = 1;
    std::atomic<int*> ptr(nullptr);
    int data = 42;

    std::thread t1([&] {
        ptr.store(&data, std::memory_order_release);
    });

    std::thread t2([&] {
        int* p;
        while ((p = ptr.load(std::memory_order_consume)) == nullptr);
        r1 = *p;
    });

    t1.join();
    t2.join();

    std::cout << "Consume: r1 = " << r1 << std::endl;
}

// 使用 std::memory_order_acquire 的测试函数
void test_acquire() {
    x = 1;
    std::atomic<bool> flag(false);

    std::thread t1([&] {
        x = 2;
        flag.store(true, std::memory_order_release);
    });

    std::thread t2([&] {
        while (!flag.load(std::memory_order_acquire));
        r1 = x;
    });

    t1.join();
    t2.join();

    std::cout << "Acquire: r1 = " << r1 << std::endl;
}

// 使用 std::memory_order_release 的测试函数
void test_release() {
    x = 1;
    std::atomic<bool> flag(false);

    std::thread t1([&] {
        x = 2;
        flag.store(true, std::memory_order_release);
    });

    std::thread t2([&] {
        while (!flag.load(std::memory_order_acquire));
        r1 = x;
    });

    t1.join();
    t2.join();

    std::cout << "Release: r1 = " << r1 << std::endl;
}

// 使用 std::memory_order_acq_rel 的测试函数
void test_acq_rel() {
    std::atomic<int> counter(0);

    std::thread t1([&] {
        for (int i = 0; i < 1000; ++i) {
            counter.fetch_add(1, std::memory_order_acq_rel);
        }
    });

    std::thread t2([&] {
        for (int i = 0; i < 1000; ++i) {
            counter.fetch_add(1, std::memory_order_acq_rel);
        }
    });

    t1.join();
    t2.join();

    std::cout << "Acq_rel: counter = " << counter.load() << std::endl;
}

// 使用 std::memory_order_seq_cst 的测试函数
void test_seq_cst() {
    x = 0;
    y = 0;
    r1 = 0;
    r2 = 0;

    std::thread t1([&] {
        x.store(1, std::memory_order_seq_cst);
        r1 = y.load(std::memory_order_seq_cst);
    });

    std::thread t2([&] {
        y.store(1, std::memory_order_seq_cst);
        r2 = x.load(std::memory_order_seq_cst);
    });

    t1.join();
    t2.join();

    std::cout << "Seq_cst: r1 = " << r1 << ", r2 = " << r2 << std::endl;
}

int main() {
    test_relaxed();
    test_consume();
    test_acquire();
    test_release();
    test_acq_rel();
    test_seq_cst();

    return 0;
}
/*
std::memory_order_relaxed：
- 这是最宽松的内存顺序，只保证原子性，不保证操作的顺序。在 test_relaxed 函数中，两个线程分别读取 x 和 y 的值，由于使用了 
- std::memory_order_relaxed，r1 和 r2 的值可能是任意组合。
std::memory_order_consume：
- 该内存顺序用于建立依赖关系。在 test_consume 函数中，一个线程存储一个指针，另一个线程等待该指针不为空，然后读取指针指向的数据。
- std::memory_order_consume 确保读取操作依赖于存储操作。
std::memory_order_acquire：
- 用于在读取原子变量时建立同步关系。在 test_acquire 函数中，一个线程设置 flag 为 true，另一个线程等待 flag 为 true，然后读取 x 的值。
- std::memory_order_acquire 确保在读取 x 之前，flag 的存储操作已经完成。
std::memory_order_release：
- 用于在写入原子变量时建立同步关系。在 test_release 函数中，一个线程设置 x 的值，然后设置 flag 为 true。
- std::memory_order_release 确保在设置 flag 之前，x 的存储操作已经完成。
std::memory_order_acq_rel：
- 结合了 std::memory_order_acquire 和 std::memory_order_release 的功能。
- 在 test_acq_rel 函数中，两个线程同时对 counter 进行原子加法操作，std::memory_order_acq_rel 确保操作的原子性和顺序性。
std::memory_order_seq_cst：
- 这是最严格的内存顺序，保证所有线程看到的操作顺序一致。
- 在 test_seq_cst 函数中，两个线程分别对 x 和 y 进行存储和读取操作，std::memory_order_seq_cst 确保所有线程看到的操作顺序是一致的。
注意事项:
由于多线程程序的不确定性，某些测试结果可能会因线程调度和硬件平台的不同而有所差异。
std::memory_order_consume 的使用场景相对较少，并且在 C++20 中已经被弃用，建议优先使用 std::memory_order_acquire。
*/
```
## 3. 原子操作的应用：自旋锁
### 核心概念
自旋锁是一种简单的锁机制，线程在尝试获取锁时会不断循环（自旋），直到锁可用。使用原子操作可以实现无锁的自旋锁。
### 示例代码
```cpp
#include <iostream>
#include <thread>
#include <atomic>

class SpinLock {
private:
    std::atomic<bool> lock_{false};

public:
    void lock() {
        while (lock_.exchange(true, std::memory_order_acquire)) {
            // 自旋等待
            std::cout<< "spinning" << std::endl;
        }
        std::cout<< "spinning2" << std::endl;
    }

    void unlock() {
        lock_.store(false, std::memory_order_release);
    }
};

SpinLock spin_lock;
int shared_variable = 0;

void increment() {
    spin_lock.lock();
    ++shared_variable;
    spin_lock.unlock();
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Shared variable value: " << shared_variable << std::endl;
    return 0;
}
/*
- SpinLock 类中的 lock_ 是一个原子布尔变量。
- lock 方法使用 exchange 原子操作尝试将 lock_ 设置为 true，如果返回 true 表示锁已被其他线程持有，线程会自旋等待。
- unlock 方法以释放顺序将 lock_ 设置为 false，释放锁。
*/

```
## 4. 无锁数据结构：无锁栈
### 核心概念
无锁栈是一种使用原子操作实现的并发栈，多个线程可以同时进行入栈和出栈操作而无需使用传统的锁。
### 示例代码
```cpp
#include <iostream>
#include <thread>
#include <atomic>

template <typename T>
class LockFreeStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::atomic<Node*> head;

public:
    LockFreeStack() : head(nullptr) {}

    void push(const T& value) {
        Node* new_node = new Node(value);
        new_node->next = head.load(std::memory_order_relaxed);
        while (!head.compare_exchange_weak(new_node->next, new_node, std::memory_order_release, std::memory_order_relaxed)) {
            // 若比较交换失败，更新 new_node->next 并重试
        }
    }

    bool pop(T& value) {
        Node* old_head = head.load(std::memory_order_relaxed);
        while (old_head &&!head.compare_exchange_weak(old_head, old_head->next, std::memory_order_acquire, std::memory_order_relaxed)) {
            // 若比较交换失败，更新 old_head 并重试
        }
        if (old_head) {
            value = old_head->data;
            delete old_head;
            return true;
        }
        return false;
    }
};

LockFreeStack<int> stack;

void producer() {
    for (int i = 0; i < 10; ++i) {
        stack.push(i);
    }
}

void consumer() {
    int value;
    for (int i = 0; i < 10; ++i) {
        if (stack.pop(value)) {
            std::cout << "Popped: " << value << std::endl;
        }
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();

    return 0;
}
/*
- LockFreeStack 类中的 head 是一个原子指针，指向栈顶节点。
- push 方法创建一个新节点，使用 compare_exchange_weak 原子操作尝试将新节点插入栈顶，如果失败则重试。
- pop 方法使用 compare_exchange_weak 原子操作尝试将栈顶节点移除，如果失败则重试。
*/
```
## 5. 多选题目
1. 关于原子操作和内存顺序，以下说法正确的是（ ）  
A. 原子操作可以完全替代传统的锁机制  
B. std::memory_order_seq_cst 是最严格的内存顺序，能保证所有线程看到的原子操作顺序一致  
C. std::memory_order_relaxed 只保证原子性，不保证操作顺序  
D. 获取 - 释放顺序可以实现不同线程之间的同步  

2. 以下哪些是无锁编程的优点（ ）  
A. 减少线程阻塞和上下文切换开销  
B. 避免死锁问题  
C. 实现简单，易于维护  
D. 性能一定比使用锁的编程方式高  

3. 关于自旋锁，以下说法正确的是（ ）  
A. 自旋锁适合锁持有时间较短的场景  
B. 自旋锁会使线程进入阻塞状态  
C. 自旋锁可以使用原子操作实现  
D. 自旋锁不会产生饥饿问题  

4. 在无锁栈的实现中，compare_exchange_weak 操作的作用是（ ）  
A. 比较并交换原子变量的值  
B. 若比较失败，会自动更新期望值  
C. 可能会出现虚假失败的情况  
D. 可以保证操作的顺序一致性  

5. 以下哪些内存顺序可以用于实现同步（ ）  
A. std::memory_order_relaxed  
B. std::memory_order_seq_cst  
C. std::memory_order_acquire  
D. std::memory_order_release  

## 5. 多选答案
1. 答案：BCD
解释：原子操作不能完全替代传统的锁机制，在某些复杂的同步场景下，锁机制仍然是必要的，A 错误；std::memory_order_seq_cst 是最严格的内存顺序，能保证所有线程看到的原子操作顺序一致，B 正确；std::memory_order_relaxed 只保证原子性，不保证操作顺序，C 正确；获取 - 释放顺序可以实现不同线程之间的同步，D 正确。
2. 答案：AB
解释：无锁编程可以减少线程阻塞和上下文切换开销，避免死锁问题，A、B 正确；无锁编程的实现通常比较复杂，不易维护，C 错误；无锁编程的性能不一定比使用锁的编程方式高，在某些情况下，锁机制可能更合适，D 错误。
3. 答案：AC
解释：自旋锁适合锁持有时间较短的场景，因为自旋会消耗 CPU 资源，如果锁持有时间过长，会导致 CPU 资源浪费，A 正确；自旋锁不会使线程进入阻塞状态，而是不断循环等待，B 错误；自旋锁可以使用原子操作实现，C 正确；自旋锁可能会产生饥饿问题，即某个线程长时间无法获取锁，D 错误。
4. 答案：ABC
解释：compare_exchange_weak 操作的作用是比较并交换原子变量的值，若比较失败，会自动更新期望值，且可能会出现虚假失败的情况，A、B、C 正确；compare_exchange_weak 操作本身不保证操作的顺序一致性，需要结合具体的内存顺序，D 错误。
5. 答案：BCD
解释：std::memory_order_relaxed 只保证原子性，不保证操作顺序，不能用于实现同步，A 错误；std::memory_order_seq_cst、std::memory_order_acquire 和 std::memory_order_release 都可以用于实现同步，B、C、D 正确。
## 7. 设计题目
1. 设计一个线程安全的计数器，使用原子操作实现
要求：
- 提供 increment() 和 decrement() 方法，分别用于增加和减少计数器的值。
- 提供 get_value() 方法，用于获取计数器的当前值。
- 确保所有操作都是线程安全的。
2. 实现一个基于原子操作的无锁队列
要求：
- 支持多线程并发的入队和出队操作。
- 使用原子操作和 CAS 技术，避免使用传统的锁机制。
- 处理队列空和队列满的情况。
3. 设计一个线程安全的环形缓冲区，使用原子操作实现
要求：
- 支持多线程并发的写入和读取操作。
- 提供 write() 和 read() 方法，分别用于写入和读取数据。
- 处理缓冲区满和缓冲区空的情况。
4. 实现一个基于原子操作的读写锁
要求：
- 支持多个线程同时进行读操作。
- 同一时间只能有一个线程进行写操作。
- 写操作和读操作不能同时进行。
- 使用原子操作实现，不使用传统的锁机制。
5. 设计一个线程安全的哈希表，使用原子操作实现
要求：
- 支持多线程并发的插入、查找和删除操作。
- 处理哈希冲突。
- 使用原子操作和 CAS 技术，避免使用传统的锁机制。
## 7. 设计题目示例答案
```cpp
/*
// 1.
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

class ThreadSafeCounter {
private:
    // 使用 std::atomic 类型来存储计数器的值，保证原子操作
    std::atomic<int> value;

public:
    // 构造函数，初始化计数器的值为 0
    ThreadSafeCounter() : value(0) {}

    // 增加计数器的值，使用 fetch_add 原子操作
    void increment() {
        value.fetch_add(1, std::memory_order_relaxed);
    }

    // 减少计数器的值，使用 fetch_sub 原子操作
    void decrement() {
        value.fetch_sub(1, std::memory_order_relaxed);
    }

    // 获取计数器的当前值，使用 load 原子操作
    int get_value() const {
        return value.load(std::memory_order_relaxed);
    }
};

// 测试线程安全计数器的函数
void test_counter(ThreadSafeCounter& counter, int num_ops) {
    for (int i = 0; i < num_ops; ++i) {
        if (i % 2 == 0) {
            counter.increment();
        } else {
            counter.decrement();
        }
    }
}

int main() {
    ThreadSafeCounter counter;
    const int num_threads = 4;
    const int num_ops_per_thread = 100000;
    std::vector<std::thread> threads;

    // 创建多个线程并启动它们
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(test_counter, std::ref(counter), num_ops_per_thread);
    }

    // 等待所有线程执行完毕
    for (auto& t : threads) {
        t.join();
    }

    // 输出最终计数器的值
    std::cout << "Final counter value: " << counter.get_value() << std::endl;

    return 0;
}

// 2. 
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    std::atomic<size_t> size;
    const size_t capacity;

    bool cas(Node*& expected, Node* desired, std::atomic<Node*>& target) {
        return target.compare_exchange_strong(expected, desired);
    }

public:
    LockFreeQueue(size_t cap) : capacity(cap) {
        Node* dummy = new Node(T());
        head = dummy;
        tail = dummy;
        size = 0;
    }

    ~LockFreeQueue() {
        T temp;
        while (dequeue(temp));
        delete head.load();
    }

    bool enqueue(const T& value) {
        size_t currentSize = size.load(std::memory_order_relaxed);
        if (currentSize >= capacity) {
            return false; // 队列已满
        }
        Node* newNode = new Node(value);
        while (true) {
            Node* oldTail = tail.load(std::memory_order_relaxed);
            Node* oldTailNext = oldTail->next.load(std::memory_order_relaxed);
            if (oldTail == tail.load(std::memory_order_relaxed)) {
                if (oldTailNext == nullptr) {
                    if (cas(oldTailNext, newNode, oldTail->next)) {
                        cas(oldTail, newNode, tail);
                        size.fetch_add(1, std::memory_order_relaxed);
                        return true;
                    }
                } else {
                    cas(oldTail, oldTailNext, tail);
                }
            }
        }
    }

    bool dequeue(T& result) {
        while (true) {
            Node* oldHead = head.load(std::memory_order_relaxed);
            Node* next = oldHead->next.load(std::memory_order_relaxed);
            if (oldHead == head.load(std::memory_order_relaxed)) {
                if (next == nullptr) {
                    return false; // 队列为空
                }
                T data = next->data;
                if (cas(oldHead, next, head)) {
                    size.fetch_sub(1, std::memory_order_relaxed);
                    result = data;
                    delete oldHead;
                    return true;
                }
            }
        }
    }

    size_t getSize() const {
        return size.load(std::memory_order_relaxed);
    }

    bool isEmpty() const {
        return getSize() == 0;
    }

    bool isFull() const {
        return getSize() >= capacity;
    }
};


// 测试代码
void producer(LockFreeQueue<int>& queue, int id) {
    for (int i = 0; i < 5; ++i) {
        while (!queue.enqueue(i + id * 5)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Producer " << id << " enqueued " << i + id * 5 << std::endl;
    }
}

void consumer(LockFreeQueue<int>& queue, int id) {
    int value;
    for (int i = 0; i < 5; ++i) {
        while (!queue.dequeue(value)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Consumer " << id << " dequeued " << value << std::endl;
    }
}

int main() {
    LockFreeQueue<int> queue(10);

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < 2; ++i) {
        producers.emplace_back(producer, std::ref(queue), i);
        consumers.emplace_back(consumer, std::ref(queue), i);
    }

    for (auto& t : producers) {
        t.join();
    }
    for (auto& t : consumers) {
        t.join();
    }

    return 0;
}

// 3.
#include <iostream>
#include <atomic>
#include <vector>
#include <thread>
#include <chrono>

template<typename T>
class ThreadSafeCircularBuffer {
private:
    std::vector<T> buffer;
    std::atomic<size_t> readIndex;
    std::atomic<size_t> writeIndex;
    const size_t capacity;

    // 计算下一个索引
    size_t nextIndex(size_t index) const {
        return (index + 1) % capacity;
    }

public:
    // 构造函数，初始化缓冲区
    ThreadSafeCircularBuffer(size_t cap) : buffer(cap), readIndex(0), writeIndex(0), capacity(cap) {}

    // 写入数据的方法
    bool write(const T& value) {
        size_t currentWriteIndex = writeIndex.load(std::memory_order_relaxed);
        size_t next = nextIndex(currentWriteIndex);

        // 检查缓冲区是否已满
        if (next == readIndex.load(std::memory_order_acquire)) {
            return false; // 缓冲区已满
        }

        // 写入数据
        buffer[currentWriteIndex] = value;
        writeIndex.store(next, std::memory_order_release);
        return true;
    }

    // 读取数据的方法
    bool read(T& result) {
        size_t currentReadIndex = readIndex.load(std::memory_order_relaxed);

        // 检查缓冲区是否为空
        if (currentReadIndex == writeIndex.load(std::memory_order_acquire)) {
            return false; // 缓冲区为空
        }

        // 读取数据
        result = buffer[currentReadIndex];
        readIndex.store(nextIndex(currentReadIndex), std::memory_order_release);
        return true;
    }

    // 获取缓冲区的容量
    size_t getCapacity() const {
        return capacity;
    }

    // 检查缓冲区是否为空
    bool isEmpty() const {
        return readIndex.load(std::memory_order_relaxed) == writeIndex.load(std::memory_order_relaxed);
    }

    // 检查缓冲区是否已满
    bool isFull() const {
        return nextIndex(writeIndex.load(std::memory_order_relaxed)) == readIndex.load(std::memory_order_relaxed);
    }
};

// 测试写入线程函数
void writer(ThreadSafeCircularBuffer<int>& buffer, int id) {
    for (int i = 0; i < 5; ++i) {
        while (!buffer.write(i + id * 5)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Writer " << id << " wrote " << i + id * 5 << std::endl;
    }
}

// 测试读取线程函数
void reader(ThreadSafeCircularBuffer<int>& buffer, int id) {
    int value;
    for (int i = 0; i < 5; ++i) {
        while (!buffer.read(value)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Reader " << id << " read " << value << std::endl;
    }
}

int main() {
    ThreadSafeCircularBuffer<int> buffer(10);

    std::vector<std::thread> writers;
    std::vector<std::thread> readers;

    // 创建写入线程
    for (int i = 0; i < 2; ++i) {
        writers.emplace_back(writer, std::ref(buffer), i);
    }

    // 创建读取线程
    for (int i = 0; i < 2; ++i) {
        readers.emplace_back(reader, std::ref(buffer), i);
    }

    // 等待所有写入线程完成
    for (auto& t : writers) {
        t.join();
    }

    // 等待所有读取线程完成
    for (auto& t : readers) {
        t.join();
    }

    return 0;
}

// 4. 
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

class AtomicReadWriteLock {
private:
    std::atomic<int> readers;
    std::atomic<bool> writer;

public:
    AtomicReadWriteLock() : readers(0), writer(false) {}

    // 加读锁
    void readLock() {
        while (true) {
            // 检查是否有写操作正在进行
            while (writer.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
            // 尝试增加读者计数
            int oldReaders = readers.load(std::memory_order_relaxed);
            if (readers.compare_exchange_strong(oldReaders, oldReaders + 1, std::memory_order_acq_rel)) {
                // 再次检查是否有写操作
                if (writer.load(std::memory_order_acquire)) {
                    // 如果有写操作，减少读者计数并继续尝试
                    readers.fetch_sub(1, std::memory_order_relaxed);
                } else {
                    break;
                }
            }
        }
    }

    // 解读锁
    void readUnlock() {
        readers.fetch_sub(1, std::memory_order_release);
    }

    // 加写锁
    void writeLock() {
        while (true) {
            // 尝试获取写锁
            if (!writer.exchange(true, std::memory_order_acq_rel)) {
                // 检查是否有读者
                while (readers.load(std::memory_order_acquire) > 0) {
                    std::this_thread::yield();
                }
                break;
            }
            std::this_thread::yield();
        }
    }

    // 解写锁
    void writeUnlock() {
        writer.store(false, std::memory_order_release);
    }
};

// 读操作线程函数
void readOperation(AtomicReadWriteLock& lock, int id) {
    lock.readLock();
    std::cout << "Reader " << id << " is reading." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Reader " << id << " finished reading." << std::endl;
    lock.readUnlock();
}

// 写操作线程函数
void writeOperation(AtomicReadWriteLock& lock, int id) {
    lock.writeLock();
    std::cout << "Writer " << id << " is writing." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Writer " << id << " finished writing." << std::endl;
    lock.writeUnlock();
}

int main() {
    AtomicReadWriteLock lock;
    std::vector<std::thread> threads;

    // 创建读线程
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(readOperation, std::ref(lock), i);
    }

    // 创建写线程
    threads.emplace_back(writeOperation, std::ref(lock), 0);

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
// 5.
#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <functional>
#include <stdexcept>
#include <memory>

// 链表节点结构体
template<typename K, typename V>
struct Node {
    K key;
    V value;
    std::atomic<Node*> next;

    Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
};

// 线程安全的哈希表类
template<typename K, typename V>
class ThreadSafeHashTable {
private:
    // 桶结构体，包含链表头指针
    struct Bucket {
        std::atomic<Node<K, V>*> head;

        Bucket() : head(nullptr) {}
        ~Bucket() {
            Node<K, V>* current = head.load(std::memory_order_relaxed);
            while (current != nullptr) {
                Node<K, V>* next = current->next.load(std::memory_order_relaxed);
                delete current;
                current = next;
            }
        }
    };

    std::vector<std::unique_ptr<Bucket>> buckets;
    size_t numBuckets;
    std::hash<K> hashFunction;

    // 获取键对应的桶索引
    size_t getBucketIndex(const K& key) const {
        return hashFunction(key) % numBuckets;
    }

    // 标记指针帮助处理并发删除
    static constexpr uintptr_t MARKED = 0x1;

    // 检查节点是否被标记
    bool is_marked(Node<K, V>* node) {
        return (reinterpret_cast<uintptr_t>(node) & MARKED) != 0;
    }

    // 获取未标记的节点指针
    Node<K, V>* get_unmarked(Node<K, V>* node) {
        return reinterpret_cast<Node<K, V>*>(
            reinterpret_cast<uintptr_t>(node) & ~MARKED);
    }

    // 获取标记的节点指针
    Node<K, V>* get_marked(Node<K, V>* node) {
        return reinterpret_cast<Node<K, V>*>(
            reinterpret_cast<uintptr_t>(node) | MARKED);
    }

public:
    // 构造函数
    ThreadSafeHashTable(size_t bucketsCount = 64) : numBuckets(bucketsCount) {
        buckets.reserve(numBuckets);
        for (size_t i = 0; i < numBuckets; ++i) {
            buckets.emplace_back(std::make_unique<Bucket>());
        }
    }

    // 插入操作
    bool insert(const K& key, const V& value) {
        size_t index = getBucketIndex(key);
        try {
            Node<K, V>* newNode = new Node<K, V>(key, value);
            while (true) {
                Node<K, V>* currentHead = buckets[index]->head.load(std::memory_order_relaxed);
                newNode->next.store(currentHead, std::memory_order_relaxed);

                if (buckets[index]->head.compare_exchange_weak(
                        currentHead, newNode,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    return true;
                }
            }
        } catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed in insert: " << e.what() << std::endl;
            return false;
        }
    }

    // 查找操作
    bool find(const K& key, V& result) {
        size_t index = getBucketIndex(key);
        Node<K, V>* current = buckets[index]->head.load(std::memory_order_relaxed);

        while (current != nullptr) {
            Node<K, V>* unmarkedCurrent = get_unmarked(current);
            if (unmarkedCurrent->key == key && !is_marked(current)) {
                result = unmarkedCurrent->value;
                return true;
            }
            current = unmarkedCurrent->next.load(std::memory_order_relaxed);
        }
        return false;
    }

    // 删除操作
    bool remove(const K& key) {
        size_t index = getBucketIndex(key);
        while (true) {
            Node<K, V>* current = buckets[index]->head.load(std::memory_order_relaxed);
            Node<K, V>* prev = nullptr;
            Node<K, V>* unmarkedCurrent;

            // 遍历链表查找目标节点
            while (true) {
                if (current == nullptr) return false;

                unmarkedCurrent = get_unmarked(current);
                if (unmarkedCurrent->key == key) break;

                prev = unmarkedCurrent;
                current = unmarkedCurrent->next.load(std::memory_order_relaxed);
            }

            // 尝试标记要删除的节点
            Node<K, V>* next = unmarkedCurrent->next.load(std::memory_order_relaxed);
            Node<K, V>* expectedNext = next;
            Node<K, V>* desiredNext = get_marked(next);

            if (!unmarkedCurrent->next.compare_exchange_weak(
                    expectedNext, desiredNext,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                continue; // CAS失败，重试
            }

            // 尝试物理删除节点
            if (prev == nullptr) {
                if (buckets[index]->head.compare_exchange_strong(
                        current, next,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    delete unmarkedCurrent;
                    return true;
                }
            } else {
                if (prev->next.compare_exchange_strong(
                        current, next,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    delete unmarkedCurrent;
                    return true;
                }
            }
        }
    }
};

// 测试插入操作的线程函数
template<typename K, typename V>
void insertTest(ThreadSafeHashTable<K, V>& hashTable, const std::vector<std::pair<K, V>>& data) {
    for (const auto& pair : data) {
        hashTable.insert(pair.first, pair.second);
    }
}

// 测试查找操作的线程函数
template<typename K, typename V>
void findTest(ThreadSafeHashTable<K, V>& hashTable, const std::vector<K>& keys) {
    V result;
    for (const auto& key : keys) {
        if (hashTable.find(key, result)) {
            std::cout << "Found key: " << key << ", value: " << result << std::endl;
        } else {
            std::cout << "Key: " << key << " not found." << std::endl;
        }
    }
}

// 测试删除操作的线程函数
template<typename K, typename V>
void removeTest(ThreadSafeHashTable<K, V>& hashTable, const std::vector<K>& keys) {
    for (const auto& key : keys) {
        if (hashTable.remove(key)) {
            std::cout << "Removed key: " << key << std::endl;
        } else {
            std::cout << "Key: " << key << " not found for removal." << std::endl;
        }
    }
}

int main() {
    ThreadSafeHashTable<int, int> hashTable;

    // 插入测试数据
    std::vector<std::pair<int, int>> insertData = {{1, 100}, {2, 200}, {3, 300}};
    std::thread insertThread(insertTest<int, int>, std::ref(hashTable), insertData);

    // 查找测试数据
    std::vector<int> findKeys = {1, 2, 3, 4};
    std::thread findThread(findTest<int, int>, std::ref(hashTable), findKeys);

    // 删除测试数据
    std::vector<int> removeKeys = {2, 3};
    std::thread removeThread(removeTest<int, int>, std::ref(hashTable), removeKeys);

    // 等待所有线程完成
    insertThread.join();
    findThread.join();
    removeThread.join();

    return 0;
}
*/


```
