#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <memory>
#include "SimpleAllocator.h"

template <typename T>
class MockAllocatorCore {
public:
    MOCK_METHOD(T*, allocate, (size_t n));
    MOCK_METHOD(void, deallocate, (T* p, size_t n));
};

template <typename T>
class MockAllocator {
public:
    using value_type = T;

    // 核心模拟对象使用共享指针管理
    std::shared_ptr<MockAllocatorCore<T>> core = 
        std::make_shared<MockAllocatorCore<T>>();

    // 支持拷贝构造（共享核心对象）
    MockAllocator() = default;
    MockAllocator(const MockAllocator&) = default;

    // 允许 rebind
    template <typename U>
    struct rebind {
        using other = MockAllocator<U>;
    };

    T* allocate(size_t n) {
        return core->allocate(n);
    }

    void deallocate(T* p, size_t n) {
        core->deallocate(p, n);
    }
};

// ================== 被测试组件 ==================
template <template <typename> class Allocator>
class DataContainer {
public:
    explicit DataContainer(size_t size, Allocator<int> alloc = {})
        : data_(size, alloc) {}
    
    void Resize(size_t new_size) {
        data_.resize(new_size);
    }
    
private:
    std::vector<int, Allocator<int>> data_;
};

// ================== 测试用例 ==================
using ::testing::Return;
using ::testing::_;

class AllocatorITest : public ::testing::Test {
protected:
    MockAllocator<int> mock_alloc_;
};

TEST_F(AllocatorITest, InitialAllocation) {
    const size_t init_size = 10;
    
    // 设置预期
    EXPECT_CALL(*mock_alloc_.core, allocate(init_size))
        .WillOnce(Return(new int[init_size]));
    EXPECT_CALL(*mock_alloc_.core, deallocate(_, init_size))
        .Times(1);

    {
        DataContainer<MockAllocator> container(init_size, mock_alloc_);
    } // 触发析构
}

// ================== 主函数 ==================
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }

/*
# 编译（需安装 gtest/gmock）
g++ -std=c++17 -o allocator_itest test_simple_allocator_it.cpp -lgtest -lgmock -pthread

# 运行测试
./allocator_itest
*/

