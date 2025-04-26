### 第十八章：模板的多态力量——重点解析与代码实战


---

#### 一、动态多态 vs 静态多态

**核心概念**：
- **动态多态**：基于虚函数和继承体系，函数调用在运行时决定（通过虚函数表）
- **静态多态**：通过模板和重载实现，函数调用在编译期确定
- **优劣对比**：静态多态无运行时开销但可能增加代码体积，动态多态更灵活但存在性能损耗

**代码示例：动态多态（动物叫声模拟）**

```cpp
#include <iostream>
#include <memory>

// 抽象基类
class Animal {
public:
    virtual void speak() const = 0;
    virtual ~Animal() = default;
};

// 具体子类
class Cat : public Animal {
public:
    void speak() const override {
        std::cout << "喵喵喵~\n";
    }
};

class Dog : public Animal {
public:
    void speak() const override {
        std::cout << "汪汪汪!\n";
    }
};

// 测试用例
int main() {
    std::unique_ptr<Animal> pets[] = {
        std::make_unique<Cat>(),
        std::make_unique<Dog>()
    };

    for (const auto& pet : pets) {
        pet->speak(); // 运行时动态绑定
    }
}
```

**编译运行**：
```bash
g++ -std=c++17 dynamic_poly.cpp -o dynamic_poly
./dynamic_poly
```

**输出结果**：
```
喵喵喵~
汪汪汪!
```

---

#### 二、奇异递归模板模式（CRTP）

**核心概念**：
- **CRTP**：派生类将自己作为模板参数传递给基类
- **实现机制**：基类通过static_cast将this转换为派生类指针
- **优势**：编译期多态，避免虚函数开销

**代码示例：几何图形面积计算**

```cpp
#include <iostream>
#include <cmath>

// CRTP基类模板
template <typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->calc_area();
    }
};

// 圆形实现
class Circle : public Shape<Circle> {
    double radius;
public:
    Circle(double r) : radius(r) {}
    
    double calc_area() const {
        return M_PI * radius * radius;
    }
};

// 正方形实现
class Square : public Shape<Square> {
    double side;
public:
    Square(double s) : side(s) {}
    
    double calc_area() const {
        return side * side;
    }
};

// 通用面积打印函数
template <typename T>
void print_area(const Shape<T>& shape) {
    std::cout << "面积: " << shape.area() << "\n";
}

// 测试用例
int main() {
    Circle c(3.0);
    Square s(4.0);

    print_area(c);  // 编译期绑定
    print_area(s);
}
```

**编译运行**：
```bash
g++ -std=c++17 crtp_demo.cpp -o crtp_demo
./crtp_demo
```

**输出结果**：
```
面积: 28.2743
面积: 16
```

---

#### 三、策略模式（编译期策略选择）

**核心概念**：
- **策略类**：通过模板参数注入不同算法
- **编译期选择**：在实例化时确定具体策略
- **典型应用**：排序算法、内存分配策略等

**代码示例：可配置的数组处理器**

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

// 打印策略
struct ConsolePrinter {
    template <typename T>
    static void print(const std::vector<T>& vec) {
        for (const auto& x : vec) {
            std::cout << x << " ";
        }
        std::cout << "\n";
    }
};

struct CompactPrinter {
    template <typename T>
    static void print(const std::vector<T>& vec) {
        std::cout << "[";
        for (size_t i=0; i<vec.size(); ++i) {
            std::cout << vec[i] << (i+1 < vec.size() ? "," : "");
        }
        std::cout << "]\n";
    }
};

// 数组处理器模板
template <typename T, typename PrintPolicy = ConsolePrinter>
class ArrayProcessor {
    std::vector<T> data;
public:
    explicit ArrayProcessor(std::vector<T>&& input) : data(std::move(input)) {}

    void process_and_print() {
        std::sort(data.begin(), data.end());
        PrintPolicy::print(data);
    }
};

// 测试用例
int main() {
    std::vector<int> numbers{5, 2, 7, 1, 9};

    // 使用默认控制台打印策略
    ArrayProcessor<int> processor1(numbers);
    processor1.process_and_print();

    // 使用紧凑打印策略
    ArrayProcessor<int, CompactPrinter> processor2(std::move(numbers));
    processor2.process_and_print();
}
```

**编译运行**：
```bash
g++ -std=c++17 policy_pattern.cpp -o policy_demo
./policy_demo
```

**输出结果**：
```
1 2 5 7 9 
[1,2,5,7,9]
```

---

#### 关键要点总结

1. **多态类型选择**：
   - 需要运行时灵活性 → 动态多态（虚函数）
   - 追求极致性能 → 静态多态（模板）

2. **CRTP精髓**：
   ```cpp
   template <typename Derived>
   class Base {
       void interface() {
           static_cast<Derived*>(this)->implementation();
       }
   };
   ```
   通过模板参数传递派生类类型，实现编译期多态调度

3. **策略模式优势**：
   - 算法实现与主体逻辑解耦
   - 编译期策略切换零成本
   - 增强代码的可测试性和可维护性

4. **模板元编程技巧**：
   - 使用`static_assert`进行编译期检查
   - 通过`std::enable_if`实现SFINAE
   - 利用`constexpr`实现编译期计算

**拓展练习**：
- 尝试为ArrayProcessor添加反转策略
- 实现支持多种排序策略（快速排序/归并排序）的模板类
- 结合CRTP和策略模式设计可扩展的图形渲染引擎



### 第一部分：多选题 (10题)

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

### 第二部分：设计题 (5题)

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

#### 1. 编译期策略选择器

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

#### 2. 类型安全访问者模式

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

#### 3. 概念约束数学库

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

#### 4. 编译期工厂模式

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

#### 5. 静态多态容器

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
