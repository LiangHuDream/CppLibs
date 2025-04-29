/*
 * Author: LiangHuDream
 * Date: 2025-04-29
 * Copyright © 2025 LiangHuDream. All rights reserved.
 * License: MIT (See LICENSE file)
 */
#include "my_template.hpp"

<<<<<<< HEAD
=======
template<typename T>
void MyTemplate<T>::print(T value) {
    std::cout<< "Value:" << value << std::endl;
}

>>>>>>> 5c5f85d3f4370fe763d280b409307715752137a3
template class MyTemplate<int>;
template class MyTemplate<double>;