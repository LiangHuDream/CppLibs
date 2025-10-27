#include <memory>
#include <iostream>

void test1() {
    // 方式1： 用make_shared创建
    // 功能: 分配int对象
    std::shared_ptr<int> sp1 = std::make_shared<int>(100);
    // 方式2：用原始指针构造(不推荐， 有陷阱)
    int * raw_ptr = new int(200);
    std::shared_ptr<int> sp2(raw_ptr);

    // 方式3： 空指针初始化
    std::shared_ptr<int> sp3; // 空， 引用计数 = 0
    std::shared_ptr<int> sp4(nullptr); // 空， 引用计数 = 0
    return;
}

int main() {
    test1();
    return 0;
}

