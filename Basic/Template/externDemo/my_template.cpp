/*
 * Author: LiangHuDream
 * Date: 2025-04-29
 * Copyright © 2025 LiangHuDream. All rights reserved.
 * License: MIT (See LICENSE file)
 */
#include "my_template.h"
#include <iostream>

template<typename T>
void MyTemplate<T>::print(T value) {
    std::cout << "Value: " << value << std::endl;
}

// 显式实例化模板（实际生成代码的地方）
template class MyTemplate<int>;    // 生成 int 版本的实例化代码
template class MyTemplate<double>; // 生成 double 版本的实例化代码