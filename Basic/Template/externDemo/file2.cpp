/*
 * Author: LiangHuDream
 * Date: 2025-04-29
 * Copyright © 2025 LiangHuDream. All rights reserved.
 * License: MIT (See LICENSE file)
 */
// file1.cpp
#include "my_template.hpp"

void func2() {
    MyTemplate<int> b;
    b.print(100);  // 使用外部实例化的 MyTemplate<int>
    MyTemplate<double> c;
    c.print(3.14);  // 使用外部实例化的 MyTemplate<double>
}