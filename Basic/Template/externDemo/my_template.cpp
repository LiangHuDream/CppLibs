/*
 * Author: LiangHuDream
 * Date: 2025-04-29
 * Copyright © 2025 LiangHuDream. All rights reserved.
 * License: MIT (See LICENSE file)
 */
#include "my_template.hpp"

template<typename T>
void MyTemplate<T>::print(T value) {
    std::cout<< "Value:" << value << std::endl;
}

template class MyTemplate<int>;
template class MyTemplate<double>;