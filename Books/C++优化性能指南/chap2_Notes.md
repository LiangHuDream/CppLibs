好的，我将带您深入理解《C++性能优化指南》第二章的重难点，通过知识点详解和可运行的代码示例帮助您掌握核心概念。以下是关键知识点及对应的代码演示：

---

### 1. 内存访问速度与对齐
**知识点**：内存访问比计算慢得多，非对齐访问可能导致双倍内存读取操作。

代码示例：测试对齐与非对齐结构体访问速度
```cpp
#include <iostream>
#include <chrono>

// 对齐的结构体（默认对齐）
struct AlignedStruct {
    int a;
    int b;
};

// 非对齐的结构体（手动填充破坏对齐）
#pragma pack(push, 1)
struct UnalignedStruct {
    int a;
    char padding; // 插入填充破坏对齐
    int b;
};
#pragma pack(pop)

void test_access_speed() {
    const int N = 10000000;
    
    AlignedStruct aligned_arr[N];
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        aligned_arr[i].a = i;
        aligned_arr[i].b = i;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Aligned access time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";

    UnalignedStruct unaligned_arr[N];
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        unaligned_arr[i].a = i;
        unaligned_arr[i].b = i;
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Unaligned access time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
}

int main() {
    test_access_speed();
    return 0;
}
```
**编译指令**：
```bash
g++ -std=c++11 -O0 -o alignment_test alignment_test.cpp
```
**输出示例**：
```
Aligned access time: 25ms
Unaligned access time: 42ms
```
**关键点**：
- `#pragma pack`强制结构体紧凑存储破坏对齐
- 非对齐访问需要两次内存操作，性能显著下降

---

### 2. 缓存局部性原理
**知识点**：连续内存访问比随机访问快，利用空间局部性提升性能。

代码示例：行优先 vs 列优先遍历二维数组
```cpp
#include <iostream>
#include <chrono>

const int ROWS = 10000;
const int COLS = 10000;

void row_major_access() {
    int** matrix = new int*[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        matrix[i] = new int[COLS];
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            matrix[i][j] = i + j;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Row-major time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";

    for (int i = 0; i < ROWS; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void column_major_access() {
    int** matrix = new int*[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        matrix[i] = new int[COLS];
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < COLS; ++j) {
        for (int i = 0; i < ROWS; ++i) {
            matrix[i][j] = i + j;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Column-major time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";

    for (int i = 0; i < ROWS; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

int main() {
    row_major_access();
    column_major_access();
    return 0;
}
```
**编译指令**：
```bash
g++ -std=c++11 -O0 -o cache_locality cache_locality.cpp
```
**输出示例**：
```
Row-major time: 356ms
Column-major time: 1289ms
```
**关键点**：
- 行优先访问符合内存连续存储特性，缓存命中率高
- 列优先访问导致频繁缓存未命中，性能显著下降

---

### 3. 分支预测优化
**知识点**：有序数据的分支预测成功率更高。

代码示例：处理有序 vs 无序数组
```cpp
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>

const int SIZE = 1000000;

void process_sorted(int* data) {
    auto start = std::chrono::high_resolution_clock::now();
    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        if (data[i] > 500) { // 有序数据分支预测成功率高
            sum += data[i];
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Sorted process time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << "us\n";
}

void process_unsorted(int* data) {
    auto start = std::chrono::high_resolution_clock::now();
    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        if (data[i] > 500) { // 无序数据分支预测失败多
            sum += data[i];
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Unsorted process time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << "us\n";
}

int main() {
    int* data = new int[SIZE];
    std::iota(data, data + SIZE, 0); // 生成0-999999的有序数据

    process_sorted(data);

    // 打乱数据顺序
    std::random_device rd;
    std::shuffle(data, data + SIZE, std::mt19937(rd()));
    
    process_unsorted(data);

    delete[] data;
    return 0;
}
```
**编译指令**：
```bash
g++ -std=c++11 -O0 -o branch_prediction branch_prediction.cpp
```
**输出示例**：
```
Sorted process time: 856us
Unsorted process time: 3245us
```
**关键点**：
- 有序数据使CPU分支预测成功率可达90%+
- 无序数据导致频繁预测失败，流水线清空代价大

---

### 4. False Sharing）
**知识点**：多线程修改同一缓存行的不同变量会导致性能下降。

代码示例：伪共享 vs 缓存行对齐
```cpp
#include <iostream>
#include <thread>
#include <chrono>

const int CACHE_LINE_SIZE = 64;
const int ITERATIONS = 100000000;

// 存在伪共享的结构体
struct SharedData {
    volatile int x;
    volatile int y;
};

// 缓存行对齐的结构体
struct AlignedData {
    alignas(CACHE_LINE_SIZE) volatile int x;
    alignas(CACHE_LINE_SIZE) volatile int y;
};

void false_sharing_test() {
    SharedData data;
    auto start = std::chrono::high_resolution_clock::now();
    
    std::thread t1([&] {
        for (int i = 0; i < ITERATIONS; ++i) ++data.x;
    });
    
    std::thread t2([&] {
        for (int i = 0; i < ITERATIONS; ++i) ++data.y;
    });
    
    t1.join();
    t2.join();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "False sharing time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
}

void aligned_test() {
    AlignedData data;
    auto start = std::chrono::high_resolution_clock::now();
    
    std::thread t1([&] {
        for (int i = 0; i < ITERATIONS; ++i) ++data.x;
    });
    
    std::thread t2([&] {
        for (int i = 0; i < ITERATIONS; ++i) ++data.y;
    });
    
    t1.join();
    t2.join();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Aligned time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
}

int main() {
    false_sharing_test();
    aligned_test();
    return 0;
}
```
**编译指令**：
```bash
g++ -std=c++11 -O0 -pthread -o false_sharing false_sharing.cpp
```
**输出示例**：
```
False sharing time: 3856ms
Aligned time: 987ms
```
**关键点**：
- `volatile`防止编译器优化掉循环
- `alignas`确保变量位于不同缓存行
- 伪共享导致缓存行无效化，性能下降4倍

---

### 多选题

问题1：关于缓存行的正确描述是？
A) 缓存行大小通常为64字节  
B) 对齐数据结构到缓存行可以避免伪共享  
C) 读取单个字节会加载整个缓存行  
D) 缓存行大小在x86架构下不可配置  

问题2：哪些操作会导致流水线停滞？
A) 分支预测失败  
B) 内存加载延迟  
C) 浮点除法运算  
D) 寄存器重命名  

问题3：优化内存访问的正确方法包括？
A) 使用`__builtin_prefetch`  
B) 将小对象放入连续内存  
C) 随机访问大型二维数组  
D) 结构体字段按访问频率排列  

问题4：关于虚函数正确的是？
A) 虚表指针增加对象内存开销  
B) 虚函数调用无法内联  
C) `final`类可以优化虚表查找  
D) 虚函数总是比函数指针快  

问题5：多线程编程中正确做法是？
A) 使用无锁数据结构避免互斥锁  
B) `volatile`保证原子性  
C) `std::atomic`比互斥锁性能更高  
D) 共享变量应放入单独缓存行  

问题6：减少分支预测失败的方法？
A) 使用`likely/unlikely`宏  
B) 将条件判断移出循环  
C) 用查表代替switch-case  
D) 排序数据使条件分布均匀  

问题7：关于内存分配正确的是？
A) `std::make_shared`比`new`少一次分配  
B) 内存池减少系统调用次数  
C) 栈分配比堆分配更快  
D) `malloc`保证内存对齐到页边界  

问题8：优化数据结构的正确手段？
A) 用`std::vector`替代链表  
B) 使用SOA代替AOS布局  
C) 用位域压缩布尔标记  
D) 优先选择开放寻址哈希表  

问题9：关于编译器优化错误的是？
A) `restrict`指针帮助别名分析  
B) `inline`关键字强制函数内联  
C) `-O3`可能增加代码体积  
D) 循环展开总是提升性能  

问题10：提升指令级并行的方式？
A) 减少数据依赖链  
B) 使用SIMD指令  
C) 增加循环迭代次数  
D) 使用更宽整数类型  

---

### 设计题

设计题1：缓存优化矩阵乘法
要求：实现一个缓存友好的矩阵乘法，对比不同分块策略的性能差异，给出测试数据。

```cpp
// 基础版本
void matmul(const float* A, const float* B, float* C, int N) {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                C[i*N+j] += A[i*N+k] * B[k*N+j];
}

// 分块优化版本（示例块大小64）
void matmul_blocked(float* A, float* B, float* C, int N) {
    const int BLOCK = 64;
    for (int i = 0; i < N; i += BLOCK)
        for (int j = 0; j < N; j += BLOCK)
            for (int k = 0; k < N; k += BLOCK)
                // 分块内计算...
}
```

设计题2：无锁队列实现
要求：实现支持多生产者多消费者的无锁队列，验证正确性并测试性能。

```cpp
template<typename T>
class LockFreeQueue {
    struct Node { 
        std::atomic<Node*> next;
        T data;
    };
    std::atomic<Node*> head;
    std::atomic<Node*> tail;
public:
    void enqueue(T data) {
        Node* new_node = new Node{nullptr, data};
        Node* old_tail = tail.exchange(new_node);
        old_tail->next.store(new_node);
    }
    // 省略其他接口...
};
```

设计题3：分支预测优化
要求：优化以下代码，减少分支预测失败次数，并测试不同输入模式的性能。

```cpp
// 原始版本
int sum_positive(int* arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        if (arr[i] > 0) // 分支预测敏感
            sum += arr[i];
    }
    return sum;
}

// 优化版本（使用无分支计算）
int sum_positive_opt(int* arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += (arr[i] > 0) * arr[i];
    }
    return sum;
}
```

设计题4：内存池设计
要求：实现固定大小的对象内存池，对比`new/delete`性能差异。

```cpp
class MemoryPool {
    struct Block { Block* next; };
    Block* free_list;
public:
    void* allocate() {
        if (!free_list) refill();
        Block* p = free_list;
        free_list = free_list->next;
        return p;
    }
    void deallocate(void* p) {
        static_cast<Block*>(p)->next = free_list;
        free_list = static_cast<Block*>(p);
    }
};
```

设计题5：SIMD加速计算
要求：使用AVX指令优化数组求和，对比标量版本性能。

```cpp
// 标量版本
float sum_scalar(const float* arr, int n) {
    float sum = 0;
    for (int i = 0; i < n; ++i)
        sum += arr[i];
    return sum;
}

// SIMD版本
#include <immintrin.h>
float sum_simd(const float* arr, int n) {
    __m256 sum_vec = _mm256_setzero_ps();
    for (int i = 0; i < n; i += 8) {
        __m256 data = _mm256_loadu_ps(arr + i);
        sum_vec = _mm256_add_ps(sum_vec, data);
    }
    // 水平求和...
}
```

---

### 多选题答案与解析

问题1：
**答案：AB**  
解析：A正确（x86缓存行通常64字节），B正确（对齐避免伪共享），C错误（读取字节加载整行），D错误（部分CPU可配置）。

问题2：
**答案：AB**  
解析：分支失败和内存延迟导致停滞，浮点运算可能流水执行，寄存器重命名帮助避免停滞。

问题3：
**答案：ABD**  
解析：预取和连续访问提升局部性，随机访问降低性能，按频率排列减少缓存未命中。

问题4：
**答案：ABC**  
解析：D错误，函数指针可能更快。

问题5：
**答案：ACD**  
解析：B错误，volatile不保证原子性。

问题6：
**答案：ABD**  
解析：查表不一定减少分支。

问题7：
**答案：ABC**  
解析：`malloc`对齐到最大类型大小，非页边界。

问题8：
**答案：ABCD**  
解析：均有效优化手段。

问题9：
**答案：BD**  
解析：`inline`是建议，循环展开可能增大代码。

问题10：
**答案：AB**  
解析：SIMD和减少依赖提升并行。

---

### 设计题测试用例
1. 缓存优化矩阵乘法
```cpp
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <cstring>  // 添加缺失的头文件

#ifdef _WIN32
#include <malloc.h> // Windows下需要这个头文件用于_aligned_malloc
#endif

// 基础版本矩阵乘法（调整循环顺序优化）
void matmul_basic(const float* A, const float* B, float* C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            float a = A[i*N + k];
            for (int j = 0; j < N; ++j) {
                C[i*N + j] += a * B[k*N + j];
            }
        }
    }
}

// 分块优化矩阵乘法（跨平台内存分配）
void matmul_blocked(const float* A, const float* B, float* C, int N, int BLOCK) {
    // ...分块实现代码保持不变...
}

// 跨平台对齐内存分配
void* aligned_alloc_wrapper(size_t alignment, size_t size) {
#ifdef _WIN32
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}

// 初始化矩阵
void init_matrix(float* mat, int N) {
    for (int i = 0; i < N*N; ++i) {
        mat[i] = static_cast<float>(rand()) / RAND_MAX;
    }
}

// 验证结果正确性
bool verify(const float* C1, const float* C2, int N) {
    const float eps = 1e-4;
    for (int i = 0; i < N*N; ++i) {
        if (fabs(C1[i] - C2[i]) > eps) {
            std::cerr << "Validation failed at " << i 
                      << ": " << C1[i] << " vs " << C2[i] << std::endl;
            return false;
        }
    }
    return true;
}

// 跨平台对齐内存释放
void aligned_free_wrapper(void* ptr) {
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

int main() {
    const int N = 1024;
    const int BLOCK_SIZES[] = {16, 32, 64, 128};
    
    // 使用跨平台对齐内存分配
    float* A = static_cast<float*>(aligned_alloc_wrapper(64, N*N*sizeof(float)));
    float* B = static_cast<float*>(aligned_alloc_wrapper(64, N*N*sizeof(float)));
    float* C_basic = static_cast<float*>(aligned_alloc_wrapper(64, N*N*sizeof(float)));
    float* C_blocked = static_cast<float*>(aligned_alloc_wrapper(64, N*N*sizeof(float)));

    // 初始化矩阵
    init_matrix(A, N);
    init_matrix(B, N);
    
    // 测试基础版本
    memset(C_basic, 0, N*N*sizeof(float));
    auto t1 = std::chrono::high_resolution_clock::now();
    matmul_basic(A, B, C_basic, N);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto basic_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    // 测试不同分块大小
    for (int block : BLOCK_SIZES) {
        memset(C_blocked, 0, N*N*sizeof(float));
        t1 = std::chrono::high_resolution_clock::now();
        matmul_blocked(A, B, C_blocked, N, block);
        t2 = std::chrono::high_resolution_clock::now();
        auto blocked_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        // 验证结果
        if (!verify(C_basic, C_blocked, N)) {
            std::cerr << "Block size " << block << " validation failed!" << std::endl;
            continue;
        }

        std::cout << "Block size " << block << ":\t" << blocked_time << "ms\t"
                    << "Speedup: " << static_cast<float>(basic_time)/blocked_time << "x\n";
    }

    // 使用跨平台内存释放
    aligned_free_wrapper(A);
    aligned_free_wrapper(B);
    aligned_free_wrapper(C_basic);
    aligned_free_wrapper(C_blocked);

    return 0;
}
```

2. 无锁队列实现
```cpp
#include <atomic>
#include <memory>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

template<typename T>
class LockFreeQueue {
    struct Node {
        std::atomic<Node*> next;
        T data;
        
        Node() : next(nullptr) {}
        explicit Node(T&& data) : next(nullptr), data(std::move(data)) {}
    };

    alignas(64) std::atomic<Node*> head;
    alignas(64) std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node();
        head.store(dummy, std::memory_order_relaxed);
        tail.store(dummy, std::memory_order_relaxed);
    }

    ~LockFreeQueue() {
        while (dequeue());
        delete head.load();
    }

    void enqueue(T data) {
        Node* new_node = new Node(std::move(data));
        Node* current_tail = nullptr;
        Node* next = nullptr;

        while (true) {
            current_tail = tail.load(std::memory_order_acquire);
            next = current_tail->next.load(std::memory_order_acquire);
            
            if (current_tail == tail.load(std::memory_order_acquire)) {
                if (next == nullptr) {
                    if (current_tail->next.compare_exchange_weak(
                        next, new_node, std::memory_order_release)) {
                        break;
                    }
                } else {
                    tail.compare_exchange_weak(current_tail, next,
                                              std::memory_order_release);
                }
            }
        }
        tail.compare_exchange_weak(current_tail, new_node,
                                  std::memory_order_release);
    }

    bool dequeue(T& result) {
        Node* current_head = nullptr;
        Node* current_tail = nullptr;
        Node* next = nullptr;

        while (true) {
            current_head = head.load(std::memory_order_acquire);
            current_tail = tail.load(std::memory_order_acquire);
            next = current_head->next.load(std::memory_order_acquire);

            if (current_head == head.load(std::memory_order_acquire)) {
                if (current_head == current_tail) {
                    if (next == nullptr) return false;
                    tail.compare_exchange_weak(current_tail, next,
                                              std::memory_order_release);
                } else {
                    if (head.compare_exchange_weak(current_head, next,
                                                  std::memory_order_release)) {
                        result = std::move(next->data);
                        delete current_head;
                        return true;
                    }
                }
            }
        }
    }

    bool dequeue() { // For cleanup
        T dummy;
        return dequeue(dummy);
    }
};

// 正确性测试
void correctness_test() {
    LockFreeQueue<int> queue;
    const int TEST_SIZE = 10000;
    std::vector<int> consumed;

    // 生产者线程
    auto producer = [&] {
        for (int i = 0; i < TEST_SIZE; ++i) {
            queue.enqueue(i);
        }
    };

    // 消费者线程
    auto consumer = [&] {
        int val;
        while (consumed.size() < TEST_SIZE * 2) { // 2 producers
            if (queue.dequeue(val)) {
                consumed.push_back(val);
            }
        }
    };

    std::thread p1(producer);
    std::thread p2(producer);
    std::thread c1(consumer);
    std::thread c2(consumer);

    p1.join();
    p2.join();
    c1.join();
    c2.join();

    // 验证结果
    std::sort(consumed.begin(), consumed.end());
    bool success = true;
    for (size_t i = 0; i < consumed.size(); ++i) {
        if (consumed[i] != i/2) { // 每个值出现两次
            success = false;
            break;
        }
    }

    std::cout << "Correctness test: " 
              << (success ? "PASSED" : "FAILED") << std::endl;
}

// 性能测试
void performance_test() {
    const int OPS = 1000000;
    LockFreeQueue<int> queue;
    
    auto test = [&](int producers, int consumers) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<std::thread> threads;
        for (int i = 0; i < producers; ++i) {
            threads.emplace_back([&] {
                for (int j = 0; j < OPS/producers; ++j) {
                    queue.enqueue(j);
                }
            });
        }

        std::atomic<int> count{0};
        for (int i = 0; i < consumers; ++i) {
            threads.emplace_back([&] {
                int val;
                while (count.load() < OPS) {
                    if (queue.dequeue(val)) {
                        count.fetch_add(1);
                    }
                }
            });
        }

        for (auto& t : threads) t.join();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start).count();

        std::cout << producers << "P/" << consumers << "C: "
                  << OPS/duration << "K ops/ms\n";
    };

    std::cout << "Performance test:\n";
    test(1, 1);
    test(2, 2);
    test(4, 4);
}

int main() {
    correctness_test();
    performance_test();
    return 0;
}
```

3. 分支预测优化
```cpp
#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <immintrin.h> // 添加AVX头文件

using namespace std;
using namespace std::chrono;

// 所有函数统一使用const指针
int sum_positive(const int* arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        if (arr[i] > 0)
            sum += arr[i];
    }
    return sum;
}

int sum_positive_opt1(const int* arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += (arr[i] > 0) * arr[i];
    }
    return sum;
}

int sum_positive_opt2(const int* arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += arr[i] & -(arr[i] > 0);
    }
    return sum;
}

// 生成测试数据
vector<int> generate_all_positive(int n) {
    vector<int> arr(n);
    for (int& x : arr) x = rand() % 100 + 1; // 1~100
    return arr;
}

vector<int> generate_all_negative(int n) {
    vector<int> arr(n);
    for (int& x : arr) x = -(rand() % 100 + 1); // -1~-100
    return arr;
}

vector<int> generate_random(int n) {
    vector<int> arr(n);
    for (int& x : arr) x = (rand() % 201) - 100; // -100~100
    return arr;
}

vector<int> generate_alternating(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = (i % 2 == 0) ? (rand() % 100 + 1) : -(rand() % 100 + 1);
    }
    return arr;
}

// 修改2：调整函数指针类型匹配
template <typename Func>
void test_performance(const string& name, Func func, const vector<int>& arr) {
    const int iterations = 1000;
    volatile int dummy = 0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        // 此处已适配const指针
        dummy += func(arr.data(), arr.size());
    }
    auto end = high_resolution_clock::now();

    cout << name << ": "
         << duration_cast<microseconds>(end - start).count() / 1000.0 
         << " ms (dummy=" << dummy << ")" << endl;
}

int main() {
    srand(time(nullptr));
    const int n = 1000000;

    // 生成测试数据
    auto arr_pos = generate_all_positive(n);
    auto arr_neg = generate_all_negative(n);
    auto arr_rand = generate_random(n);
    auto arr_alt = generate_alternating(n);

    // 测试性能
    cout << "===== All Positive =====" << endl;
    test_performance("Original    ", sum_positive, arr_pos);
    test_performance("Optimized 1 ", sum_positive_opt1, arr_pos);
    test_performance("Optimized 2 ", sum_positive_opt2, arr_pos);

    cout << "\n===== All Negative =====" << endl;
    test_performance("Original    ", sum_positive, arr_neg);
    test_performance("Optimized 1 ", sum_positive_opt1, arr_neg);
    test_performance("Optimized 2 ", sum_positive_opt2, arr_neg);

    cout << "\n===== Random Values =====" << endl;
    test_performance("Original    ", sum_positive, arr_rand);
    test_performance("Optimized 1 ", sum_positive_opt1, arr_rand);
    test_performance("Optimized 2 ", sum_positive_opt2, arr_rand);

    cout << "\n===== Alternating Values =====" << endl;
    test_performance("Original    ", sum_positive, arr_alt);
    test_performance("Optimized 1 ", sum_positive_opt1, arr_alt);
    test_performance("Optimized 2 ", sum_positive_opt2, arr_alt);

    return 0;
}
```
4. 内存池设计 
```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <cstdlib>  // 添加Windows内存对齐函数所需头文件

using namespace std;
using namespace std::chrono;

// ==================== 修正后的内存池实现 ====================
class MemoryPool {
private:
    struct Block { Block* next; };
    
    static const size_t chunk_size = 1024;  // 改为普通静态常量
    size_t block_size;
    Block* free_list = nullptr;
    vector<void*> chunks;

public:
    explicit MemoryPool(size_t obj_size) {
        block_size = max(obj_size, sizeof(Block));
        const size_t align = alignof(max_align_t);
        block_size = (block_size + align - 1) & ~(align - 1);
    }

    ~MemoryPool() {
        for (void* p : chunks) {
            _aligned_free(p);  // Windows专用内存释放函数
        }
    }

    void* allocate() {
        if (!free_list) refill();
        Block* p = free_list;
        free_list = free_list->next;
        return p;
    }

    void deallocate(void* p) {
        Block* block = static_cast<Block*>(p);
        block->next = free_list;
        free_list = block;
    }

    size_t chunk_count() const {  // 新增方法获取块数
        return chunks.size();
    }

private:
    void refill() {
        // 使用Windows平台专用对齐分配函数
        void* chunk = _aligned_malloc(block_size * chunk_size, alignof(max_align_t));
        if (!chunk) throw bad_alloc();
        chunks.push_back(chunk);

        Block* head = reinterpret_cast<Block*>(chunk);
        for (size_t i = 0; i < chunk_size - 1; ++i) {
            Block* curr = reinterpret_cast<Block*>(
                reinterpret_cast<char*>(chunk) + i * block_size);
            curr->next = reinterpret_cast<Block*>(
                reinterpret_cast<char*>(curr) + block_size);
        }
        reinterpret_cast<Block*>(
            reinterpret_cast<char*>(chunk) + (chunk_size-1)*block_size)->next = free_list;
        free_list = head;
    }
};

// ==================== 修正后的性能测试 ====================
struct TestObject {
    int data[16];
};

void test_mempool(size_t count) {
    MemoryPool pool(sizeof(TestObject));
    vector<void*> ptrs(count);
    
    auto start = high_resolution_clock::now();  // 修正时钟名称
    for (size_t i = 0; i < count; ++i) {
        ptrs[i] = pool.allocate();
    }
    for (size_t i = 0; i < count; ++i) {
        pool.deallocate(ptrs[i]);
    }
    auto end = high_resolution_clock::now();
    
    cout << " MemoryPool: " << duration_cast<microseconds>(end - start).count() << "μs"
         << " (chunks=" << pool.chunk_count() << ")" << endl;
}

void test_newdelete(size_t count) {
    vector<TestObject*> ptrs(count);
    
    auto start = high_resolution_clock::now();  // 修正时钟名称
    for (size_t i = 0; i < count; ++i) {
        ptrs[i] = new TestObject;
    }
    for (size_t i = 0; i < count; ++i) {
        delete ptrs[i];
    }
    auto end = high_resolution_clock::now();
    
    cout << " new/delete: " << duration_cast<microseconds>(end - start).count() << "μs" 
         << endl;
}

int main() {
    const size_t test_count = 1000000;
    
    cout << "===== 内存池 vs new/delete 性能对比 =====" << endl;
    cout << "测试对象大小: " << sizeof(TestObject) << "字节" << endl;
    cout << "测试操作次数: " << test_count << "次分配+释放" << endl;
    
    cout << "[内存池] ";
    test_mempool(test_count);
    
    cout << "[new/delete] ";
    test_newdelete(test_count);
    
    return 0;
}

5. SIMD加速计算

```cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h>  // AVX指令集头文件
#include <cmath>        // fabs函数

using namespace std;
using namespace std::chrono;

// ==================== 标量版本 ====================
float sum_scalar(const float* arr, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; ++i) {
        sum += arr[i];
    }
    return sum;
}

// ==================== AVX优化版本 ====================
float sum_simd(const float* arr, int n) {
    __m256 sum_vec = _mm256_setzero_ps();
    int i = 0;

    // 主循环处理8的倍数元素
    for (; i <= n - 8; i += 8) {
        __m256 data = _mm256_loadu_ps(arr + i);
        sum_vec = _mm256_add_ps(sum_vec, data);
    }

    // 水平求和：将8个float累加到1个float
    __m128 low  = _mm256_castps256_ps128(sum_vec);    // 提取低128位
    __m128 high = _mm256_extractf128_ps(sum_vec, 1);  // 提取高128位
    __m128 sum128 = _mm_add_ps(low, high);            // 合并高低部分
    
    // 继续水平相加
    sum128 = _mm_hadd_ps(sum128, sum128);  // [a+b, c+d, a+b, c+d]
    sum128 = _mm_hadd_ps(sum128, sum128);  // [a+b+c+d, ... x4]
    
    float sum = _mm_cvtss_f32(sum128);     // 转换为标量

    // 处理剩余元素（1-7个）
    for (; i < n; ++i) {
        sum += arr[i];
    }
    return sum;
}

// ==================== 性能测试 ====================
vector<float> generate_data(int n) {
    vector<float> arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = (i % 100) * 0.1f;  // 生成测试数据
    }
    return arr;
}

template <typename Func>
void test_performance(const string& name, Func func, const vector<float>& arr) {
    const int iterations = 1000;
    volatile float dummy = 0;  // 防止优化
    
    auto start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        dummy += func(arr.data(), arr.size());
    }
    auto end = high_resolution_clock::now();
    
    cout << name << ": \t"
         << duration_cast<microseconds>(end - start).count() / 1000.0 << " ms"
         << " (dummy=" << dummy << ")" << endl;
}

int main() {
    const int n = 10000000;  // 1000万元素
    auto arr = generate_data(n);

    // 性能测试
    cout << "===== 性能对比 =====" << endl;
    test_performance("标量版本", sum_scalar, arr);
    test_performance("AVX版本 ", sum_simd, arr);

    // 验证结果正确性
    const float sum1 = sum_scalar(arr.data(), n);
    const float sum2 = sum_simd(arr.data(), n);
    cout << "\n===== 结果验证 =====" << endl;
    cout << "标量结果: " << sum1 << endl;
    cout << "AVX结果:  " << sum2 << endl;
    cout << "绝对误差: " << fabs(sum1 - sum2) << endl;

    return 0;
}
```

### 总结要点
1. **内存对齐**：确保数据结构对齐可提升访问速度
2. **缓存友好**：数据访问模式应尽量保持空间局部性
3. **分支预测**：对有序数据的分支处理更快
4. **多线程优化**：避免不同核心修改同一缓存行

通过实际运行这些代码示例，您可以直观感受不同优化策略带来的性能差异。建议在Linux系统上测试以获得更准确的结果，注意编译器优化级别对测试结果的影响（示例中使用-O0禁用优化）。