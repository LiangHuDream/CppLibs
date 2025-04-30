/**
 * Copyright (c) 2025-2025 LiangHuDream
 * MIT License. See LICENSE for details.
 *
 * Module:池式结构
 * Description:线程池实现测试
 *
 * Date:2025-04-30
 * Author:LiangHuDream
 */

#include <gtest/gtest.h>
#include "thread_pool.h"

using namespace PoolTypeStructure;
// 全局输出互斥量（生产环境应封装为日志模块）
namespace {
    std::mutex io_mutex;  // 互斥量定义
}

class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        pool_ = std::make_unique<ThreadPool>(4);
    }

    void TearDown() override {
        pool_->Shutdown();
    }

    std::unique_ptr<ThreadPool> pool_;
    std::mutex cout_mutex_;
};

// 测试用例1：基本功能验证
TEST_F(ThreadPoolTest, BasicFunctionality) {
    std::atomic<int> counter{0};
    constexpr int TASK_NUM = 100;
    std::vector<std::future<void>> futures;
    futures.reserve(TASK_NUM);

    for (int i = 0; i < TASK_NUM; ++i) {
        futures.emplace_back(pool_->SubmitTask([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            counter.fetch_add(1, std::memory_order_relaxed);
        }));
    }

    for (auto& future : futures) {
        future.wait();
    }

    ASSERT_EQ(counter.load(), TASK_NUM);
}

// 测试用例2：异常处理验证
TEST_F(ThreadPoolTest, ExceptionHandling) {
    auto future = pool_->SubmitTask([]() {
        throw std::runtime_error("Test exception");
        return 42;
    });

    EXPECT_THROW({
        try {
            future.get();
        } catch (const std::runtime_error& ex) {
            EXPECT_STREQ(ex.what(), "Test exception");
            throw;
        }
    }, std::runtime_error);
}

// 测试用例3：关闭后提交任务
TEST_F(ThreadPoolTest, PostAfterShutdown) {
    pool_->Shutdown();
    
    EXPECT_THROW({
        pool_->SubmitTask([](){});
    }, std::runtime_error);
}

// 测试用例4：结果返回值验证
TEST_F(ThreadPoolTest, ReturnValueVerification) {
    auto future = pool_->SubmitTask([]() -> int {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 42;
    });

    EXPECT_EQ(future.get(), 42);
}

// 测试用例5：并发压力测试
TEST_F(ThreadPoolTest, ConcurrentStressTest) {
    constexpr int STRESS_TASKS = 1000;
    std::atomic<int> success_count{0};
    std::vector<std::future<void>> futures;
    futures.reserve(STRESS_TASKS);

    for (int i = 0; i < STRESS_TASKS; ++i) {
        futures.emplace_back(pool_->SubmitTask([&success_count, i]() {
            if (i % 10 == 0) {
                throw std::logic_error("Intentional error");
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            success_count.fetch_add(1, std::memory_order_relaxed);
        }));
    }

    int exception_count = 0;
    for (auto& future : futures) {
        try {
            future.get();
        } catch (...) {
            ++exception_count;
        }
    }

    const int expected_success = STRESS_TASKS - (STRESS_TASKS / 10);
    ASSERT_EQ(success_count.load(), expected_success);
    ASSERT_EQ(exception_count, STRESS_TASKS / 10);
}

// 测试用例6：线程池析构测试
TEST(ThreadPoolDestructorTest, DestructorBehavior) {
    std::atomic<int> cleanup_count{0};
    {
        ThreadPool local_pool(2);
        for (int i = 0; i < 10; ++i) {
            local_pool.SubmitTask([&cleanup_count]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                cleanup_count.fetch_add(1);
            });
        }
    } // 离开作用域自动调用析构

    ASSERT_EQ(cleanup_count.load(), 10);
}

/*
g++ -std=c++17 -pthread thread_pool.cpp thread_pool_test.cpp -lgtest_main -lgtest -o thread_pool_test
./thread_pool_test
*/