#include <iostream>
#include <memory>    // 使用智能指针 std::unique_ptr
#include <vector>   // 使用标准库容器 std::vector

// 零法则：无需手动定义析构函数、拷贝/移动构造函数或赋值操作符
class A {
private:
    std::unique_ptr<int[]> data;  // 使用智能指针管理动态数组
    std::vector<int> buffer;      // 使用标准库容器管理动态资源

public:
    // 构造函数：初始化成员
    explicit A(size_t size = 0) : data(std::make_unique<int[]>(size)), buffer(size, 0) {
        std::cout << "构造函数: size=" << size << "\n";
    }

    // 辅助函数：打印资源地址（验证所有权）
    void print() const {
        std::cout << "data地址: " << data.get() << ", buffer地址: " << buffer.data() << "\n";
    }

    // 注：无需定义析构函数、拷贝/移动操作！
};

int main() {
    std::cout << "----- 测试构造函数 -----\n";
    A a1(3);
    a1.print();

    std::cout << "\n----- 测试移动构造函数 -----\n";
    A a2 = std::move(a1);  // 移动构造函数（编译器自动生成）
    a2.print();
    a1.print();  // a1 的 data 被置空

    std::cout << "\n----- 测试移动赋值操作符 -----\n";
    A a3;
    a3 = std::move(a2);  // 移动赋值操作符（编译器自动生成）
    a3.print();
    a2.print();  // a2 的 data 被置空

    // 以下代码会编译错误：因为 std::unique_ptr 禁止拷贝
    // A a4 = a3;      // 拷贝构造函数被隐式删除
    // A a5; a5 = a3;  // 拷贝赋值操作符被隐式删除

    std::cout << "\n----- 程序结束，自动调用析构函数 -----\n";
    return 0;
}