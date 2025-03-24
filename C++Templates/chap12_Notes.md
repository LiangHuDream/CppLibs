好的，我将按照您的要求分步骤讲解第十三章《Names in Templates》的重难点，并结合代码示例和测试用例进行说明。以下是详细的学习计划：

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
template<typename T>
class Base {
public:
    using type = T;
};

template<typename T>
class Derived : public Base<T> {
public:
    Derived() {
        Base::type x;  // 隐式使用注入类名Base<T>
    }
};

int main() {
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