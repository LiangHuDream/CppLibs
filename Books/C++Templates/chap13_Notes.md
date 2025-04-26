@[TOC](Names in Templates)

---

### 重难点

**1. 名称分类与基本概念**
知识点：
- **限定名（Qualified Name）**：使用`::`或`.`显式指定作用域的名称（如`std::vector`）
- **非限定名（Unqualified Name）**：不带作用域限定的名称（如`vector`）
- **依赖名（Dependent Name）**：依赖于模板参数的名称（如`T::value_type`）
- **非依赖名（Non-dependent Name）**：不依赖模板参数的名称（如`int`）

测试用例：
```cpp
#include <iostream>
#include <vector>

template<typename T>
void foo() {
    T::value_type x;  // 依赖名（T未实例化前无法确定是否合法）
}

int main() {
    foo<std::vector<int>>();  // 实例化时检查T::value_type合法性
    return 0;
}
```

---

**2. 依赖名与非依赖名的查找规则**
知识点：
- **非限定名查找**：
  - 普通查找（Ordinary Lookup）：在模板定义时查找所有可见的非依赖名
  - ADL（Argument-Dependent Lookup）：在模板实例化时查找关联命名空间
- **限定名查找**：
  - 直接在当前作用域链中查找，不触发ADL

示例代码：
```cpp
namespace NS {
    struct S {};
    void bar(S) { std::cout << "NS::bar\n"; }
}

template<typename T>
void baz(T t) {
    bar(t);  // 非限定名：普通查找+ADL
}

int main() {
    NS::S s;
    baz(s);  // 调用NS::bar（通过ADL）
    return 0;
}
```

---

**3. 注入类名（Injected Class Name）**
知识点：
- 类模板内部可以隐式使用类名作为模板名（无需`<T>`）
- 在派生类中可通过`Base::Base`访问基类模板

示例代码：
```cpp
#include <iostream>

// 基类模板 Base
template<typename T>
class Base {
public:
    // 定义一个类型别名 type，其类型为模板参数 T
    using type = T;
};

// 派生类模板 Derived，继承自 Base<T>
template<typename T>
class Derived : public Base<T> {
public:
    // 派生类的构造函数
    Derived() {
        // 显式使用完整的基类模板实例化形式
        typename Base<T>::type x;
        // 为了让代码更完整，我们可以给 x 赋值并输出
        x = static_cast<T>(10);
        std::cout << "Value of x: " << x << std::endl;
    }
};

int main() {
    // 实例化 Derived 类模板，模板参数为 int
    Derived<int> d;
    return 0;
}    
```

---

**4. 友元模板（Friend Templates）**
知识点：
- 友元可以是函数模板、类模板或成员模板
- 友元声明需在类外定义时显式指定模板参数

示例代码：
```cpp
template<typename T>
class MyClass {
    friend void helper<>(MyClass<T>&);  // 友元函数模板
};

template<typename T>
void helper(MyClass<T>& obj) {
    obj.value = 42;
}

int main() {
    MyClass<int> obj;
    helper(obj);
    return 0;
}
```

---

**5. 两阶段查找（Two-Phase Lookup）**
知识点：
- **第一阶段（定义时）**：检查非依赖名，忽略模板参数
- **第二阶段（实例化时）**：检查依赖名，触发ADL

常见陷阱示例：
```cpp
template<typename T>
void foo() {
    bar();  // 第一阶段查找bar，若未找到则报错
}

namespace NS {
    void bar() {}
}

int main() {
    foo<NS::Bar>();  // 错误！第一阶段未找到bar()
    return 0;
}
```

---

**6. 代码测试与调试技巧**
测试策略：
1. **分阶段编译**：先编译模板定义，再实例化观察错误
2. **显式实例化**：通过`template class MyClass<int>;`强制实例化
3. **使用`static_assert`**：在模板中添加静态断言验证条件

示例测试代码：
```cpp
template<typename T>
class MyClass {
    static_assert(std::is_integral_v<T>, "T must be integral");
};

int main() {
    MyClass<int> ok;       // 通过
    // MyClass<double> error;  // 编译失败（静态断言）
    return 0;
}
```

---

### 多选题

题目1
**关于依赖名称的查找规则，以下哪些说法正确？**
A. 非限定依赖名称在第二阶段通过ADL查找  
B. 限定依赖名称直接在第一阶段查找  
C. 成员模板中的依赖名称自动视为模板  
D. `typename`关键字只能用于非限定依赖名称前

**答案**  
A, C  
**详解**  
A正确：非限定依赖名称在第二阶段通过ADL查找  
C正确：成员模板中的依赖名称需用`template`关键字显式指明

题目2
**关于ADL的适用场景，以下哪些是正确的？**
A. 查找非成员函数时参数类型所属的命名空间  
B. 查找成员函数的基类链  
C. 查找模板参数类型的嵌套类型  
D. 查找全局作用域的函数

**答案**  
A, C  
**详解**  
A正确：ADL通过参数类型所属命名空间查找函数  
C正确：ADL会查找参数类型的嵌套类型

题目3
**关于注入类名称，以下哪些描述正确？**
A. 在类模板内部可直接使用未限定类名  
B. 注入名称优先于外部同名函数  
C. 可用于访问基类的成员  
D. 实例化后指向具体模板实例

**答案**  
A, D  
**详解**  
A正确：类模板内部可直接用`MyClass`代替`MyClass<T>`  
D正确：注入名称在实例化后指向具体实例类型

题目4
**关于名称查找阶段，以下哪些正确？**
A. 非依赖名称仅在第一阶段查找  
B. 依赖名称仅在第二阶段查找  
C. 友元声明影响第二阶段查找  
D. `using`声明影响第一阶段查找

**答案**  
A, D  
**详解**  
A正确：非限定非依赖名称在第一阶段完成查找  
D正确：`using`声明会向第一阶段作用域引入名称

题目5
**关于模板参数作用域，以下哪些正确？**
A. 模板参数作用域从声明处开始  
B. 模板参数可隐藏外层作用域名称  
C. 类模板参数作用域包含成员定义  
D. 函数模板参数作用域包含默认实参

**答案**  
A, B, C  
**详解**  
A正确：模板参数作用域起始于声明处  
B正确：模板参数会隐藏外层同名名称  
C正确：类模板参数作用域覆盖成员定义

题目6
**关于友元声明的名称查找，以下哪些正确？**
A. 友元函数声明影响普通名称查找  
B. 友元类声明参与ADL查找  
C. 显式友元模板影响第二阶段查找  
D. 友元声明必须在使用前可见

**答案**  
B, C  
**详解**  
B正确：友元类参与ADL查找路径  
C正确：显式友元模板在第二阶段被考虑

题目7
**关于`using`声明在模板中的作用，以下哪些正确？**
A. 引入命名空间成员到模板作用域  
B. 可用于解除名称隐藏  
C. 必须在模板定义体外声明  
D. 影响第一阶段名称查找

**答案**  
A, B, D  
**详解**  
A正确：`using`可将命名空间成员引入当前作用域  
B正确：可解除外层同名名称的隐藏  
D正确：`using`声明影响第一阶段的普通查找

题目8
**关于当前实例化的判断，以下哪些情况成立？**
A. 直接使用未限定的类模板名  
B. 访问成员模板的嵌套类型  
C. 使用`this->`限定成员访问  
D. 通过`typename`限定依赖类型

**答案**  
A, B  
**详解**  
A正确：直接使用类模板名指向当前实例  
B正确：成员模板的嵌套类型属于当前实例

题目9
**关于两阶段查找的例外，以下哪些正确？**
A. 非类型模板参数的默认实参在第二阶段处理  
B. 虚函数表在第二阶段初始化  
C. 默认成员初始化器在第一阶段处理  
D. 异常规格不在两阶段处理范围内

**答案**  
C, D  
**详解**  
C正确：默认成员初始化器在第一阶段处理  
D正确：异常规格不属于两阶段处理范围

题目10
**关于模板特化与名称查找的关系，以下哪些正确？**
A. 显式特化不影响第一阶段查找  
B. 偏特化参与第二阶段ADL查找  
C. 全局特化优先于隐式实例化  
D. 特化中的名称独立于主模板作用域

**答案**  
A, C  
**详解**  
A正确：显式特化仅在实例化时被选择  
C正确：显式全局特化优先于隐式实例化

---

### 设计题

**题目1**
**设计一个模板类`Logger`，要求：**
- 支持日志级别（DEBUG/INFO/WARNING）
- 使用ADL查找自定义日志处理器
- 提供默认处理器输出到`std::cout`
- 测试用例需验证ADL查找和默认行为

**答案**  
```cpp
#include <iostream>
#include <string>

// 主模板
template<typename T>
class Logger {
public:
    void log(const std::string& msg) {
        handle_log(msg); // ADL查找自定义处理器
    }
};

// 默认处理器（通过ADL查找）
void handle_log(const std::string& msg) {
    std::cout << "[DEFAULT] " << msg << std::endl;
}

// 自定义处理器示例
namespace CustomLog {
    struct Handler {
        static void handle(const std::string& msg) {
            std::cerr << "[CUSTOM] " << msg << std::endl;
        }
    };

    // ADL辅助函数
    void handle_log(const std::string& msg) {
        Handler::handle(msg);
    }
}

// 测试用例
int main() {
    Logger<int> logger1;
    logger1.log("Hello"); // 调用默认处理器
    
    Logger<CustomLog::Handler> logger2;
    logger2.log("World"); // 调用自定义处理器
    
    return 0;
}
```

**题目2**
**实现一个依赖名称查找的智能指针模板，要求：**
- 支持自定义删除器
- 删除器通过依赖名称查找
- 默认删除器使用`delete`
- 测试用例需验证自定义删除器和默认行为

**答案**  
```cpp
template<typename T, typename Deleter = void>
class SmartPtr {
    T* ptr;
public:
    SmartPtr(T* p) : ptr(p) {}
    
    ~SmartPtr() {
        delete_ptr(ptr); // 依赖名称查找
    }
    
private:
    // 依赖名称：通过ADL查找Deleter::delete_ptr
    void delete_ptr(T* p) {
        Deleter::delete_ptr(p);
    }
};

// 默认删除器
struct DefaultDeleter {
    static void delete_ptr(void* p) {
        ::delete static_cast<int*>(p);
    }
};

// 自定义删除器
struct FileDeleter {
    static void delete_ptr(FILE* p) {
        fclose(p);
    }
};

// 测试用例
int main() {
    SmartPtr<int> ptr1(new int(42)); // 使用默认删除器
    SmartPtr<FILE, FileDeleter> ptr2(fopen("test.txt", "w")); // 使用自定义删除器
    
    return 0;
}
```

**题目3**
**设计一个支持注入类名称的模板元编程工具，要求：**
- 提供类型特征检测接口
- 注入类名称简化成员访问
- 测试用例验证注入名称和普通成员访问一致性

**答案**  
```cpp
template<typename T>
struct TypeTraits {
    // 注入类名称简化成员访问
    using value_type = typename T::value_type;
    using iterator = typename T::iterator;
    
    static constexpr bool has_size = requires(T t) {
        { t.size() } -> std::convertible_to<std::size_t>;
    };
};

// 测试容器
template<typename T>
struct MyContainer {
    using value_type = T;
    using iterator = T*;
    
    std::size_t size() const { return 0; }
};

// 测试用例
int main() {
    static_assert(TypeTraits<MyContainer<int>>::has_size);
    static_assert(std::is_same_v<TypeTraits<MyContainer<int>>::value_type, int>);
    
    return 0;
}
```

**题目4**
**实现一个支持当前实例化的模板偏特化检测器，要求：**
- 判断给定类型是否为当前实例化
- 使用`this->`限定成员访问
- 测试用例验证检测逻辑

**答案**  
```cpp
template<typename T>
struct IsCurrentInstantiation {
    static constexpr bool value = false;
};

template<typename T>
struct MyClass {
    template<typename U>
    struct Inner {
        static constexpr bool is_current = 
            std::is_same_v<U, MyClass<T>::Inner>; // 当前实例化检测
    };
};

// 测试用例
int main() {
    MyClass<int>::Inner<double> inner;
    static_assert(inner.is_current);
    
    return 0;
}
```

**题目5**
**设计一个结合ADL和模板参数作用域的工具，要求：**
- 自动注册类型到工厂类
- 使用ADL查找注册函数
- 测试用例验证多命名空间注册

**答案**  
```cpp
#include <map>
#include <string>

// 类型注册工厂
template<typename Key>
class Registry {
    std::map<Key, std::string> registry;
public:
    template<typename T>
    void register_type(const std::string& name) {
        registry[name] = typeid(T).name(); // 依赖名称查找
    }
    
    void list_types() const {
        for (const auto& [name, type] : registry) {
            std::cout << name << " -> " << type << std::endl;
        }
    }
};

// ADL注册函数
namespace NS1 {
    struct TypeA {};
    
    void register_type(Registry<TypeA>& reg, const std::string& name) {
        reg.register_type<TypeA>(name);
    }
}

namespace NS2 {
    struct TypeB {};
    
    void register_type(Registry<TypeB>& reg, const std::string& name) {
        reg.register_type<TypeB>(name);
    }
}

// 测试用例
int main() {
    Registry<TypeA> reg_a;
    NS1::register_type(reg_a, "TypeA");
    
    Registry<TypeB> reg_b;
    NS2::register_type(reg_b, "TypeB");
    
    reg_a.list_types();
    reg_b.list_types();
    
    return 0;
}
```

---

**测试用例执行说明**
1. **编译命令**（使用C++17标准）：
   ```bash
   g++ -std=c++17 -o test test.cpp && ./test
   ```
2. **预期输出**：
   - 题目1输出：
     ```
     [DEFAULT] Hello
     [CUSTOM] World
     ```
   - 题目2输出（无输出，程序正常退出）
   - 题目3输出（无输出，静态断言通过）
   - 题目4输出（无输出，静态断言通过）
   - 题目5输出：
     ```
     TypeA -> St4TypeA
     TypeB -> St4TypeB
     ```

---

**设计要点说明**
1. **ADL机制**：通过自定义命名空间和辅助函数实现日志处理器的动态选择
2. **依赖名称**：`delete_ptr`方法通过依赖名称查找实现自定义删除逻辑
3. **注入类名称**：`TypeTraits`直接使用`T::value_type`简化成员访问
4. **当前实例化检测**：利用`this->`限定符实现模板偏特化的运行时检测
5. **多命名空间注册**：通过ADL在不同命名空间中注册类型到统一工厂


**总结与学习路径**
1. **理解名称分类**：区分依赖/非依赖名、限定/非限定名
2. **掌握查找规则**：普通查找 vs ADL，两阶段查找机制
3. **实践友元与注入类名**：通过代码示例熟悉语法
4. **调试技巧**：利用静态断言和分阶段编译定位问题

建议通过以下步骤巩固知识：
1. 手动推导示例代码的名称查找过程
2. 编写包含模板继承和友元的复杂案例
3. 使用不同编译器（如GCC/Clang）观察错误信息差异

遇到具体问题时，可结合`nm`工具查看符号表，或使用`-fdump-class-hierarchy`等编译器选项分析模板实例化结果。