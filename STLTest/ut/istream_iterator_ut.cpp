/**
 * Copyright (c) 2025-2025 LiangHuDream
 * MIT License. See LICENSE for details.
 * 
 * Module:STL
 * Description:测试IstreamIterator的UT
 * 
 * Date:2025-05-12
 * Author:LiangHuDream
 */

#include <gtest/gtest.h>
#include <sstream> // 使用字符串流模拟输入流
#include "IstreamIterator.h" // 包含istreamIterator类的头文件

TEST(IstreamIteratorTest, DefaultConstructorIsEnd) {
    istreamIterator<int> end_iter; // 默认构造的结束迭代器
    istreamIterator<int> end_sentinel; 
    EXPECT_TRUE(end_iter == end_sentinel); // 应当相等
}

TEST(IstreamIteratorTest, ReadIntegersCorrectly) {
    // 准备测试输入流
    std::istringstream input("42 99 7");
    istreamIterator<int> iter(input);
    istreamIterator<int> end;

    // 验证读取顺序和值
    EXPECT_EQ(*iter, 42); 
    ++iter; // 前置递增测试
    EXPECT_EQ(*iter, 99);
    iter++; // 后置递增测试
    EXPECT_EQ(*iter, 7);
    ++iter;
    EXPECT_TRUE(iter == end); // 流结束后应等于结束迭代器
}

TEST(IstreamIteratorTest, HandleStringStream) {
    std::istringstream input("Hello World C++");
    istreamIterator<std::string> iter(input);
    istreamIterator<std::string> end;

    EXPECT_EQ(*iter, "Hello");
    ++iter;
    EXPECT_EQ(*iter, "World");
    ++iter;
    EXPECT_EQ(*iter, "C++");
    ++iter;
    ASSERT_EQ(iter, end); // 流结束后断言终止
}

TEST(IstreamIteratorTest, DetectStreamEndCorrectly) {
    std::istringstream empty_input("");
    istreamIterator<double> iter(empty_input); // 构造时触发read()
    istreamIterator<double> end;
    
    EXPECT_TRUE(iter == end); // 空流构造后直接为结束状态
}

TEST(IstreamIteratorTest, ComparisonOperators) {
    std::istringstream stream1("A B");
    std::istringstream stream2("X Y");

    istreamIterator<std::string> a(stream1);
    istreamIterator<std::string> b(stream2);
    istreamIterator<std::string> end;

    // 不同流的迭代器不应相等
    EXPECT_NE(a, b); 
    
    // 同一流的不同状态迭代器
    ++a;
    EXPECT_NE(a, istreamIterator<std::string>(stream1));
    
    // 两个结束迭代器应相等
    EXPECT_EQ(end, istreamIterator<std::string>());
}

/*
g++ -std=c++11 istream_iterator_ut.cpp -lgtest -lgtest_main -lpthread -o istream_iterator_ut
*/