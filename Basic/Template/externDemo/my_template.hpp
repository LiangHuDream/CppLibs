/*
 * Author: LiangHuDream
 * Date: 2025-04-29
 * Copyright © 2025 LiangHuDream. All rights reserved.
 * License: MIT (See LICENSE file)
 */
#pragma once
#include <iostream>
template<typename T>
class MyTemplate {
    public:
        void print(T value){
            std::cout << "General Value: " << value << std::endl;
        }
};

extern template class MyTemplate<int>;    // 生成 int 版本的实例化代码
extern template class MyTemplate<double>; // 生成 double 版本的实例化代码
void func1();
void func2();