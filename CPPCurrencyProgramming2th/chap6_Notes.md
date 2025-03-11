## 1. 核心知识点与代码详解
### 并发数据结构设计指南
- 线程安全：确保所有操作原子性，避免条件竞争。
- 异常安全：保证操作在异常发生时数据结构仍处于有效状态。
- 锁的粒度：使用细粒度锁（如每个桶一个锁）提高并发性能。
- 死锁预防：按固定顺序加锁，避免嵌套锁。

## 2. 典型示例：线程安全队列
通过互斥锁和条件变量实现线程安全队列：
```cpp
#include <bits/stdc++.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
using namespace std;
template<typename T>
class threadsafe_queue {
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;

public:
    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(std::move(new_value));
        data_cond.notify_one(); // 通知等待的消费者
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this]{ return !data_queue.empty(); });
        value = std::move(data_queue.front());
        data_queue.pop();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return false;
        value = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }
}; 
int main() {
    threadsafe_queue <int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    int x;
    q.wait_and_pop(x);
    cout << x << endl;
    q.wait_and_pop(x);
    cout << x << endl;
    q.wait_and_pop(x);
    cout << x << endl;
    return 0;
}

```

##  3. 多选题
1. 选择正确的线程安全栈实现特性：  
A. 粗粒度锁保护整个栈  
B. 返回std::shared_ptr避免异常  
C. 使用双重检查锁定优化性能  
D. 使用std::lock_guard管理锁  


2. 哪些操作可能导致死锁？  
A. 递归锁定同一互斥量  
B. 固定顺序获取多个锁  
C. 持有锁时调用用户代码  
D. 同时获取读写锁的读和写  

3. 关于条件变量正确的说法是：  
A. 必须在循环中使用避免虚假唤醒  
B. notify_one唤醒所有等待线程  
C. 使用时需持有互斥锁  
D. 只能与std::mutex配合使用  

4. 性能优化手段包括：  
A. 细粒度锁减少竞争  
B. 使用无锁结构替换锁  
C. 合并多个独立操作为一个锁块  
D. 增加临界区大小  

5. 哪些属于异常安全的锁用法？
A. 在锁内分配堆内存  
B. 持有锁时抛出异常  
C. 用std::lock_guard管理锁  
D. 使用try/catch在临界区外处理异常  

/*
1.答案： ABD
解析： 双重检查锁定在C++中不安全，需用原子操作或内存顺序控制。
2.答案： ACD
解析： A. 递归锁定同一互斥量
若使用非递归的互斥量（如 std::mutex），同一线程递归锁定会导致该线程无限等待自己释放锁，造成死锁。虽然递归互斥量（如 std::recursive_mutex）允许递归锁定，但非递归情况下会触发死锁，因此该选项正确。
C. 持有锁时调用用户代码
用户代码可能获取其他锁，且无法保证锁的顺序与当前线程一致，容易引发未控制的锁依赖循环。例如，若用户代码内部按不同顺序获取多个锁，可能导致循环等待（满足死锁条件），因此该选项正确。
D. 同时获取读写锁的读和写
若线程持有读锁，未释放的情况下尝试获取写锁（或反之），在读写锁不支持升级的情况下将无限阻塞自身。例如，使用 std::shared_mutex 时，读锁未释放直接请求写锁会导致死锁，因此该选项正确。
3.答案： AC
解析： notify_one唤醒一个线程；条件变量可与任何Lockable类型配合。
4.答案： AB
解析： C/D会增加锁竞争，降低并发性。
5.答案： C
解析： lock_guard保证离开作用域释放锁，即使异常抛出。
*/
## 4. 设计题目
1. 设计线程安全的LRU缓存
要求：
- 基于哈希表和双向链表实现
- get和put操作线程安全
- 当缓存满时淘汰最近最少使用的项
**关键点**： 对链表的修改需加锁，可用读写锁优化高频读取。

2. 实现环形缓冲区
要求：

- 固定容量，支持多生产者和消费者
- 无等待的push和pop操作（缓冲区满/空时返回失败）
- **关键点**： 使用原子变量维护头尾指针，避免锁竞争。

3. 线程安全的对象池
要求：

- 支持多线程安全借还对象
- 对象复用避免频繁构造析构
- 池为空时阻塞直到有对象归还
- **关键点**： 使用互斥锁保护池状态，条件变量实现等待。

4. 线程安全的有序链表
要求：

- 支持并发插入、删除和查找
- 不同操作间无竞态条件
- 允许遍历链表
-**关键点**： 使用节点级锁或跳表结构减少锁竞争。

5. 高效生产者-多消费者队列
要求：

- 多个消费者能并行处理数据
- 允许批量任务提交
- 避免消费者间任务重复获取
**关键点**： 使用任务分片，每个消费者维护独立的任务队列，主队列分配任务。

## 5. 设计题目参考答案
```cpp
// 1.
#include <unordered_map>
#include <list>
#include <shared_mutex>

template<typename K, typename V>
class ConcurrentLRU {
    using List = typename std::list<std::pair<K, V>>;
    using ListIter = typename List::iterator;
    
    std::shared_mutex mtx;
    List lru_list;
    std::unordered_map<K, ListIter> cache_map;
    size_t capacity;

    void evict() {
        auto last = lru_list.end();
        last--;
        cache_map.erase(last->first);
        lru_list.pop_back();
    }

public:
    ConcurrentLRU(size_t cap) : capacity(cap) {}

    bool get(const K& key, V& value) {
        std::shared_lock lock(mtx); // 读锁
        auto it = cache_map.find(key);
        if (it == cache_map.end()) return false;

        // 移动到头部（需升级为写锁）
        lock.unlock();
        {
            std::unique_lock write_lock(mtx);
            lru_list.splice(lru_list.begin(), lru_list, it->second);
        }
        value = it->second->second;
        return true;
    }

    void put(const K& key, const V& value) {
        std::unique_lock lock(mtx); // 写锁
        auto it = cache_map.find(key);
        if (it != cache_map.end()) {
            lru_list.splice(lru_list.begin(), lru_list, it->second);
            it->second->second = value;
            return;
        }

        if (cache_map.size() >= capacity) {
            evict();
        }
        lru_list.emplace_front(key, value);
        cache_map[key] = lru_list.begin();
    }
};

int main() {
    ConcurrentLRU<int, int> cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    int value;
    cache.get(1, value); // value = 1
    cache.put(4, 4);     // 1被淘汰
    cache.get(1, value); // value = -1
    return 0;
}

// 2. 

#include <atomic>
#include <atomic>

template<typename T, size_t N>
class RingBuffer {
    std::atomic<size_t> head_{0}, tail_{0};
    T data_[N];

public:
    bool push(const T& val) {
        size_t curr_head = head_.load(std::memory_order_relaxed);
        size_t next_tail = (tail_.load(std::memory_order_acquire) + 1) % N;

        if (next_tail == curr_head) return false; // 已满
        
        data_[tail_] = val;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    bool pop(T& val) {
        size_t curr_tail = tail_.load(std::memory_order_relaxed);
        if (head_.load(std::memory_order_acquire) == curr_tail) 
            return false; // 已空
        
        val = data_[head_];
        head_.store((head_ + 1) % N, std::memory_order_release);
        return true;
    }
};

int main() {
    RingBuffer<int, 10> rb;
    rb.push(1);
    rb.push(2);
    int val;
    rb.pop(val);
    return 0;
}

// 3. 
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ObjectPool {
    std::queue<T*> pool;
    std::mutex mtx;
    std::condition_variable cv;

public:
    ObjectPool(size_t size) {
        for (size_t i = 0; i < size; ++i) {
            pool.push(new T());
        }
    }

    T* borrow() {
        std::unique_lock lock(mtx);
        cv.wait(lock, [this] { return !pool.empty(); });
        auto obj = pool.front();
        pool.pop();
        return obj;
    }

    void release(T* obj) {
        std::lock_guard lock(mtx);
        pool.push(obj);
        cv.notify_one();
    }

    ~ObjectPool() {
        while (!pool.empty()) {
            delete pool.front();
            pool.pop();
        }
    }
};

int main() {
    ObjectPool<int> pool(10);

    auto obj1 = pool.borrow();
    auto obj2 = pool.borrow();

    pool.release(obj1);
    pool.release(obj2); 
    return 0;
}
// 4.
#include <mutex>

class ThreadSafeSortedList {
    struct Node {
        int val;
        Node* next;
        std::mutex mtx;

        Node(int v) : val(v), next(nullptr) {}
    };

    Node* head;
    std::mutex head_mtx;

public:
    ThreadSafeSortedList() : head(nullptr) {}

    void insert(int value) {
        Node* new_node = new Node(value);
        std::unique_lock head_lock(head_mtx);
        if (!head || head->val >= value) {
            new_node->next = head;
            head = new_node;
            return;
        }

        Node* prev = head;
        std::unique_lock prev_lock(prev->mtx);
        Node* curr = prev->next;
        while (curr && curr->val < value) {
            std::unique_lock curr_lock(curr->mtx);
            prev_lock.unlock();
            prev = curr;
            curr = curr->next;
            prev_lock = std::move(curr_lock);
        }
        prev->next = new_node;
        new_node->next = curr;
    }
};

int main() {
    ThreadSafeSortedList list;
    list.insert(5);
    list.insert(3);
    list.insert(8);
    return 0;
}
// 5. 
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <iostream>

// 自定义带互斥锁的队列结构
template<typename T>
struct LockedQueue {
    std::queue<T> queue;
    std::mutex mtx;
};

template<typename T>
class WorkSharingQueue {
    std::vector<LockedQueue<T>> worker_queues;
    std::atomic<size_t> index{0};
    size_t num_workers;

public:
    WorkSharingQueue(size_t workers) : num_workers(workers), worker_queues(workers) {}

    bool push(T task) {
        // 使用 std::memory_order_seq_cst 保证顺序一致性
        size_t idx = index.fetch_add(1, std::memory_order_seq_cst) % num_workers;
        std::lock_guard<std::mutex> lock(worker_queues[idx].mtx);
        worker_queues[idx].queue.push(std::move(task));
        return true;
    }

    bool pop(size_t worker_id, T& task) {
        auto& q = worker_queues[worker_id];
        {
            std::lock_guard<std::mutex> lock(q.mtx);
            if (!q.queue.empty()) {
                task = q.queue.front();
                q.queue.pop();
                return true;
            }
        }

        // 尝试工作窃取
        for (size_t i = 0; i < num_workers; ++i) {
            if (i == worker_id) continue;
            auto& steal_q = worker_queues[i];
            std::lock_guard<std::mutex> steal_lock(steal_q.mtx);
            if (!steal_q.queue.empty()) {
                task = steal_q.queue.front();
                steal_q.queue.pop();
                return true;
            }
        }
        return false;
    }
};

int main() {
    WorkSharingQueue<int> q(4);
    int task;
    q.push(1);
    q.push(2);
    q.push(3);
    q.push(4);

    if (q.pop(0, task)) {
        std::cout << "Thread 0 processed task: " << task << std::endl;
    }
    if (q.pop(1, task)) {
        std::cout << "Thread 1 processed task: " << task << std::endl;
    }
    if (q.pop(2, task)) {
        std::cout << "Thread 2 processed task: " << task << std::endl;
    }
    if (q.pop(3, task)) {
        std::cout << "Thread 3 processed task: " << task << std::endl;
    }

    return 0;
}    
```