/*
    本次优化说明：
    内存顺序调整：
    在enqueue和dequeue方法中，将加载tail和head时的内存顺序从std::memory_order_relaxed改为std::memory_order_acquire。
    这是因为在这些位置，我们需要确保后续的内存访问不会看到旧的值，即需要获取最新的节点状态。
    
    代码风格调整：
    将capacity的初始化移到构造函数初始化列表中，使代码更简洁。
    在enqueue和dequeue方法中，将next变量的声明移到相应的if语句内部，以缩小其作用域，提高代码的可读性。
    
    测试代码调整：
    在producer函数中，将计算出的值i + id * 5存储到局部变量value中，再传递给enqueue方法，这样可以使代码更清晰。
    这些优化主要关注于代码的可读性、健壮性和潜在的并发性能。在实际应用中，根据具体的需求和性能测试结果，可能还需要进一步的调整和优化。
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

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

    // Helper function to perform CAS operation
    bool cas(Node*& expected, Node* desired, std::atomic<Node*>& target) { // Node*&是什么用法
        return target.compare_exchange_strong(expected, desired);
    }

public:
    LockFreeQueue(size_t cap)
        : capacity(cap), size(0), head(new Node(T())), tail(head.load()) {}

    ~LockFreeQueue() {
        T temp;
        while (dequeue(temp));
        Node* headNode = head.load();
        delete headNode;
    }

    bool enqueue(const T& value) {
        size_t currentSize = size.load(std::memory_order_relaxed);
        if (currentSize >= capacity) {
            return false; // Queue is full
        }

        Node* newNode = new Node(value);
        while (true) {
            Node* oldTail = tail.load(std::memory_order_acquire);
            if (oldTail == tail.load(std::memory_order_acquire)) {
                Node* next = oldTail->next.load(std::memory_order_relaxed);
                if (next == nullptr) {
                    if (oldTail->next.compare_exchange_strong(next, newNode)) {
                        tail.compare_exchange_strong(oldTail, newNode);
                        size.fetch_add(1, std::memory_order_release);
                        return true;
                    }
                } else {
                    tail.compare_exchange_strong(oldTail, next);
                }
            }
        }
    }

    bool dequeue(T& result) {
        while (true) {
            Node* oldHead = head.load(std::memory_order_acquire);
            Node* next = oldHead->next.load(std::memory_order_relaxed);
            if (oldHead == head.load(std::memory_order_acquire)) {
                if (next == nullptr) {
                    return false; // Queue is empty
                }
                result = next->data;
                if (cas(oldHead, next, head)) {
                    size.fetch_sub(1, std::memory_order_relaxed);
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

// Producer function for testing
void producer(LockFreeQueue<int>& queue, int id) {
    for (int i = 0; i < 5; ++i) {
        int value = i + id * 5;
        while (!queue.enqueue(value)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Producer " << id << " enqueued " << value << std::endl;
    }
}

// Consumer function for testing
void consumer(LockFreeQueue<int>& queue, int id) {
    for (int i = 0; i < 5; ++i) {
        int value;
        while (!queue.dequeue(value)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Consumer " << id << " dequeued " << value << std::endl;
    }
}

int main() {
    const size_t queueCapacity = 10;
    LockFreeQueue<int> queue(queueCapacity);

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // Create and start producers and consumers
    for (int i = 0; i < 2; ++i) {
        producers.emplace_back(producer, std::ref(queue), i);
        consumers.emplace_back(consumer, std::ref(queue), i);
    }

    // Join all producer threads
    for (auto& t : producers) {
        t.join();
    }

    // Join all consumer threads
    for (auto& t : consumers) {
        t.join();
    }

    return 0;
}