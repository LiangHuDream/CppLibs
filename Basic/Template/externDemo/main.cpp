/*
 * Author: LiangHuDream
 * Date: 2025-04-29
 * Copyright © 2025 LiangHuDream. All rights reserved.
 * License: MIT (See LICENSE file)
 */

#include "my_template.h"

int main() {
    MyTemplate<int> a;
    a.print(42);

    MyTemplate<double> b;
    b.print(3.14);
    return 0;
}