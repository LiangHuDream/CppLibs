/*
 * Author: LiangHuDream
 * Date: 2025-04-29
 * Copyright © 2025 LiangHuDream. All rights reserved.
 * License: MIT (See LICENSE file)
 */
// file1.cpp
#include "my_template.hpp"

void func1() {
    MyTemplate<int> a;
    a.print(42);  // 使用外部实例化的 MyTemplate<int>
    MyTemplate<double> b;
    a.print(3.14);  // 使用外部实例化的 MyTemplate<double>
}