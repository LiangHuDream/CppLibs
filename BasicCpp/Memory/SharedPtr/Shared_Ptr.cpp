#include <memory>
#include <iostream>
#include <cstdio>
#include <thread>

void test1() {
    // 方式1： 用make_shared创建
    // 功能: 分配int对象
    std::shared_ptr<int> sp1 = std::make_shared<int>(100);
    // 方式2：用原始指针构造(不推荐， 有陷阱)
    int * raw_ptr = new int(200);
    std::shared_ptr<int> sp2(raw_ptr);

    // 方式3： 空指针初始化
    std::shared_ptr<int> sp3; // 空， 引用计数 = 0
    std::cout << "sp3 count: " << sp3.use_count() << std::endl;
    std::shared_ptr<int> sp4(nullptr); // 空， 引用计数 = 0
    std::cout << "sp4 count: " << sp4.use_count() << std::endl;
    return;
}

void test2() {
    auto sp = std::make_shared<int>(10);
    std::cout << *sp << std::endl; // 解引用输出10
    *sp = 20;
    std::cout << *sp << std::endl; // 输出20
    int * raw = sp.get();
    std::cout << *raw << std::endl;
}

void test3() {
    auto sp1 = std::make_shared<int>(100);
    std::cout << "sp1 count:" << sp1.use_count() << std::endl;

    std::shared_ptr<int> sp2 = sp1;
    std::cout << "sp1 count:" << sp1.use_count() << std::endl;
    std::cout << "sp2 count:" << sp2.use_count() << std::endl;

    std::shared_ptr<int> sp3;
    sp3 = sp1;
    std::cout << "sp1 count:" << sp1.use_count() << std::endl;

    sp3 = std::make_shared<int>(200);
    std::cout << "sp1 count:"<< sp1.use_count() << std::endl;
    return;
}

void test4() {
    auto sp1 = std::make_shared<int>(10);
    auto sp2 = sp1;
    return;
}

// 值传递
void func1(std::shared_ptr<int> sp) {
    std::cout << "func1 count:" << sp.use_count() << std::endl; // 2
}

// 引用传递
void func2(const std::shared_ptr<int>& sp) {
    std::cout << "func2 count: " << sp.use_count() << std::endl; // 1
}

std::shared_ptr<int> create_int(int value) {
    return std::make_shared<int>(value);
}

void test5() {
    auto sp = std::make_shared<int>(10);
    std::cout << "count: "  << sp.use_count() << std::endl;
    func1(sp);
    func2(sp);
    auto sp2 = create_int(20);
}


// 管理数组
void test6() {
    std::shared_ptr<int> sp1(new int[10], [](int* p) {
        delete[] p;
        std::cout << "数组已释放\n";
    });
    std::shared_ptr<int> sp2(new int[10], std::default_delete<int[]>());
}

void test7() {
    std::shared_ptr<FILE> file(fopen("test.txt", "w"),
    [](FILE* f) {
        if (f) {
            fclose(f);
        }
        std::cout << "文件已关闭\n"; 
    });
    if (file) {
        fputs("hello", file.get()); 
    }
    return;
}

struct A;
struct B;
struct A {
    std::shared_ptr<B> b_ptr;
    ~A() { std::cout << "A 被销毁\n"; }
};

struct B {
    std::shared_ptr<A> a_ptr;
    ~B() { std::cout << "B 被销毁\n"; }
};

// 循环引用（导致内存泄漏）
void test8() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();

    a->b_ptr = b;
    b->a_ptr = a;
}

struct A1;
struct B1;

struct A1 {
    std::weak_ptr<B> b_ptr; // 用 weak_ptr 替代 shared_ptr
    ~A1() { std::cout << "A 被销毁\n"; }
};

struct B1 {
    std::weak_ptr<A> a_ptr; // 用 weak_ptr 替代 shared_ptr
    ~B1() { std::cout << "B 被销毁\n"; }
};

// 循环引用（导致内存泄漏）-- 解决
// void test9() {
//     auto a = std::make_shared<A1>();
//     auto b = std::make_shared<B1>();

//     a->b_ptr = b; 
//     b->a_ptr = a; 
// }

// 陷阱2：重复托管同一原始指针
void test10() {
    int* raw = new int(10);

    std::shared_ptr<int> sp1(raw);
    std::shared_ptr<int> sp2(raw); // 错误！sp1 和 sp2 各自管理 raw，会释放两次
}

void test11() {
    auto sp1 = std::make_shared<int>(10);
    int* raw = sp1.get();

    std::shared_ptr<int> sp2(raw); // 错误！sp1 和 sp2 会重复释放 raw
}

// 陷阱3：用 get() 返回的原始指针构造新 shared_ptr
void test12() {
    auto sp1 = std::make_shared<int>(10);
    int* raw = sp1.get();

    std::shared_ptr<int> sp2(raw); // 错误！sp1 和 sp2 会重复释放 raw
}

void increment(std::shared_ptr<int> sp) {
    for (int i = 0; i < 10000; ++i) {
        (*sp)++; // 对对象的修改不是线程安全的，需加锁
    }
}

void test13() {
    auto sp = std::make_shared<int>(0);

    std::thread t1(increment, sp);
    std::thread t2(increment, sp);

    t1.join();
    t2.join();
    std::cout << *sp << std::endl; // 可能不是 2000（因为无同步）
}

int main() {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    // test9();
    test10();
    test11();
    test12();
    test13();
    return 0;
}

