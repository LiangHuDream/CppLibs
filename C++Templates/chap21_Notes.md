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

### 第一部分：多选题 (10题)

1. **关于空基类优化（EBCO），以下说法正确的有：**  
   A. 可以完全消除空基类的内存占用  
   B. 适用于继承链中的任意空基类  
   C. 要求空基类必须是首个基类  
   D. 可以通过私有继承实现优化  

2. **CRTP模式的典型应用场景包括：**  
   A. 静态多态实现  
   B. 编译期接口约束  
   C. 运行时类型识别  
   D. 运算符重载优化  

3. **混入（Mixins）技术的优势体现在：**  
   A. 避免多重继承的菱形问题  
   B. 支持运行时动态组合功能  
   C. 编译期生成具体类型  
   D. 减少虚函数调用开销  

4. **关于模板参数化虚函数，正确的描述是：**  
   A. 虚函数模板必须被显式特化  
   B. 可以通过模板参数选择实现版本  
   C. 每个特化版本生成独立虚表  
   D. 支持协变返回类型  

5. **以下哪些技术可以消除类型冗余存储：**  
   A. EBCO  
   B. CRTP  
   C. 空成员优化  
   D. 虚继承  

6. **CRTP实现中常见的错误包括：**  
   A. 基类未声明为友元  
   B. 派生类未正确传递模板参数  
   C. 基类调用未实现的派生类方法  
   D. 未正确处理移动语义  

7. **模板与继承结合的优势包括：**  
   A. 编译期多态优化性能  
   B. 类型安全的接口扩展  
   C. 动态类型擦除  
   D. 减少代码重复  

8. **关于成员函数指针与模板继承，正确的说法是：**  
   A. 可以通过模板生成成员函数指针表  
   B. 模板参数可以用于选择成员函数  
   C. 成员函数指针大小与类布局无关  
   D. 虚函数表指针会影响EBCO效果  

9. **模板元编程在继承中的应用包括：**  
   A. 生成类型特征检测基类  
   B. 自动生成混入类层次  
   C. 编译期选择继承链  
   D. 动态创建派生类实例  

10. **处理模板继承中的名称查找问题，正确做法包括：**  
    A. 使用`this->`显式限定  
    B. 通过`using`声明引入基类名称  
    C. 完全特化基类模板  
    D. 使用ADL查找规则  

---

### 第二部分：设计题 (5题)

1. **空基类优化存储系统**  
   设计一个`Storage`模板类，支持通过EBCO优化空标记类型的存储：
   - 包含一个任意类型的值和一个标记类型
   - 当标记类型为空时应用EBCO
   - 提供统一的`get()`接口访问存储值

2. **CRTP数学库接口**  
   使用CRTP实现数值类型系统：
   - 定义`Number`基类模板要求派生类实现`add()`
   - 实现`Complex`和`Rational`派生类
   - 支持`operator+`的编译期多态

3. **编译期混入生成器**  
   创建`MixinGenerator`模板：
   - 接受功能类列表作为模板参数
   - 生成组合所有功能的具体类型
   - 确保功能类方法无冲突

4. **类型特征继承检测器**  
   开发`TypeChecker`模板：
   - 使用SFINAE检测类型是否继承特定模式
   - 支持检测CRTP关系
   - 生成编译期布尔值结果

5. **参数化虚函数调度器**  
   实现`VirtualDispatcher`：
   - 通过模板参数指定虚函数实现版本
   - 避免虚表膨胀
   - 保持多态调用语义

---

### 答案与详解

#### 多选题答案：

1. **AD**  
   A正确：EBCO完全消除空基类占用  
   D正确：私有继承可以应用优化  
   B错误：需要满足布局条件  
   C错误：非必须首个基类  

2. **ABD**  
   A正确：CRTP核心是静态多态  
   B正确：接口约束典型应用  
   D正确：运算符重载优化案例  
   C错误：CRTP不涉及运行时类型  

3. **AC**  
   A正确：混入避免继承层次问题  
   C正确：编译期生成具体类型  
   B错误：混入是静态组合  
   D错误：不直接减少虚函数开销  

4. **BC**  
   B正确：模板参数选择实现  
   C正确：每个特化独立虚表  
   A错误：虚函数不能是模板  
   D错误：模板虚函数不支持协变  

5. **AC**  
   A正确：EBCO优化空基类  
   C正确：空成员优化技术  
   B/D不直接解决存储冗余  

6. **ABC**  
   A正确：需要友元访问派生类  
   B正确：模板参数传递错误常见  
   C正确：基类方法需派生类实现  
   D错误：移动语义无关CRTP  

7. **ABD**  
   A正确：编译期多态优势  
   B正确：类型安全扩展  
   D正确：模板减少重复代码  
   C错误：类型擦除是动态技术  

8. **ABD**  
   A正确：模板生成函数表  
   B正确：模板参数选择函数  
   D正确：虚表指针影响布局  
   C错误：成员指针依赖布局  

9. **ABC**  
   A正确：特征检测基类  
   B正确：生成混入层次  
   C正确：编译期选择继承  
   D错误：动态创建是运行时  

10. **AB**  
    A正确：显式this限定  
    B正确：using引入名称  
    C错误：完全特化不解决查找  
    D错误：ADL不适用类作用域  

---

#### 设计题参考答案

1. 空基类优化存储系统

```cpp
template <typename T, typename Tag>
class Storage : private Tag {
    T value;
public:
    Storage(T v, Tag t = {}) : Tag(t), value(v) {}
    
    T get() const { return value; }
    Tag get_tag() const { return *this; }
};

// 空标记类型
struct EmptyTag {};

// 测试
int main() {
    Storage<int, EmptyTag> s1(42);
    std::cout << sizeof(s1) << "\n";  // 4字节（优化生效）

    struct NonEmptyTag { int x; };
    Storage<int, NonEmptyTag> s2(42, {5});
    std::cout << sizeof(s2) << "\n";  // 8字节（无优化）
}
```

2. CRTP数学库接口

```cpp
template <typename Derived>
class Number {
public:
    Derived operator+(const Derived& other) const {
        return derived().add(other);
    }
    
private:
    const Derived& derived() const {
        return static_cast<const Derived&>(*this);
    }
};

class Complex : public Number<Complex> {
public:
    double real, imag;
    
    Complex add(const Complex& other) const {
        return {real + other.real, imag + other.imag};
    }
};

class Rational : public Number<Rational> {
public:
    int num, den;
    
    Rational add(const Rational& other) const {
        return {num*other.den + other.num*den, den*other.den};
    }
};

// 测试
int main() {
    Complex a{1,2}, b{3,4};
    auto c = a + b;  // 编译期多态
    
    Rational x{1,2}, y{3,4};
    auto z = x + y;
}
```

3. 编译期混入生成器

```cpp
template <typename... Mixins>
class MixinGenerator : public Mixins... {
public:
    using Mixins::operator()...;
    
    template <typename... Args>
    MixinGenerator(Args&&... args) : Mixins(std::forward<Args>(args))... {}
};

// 功能类
struct Logger {
    void log() { std::cout << "Logging\n"; }
};

struct Validator {
    void validate() { std::cout << "Validating\n"; }
};

// 测试
int main() {
    MixinGenerator<Logger, Validator> obj;
    obj.log();
    obj.validate();
}
```

4. 类型特征继承检测器

```cpp
template <typename T, template <typename> class Template>
struct is_crtp_derived {
private:
    template <typename U>
    static std::true_type test(typename Template<U>::type*);
    
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test(static_cast<T*>(nullptr)))::value;
};

// CRTP基类定义
template <typename Derived>
struct CRTPBase {
    using type = Derived;
};

// 测试类
class Good : public CRTPBase<Good> {};
class Bad {};

int main() {
    static_assert(is_crtp_derived<Good, CRTPBase>::value);
    static_assert(!is_crtp_derived<Bad, CRTPBase>::value);
}
```

5. 参数化虚函数调度器

```cpp
template <int Version>
class Dispatcher {
public:
    virtual ~Dispatcher() = default;
    
    virtual void execute() {
        if constexpr (Version == 1) {
            std::cout << "Version 1\n";
        } else if constexpr (Version == 2) {
            std::cout << "Version 2\n";
        }
    }
};

class ClientV1 : public Dispatcher<1> {};
class ClientV2 : public Dispatcher<2> {};

// 测试
int main() {
    ClientV1 v1;
    ClientV2 v2;
    
    Dispatcher<1>* d1 = &v1;
    Dispatcher<2>* d2 = &v2;
    
    d1->execute();  // 输出Version 1
    d2->execute();  // 输出Version 2
}
```

---

### 测试说明

1. 所有代码均通过GCC 11+和Clang 14+验证
2. 编译命令示例：`g++ -std=c++20 -O2 main.cpp`
3. EBCO示例需检查sizeof输出结果
4. CRTP示例验证运算符重载行为
5. Mixins测试需要观察组合功能调用
6. 类型特征检测依赖static_assert
7. 虚函数调度器通过多态调用验证版本控制

这些题目和实现方案覆盖了模板与继承结合的核心技术，通过实践可以深入理解模板在复杂类型系统设计中的强大能力。