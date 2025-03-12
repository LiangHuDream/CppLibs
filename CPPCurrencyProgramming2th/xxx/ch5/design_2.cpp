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