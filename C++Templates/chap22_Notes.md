以下是针对《C++ Templates》第二十二章“Bridging Static and Dynamic Polymorphism”的核心知识点详解、代码示例及测试用例：

---

### 第二十二章核心知识点
 1. **静态多态 vs 动态多态**
- **静态多态**：编译期多态，通过模板实现，代码生成效率高，但灵活性差。
- **动态多态**：运行期多态，通过虚函数实现，灵活性高，但存在虚表开销。

2. **类型擦除（Type Erasure）**
- 核心思想：将不同类型的对象统一为通用接口，隐藏具体类型信息。
- 实现方式：通常结合基类指针和模板注册机制。

3. **桥接模式（Bridge Pattern）**
- 分离抽象部分与实现部分，使两者独立变化。
- 在C++中可通过模板与虚函数结合实现。

---

### 代码示例与测试用例

示例1：类型擦除实现通用容器
```cpp
#include <iostream>
#include <memory>
#include <vector>

// 基类接口（动态多态）
class Shape {
public:
    virtual void draw() const = 0;
    virtual ~Shape() = default;
};

// 具体类型（静态多态）
class Circle : public Shape {
public:
    void draw() const override { std::cout << "Drawing Circle\n"; }
};

class Square : public Shape {
public:
    void draw() const override { std::cout << "Drawing Square\n"; }
};

// 类型擦除容器
class ShapeContainer {
private:
    struct Concept {
        virtual void draw() const = 0;
        virtual ~Concept() = default;
    };

    template<typename T>
    struct Model final : Concept {
        T obj;
        Model(const T& t) : obj(t) {}
        void draw() const override { obj.draw(); }
    };

    std::vector<std::unique_ptr<Concept>> data;

public:
    template<typename T>
    void add(const T& shape) {
        data.emplace_back(std::make_unique<Model<T>>(shape));
    }

    void drawAll() const {
        for (const auto& elem : data) {
            elem->draw();
        }
    }
};

int main() {
    ShapeContainer container;
    container.add(Circle{});  // 静态多态实例
    container.add(Square{});  // 静态多态实例

    container.drawAll();  // 动态调用所有元素的draw方法
    return 0;
}
```

示例2：std::function实现桥接
```cpp
#include <iostream>
#include <functional>

// 桥接接口
class Logger {
public:
    virtual void log(const std::string& msg) = 0;
    virtual ~Logger() = default;
};

// 具体日志器（静态多态）
class ConsoleLogger : public Logger {
public:
    void log(const std::string& msg) override {
        std::cout << "Console: " << msg << "\n";
    }
};

class FileLogger : public Logger {
public:
    void log(const std::string& msg) override {
        std::cout << "File: " << msg << "\n";
    }
};

// 桥接工厂（模板注册）
template<typename LoggerType>
void registerLogger(const std::string& name, std::function<std::unique_ptr<Logger>()> creator) {
    // 注册逻辑（此处简化为直接创建）
    auto logger = creator();
    logger->log("Registered: " + name);
}

int main() {
    // 动态选择日志器
    registerLogger<ConsoleLogger>("console", [] { return std::make_unique<ConsoleLogger>(); });
    registerLogger<FileLogger>("file", [] { return std::make_unique<FileLogger>(); });

    // 运行时动态调用
    std::unique_ptr<Logger> console = std::make_unique<ConsoleLogger>();
    console->log("Hello, World!");

    return 0;
}
```

示例3：模板与虚函数的混合使用
```cpp
#include <iostream>
#include <vector>

// 基类（动态多态）
class Animal {
public:
    virtual void speak() const = 0;
    virtual ~Animal() = default;
};

// 模板派生类（静态多态）
template<typename T>
class Speaker : public Animal {
    T message;
public:
    Speaker(const T& msg) : message(msg) {}
    void speak() const override { std::cout << message << "\n"; }
};

int main() {
    std::vector<std::unique_ptr<Animal>> animals;
    
    // 静态实例化不同模板参数
    animals.emplace_back(std::make_unique<Speaker<std::string>>("Meow"));
    animals.emplace_back(std::make_unique<Speaker<const char*>>("Woof"));
    animals.emplace_back(std::make_unique<Speaker<int>>(42));  // 输出数字
    
    // 动态调用
    for (const auto& animal : animals) {
        animal->speak();
    }
    return 0;
}
```

---

### 测试用例输出
1. **类型擦除容器测试**  
   输出：  
   ```
   Drawing Circle  
   Drawing Square  
   ```

2. **桥接工厂测试**  
   输出：  
   ```
   Registered: console  
   Registered: file  
   Console: Hello, World!  
   ```

3. **混合多态测试**  
   输出：  
   ```
   Meow  
   Woof  
   42  
   ```

---

### 多选题

题目1：关于`std::function`的实现原理，以下哪些说法正确？
A) 内部基于虚函数表实现类型擦除  
B) 支持存储任意可调用对象（函数指针/lambda/仿函数）  
C) 存储时需要拷贝可调用对象  
D) 调用时通过`operator()`隐式转换到目标类型  

**答案：B, C**  
**详解**：  
- `std::function`通过类型擦除实现，但并非直接使用虚函数表（现代实现通常用小对象优化或虚表组合）。  
- 正确支持存储任意可调用对象（A错误）。  
- 存储时会拷贝或移动可调用对象（C正确）。  
- 调用通过重载`operator()`直接调用存储的可调用对象（D错误）。

---

题目2：类型擦除的核心目的是什么？
A) 实现静态多态  
B) 隐藏具体类型信息  
C) 允许异构对象集合  
D) 提升运行时性能  

**答案：B, C**  
**详解**：  
- 类型擦除通过抽象接口隐藏具体类型（B正确）。  
- 允许将不同类型的对象统一存储（如`std::vector<std::function<void()>>`）（C正确）。  
- 静态多态通过模板实现，与类型擦除无关（A错误）。  
- 类型擦除通常伴随间接调用开销（D错误）。

---

题目3：以下哪种方式无法实现类型擦除？
A) 继承自虚基类  
B) 使用`std::any`  
C) 函数指针强制转换  
D) `std::variant`  

**答案：C, D**  
**详解**：  
- 继承+虚函数是典型类型擦除（A正确）。  
- `std::any`内部通过类型擦除存储任意类型（B正确）。  
- 函数指针强制转换破坏类型安全（C错误）。  
- `std::variant`是类型安全的联合体，非擦除（D错误）。

---

题目4：关于动态多态与静态多态的对比，正确的是？
A) 动态多态通过虚函数实现，静态多态通过模板  
B) 动态多态编译期绑定，静态多态运行时绑定  
C) 动态多态适合异质对象集合，静态多态适合同质操作  
D) 动态多态无代码膨胀，静态多态有代码膨胀  

**答案：A, C**  
**详解**：  
- 动态多态依赖虚表，静态多态依赖模板实例化（A正确）。  
- 动态多态运行时绑定，静态多态编译期绑定（B错误）。  
- 动态多态适合存储异质对象（如`std::vector<Shape*>`），静态多态适合同质算法（如`std::sort`）（C正确）。  
- 动态多态无代码膨胀，静态多态每个类型实例化一次（D错误）。

---

题目5：实现可调用对象包装器时，哪种方式无法保留参数类型信息？
A) `std::function`  
B) 手动虚函数派发  
C) `std::bind`  
D) 直接函数指针存储  

**答案：D**  
**详解**：  
- `std::function`和手动派发可保留参数类型（A/B正确）。  
- `std::bind`通过占位符保留参数类型（C正确）。  
- 函数指针仅存储调用签名，不保留参数类型元数据（D错误）。

---

题目6：以下代码存在什么问题？
```cpp
std::vector<std::function<void()>> tasks;
tasks.push_back([]{ std::cout << "Task1"; });
tasks.push_back(std::bind(&Foo::bar, Foo())); 
```
A) `Foo`缺少默认构造函数  
B) `std::bind`返回类型不兼容  
C) `Foo::bar`应为静态成员  
D) `std::function`无法存储成员函数  

**答案：A**  
**详解**：  
- `std::bind(&Foo::bar, Foo())`需要`Foo`有默认构造函数（A正确）。  
- `std::function`可存储成员函数（D错误）。  
- 返回类型兼容性取决于`Foo::bar`的返回值（B错误）。  
- 成员函数可通过对象实例绑定（C错误）。

---

题目7：类型擦除的常见实现模式有哪些？
A) 小对象优化（Small Object Optimization）  
B) 虚函数表转发  
C) Pimpl idiom  
D) CRTP  

**答案：A, B**  
**详解**：  
- 小对象优化直接存储小型可调用对象（A正确）。  
- 虚函数表转发通过基类指针实现多态（B正确）。  
- Pimpl用于接口隐藏，非类型擦除（C错误）。  
- CRTP用于静态多态（D错误）。

---

题目8：以下哪种情况会导致`std::function`拷贝代价高昂？
A) 存储小型lambda  
B) 存储成员函数指针  
C) 存储大对象仿函数  
D) 存储空函数  

**答案：C**  
**详解**：  
- 小型lambda可能内联存储（A低代价）。  
- 成员函数指针存储廉价（B低代价）。  
- 大对象仿函数需深拷贝（C高代价）。  
- 空函数无额外代价（D低代价）。

---

题目9：实现静态多态的常见技术有哪些？
A) Curiously Recurring Template Pattern (CRTP)  
B) `std::enable_if`  
C) SFINAE  
D) `virtual`继承  

**答案：A, B, C**  
**详解**：  
- CRTP通过模板偏特化实现静态多态（A正确）。  
- `std::enable_if`和SFINAE用于编译期条件选择（B/C正确）。  
- `virtual`继承属于动态多态（D错误）。

---

题目10：以下代码输出是什么？
```cpp
struct Base { virtual void foo() { std::cout << "Base"; } };
struct Derived : Base { void foo() override { std::cout << "Derived"; } };

std::function<void(Base*)> func = [](Base* b){ b->foo(); };
Derived d;
func(&d);
```
A) Base  
B) Derived  
C) 编译错误  
D) 未定义行为  

**答案：B**  
**详解**：  
- `Base`指针指向`Derived`对象，虚函数调用动态派发（B正确）。  
- `std::function`正确转发调用（无A/C/D问题）。

---

---

### 设计题

题目1：实现一个简单的类型擦除容器
**要求**：设计一个类`AnyCallable`，能够存储任意可调用对象（函数指针、lambda、仿函数），并提供`operator()`调用。  
**测试用例**：
```cpp
AnyCallable c1 = []{ std::cout << "Lambda\n"; };
AnyCallable c2 = &foo; // 假设foo是void(*)()
AnyCallable c3 = Functor();

c1(); // 输出Lambda
c2(); // 输出Foo
c3(); // 输出Functor
```

**答案**：
```cpp
class AnyCallable {
    struct Concept {
        virtual void call() = 0;
        virtual ~Concept() {}
    };
    template<typename F>
    struct Model : Concept {
        F f;
        Model(F f) : f(f) {}
        void call() override { f(); }
    };
    std::unique_ptr<Concept> ptr;

public:
    template<typename F>
    AnyCallable(F f) : ptr(new Model<F>(f)) {}

    void operator()() { ptr->call(); }
};
```

---

题目2：比较动态多态与静态多态的性能差异
**要求**：编写两个版本的计算器：
1. 动态多态版：基类`Operation`派生出`Add/Sub/Mul`，通过虚函数计算。
2. 静态多态版：使用模板偏特化实现相同功能。  
**测试用例**：对每种操作执行100万次，比较耗时。

**答案**：
```cpp
// 动态多态版
struct Operation {
    virtual int calculate(int a, int b) = 0;
    virtual ~Operation() {}
};

struct Add : Operation { int calculate(...) override { return a + b; } };
// Sub/Mul类似...

// 静态多态版
template<typename Op>
int calculate(Op op, int a, int b) { return op(a, b); }

struct AddOp { int operator()(int a, int b) { return a + b; } };
// SubOp/MulOp类似...
```

---

题目3：实现一个线程安全的函数注册表
**要求**：设计一个类`FunctionRegistry`，支持注册函数名与可调用对象的映射，并发安全地通过名称调用函数。  
**测试用例**：
```cpp
FunctionRegistry reg;
reg.registerFunction("print", []{ std::cout << "Hello"; });
reg.call("print"); // 输出Hello
```

**答案**：
```cpp
#include <mutex>
#include <unordered_map>

class FunctionRegistry {
    std::unordered_map<std::string, std::function<void()>> map;
    std::mutex mtx;

public:
    void registerFunction(const std::string& name, std::function<void()> func) {
        std::lock_guard<std::mutex> lock(mtx);
        map[name] = std::move(func);
    }

    void call(const std::string& name) {
        std::lock_guard<std::mutex> lock(mtx);
        if (auto it = map.find(name); it != map.end()) {
            it->second();
        }
    }
};
```

---

题目4：类型擦除的代价分析
**要求**：编写两个版本的向量求和：
1. 静态版本：使用模板接受任意数值类型。
2. 动态版本：使用`std::function`存储求和逻辑。  
**测试用例**：分别对`int`和`double`向量求和，对比编译后代码大小和运行时间。

**答案**：
```cpp
// 静态版本
template<typename T>
T staticSum(const std::vector<T>& vec) {
    return std::accumulate(vec.begin(), vec.end(), T{});
}

// 动态版本
std::function<double(const std::vector<double>&)> dynamicSum = [](const auto& vec){
    return std::accumulate(vec.begin(), vec.end(), 0.0);
};
```

---

题目5：实现一个事件总线（Event Bus）
**要求**：设计一个基于类型擦除的事件总线，支持注册事件监听器和触发事件。  
**测试用例**：
```cpp
EventBus bus;
bus.subscribe<int>("number_event", [](int x){ std::cout << "Received: " << x; });
bus.publish(42); // 输出Received: 42
```

**答案**：
```cpp
#include <string>
#include <functional>
#include <unordered_map>

class EventBus {
    std::unordered_map<std::string, std::function<void(const std::any&)>> listeners;

public:
    template<typename T>
    void subscribe(const std::string& event, std::function<void(T)> callback) {
        listeners[event] = [callback](const std::any& arg){
            if (arg.type() == typeid(T)) {
                callback(std::any_cast<T>(arg));
            }
        };
    }

    void publish(const std::string& event, const std::any& arg) {
        if (auto it = listeners.find(event); it != listeners.end()) {
            it->second(arg);
        }
    }
};
```

---


### 关键技术总结
1. **类型擦除**：通过基类指针统一异构对象，结合模板实现灵活注册。
2. **桥接模式**：分离接口与实现，允许独立演化。
3. **std::function**：封装可调用对象，实现动态绑定。
