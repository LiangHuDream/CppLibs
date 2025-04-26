#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <map>
#include <thread>
#include <memory>
#include <atomic>
#include "../../STL/SimpleAllocator.h"
// ================== 系统测试套件 ==================
class SimpleAllocatorST : public ::testing::Test {
protected:
  static constexpr size_t kStressIterations = 100000;

  // 将 TrackedObject 定义为测试套件的嵌套类
  struct TrackedObject {
    static int constructed;
    static int destroyed;
    std::string data;

    TrackedObject() : data("default") { ++constructed; }
    TrackedObject(const std::string &s) : data(s) { ++constructed; }
    ~TrackedObject() { ++destroyed; }
  };

  // 内存跟踪分配器（定义在类作用域内）
  template <typename T> class TrackingAllocator : public SimpleAllocator<T> {
  public:
    static size_t total_allocated;
    static size_t total_deallocated;

    T *allocate(size_t n) {
      total_allocated += n * sizeof(T);
      return SimpleAllocator<T>::allocate(n);
    }

    void deallocate(T *p, size_t n) {
      total_deallocated += n * sizeof(T);
      SimpleAllocator<T>::deallocate(p, n);
    }
  };
};

// 静态成员初始化
int SimpleAllocatorST::TrackedObject::constructed = 0;
int SimpleAllocatorST::TrackedObject::destroyed = 0;

template <typename T>
size_t SimpleAllocatorST::TrackingAllocator<T>::total_allocated = 0;

template <typename T>
size_t SimpleAllocatorST::TrackingAllocator<T>::total_deallocated = 0;

// ================== 测试用例 ==================
// 测试1: 验证与std::vector的集成能力
// 1. 执行1000次push_back操作验证容量增长
// 2. 计算元素总和验证数据完整性
// 3. 清空后验证内存释放
TEST_F(SimpleAllocatorST, VectorIntegration) {
  std::vector<int, SimpleAllocator<int>> vec;

  for (int i = 0; i < 1000; ++i) {
    vec.push_back(i);
  }
  EXPECT_EQ(vec.size(), 1000);
  EXPECT_EQ(vec.back(), 999);

  int sum = 0;
  for (const auto &n : vec)
    sum += n;
  EXPECT_EQ(sum, 499500);

  vec.clear();
  EXPECT_TRUE(vec.empty());
}

// 测试2: 验证与std::list的兼容性
// 1. 显式指定链表节点分配器类型
// 2. 测试头尾插入各500元素
// 3. 使用remove_if移除偶数元素
// 注意：测试分配器对链表节点结构的内存管理能力
TEST_F(SimpleAllocatorST, ListIntegration) {
  // 显式指定节点分配器类型
  using ListNodeAllocator = SimpleAllocator<std::_List_node<int>>;
  std::list<int, SimpleAllocator<int>> lst;

  for (int i = 0; i < 500; ++i) {
    lst.push_front(i);
    lst.push_back(i);
  }
  EXPECT_EQ(lst.size(), 1000);

  lst.remove_if([](int n) { return n % 2 == 0; });
  EXPECT_LT(lst.size(), 1000);
}

// 测试3: 验证关联容器的集成能力
// 1. 使用自定义分配器的map容器
// 2. 插入3个键值对
// 3. 验证查找和访问功能
// 重点：测试pair<const Key, Value>类型的内存分配
TEST_F(SimpleAllocatorST, MapIntegration) {
  using MapType = std::map<int, std::string, std::less<int>,
                           SimpleAllocator<std::pair<const int, std::string>>>;

  MapType my_map;
  my_map[1] = "One";
  my_map[2] = "Two";
  my_map[3] = "Three";

  EXPECT_EQ(my_map.size(), 3);
  EXPECT_EQ(my_map.at(2), "Two");
}

// 测试4: 多线程压力测试
// 1. 启动10个并行工作线程
// 2. 每个线程执行10万次push/clear操作
// 3. 使用原子计数器统计成功线程数
// 验证：线程安全性和资源竞争处理能力
TEST_F(SimpleAllocatorST, MultithreadedStress) {
  std::atomic<int> success_count{0};
  std::vector<std::thread> threads;

  auto worker = [&]() {
    std::vector<int, SimpleAllocator<int>> local_vec;
    for (size_t i = 0; i < kStressIterations; ++i) {
      local_vec.push_back(i);
      if (i % 100 == 0)
        local_vec.clear();
    }
    ++success_count;
  };

  for (int i = 0; i < 10; ++i) {
    threads.emplace_back(worker);
  }

  for (auto &t : threads) {
    t.join();
  }

  EXPECT_EQ(success_count, 10);
}

// 测试5: 对象生命周期管理验证
// 1. 使用带计数器的TrackedObject类型
// 2. 创建包含100个对象的vector
// 3. 验证构造次数与析构次数匹配
// 目的：确保分配器正确调用构造/析构函数
TEST_F(SimpleAllocatorST, ObjectLifecycle) {
  TrackedObject::constructed = 0;
  TrackedObject::destroyed = 0;

  {
    std::vector<TrackedObject, SimpleAllocator<TrackedObject>> vec;
    vec.reserve(100);

    for (int i = 0; i < 100; ++i) {
      vec.emplace_back("object_" + std::to_string(i));
    }

    EXPECT_EQ(TrackedObject::constructed, 100);
    EXPECT_EQ(vec[99].data, "object_99");
  }

  EXPECT_EQ(TrackedObject::destroyed, 100);
}

// 测试6: 内存泄漏检测
// 1. 使用继承自SimpleAllocator的TrackingAllocator
// 2. 记录总分配和释放字节数
// 3. 创建包含1万元素的vector后释放
// 断言：分配内存量必须等于释放内存量
TEST_F(SimpleAllocatorST, MemoryLeakCheck) {
  // 使用类作用域内的 TrackingAllocator
  using TestAllocator = TrackingAllocator<int>;
  TestAllocator::total_allocated = 0;
  TestAllocator::total_deallocated = 0;

  {
    std::vector<int, TestAllocator> vec;
    for (int i = 0; i < 10000; ++i) {
      vec.push_back(i);
    }
  }

  EXPECT_EQ(TestAllocator::total_allocated, TestAllocator::total_deallocated);
}

// ================== 主函数 ==================
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
/*
# 编译
g++ -std=c++17 -O3 -o st_test test_simple_allocator_st.cpp -lgtest -pthread

# 运行测试
./st_test

# 使用 Valgrind 检测内存泄漏
valgrind --leak-check=full --error-exitcode=1 ./st_test
*/