@[TOC](Future Directions)


---

#### 核心重难点：
1. **泛型非类型模板参数**  
   - 允许任意类型作为非类型模板参数（如`template<typename T, auto N>`）
   - 需解决类型推导和链接问题

2. **编译期控制流**  
   - `constexpr if`替代模板偏特化（减少代码膨胀）
   - 折叠表达式优化可变参数模板处理

3. **反射与元编程增强**  
   - 类型检查（`is_convertible_v`等）
   - 反射提案（如成员变量/函数查询）

4. **模块化支持**  
   - 解决传统头文件包含模型的编译效率问题
   - 显式实例化与模块接口的交互

5. **语法简化**  
   - 放宽`typename`使用限制
   - `void`类型规范化（统一处理无返回值函数）

17.1 Concepts（概念）
**功能**：通过显式约束模板参数类型，提升编译时类型检查和错误提示的可读性。

#### 示例代码：
```cpp
#include <iostream>
#include <vector>
#include <concepts>

// 定义一个Concept，要求类型支持加法运算
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

// 使用Concept约束模板参数
template<Addable T>
T add(T a, T b) {
    return a + b;
}

int main() {
    std::cout << add(1, 2) << std::endl;          // 输出3（int满足Addable）
    std::cout << add(1.5, 2.5) << std::endl;      // 输出4（double满足Addable）
    
    // std::vector<int>不满足Addable（无operator+），编译报错
    // std::cout << add(std::vector<int>{1}, std::vector<int>{2}) << std::endl;
    
    return 0;
}
```

**编译命令**（需支持C++20）：
```bash
g++ -std=c++20 concepts.cpp -o concepts
```

---

17.11 Modules（模块）
**功能**：替代传统头文件，提升编译速度和封装性。

示例代码：
**math_module.ixx**（模块接口文件）：
```cpp
export module math;

export template<typename T>
T add(T a, T b) {
    return a + b;
}
```

**main.cpp**（使用模块）：
```cpp
import math;

int main() {
    std::cout << add(3, 5) << std::endl;  // 输出8
    return 0;
}
```

**编译命令**（需支持C++20模块）：
```bash
g++ -std=c++20 -fmodules-ts -c math_module.ixx -o math_module.o
g++ -std=c++20 -fmodules-ts main.cpp math_module.o -o main
```

---

### 17.6 Deduction for Nonfinal Pack Expansions（非终结包展开推导）
**功能**：允许在模板参数包展开时进行类型推导。

示例代码：
```cpp
#include <iostream>
#include <tuple>

// 推导包展开中的类型
template<typename... Ts>
auto make_tuple_and_print(Ts&&... args) {
    auto t = std::make_tuple(std::forward<Ts>(args)...);
    std::cout << "Tuple elements: ";
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
    return t;
}

int main() {
    auto t = make_tuple_and_print(1, 2.5, "Hello");
    // 输出类型：std::tuple<int, double, const char*>
    return 0;
}
```

**编译命令**：
```bash
g++ -std=c++20 pack_deduction.cpp -o pack_deduction
```

---

关键知识点总结

| 特性                | 核心优势                                                                 | 典型应用场景                     |
|---------------------|--------------------------------------------------------------------------|----------------------------------|
| **Concepts**        | 显式约束模板参数，增强编译错误信息                                       | 泛型算法、类型安全检查           |
| **Modules**         | 替代头文件，避免宏污染，提升编译速度                                     | 大型项目模块化管理               |
| **包展开推导**      | 简化变长模板参数处理，支持复杂类型构造                                   | 元编程、通用容器实现             |

---

扩展练习
尝试实现一个使用Concepts约束的`Comparable`概念，并编写一个通用的比较函数模板：

```cpp
#include <iostream>
#include <concepts>

template<typename T>
concept Comparable = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
};

template<Comparable T>
bool safe_compare(const T& a, const T& b) {
    return a == b;
}

int main() {
    std::cout << std::boolalpha;
    std::cout << safe_compare(10, 20) << std::endl;       // 输出false
    std::cout << safe_compare(std::string("Hello"), std::string("World")) << std::endl;  // 输出false
    
    // std::vector<int>无法比较，编译报错
    // safe_compare(std::vector<int>{1}, std::vector<int>{2});
    
    return 0;
}
```
---

多选题

题目1：关于C++20 `concept`的描述正确的是？
A. 必须在模板参数列表中使用`requires`子句  
B. 可以定义默认约束条件  
C. 支持逻辑运算符组合约束  
D. 能够自动推导约束条件  

---

题目2：`constexpr if`与普通`if`的关键区别是？
A. 编译期求值条件  
B. 支持非布尔类型条件  
C. 分支代码生成策略不同  
D. 可以出现在类外定义  

**答案：A、C**

**解析：**
- A正确：条件在编译期求值
- C正确：不满足条件的分支会被丢弃（SFINAE）
- B错误：必须为布尔常量表达式
- D错误：仅限函数作用域

---

题目3：泛型lambda的捕获列表中可以出现？
A. `auto`类型占位符  
B. 静态成员变量  
C. 非静态成员函数指针  
D. 模板参数包  

**答案：B、C**

**解析：**
- B正确：可以捕获静态成员（如`[=] { return MyClass::value; }`）
- C正确：可以捕获成员函数指针（如`[&f] { return obj.*f(); }`）
- A错误：捕获列表不支持`auto`
- D错误：模板参数包无法直接捕获

---

题目4：关于折叠表达式的错误说法是？
A. 支持所有二元操作符  
B. 可以展开参数包到逗号表达式  
C. 空包展开有明确定义  
D. 左右折叠优先级相同  

**答案：A、D**

**解析：**
- A错误：不支持`||`, `&&`, `,`以外的操作符需要括号包裹
- D错误：左折叠`(args + ...)`和右折叠`(... + args)`优先级不同
- B正确：可以展开到逗号表达式（如`([](auto x){}, ...)`）
- C正确：空包左折叠为`identity`，右折叠为`identity`

---

题目5：C++23中`std::type_identity_t`的主要用途是？
A. 禁止类型推导  
B. 强制类型转换  
C. 消除ADL影响  
D. 延迟模板实例化  

**答案：A、C**

**解析：**
- A正确：`std::type_identity_t<T>`阻止`T`被推导（如`decltype(auto) = std::type_identity_t<int>{}`）
- C正确：消除ADL查找（如`std::type_identity_t<decltype(f)>::type`）
- B错误：不改变类型本质
- D错误：不影响实例化时机

---

题目6：关于模块化的正确说法是？
A. 模块接口单元必须使用`export`关键字  
B. 模块实现单元可以包含`#include`指令  
C. 模块可以导出宏定义  
D. 模块编译单元之间自动处理依赖关系  

**答案：A、D**

**解析：**
- A正确：模块接口需显式导出实体（如`export module M; export int f();`）
- D正确：模块编译器自动管理依赖传递
- B错误：实现单元不应包含`#include`
- C错误：宏无法跨模块可见

---

题目7：C++20 `requires`子句可以用于？
A. 函数模板约束  
B. 类模板约束  
C. 变量模板约束  
D. 构造函数约束  

**答案：A、B、C、D**

**解析：**
- 所有选项均正确：`requires`可用于任何模板声明的约束（如`template<typename T> requires C<T> class C {};`）

---

题目8：关于`[[nodiscard]]`属性的模板化用法正确的是？
A. 可以作用于类模板  
B. 可以作用于函数模板  
C. 可以作用于变量模板  
D. 无法作用于成员模板  

**答案：A、B、C**

**解析：**
- A正确：`[[nodiscard]] struct S {};`
- B正确：`[[nodiscard]] template<typename T> T f();`
- C正确：`[[nodiscard]] template<typename T> T x;`
- D错误：成员模板同样可以标记（如`class C { [[nodiscard]] T f(); };`）

---

题目9：C++23中`std::is_constant_evaluated()`的典型应用场景是？
A. 禁止编译期计算  
B. 优化运行时分支  
C. 实现编译期调试输出  
D. 检测异常安全等级  

**答案：B、C**

**解析：**
- B正确：避免编译期不必要的开销（如`if (!std::is_constant_evaluated()) { /* runtime code */ }`）
- C正确：在编译期执行日志输出（需配合`if constexpr`）
- A错误：无法禁止编译期计算
- D错误：与异常安全无关

---

题目10：关于`std::format`与模板的结合错误说法是？
A. 可以格式化任意类型（需自定义`formatter`）  
B. 支持编译期字符串检查  
C. 自动推导格式说明符  
D. 比`printf`更易出错  

**答案：C、D**

**解析：**
- C错误：需显式指定格式说明符（如`std::format("{:.2f}", 3.1415)`）
- D错误：类型安全优于`printf`
- A正确：通过ADL注册自定义格式化器
- B正确：`constexpr`上下文支持编译期检查

---

### 设计题

题目1：实现一个支持编译期字符串拼接的`concat`模板
```cpp
#include <iostream>
#include <string_view>

template<std::size_t N, std::size_t M>
constexpr auto concat(const char(&a)[N], const char(&b)[M]) {
    std::array<char, N + M - 1> result{};
    std::copy_n(a, N-1, result.begin());
    std::copy_n(b, M, result.begin() + N - 1);
    return result;
}

int main() {
    constexpr auto str = concat("Hello", " World!");
    static_assert(str == "Hello World!");
    std::cout << str.data() << std::endl;
}
```

题目2：使用`constexpr if`优化斐波那契数列计算
```cpp
#include <iostream>

template<int N>
constexpr int fib() {
    if constexpr (N <= 1) return N;
    return fib<N-1>() + fib<N-2>();
}

int main() {
    static_assert(fib<10>() == 55);
    std::cout << "Fibonacci(10): " << fib<10>() << std::endl;
}
```

题目3：实现类型安全的变长参数包装器
```cpp
#include <tuple>
#include <iostream>

template<typename... Args>
void log(Args&&... args) {
    ((std::cout << std::forward<Args>(args) << " "), ...);
    std::cout << std::endl;
}

int main() {
    log("Hello", 42, 3.14, std::string("World"));
}
```

题目4：基于`std::variant`的类型安全访问器
```cpp
#include <variant>
#include <iostream>
#include <string>

template<typename... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

int main() {
    std::variant<int, std::string> v = "Hello";
    
    std::visit(overloaded{
        [](int i) { std::cout << "int: "<< i << std::endl; },
        [](const std::string& s) { std::cout << "string: "<< s << std::endl; }
    }, v);
}
```

题目5：编译期素数检测
```cpp
#include <iostream>
#include <bit>

template<unsigned N, unsigned D>
struct is_prime_helper {
    static constexpr bool value = (N % D != 0) && is_prime_helper<N, D-1>::value;
};

template<unsigned N>
struct is_prime_helper<N, 1> {
    static constexpr bool value = true;
};

template<unsigned N>
constexpr bool is_prime() {
    if constexpr (N < 2) return false;
    return is_prime_helper<N, N/2>::value;
}

int main() {
    static_assert(is_prime<7>());
    static_assert(!is_prime<9>());
    std::cout << "Is 13 prime? " << std::boolalpha << is_prime<13>() << std::endl;
}
```

---

#### 多选题答案
1. **答案：B、C**

**解析：**
- B正确：`concept`可以定义默认约束（如`template<typename T> concept C = ...`）
- C正确：支持`&&`, `||`, `!`组合约束（如`C1 && !C2`)
- A错误：C++20允许隐式约束（无需`requires`）
- D错误：约束需显式声明

2. **答案：A、C**  
   - `constexpr if`在编译期求值条件并丢弃不满足的分支

3. **答案：B、C**  
   - 泛型lambda可捕获静态成员和成员函数指针

4. **答案：A、D**  
   - 折叠表达式不支持所有操作符且左右折叠优先级不同

5. **答案：A、C**  
   - `std::type_identity_t`阻止类型推导并消除ADL影响

6. **答案：A、D**  
   - 模块需显式导出实体且自动处理依赖关系

7. **答案：A、B、C、D**  
   - `requires`可约束所有模板类型

8. **答案：A、B、C**  
   - `[[nodiscard]]`可修饰类/函数/变量模板

9. **答案：B、C**  
   - `std::is_constant_evaluated()`优化运行时分支和调试输出

10. **答案：C、D**  
    - `std::format`需显式指定格式且比`printf`更安全

#### 设计题详解
1. **编译期字符串拼接**  
   - 使用`std::array`存储结果，递归拼接字符数组

2. **斐波那契数列优化**  
   - `constexpr if`终止递归条件，避免无限展开

3. **变长参数包装器**  
   - 折叠表达式展开参数包，完美转发所有参数

4. **类型安全访问器**  
   - `std::visit`结合`overloaded`实现多态访问逻辑

5. **编译期素数检测**  
   - 递归模板实现试除法，`std::bit`辅助优化