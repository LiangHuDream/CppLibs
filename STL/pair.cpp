#include <utility>
#include <iostream>
#include <cstring> // for strcmp

template <typename T1, typename T2>
class Pair {
public:
    // 公共成员
    T1 first;
    T2 second;
    
    using first_type = T1;
    using second_type = T2;
    
    // =============== 构造函数组 ===============
    // 默认构造
    constexpr Pair() : first(), second() {}
    
    // 双参数构造
    Pair(const T1& a, const T2& b) : first(a), second(b) {}
    
    // 拷贝构造
    Pair(const Pair&) = default;
    
    // 移动构造 (C++11)
    Pair(Pair&& other) noexcept
        : first(std::move(other.first)), 
          second(std::move(other.second)) {}
    
    // 从不同类型拷贝构造
    template <typename U1, typename U2>
    Pair(const Pair<U1, U2>& other)
        : first(other.first), second(other.second) {}
    
    // 从不同类型移动构造
    template <typename U1, typename U2>
    Pair(Pair<U1, U2>&& other)
        : first(std::move(other.first)), 
          second(std::move(other.second)) {}
    
    // =============== 赋值运算符组 ===============
    // 拷贝赋值
    Pair& operator=(const Pair& other) {
        if (this != &other) {
            first = other.first;
            second = other.second;
        }
        return *this;
    }
    
    // 移动赋值 (C++11)
    Pair& operator=(Pair&& other) noexcept {
        first = std::move(other.first);
        second = std::move(other.second);
        return *this;
    }
    
    // =============== 功能方法 ===============
    // 交换两个pair
    void swap(Pair& other) noexcept {
        using std::swap;
        swap(first, other.first);
        swap(second, other.second);
    }
    
    // 转换为字符串（调试用）
    operator std::string() const {
        return "(" + std::to_string(first) + ", " + 
               std::to_string(second) + ")";
    }
};

// 非成员swap函数
template <typename T1, typename T2>
void swap(Pair<T1, T2>& lhs, Pair<T1, T2>& rhs) noexcept {
    lhs.swap(rhs);
}

// =============== 比较运算符实现 ===============
template <typename T1, typename T2>
bool operator==(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename T1, typename T2>
bool operator!=(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs) {
    return !(lhs == rhs);
}

template <typename T1, typename T2>
bool operator<(const Pair<T1, T2>& lhs, const Pair<T1, T2>& rhs) {
    return lhs.first < rhs.first || 
           (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

// ... 其他比较运算符类似实现 ...

// =============== 特化版本：处理字符串比较 ===============
template <>
bool operator==(const Pair<const char*, const char*>& lhs, 
                const Pair<const char*, const char*>& rhs) {
    return strcmp(lhs.first, rhs.first) == 0 && 
           strcmp(lhs.second, rhs.second) == 0;
}

// =============== Make_Pair函数实现 ===============
template <typename T1, typename T2>
Pair<T1, T2> Make_Pair(T1&& first, T2&& second) {
    return Pair<T1, T2>(std::forward<T1>(first), 
                        std::forward<T2>(second));
}

#include <iostream>
#include <vector>
#include <string>

void testPair() {
    std::cout << "================== TEST START ==================\n";
    
    // 测试1: 基本构造与访问
    Pair<int, double> p1(42, 3.14);
    std::cout << "p1: (" << p1.first << ", " << p1.second << ")\n";
    
    // 测试2: 拷贝构造
    auto p2 = p1;
    std::cout << "p2: (" << p2.first << ", " << p2.second << ")\n";
    
    // 测试3: 移动构造
    auto p3 = std::move(p1);
    std::cout << "p3: (" << p3.first << ", " << p3.second << ")\n";
    std::cout << "p1 after move: (" << p1.first << ", " << p1.second << ")\n";
    
    // 测试4: Make_Pair
    auto p4 = Make_Pair(std::string("Hello"), 123);
    std::cout << "p4: (" << p4.first << ", " << p4.second << ")\n";
    
    // 测试5: 不同类型构造
    Pair<double, int> p5 = p4;
    std::cout << "p5: (" << p5.first << ", " << p5.second << ")\n";
    
    // 测试6: 比较运算符
    Pair<int, int> a(1, 2);
    Pair<int, int> b(1, 2);
    Pair<int, int> c(2, 1);
    std::cout << "a == b: " << (a == b) << "\n";
    std::cout << "a != c: " << (a != c) << "\n";
    std::cout << "a < c: " << (a < c) << "\n";
    
    // 测试7: 字符串特化
    Pair<const char*, const char*> s1("key", "value");
    Pair<const char*, const char*> s2("key", "value");
    Pair<const char*, const char*> s3("key", "different");
    std::cout << "s1 == s2: " << (s1 == s2) << "\n";
    std::cout << "s1 != s3: " << (s1 != s3) << "\n";
    
    // 测试8: 在容器中使用
    std::vector<Pair<int, std::string>> employees;
    employees.emplace_back(101, "Alice");
    employees.emplace_back(102, "Bob");
    employees.emplace_back(103, "Charlie");
    
    std::cout << "Employees:\n";
    for (const auto& e : employees) {
        std::cout << "  ID: " << e.first << ", Name: " << e.second << "\n";
    }
    
    // 测试9: swap功能
    Pair<int, int> x(10, 20);
    Pair<int, int> y(30, 40);
    std::cout << "Before swap: x=" << (std::string)x << ", y=" << (std::string)y << "\n";
    x.swap(y);
    std::cout << "After swap: x=" << (std::string)x << ", y=" << (std::string)y << "\n";
    
    std::cout << "================== TEST END ====================\n";
}

int main() {
    testPair();
    return 0;
}
