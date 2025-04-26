#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include "SimpleAllocator.h"

// ================== 单元测试 ==================
class SimpleAllocatorUT : public ::testing::Test {
protected:
    void SetUp() override {
        // 通用初始化（可选）
    }
};

// 测试1: 基础分配/释放
TEST_F(SimpleAllocatorUT, AllocateDeallocate) {
    SimpleAllocator<int> alloc;
    constexpr size_t n = 5;
    
    int* ptr = alloc.allocate(n);
    EXPECT_NE(ptr, nullptr);   // 验证分配成功
    EXPECT_NO_THROW(alloc.deallocate(ptr, n));  // 验证释放不抛异常
}

// 测试2: 对象构造/析构
TEST_F(SimpleAllocatorUT, ConstructDestroy) {
    SimpleAllocator<std::string> alloc;
    std::string* ptr = alloc.allocate(1);
    
    alloc.construct(ptr, "test");  // 带参数构造
    EXPECT_EQ(*ptr, "test");       // 验证构造成功
    
    alloc.destroy(ptr);            // 显式析构
    alloc.deallocate(ptr, 1);
}

// 测试3: rebind 机制
TEST_F(SimpleAllocatorUT, Rebind) {
    using IntAllocator = SimpleAllocator<int>;
    using ReboundAllocator = IntAllocator::rebind<double>::other;
    
    ReboundAllocator alloc;
    double* ptr = alloc.allocate(3);
    EXPECT_NE(ptr, nullptr);
    alloc.deallocate(ptr, 3);
}

// 测试4: 与STL容器集成
TEST_F(SimpleAllocatorUT, STLVectorIntegration) {
    std::vector<int, SimpleAllocator<int>> vec;
    
    vec.push_back(42);
    vec.push_back(100);
    vec.push_back(200);
    
    EXPECT_EQ(vec[0], 42);
    EXPECT_EQ(vec[1], 100);
    EXPECT_EQ(vec[2], 200);
}

// 测试5: 零大小分配
TEST_F(SimpleAllocatorUT, ZeroSizeAllocation) {
    SimpleAllocator<int> alloc;
    EXPECT_THROW(alloc.allocate(0), std::bad_alloc);  // 验证零分配异常
}

// ================== 主函数 ==================
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }

/*
g++ -std=c++17 -o test_ut simple_allocator_ut.cpp -lgtest -lgmock -pthread
valgrind --leak-check=full ./test_ut
*/