// File: test_simple_allocator_perf.cpp
#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <map>
#include <thread>
#include <chrono>
#include <atomic>
#include "SimpleAllocator.h"

using namespace std::chrono;

// ================== 性能测试工具 ==================
class PerfTimer {
public:
  using Clock = high_resolution_clock;
  time_point<Clock> start;

  void Reset() { start = Clock::now(); }
  double ElapsedMs() { 
    return duration_cast<microseconds>(Clock::now() - start).count() / 1000.0;
  }
};

// ================== 测试套件 ==================
class SimpleAllocatorPerf : public ::testing::Test {
protected:
  static constexpr size_t kSmallSize = 64;    // 64B
  static constexpr size_t kLargeSize = 1 << 20; // 1MB
  static constexpr int kThreadCount = 8;
  static constexpr int kOpsPerThread = 100000;

  // 性能统计
  void LogPerf(const char* test_name, double ms, size_t ops) {
    printf("[PERF] %-20s: %8.2f ms | %8.2f ns/op | %zu ops\n", 
           test_name, ms, ms*1e6/ops, ops);
  }
};

// ================== 基础操作性能测试 ==================
TEST_F(SimpleAllocatorPerf, AllocDeallocSingleThread) {
    SimpleAllocator<char> alloc;
    constexpr size_t kIterations = 1e6;
    
    // 添加类型转换
    volatile char* ptr; // 保持volatile防止优化
    PerfTimer timer;
    timer.Reset();
    
    for (size_t i = 0; i < kIterations; ++i) {
      ptr = alloc.allocate(kSmallSize);
      alloc.deallocate(const_cast<char*>(ptr), kSmallSize); // 关键修复
    }
  
    double elapsed = timer.ElapsedMs();
    LogPerf("SingleThread Small", elapsed, kIterations);
  }

TEST_F(SimpleAllocatorPerf, LargeBlockPerformance) {
    SimpleAllocator<char> alloc;
    constexpr size_t kIterations = 1000;
    
    volatile char* ptr; // 保持volatile防止优化
    PerfTimer timer;
    timer.Reset();
    
    for (size_t i = 0; i < kIterations; ++i) {
      ptr = alloc.allocate(kLargeSize);
      alloc.deallocate(const_cast<char*>(ptr), kLargeSize); // 添加类型转换
    }

  double elapsed = timer.ElapsedMs();
  LogPerf("Large Block", elapsed, kIterations);
}

// ================== 对象生命周期性能 ==================
TEST_F(SimpleAllocatorPerf, ObjectConstructionCost) {
  struct TestObject {
    std::array<char, 256> data;
    TestObject() { data.fill(0); }
    ~TestObject() { data.fill(0xFF); }
  };

  SimpleAllocator<TestObject> alloc;
  constexpr size_t kIterations = 1e5;

  PerfTimer timer;
  timer.Reset();

  for (size_t i = 0; i < kIterations; ++i) {
    TestObject* p = alloc.allocate(1);
    alloc.construct(p);
    alloc.destroy(p);
    alloc.deallocate(p, 1);
  }

  double elapsed = timer.ElapsedMs();
  LogPerf("Object Lifecycle", elapsed, kIterations);
}

// ================== STL容器性能对比 ==================
TEST_F(SimpleAllocatorPerf, VectorPushPerf) {
  constexpr size_t kElements = 1e6;
  
  // 使用自定义分配器
  {
    std::vector<int, SimpleAllocator<int>> vec;
    PerfTimer timer;
    timer.Reset();
    
    for (int i = 0; i < kElements; ++i) {
      vec.push_back(i);
    }

    double elapsed = timer.ElapsedMs();
    LogPerf("Vector Push Custom", elapsed, kElements);
  }

  // 使用标准分配器
  {
    std::vector<int> vec;
    PerfTimer timer;
    timer.Reset();
    
    for (int i = 0; i < kElements; ++i) {
      vec.push_back(i);
    }

    double elapsed = timer.ElapsedMs();
    LogPerf("Vector Push Std", elapsed, kElements);
  }
}

// ================== 多线程性能测试 ==================
TEST_F(SimpleAllocatorPerf, ConcurrentAllocDealloc) {
  std::vector<std::thread> threads;
  std::atomic<size_t> total_ops{0};

  auto worker = [&]() {
    SimpleAllocator<int> alloc;
    for (int i = 0; i < kOpsPerThread; ++i) {
      int* p = alloc.allocate(1);
      *p = i; // 实际使用内存
      alloc.deallocate(p, 1);
    }
    total_ops += kOpsPerThread;
  };

  PerfTimer timer;
  timer.Reset();
  
  for (int i = 0; i < kThreadCount; ++i) {
    threads.emplace_back(worker);
  }

  for (auto& t : threads) {
    t.join();
  }

  double elapsed = timer.ElapsedMs();
  LogPerf("Concurrent Alloc", elapsed, total_ops.load());
}

// ================== 内存碎片测试 ==================
TEST_F(SimpleAllocatorPerf, MemoryFragmentationTest) {
  SimpleAllocator<int> alloc;
  constexpr size_t kBlocks = 1000;
  std::vector<int*> ptrs;

  // 分配不同大小的内存块
  for (size_t i = 0; i < kBlocks; ++i) {
    ptrs.push_back(alloc.allocate((i % 10) + 1));
  }

  // 随机释放
  std::random_shuffle(ptrs.begin(), ptrs.end());
  for (auto p : ptrs) {
    alloc.deallocate(p, 1); // 注意实际分配大小需记录
  }

  // 重新分配测试
  PerfTimer timer;
  timer.Reset();
  for (size_t i = 0; i < kBlocks; ++i) {
    ptrs[i] = alloc.allocate((i % 10) + 1);
  }
  double elapsed = timer.ElapsedMs();
  LogPerf("Fragmented Alloc", elapsed, kBlocks);
}

// ================== 主函数 ==================
// int main(int argc, char** argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
/*
# 编译
g++ -std=c++17 -O3 -o perf_test test_simple_allocator_perf.cpp -lgtest -pthread

# 运行测试（输出详细性能数据）
./perf_test --gtest_filter=*Perf*
*/
