#include <iostream>
#include <utility>  // for std::move

class A {
private:
    int* data;       // 动态数组指针
    size_t size;     // 数组大小

public:
    // 1. 普通构造函数
    explicit A(size_t sz = 0) : data(nullptr), size(sz) {
        if (sz > 0) {
            data = new int[sz];
            std::fill(data, data + sz, 0);  // 初始化为0
        }
        std::cout << "构造函数: size=" << sz << "\n";
    }

    // 2. 析构函数
    ~A() {
        delete[] data;
        std::cout << "析构函数: size=" << size << "\n";
    }

    // 3. 拷贝构造函数
    A(const A& other) : data(nullptr), size(other.size) {
        if (other.size > 0) {
            data = new int[other.size];
            std::copy(other.data, other.data + other.size, data);
        }
        std::cout << "拷贝构造函数: size=" << size << "\n";
    }

    // 4. 拷贝赋值操作符
    A& operator=(const A& other) {
        if (this != &other) {  // 处理自赋值
            delete[] data;     // 释放旧资源
            size = other.size;
            data = new int[size];
            std::copy(other.data, other.data + size, data);
        }
        std::cout << "拷贝赋值操作符: size=" << size << "\n";
        return *this;
    }

    // 5. 移动构造函数
    A(A&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;  // 置空源对象指针
        other.size = 0;
        std::cout << "移动构造函数: size=" << size << "\n";
    }

    // 6. 移动赋值操作符
    A& operator=(A&& other) noexcept {
        if (this != &other) {  // 处理自赋值
            delete[] data;     // 释放当前资源
            data = other.data;
            size = other.size;
            other.data = nullptr;  // 置空源对象
            other.size = 0;
        }
        std::cout << "移动赋值操作符: size=" << size << "\n";
        return *this;
    }

    // 辅助函数：打印数组地址（验证资源是否转移）
    void print() const {
        std::cout << "data地址: " << data << ", size=" << size << "\n";
    }
};

// 测试函数
int main() {
    std::cout << "----- 测试构造函数 -----\n";
    A a1(3);        // 普通构造函数
    a1.print();

    std::cout << "\n----- 测试拷贝构造函数 -----\n";
    A a2 = a1;      // 拷贝构造函数
    a2.print();

    std::cout << "\n----- 测试移动构造函数 -----\n";
    A a3 = std::move(a1);  // 移动构造函数（a1.data 变为 nullptr）
    a3.print();
    a1.print();  // a1 已失效

    std::cout << "\n----- 测试拷贝赋值操作符 -----\n";
    A a4;
    a4 = a2;        // 拷贝赋值操作符
    a4.print();

    std::cout << "\n----- 测试移动赋值操作符 -----\n";
    A a5;
    a5 = std::move(a2);  // 移动赋值操作符（a2.data 变为 nullptr）
    a5.print();
    a2.print();  // a2 已失效

    std::cout << "\n----- 程序结束，自动调用析构函数 -----\n";
    return 0;
}

/*
### **关键区别总结**
| **函数**               | **作用**                                   | **调用场景**                  | **资源处理**               |
|------------------------|-------------------------------------------|-------------------------------|----------------------------|
| **构造函数**           | 分配资源                                  | `A a1(3);`                    | 新分配内存                 |
| **析构函数**           | 释放资源                                  | 对象销毁时自动调用            | 释放内存                   |
| **拷贝构造函数**       | 深拷贝初始化新对象                        | `A a2 = a1;`                  | 复制资源                   |
| **移动构造函数**       | 转移资源初始化新对象                      | `A a3 = std::move(a1);`       | 接管资源，源对象置空       |
| **拷贝赋值操作符**     | 深拷贝赋值给已存在对象                    | `a4 = a2;`                    | 释放旧资源后复制新资源     |
| **移动赋值操作符**     | 转移资源赋值给已存在对象                  | `a5 = std::move(a2);`         | 释放旧资源后接管新资源     |
*/