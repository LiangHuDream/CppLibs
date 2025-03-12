#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

class ThreadSafeCounter {
private:
    std::atomic<int> value;

public:
    ThreadSafeCounter() : value(0) {}

    void increment() {
        value.fetch_add(1, std::memory_order_relaxed);
    }

    void decrement() {
        value.fetch_sub(1, std::memory_order_relaxed);
    }

    int get_value() const {
        return value.load(std::memory_order_relaxed);
    }
};

// 测试函数，放在单独的测试模块中会更合适
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
    constexpr int num_threads = 4;       // 使用constexpr表示常量
    constexpr int num_ops_per_thread = 100000;
    std::vector<std::thread> threads;

    // 创建并启动线程
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(test_counter, std::ref(counter), num_ops_per_thread);
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    // 输出最终结果
    std::cout << "Final counter value: " << counter.get_value() << std::endl;

    return 0;
}

/*
## 代码优化分析
### 1. 算法与数据结构优化

- **选择高效算法**：确保您使用的算法适合您的问题规模。例如，对于大数据集，使用O(n log n)的排序算法通常比O(n^2)的算法更高效。
- **优化数据结构**：选择适当的数据结构可以显著提高性能。例如，使用哈希表进行快速查找，使用链表进行频繁插入和删除操作。

### 2. 代码级别的优化

- **减少不必要的计算**：避免在循环中执行可以移到循环外的计算。
- **利用缓存**：对于频繁访问的数据，考虑使用缓存来减少访问时间。
- **避免过度使用资源**：比如，不要在不必要时创建大量对象或分配大块内存。

### 3. 并行与并发

- **利用多核处理器**：通过多线程或多进程来并行处理任务，可以显著提高性能，特别是对于计算密集型任务。
- **异步编程**：对于I/O密集型任务，使用异步编程可以避免线程阻塞，提高程序响应性。

### 4. 编译器与解释器优化

- **利用编译器优化**：许多现代编译器都提供优化选项，如GCC的-O2和-O3优化级别。
- **解释器优化**：对于解释型语言，了解解释器的性能特性和优化技巧也很重要。

### 5. 性能测试与分析

- **使用性能分析工具**：如gprof、Valgrind、Visual Studio Profiler等，这些工具可以帮助您识别性能瓶颈。
- **基准测试**：在优化前后进行基准测试，以量化性能改进。

### 结论

代码优化是一个持续的过程，需要综合考虑多个因素。为了获得最佳的优化效果，我建议您：

1. **明确优化目标**：确定您是想提高速度、减少内存使用，还是改善代码的可读性。
2. **分析性能瓶颈**：使用性能分析工具来识别代码中的热点。
3. **逐步优化**：不要试图一次性解决所有问题，而是逐步进行，每次优化后都进行测试以验证效果。

如果您能提供更多关于您的代码和具体需求的信息，我可以为您提供更具体的优化建议。
 */