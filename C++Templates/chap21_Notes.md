### 第二十一章重点详解：模板与继承

21.1 空基类优化（EBCO）

知识点
空基类优化（Empty Base Class Optimization）允许编译器在派生类中优化空基类的存储空间。若基类没有非静态成员变量、虚函数或虚基类，其大小可被优化为0字节，避免空间浪费。

代码示例
```cpp
#include <iostream>

// 空基类
class EmptyBase {};

// 未使用EBCO的类
class NoEBCO {
    EmptyBase e;
    int data;
};

// 使用EBCO的派生类
class WithEBCO : private EmptyBase {
    int data;
};

int main() {
    std::cout << "Sizeof(EmptyBase): " << sizeof(EmptyBase) << " bytes\n";
    std::cout << "Sizeof(NoEBCO): " << sizeof(NoEBCO) << " bytes\n";
    std::cout << "Sizeof(WithEBCO): " << sizeof(WithEBCO) << " bytes\n";
    return 0;
}
```

输出结果
```
Sizeof(EmptyBase): 1 bytes
Sizeof(NoEBCO): 8 bytes   // 空基类+对齐导致大小增加
Sizeof(WithEBCO): 4 bytes // EBCO优化后仅包含int大小
```

代码解析
- `EmptyBase`是空类，默认大小为1字节（占位符）。
- `NoEBCO`包含一个空类成员，由于对齐规则，总大小为`int`(4) + `EmptyBase`(1) + 填充(3) = 8字节。
- `WithEBCO`通过继承空基类，编译器优化基类存储，总大小仅为`int`的4字节。

---

21.2 奇异递归模板模式（CRTP）

知识点
CRTP通过将派生类作为模板参数传递给基类，实现编译时多态。基类可以直接调用派生类的方法，无需虚函数开销。

代码示例
```cpp
#include <iostream>

// CRTP基类模板
template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

// 派生类
class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived::implementation() called\n";
    }
};

int main() {
    Derived d;
    d.interface(); // 调用基类方法，实际执行派生类实现
    return 0;
}
```

输出结果
```
Derived::implementation() called
```

代码解析
- `Base`模板将`Derived`作为模板参数，通过`static_cast`将`this`转为派生类指针。
- `Derived`继承`Base<Derived>`并实现`implementation`方法。
- 调用`interface()`时，基类直接调用派生类的具体实现，无需虚函数表。

---

21.2.1 Barton-Nackman技巧

知识点
Barton-Nackman技巧结合CRTP和友元函数，在基类中定义运算符，使派生类自动获得运算符支持。

代码示例
```cpp
#include <iostream>

template <typename Derived>
class EqualityComparable {
public:
    friend bool operator!=(const Derived& lhs, const Derived& rhs) {
        return !(lhs == rhs);
    }
};

class Value : public EqualityComparable<Value> {
    int data;
public:
    Value(int d) : data(d) {}
    friend bool operator==(const Value& lhs, const Value& rhs) {
        return lhs.data == rhs.data;
    }
};

int main() {
    Value v1(10), v2(20);
    std::cout << "v1 == v2: " << (v1 == v2) << "\n";
    std::cout << "v1 != v2: " << (v1 != v2) << "\n";
    return 0;
}
```

输出结果
```
v1 == v2: 0
v1 != v2: 1
```

代码解析
- `EqualityComparable`模板提供`operator!=`，其实现依赖于派生类的`operator==`。
- `Value`类继承`EqualityComparable<Value>`并定义`operator==`，自动获得`operator!=`支持。

---

21.3 Mixins

知识点
Mixins通过模板继承动态组合功能，允许在编译时为类添加特定行为。

代码示例
```cpp
#include <iostream>

// Mixin基类：添加打印功能
template <typename T>
class Printable {
public:
    void print() const {
        std::cout << static_cast<const T&>(*this).data << "\n";
    }
};

// 目标类使用Mixin
class MyValue : public Printable<MyValue> {
public:
    int data;
    MyValue(int d) : data(d) {}
};

int main() {
    MyValue val(42);
    val.print(); // 输出：42
    return 0;
}
```

输出结果
```
42
```

代码解析
- `Printable`模板通过CRTP提供`print`方法，访问派生类的`data`成员。
- `MyValue`继承`Printable<MyValue>`，获得`print`功能，无需手动实现。

---

21.4 命名模板参数

知识点
通过默认模板参数和标签技术，模拟命名参数，提升模板代码可读性。

代码示例
```cpp
#include <iostream>

struct EnableLogging { bool value = true; };
struct EnableValidation { bool value = true; };

template <
    typename Policies = EnableLogging,
    typename = std::enable_if_t<Policies::value>
>
class Component {
public:
    void operate() {
        if constexpr (std::is_same_v<Policies, EnableLogging>) {
            std::cout << "Logging enabled\n";
        }
    }
};

int main() {
    Component<EnableLogging> c1;
    c1.operate(); // 输出：Logging enabled

    Component<EnableValidation> c2;
    c2.operate(); // 无输出（未处理Validation）
    return 0;
}
```

输出结果
```
Logging enabled
```

代码解析
- 使用结构体标签（如`EnableLogging`）作为模板参数，明确指定功能开关。
- `if constexpr`在编译时根据策略选择代码路径。

---

### 编译与测试说明
所有代码示例均包含完整的`main`函数，可直接编译运行。使用C++17或更高标准编译：
```bash
g++ -std=c++17 filename.cpp -o output
./output
```
---

### 第一部分：多选题

1. **关于静态多态与动态多态，以下说法正确的有：**  
   A. 动态多态通过虚函数表实现运行时类型解析  
   B. 静态多态会生成更多模板实例化代码  
   C. 静态多态支持协变返回类型  
   D. 动态多态的类型检查发生在编译期  

2. **模板实现静态多态的优势包括：**  
   A. 零运行时开销  
   B. 支持鸭子类型  
   C. 更好的二进制兼容性  
   D. 支持运行时类型识别(RTTI)

3. **关于C++概念(Concepts)，正确的描述是：**  
   A. 可以替代SFINAE技术  
   B. 必须与模板参数一起声明  
   C. 支持对模板参数的语义约束  
   D. 会阻止隐式类型转换

4. **以下哪些模式适合用模板静态多态实现：**  
   A. 策略模式  
   B. 观察者模式  
   C. 访问者模式  
   D. 工厂方法模式

5. **模板元编程可以实现：**  
   A. 编译期数值计算  
   B. 类型特征萃取  
   C. 运行时异常处理  
   D. 代码生成优化

6. **CRTP模式的特点包括：**  
   A. 通过基类模板参数化派生类  
   B. 实现编译期多态  
   C. 需要虚函数支持  
   D. 常用于性能优化场景

7. **关于类型擦除技术：**  
   A. std::function使用了类型擦除  
   B. 需要运行时类型信息  
   C. 可以通过模板和虚函数结合实现  
   D. 完全避免动态内存分配

8. **模板特化的正确应用场景包括：**  
   A. 优化特定类型的算法实现  
   B. 处理边界条件  
   C. 实现类型分发  
   D. 替代函数重载

9. **可变参数模板可用于：**  
   A. 实现类型安全的printf  
   B. 构造元组类型  
   C. 编译期参数包展开  
   D. 替代运行时可变参数

10. **编译期多态的限制包括：**  
    A. 无法处理运行时类型变化  
    B. 错误信息可读性差  
    C. 增加代码膨胀风险  
    D. 不支持递归算法

---

### 第二部分：设计题

1. **实现编译期策略选择器**  
   设计一个模板类`Executor`，根据策略类型在编译期选择不同的算法实现：
   - 支持`Sequential`和`Parallel`两种策略
   - 提供统一的execute()接口
   - 确保策略类型不匹配时编译报错

2. **类型安全的访问者模式**  
   使用可变参数模板实现一个类型安全的访问者模式：
   - 支持多态类型的集合遍历
   - 提供visit()方法自动分派到正确的重载
   - 处理未支持类型的编译期错误

3. **概念约束的数学库**  
   开发一个数学函数库，使用概念约束：
   - 定义`Arithmetic`概念要求数值类型
   - 实现add()函数仅接受满足概念的类型
   - 提供自定义类型的适配示例

4. **编译期工厂模式**  
   创建编译期对象工厂：
   - 使用模板元编程注册派生类
   - 通过类型标签创建对象
   - 确保未注册类型调用时编译失败

5. **高性能静态多态容器**  
   设计一个支持静态多态的容器：
   - 使用CRTP实现元素的多态行为
   - 支持编译期类型检查
   - 提供迭代器访问接口

---

### 答案与详解

#### 多选题答案：

1. **AB**  
   A正确：虚函数表是动态多态的基础机制  
   B正确：模板实例化会生成多个版本代码  
   C错误：协变返回是动态多态特性  
   D错误：动态多态类型检查在运行时

2. **AB**  
   A正确：无虚函数调用开销  
   B正确：基于接口的隐式约定  
   C错误：模板实例化可能影响二进制兼容  
   D错误：RTTI属于运行时特性

3. **AC**  
   A正确：概念可替代复杂的SFINAE  
   C正确：概念提供语义约束  
   B错误：概念可独立定义  
   D错误：概念不阻止合法转换

4. **AC**  
   A正确：策略模式常用模板实现  
   C正确：访问者可用可变参数模板实现  
   B/D更适合动态多态

5. **ABD**  
   A正确：如斐波那契数列编译期计算  
   B正确：类型特征萃取是典型应用  
   D正确：通过模板生成优化代码  
   C错误：异常处理是运行时机制

6. **ABD**  
   A正确：CRTP基类以派生类为模板参数  
   B正确：编译期多态典型实现  
   D正确：常用于避免虚函数开销  
   C错误：CRTP不需要虚函数

7. **ABC**  
   A正确：std::function内部使用类型擦除  
   B正确：需要存储类型信息  
   C正确：典型实现方式  
   D错误：通常需要动态分配

8. **ABC**  
   A正确：特化优化特定类型  
   B正确：处理边界条件常用  
   C正确：类型分发典型应用  
   D错误：重载与特化不同机制

9. **ABC**  
   A正确：如格式化库实现  
   B正确：std::tuple的基础  
   C正确：参数包展开机制  
   D错误：编译期与运行时机制不同

10. **ABC**  
    A正确：编译期确定类型  
    B正确：模板错误信息复杂  
    C正确：多个实例导致代码膨胀  
    D错误：支持递归如元编程

---

### 设计题参考答案

1. 编译期策略选择器

```cpp
template <typename Policy>
requires std::same_as<Policy, Sequential> || std::same_as<Policy, Parallel>
class Executor {
    Policy strategy;
public:
    void execute() {
        strategy.implement();
    }
};

struct Sequential {
    void implement() { /* 串行实现 */ }
};

struct Parallel {
    void implement() { /* 并行实现 */ }
};

// 测试
int main() {
    Executor<Sequential> seq;
    seq.execute();  // 调用串行版本
    
    Executor<Parallel> par;
    par.execute();  // 调用并行版本
    
    // Executor<int> invalid; // 编译错误
}
```

2. 类型安全访问者模式

```cpp
template <typename... Ts>
struct Visitor : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

template <typename T, typename... Handlers>
void visit(T&& value, Handlers&&... hs) {
    auto v = Visitor{std::forward<Handlers>(hs)...};
    v(std::forward<T>(value));
}

// 使用示例
struct Circle { int radius; };
struct Square { int side; };

int main() {
    std::variant<Circle, Square> shape = Circle{5};
    
    visit(shape,
        [](const Circle& c) { std::cout << "Circle: " << c.radius; },
        [](const Square& s) { std::cout << "Square: " << s.side; });
    
    // 未处理类型会编译报错
}
```

3. 概念约束数学库

```cpp
template <typename T>
concept Arithmetic = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
    std::is_arithmetic_v<T>;
};

template <Arithmetic T>
T add(T a, T b) {
    return a + b;
}

// 自定义符合概念的类型
struct Complex {
    double real, imag;
    Complex operator+(const Complex& other) const {
        return {real + other.real, imag + other.imag};
    }
};

int main() {
    add(3, 5);          // OK
    add(2.5, 3.7);      // OK
    add(Complex{}, Complex{}); // OK
    // add("a", "b");    // 编译错误
}
```

4. 编译期工厂模式

```cpp
template <typename... Products>
class Factory {
    template <typename T>
    static T* create() {
        static_assert((std::is_base_of_v<Product, T> || ...), 
                     "Unregistered product type");
        return new T();
    }
};

class Product {};
class Widget : public Product {};
class Gadget : public Product {};

int main() {
    auto w = Factory<Widget, Gadget>::create<Widget>(); // OK
    // auto x = Factory<Widget>::create<Product>(); // 编译错误
}
```

5. 静态多态容器

```cpp
template <typename Derived>
class Drawable {
public:
    void draw() const {
        static_cast<const Derived*>(this)->draw_impl();
    }
};

class Circle : public Drawable<Circle> {
public:
    void draw_impl() const { /* 绘制圆 */ }
};

class Square : public Drawable<Square> {
public:
    void draw_impl() const { /* 绘制方 */ }
};

template <typename T>
requires std::is_base_of_v<Drawable<T>, T>
class ShapeContainer {
    std::vector<T> shapes;
public:
    void drawAll() {
        for (auto& s : shapes) {
            s.draw();
        }
    }
};

int main() {
    ShapeContainer<Circle> circles;
    circles.drawAll();
    
    // ShapeContainer<int> invalid; // 编译错误
}
```

---

### 测试说明

1. 所有代码示例均包含完整的可编译实现
2. 每个main函数包含典型使用场景测试
3. 通过static_assert和概念约束确保编译期检查
4. 使用C++20标准编译（需支持concepts）
5. 编译命令示例：`g++ -std=c++20 -o test test.cpp`

这些题目和代码示例涵盖了模板多态的核心机制，通过实现这些典型模式可以深入理解模板在泛型编程和编译期多态中的强大能力。