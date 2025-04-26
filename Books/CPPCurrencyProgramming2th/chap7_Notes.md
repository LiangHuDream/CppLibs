@[TOC](设计无锁的并发数据结构)

## 1. 核心概念与难点
### 1.1  无锁（Lock-Free）条件

**定义**：一种并发算法的实现方式，保证无限执行进程中至少有一个线程能推进操作（系统整体进步）。
**关键特性**：无锁 ≠ 阻塞无关（Non-Blocking）。锁可能导致死锁，而无锁结构通过原子操作和算法设计规避锁。
层次划分：
**无锁（Lock-Free）**：至少一个线程能持续运行。
**无等待（Wait-Free）**：所有线程能在有限步内完成操作。
**无障碍（Obstruction-Free）**：线程在无竞争时独立完成操作。
### 1. 2 原子操作的重要性

必须依赖 std::atomic 类型和相关操作（load, store, exchange, compare_exchange_strong/weak）。
原子操作的正确性：仅当数据依赖的整个逻辑被原子化时才能保证线程安全。
### 1.3 内存顺序（Memory Order）

关键选项：
memory_order_relaxed：无同步约束（仅保证原子性）。
memory_order_acquire/release：同步操作的读写屏障。
memory_order_seq_cst（默认）：全局顺序一致，性能最差但最安全。
设计策略：尽量使用宽松内存顺序，但需结合 happens-before 关系验证正确性。
### 1.4 ABA问题

场景：线程A读取变量值为A，其他线程将值改为B后又改回A，导致A的 CAS 误判未修改。
解决方案：
- 版本号/标签指针（Tagged Pointer）：通过额外位数记录修改次数。
- 延迟回收（如Hazard Pointer）：确保被其他线程引用的对象不被回收。
## 2. 代码解析：无锁栈的实现（简化）
```cpp
template <typename T>
class LockFreeStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& data) : data(data), next(nullptr) {}
    };

    std::atomic<Node*> head;

public:
    void push(const T& data) {
        Node* new_node = new Node(data);
        new_node->next = head.load(std::memory_order_relaxed);
        while (!head.compare_exchange_weak(new_node->next, new_node,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));
    }

    bool pop(T& result) {
        Node* old_head = head.load(std::memory_order_acquire);
        while (old_head &&
               !head.compare_exchange_weak(old_head, old_head->next,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));
        if (!old_head) return false;
        result = old_head->data;
        delete old_head; // 可能引发ABA问题！
        return true;
    }
};

```
**代码问题**：
**1.ABA问题**：pop 中 delete 操作可能导致后续 push 复用同一地址。
**2.内存泄漏**：无内存回收机制（需结合Hazard Pointer或引用计数）。
**3.内存顺序误用**：push 中的 compare_exchange_weak 的success和failure内存序需严格设计。
## 3. 多选题目
1. 关于无锁编程，以下哪项是正确的？  
A. 无锁算法总是快于基于锁的算法  
B. 所有无锁算法都是无等待的  
C. 无锁算法可以避免死锁和优先级反转  
D. std::atomic 的默认内存序是顺序一致性（memory_order_seq_cst）  

2. ABA问题的解决方式包括（多选）：  
A. 互斥锁保护共享数据  
B. 带标签指针或版本号的原子操作  
C. 使用Hazard Pointer延迟内存回收  
D. 完全依赖memory_order_seq_cst保持全局顺序  

3. 在无锁栈的push操作中，compare_exchange_weak的正确内存序组合是：  
A. success=release, failure=acquire  
B. success=release, failure=relaxed  
C. success=relaxed, failure=relaxed  
D. success=seq_cst, failure=seq_cst  

4. 关于原子操作的内存顺序，错误的是：  
A. memory_order_acquire保证后续操作的读可见性  
B. memory_order_release确保之前的所有写操作对其他线程可见  
C. 宽松内存序可能导致处理器重排序无关操作  
D. memory_order_relaxed的原子操作不保证多线程间可见性  

5. 无锁链表中，使用compare_exchange_weak的目的是： 
A. 确保高并发下的原子性  
B. 代替互斥锁减少开销  
C. 处理CPU级别的忙等待  
D. 动态调整内存分配策略  

## 4. 设计题目
1. 设计一个无锁队列，要求支持多生产者和多消费者，说明如何解决ABA问题。
2. 实现一个线程安全的无锁单链表，并分析插入和删除操作的原子性保障。
3. 指出以下代码的ABA问题并提出修复方案：
```cpp
// 伪代码：无锁栈的pop操作
Node* old_top = top.load();
do {
    Node* new_top = old_top->next;
} while (!top.compare_exchange_weak(old_top, new_top));
delete old_top;

```
4. 如何通过版本号机制增强上述无锁栈的安全性？给出代码片段。
5. 分析Hazard Pointer和RCU在内存回收场景中的优缺点，适用场景。

## 5. 多选题答案
多选题答案：
1.C, D （C正确：无锁避免锁问题；D正确：默认seq_cst）
2.B, C
3.B （push操作只需在成功时释放，失败时松散加载）
4.D （relaxed保证原子性，但可能延迟可见性）
5.A, B
## 6. 设计题参考答案
1. 无锁队列设计：
- 使用两个原子指针（head和tail）。
- 解决ABA：节点添加版本号，CAS操作检查指针+版本号。
```cpp
#include <iostream>
#include <atomic>
#include <memory>

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node(T val) : data(val), next(nullptr) {}
    };

    struct TaggedPtr {
        Node* ptr;
        size_t tag;
        bool operator==(const TaggedPtr& other) const {
            return ptr == other.ptr && tag == other.tag;
        }
    };

    std::atomic<TaggedPtr> head, tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node(T());
        head.store({dummy, 0});
        tail.store({dummy, 0});
    }

    void enqueue(T val) {
        Node* newNode = new Node(val);
        TaggedPtr old_tail;
        while (true) {
            old_tail = tail.load();
            Node* old_next = old_tail.ptr->next.load();
            if (old_next == nullptr) {
                TaggedPtr new_tail{newNode, old_tail.tag + 1};
                if (old_tail.ptr->next.compare_exchange_weak(old_next, newNode)) {
                    tail.compare_exchange_weak(old_tail, new_tail);
                    return;
                }
            } else {
                tail.compare_exchange_weak(old_tail, {old_next, old_tail.tag + 1});
            }
        }
    }

    bool dequeue(T& val) {
        TaggedPtr old_head;
        while (true) {
            old_head = head.load();
            TaggedPtr old_tail = tail.load();
            Node* next = old_head.ptr->next.load();
            if (old_head.ptr == old_tail.ptr) {
                if (next == nullptr) return false;
                tail.compare_exchange_weak(old_tail, {next, old_tail.tag + 1});
            } else {
                val = next->data;
                TaggedPtr new_head{next, old_head.tag + 1};
                if (head.compare_exchange_weak(old_head, new_head)) {
                    delete old_head.ptr; // 需配合安全内存回收机制
                    return true;
                }
            }
        }
    }
};

int main() {
    LockFreeQueue<int> queue;

    // 入队操作
    std::cout << "Enqueuing elements: ";
    for (int i = 0; i < 5; ++i) {
        queue.enqueue(i);
        std::cout << i << " ";
    }
    std::cout << std::endl;

    // 出队操作
    std::cout << "Dequeuing elements: ";
    int value;
    while (queue.dequeue(value)) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    return 0;
}

```
2. 单链表的原子插入/删除：
- 插入：CAS更新next指针直至成功。
- 删除：标记逻辑删除后再物理回收（如Hazard Pointer）。
```cpp
#include <iostream>
#include <atomic>

template<typename T>
class LockFreeList {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node(T val) : data(val), next(nullptr) {}
    };

    std::atomic<Node*> head;

public:
    void insert(T val) {
        Node* newNode = new Node(val);
        // 定义一个普通指针变量来存储期望值
        Node* expected = head.load();
        while (!head.compare_exchange_weak(expected, newNode)) {
            // CAS 操作失败后，更新期望值
            expected = head.load();
        }
    }

    bool remove(T val) {
        Node* prev;
        Node* curr;
        do {
            prev = head.load();
            curr = prev;
            while (curr) {
                if (curr->data == val) {
                    Node* next = curr->next.load();
                    if (prev == head.load()) {
                        if (head.compare_exchange_weak(prev, next)) {
                            delete curr;
                            return true;
                        }
                    } else {
                        if (prev->next.compare_exchange_weak(curr, next)) {
                            delete curr;
                            return true;
                        }
                    }
                }
                prev = curr;
                curr = curr->next.load();
            }
        } while (curr);
        return false;
    }

    // 辅助函数：打印链表元素
    void printList() {
        Node* curr = head.load();
        while (curr) {
            std::cout << curr->data << " ";
            curr = curr->next.load();
        }
        std::cout << std::endl;
    }
};

int main() {
    LockFreeList<int> list;

    // 插入元素
    std::cout << "Inserting elements: 1, 2, 3" << std::endl;
    list.insert(1);
    list.insert(2);
    list.insert(3);

    // 打印链表
    std::cout << "List after insertion: ";
    list.printList();

    // 删除元素
    std::cout << "Removing element 2" << std::endl;
    bool removed = list.remove(2);
    if (removed) {
        std::cout << "Element 2 removed successfully." << std::endl;
    } else {
        std::cout << "Element 2 not found in the list." << std::endl;
    }

    // 再次打印链表
    std::cout << "List after removal: ";
    list.printList();

    return 0;
}

```
3. ABA修复：
- 引入Node结构体的Tag字段，每次修改递增。
```cpp
#include <iostream>
#include <atomic>
#include <memory>

struct Node {
    int value;
    std::atomic<Node*> next;
};

struct TaggedPointer {
    Node* ptr;
    size_t tag;
};

std::atomic<TaggedPointer> top{{nullptr, 0}};

// 向链表中插入元素的函数
void push(int value) {
    Node* new_node = new Node{value, nullptr};
    TaggedPointer old_top = top.load();
    while (true) {
        new_node->next = old_top.ptr;
        TaggedPointer new_top{new_node, old_top.tag + 1};
        if (top.compare_exchange_weak(old_top, new_top)) {
            return;
        }
    }
}

bool pop(int& value) {
    TaggedPointer old_top = top.load();
    while (true) {
        if (!old_top.ptr) return false;
        Node* new_ptr = old_top.ptr->next.load();
        TaggedPointer new_top{new_ptr, old_top.tag + 1};
        if (top.compare_exchange_weak(old_top, new_top)) {
            value = old_top.ptr->value;
            // 延迟删除，使用Hazard Pointer等安全回收
            delete old_top.ptr; // 这里简单删除，实际需要更安全的回收机制
            return true;
        }
    }
}

int main() {
    // 插入一些元素
    push(1);
    push(2);
    push(3);

    int value;
    // 弹出元素并输出
    while (pop(value)) {
        std::cout << "Popped value: " << value << std::endl;
    }
    std::cout << "Stack is empty." << std::endl;

    return 0;
}

```
4. 版本号代码示例
```cpp
struct Node {
    T data;
    std::atomic<size_t> version;
    Node* next;
};
// CAS时比较指针和版本号的组合

```
```cpp
#include <iostream>
#include <atomic>

struct Node {
    int data;
    Node* next;
};

struct TaggedPtr {
    Node* ptr;
    uintptr_t count;
};

std::atomic<TaggedPtr> top;

void push(int data) {
    Node* newNode = new Node{data, nullptr};
    TaggedPtr old_top = top.load();
    TaggedPtr new_top;  // 将 new_top 的声明移到循环体外
    do {
        newNode->next = old_top.ptr;
        new_top = {newNode, old_top.count + 1};
    } while (!top.compare_exchange_weak(old_top, new_top));
}

bool pop(int& data) {
    TaggedPtr old_top = top.load();
    while (old_top.ptr) {
        TaggedPtr new_top{old_top.ptr->next, old_top.count + 1};
        if (top.compare_exchange_weak(old_top, new_top)) {
            data = old_top.ptr->data;
            delete old_top.ptr; // 需配合内存回收策略
            return true;
        }
    }
    return false;
}

int main() {
    // 测试 push 操作，向栈中添加元素
    std::cout << "Pushing elements onto the stack: 1, 2, 3" << std::endl;
    push(1);
    push(2);
    push(3);

    // 测试 pop 操作，从栈中弹出元素并输出
    int poppedData;
    std::cout << "Popping elements from the stack:" << std::endl;
    while (pop(poppedData)) {
        std::cout << "Popped: " << poppedData << std::endl;
    }

    // 尝试再次弹出元素，验证栈是否为空
    if (!pop(poppedData)) {
        std::cout << "Stack is empty." << std::endl;
    }

    return 0;
}

```
5. Hazard Pointer vs RCU (Read-Copy-Update)：
**Hazard Pointer**：
- 优点：精确控制内存回收时机，低延迟，适用于高竞争环境。
- 缺点：实现复杂，每个线程需维护指针列表，可能限制可扩展性。
- 适用场景：频繁写入，需保证实时性的系统（如实时游戏服务器）。
**RCU (Read-Copy-Update)**：
- 优点：读者无锁，开销极低，适合读多写少；实现相对简单。
- 缺点：写者需等待所有读者退出，内存回收延迟大。
- 适用场景：读主导的数据结构（如路由表，配置信息）。

Hazard Pointer实现
```cpp
#include <atomic>
#include <vector>
#include <thread>
#include <iostream>

const int HP_MAX_THREADS = 4;
const int HP_MAX = 2;

// 链表节点结构体
struct Node {
    int data;
    Node* next;
};

// 定义 TaggedPtr 类型
struct TaggedPtr {
    Node* ptr;
    size_t tag;
};

// 风险指针结构体
struct HazardPointer {
    std::atomic<void*> ptr[HP_MAX];
    std::atomic<void*> retire_list[HP_MAX * HP_MAX_THREADS];
    std::atomic<int> retire_count;

    HazardPointer() : retire_count(0) {
        for (auto& p : ptr) p.store(nullptr);
        for (auto& r : retire_list) r.store(nullptr);
    }
};

// 线程局部变量，存储线程 ID
thread_local int thread_id = -1;
// 全局风险指针实例
HazardPointer hp;
// 全局线程 ID 计数器
std::atomic<int> global_id{0};

// 提前声明函数
void Scan();
void RetireNode(void* ptr);

// 注册线程 ID
void RegisterThread() {
    thread_id = global_id.fetch_add(1, std::memory_order_relaxed);
}

// 安全回收内存
void RetireNode(void* ptr) {
    int index = hp.retire_count.fetch_add(1, std::memory_order_relaxed);
    hp.retire_list[index].store(ptr, std::memory_order_release);

    if (index + 1 >= HP_MAX * HP_MAX_THREADS) {
        Scan();
    }
}

// 扫描风险指针
void Scan() {
    std::vector<void*> hps;
    // 收集所有风险指针
    for (auto& p : hp.ptr) {
        void* val = p.load(std::memory_order_acquire);
        if (val) hps.push_back(val);
    }

    int count = hp.retire_count.load(std::memory_order_relaxed);
    hp.retire_count.store(0, std::memory_order_relaxed);

    for (int i = 0; i < count; i++) {
        void* ptr = hp.retire_list[i].load(std::memory_order_relaxed);
        bool found = false;
        // 检查是否有其他线程正在使用该节点
        for (auto h : hps) {
            if (ptr == h) {
                found = true;
                break;
            }
        }
        if (!found) {
            // 正确转换为 Node* 类型并删除
            delete static_cast<Node*>(ptr);
        } else {
            RetireNode(ptr);
        }
    }
}

// 链表头指针
std::atomic<TaggedPtr> head;

// 向链表中插入节点
void push(int data) {
    Node* newNode = new Node{data, nullptr};
    TaggedPtr old_top = head.load();
    TaggedPtr new_top{newNode, old_top.tag + 1};
    do {
        newNode->next = old_top.ptr;
        new_top = {newNode, old_top.tag + 1};
    } while (!head.compare_exchange_weak(old_top, new_top));
}

// 从链表中弹出节点
void Pop() {
    if (thread_id == -1) {
        RegisterThread();
    }
    TaggedPtr old_top = head.load();
    while (old_top.ptr) {
        TaggedPtr new_top{old_top.ptr->next, old_top.tag + 1};
        if (head.compare_exchange_weak(old_top, new_top)) {
            RetireNode(old_top.ptr);
            return;
        }
    }
}

// 测试函数
void testFunction() {
    RegisterThread();
    for (int i = 0; i < 10; ++i) {
        push(i);
    }
    for (int i = 0; i < 10; ++i) {
        Pop();
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < HP_MAX_THREADS; ++i) {
        threads.emplace_back(testFunction);
    }

    for (auto& t : threads) {
        t.join();
    }

    // 确保所有待回收节点都被处理
    Scan();

    std::cout << "All threads finished. List is empty." << std::endl;
    return 0;
}

```
RCU实现
```cpp
#include <atomic>
#include <vector>
#include <thread>
#include <iostream>

struct RCUData {
    std::atomic<int> counter{0};
    std::atomic<bool> updating{false};
    void* data_ptr;
};

RCUData global_data;

// 读者进入临界区
void ReadLock() {
    global_data.counter.fetch_add(1, std::memory_order_acquire);
}

// 读者离开临界区
void ReadUnlock() {
    global_data.counter.fetch_sub(1, std::memory_order_release);
}

// 写者更新数据
void WriteUpdate(void* new_data) {
    // 等待所有现有读者退出
    global_data.updating.store(true, std::memory_order_release);
    while(global_data.counter.load(std::memory_order_acquire) > 0) {
        std::this_thread::yield();
    }
    
    // 更新数据并回收旧数据
    void* old = global_data.data_ptr;
    global_data.data_ptr = new_data;
    global_data.updating.store(false, std::memory_order_release);
    
    delete static_cast<int*>(old); // 延迟回收
}

// 示例使用
void Reader() {
    ReadLock();
    // 安全访问数据
    std::cout << "Reading: " << *static_cast<int*>(global_data.data_ptr) << "\n";
    ReadUnlock();
}

void Writer() {
    int* new_data = new int(42);
    WriteUpdate(new_data);
}

int main() {
    // 初始化数据
    global_data.data_ptr = new int(10);

    // 创建读者线程和写者线程
    std::vector<std::thread> readers;
    std::vector<std::thread> writers;

    // 创建 3 个读者线程
    for (int i = 0; i < 3; ++i) {
        readers.emplace_back(Reader);
    }

    // 创建 1 个写者线程
    writers.emplace_back(Writer);

    // 等待所有读者线程完成
    for (auto& reader : readers) {
        reader.join();
    }

    // 等待所有写者线程完成
    for (auto& writer : writers) {
        writer.join();
    }

    // 最后一次读取，验证数据是否更新
    Reader();

    // 清理最后一次分配的数据
    delete static_cast<int*>(global_data.data_ptr);

    return 0;
}

```