Let's dive into **Chapter 22: Bridging Static and Dynamic Polymorphism** from *C++ Templates: The Complete Guide*. This chapter explores how to combine the strengths of static polymorphism (templates) and dynamic polymorphism (virtual functions) to create flexible, efficient, and reusable code. Below is a step-by-step breakdown of the key concepts, with code examples and tests.

---

### 1. **Dynamic vs. Static Polymorphism**
- **Dynamic Polymorphism**: Achieved via inheritance and virtual functions. The exact function called is determined at runtime.
- **Static Polymorphism**: Achieved via templates. Type resolution happens at compile time, avoiding runtime overhead.

**Example**:  
```cpp
// Dynamic polymorphism (virtual functions)
class Animal {
public:
    virtual void speak() const = 0;
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void speak() const override { std::cout << "Woof!\n"; }
};

class Cat : public Animal {
public:
    void speak() const override { std::cout << "Meow!\n"; }
};

// Static polymorphism (templates)
template<typename T>
void make_sound(const T& animal) {
    animal.speak();
}
```

**Test Case**:
```cpp
int main() {
    Dog dog;
    Cat cat;
    
    make_sound(dog); // Static: calls Dog::speak()
    make_sound(cat); // Static: calls Cat::speak()
    
    std::vector<Animal*> animals = {&dog, &cat};
    for (const auto& animal : animals) {
        animal->speak(); // Dynamic: calls overridden method
    }
    return 0;
}
```

---

### 2. **Bridging Static and Dynamic Polymorphism**
**2.1 Function Objects and `std::function`**
Use `std::function` to wrap callable objects (functions, lambdas, functors) into a uniform interface.

**Example**:
```cpp
#include <functional>

class Logger {
public:
    void log(const std::function<void()>& func) {
        std::cout << "Logging...\n";
        func();
    }
};

int main() {
    Logger logger;
    logger.log([] { std::cout << "Hello from lambda!\n"; });
    return 0;
}
```

**Test Case**:
```cpp
int main() {
    Logger logger;
    logger.log([] { std::cout << "Lambda 1\n"; });
    logger.log([] { std::cout << "Lambda 2\n"; });
    return 0;
}
```

---

**2.2 Bridge Interface**
Create an abstract base class (interface) and use templates to delegate implementation to derived classes.

**Example**:
```cpp
class BridgeInterface {
public:
    virtual ~BridgeInterface() = default;
    virtual void execute() const = 0;
};

template<typename T>
class Bridge : public BridgeInterface {
    T impl;
public:
    Bridge(const T& t) : impl(t) {}
    void execute() const override { impl(); }
};

int main() {
    Bridge<std::function<void()>> bridge([] { std::cout << "Bridge executed!\n"; });
    bridge.execute();
    return 0;
}
```

**Test Case**:
```cpp
int main() {
    Bridge<std::function<void()>> bridge([] { std::cout << "Hello from Bridge!\n"; });
    bridge.execute();
    return 0;
}
```

---

#### **2.3 Type Erasure**
Erase the concrete type at compile time to create a uniform runtime interface.

**Example**:
```cpp
class AnyCallable {
    struct Concept {
        virtual ~Concept() = default;
        virtual void invoke() const = 0;
    };
    
    template<typename T>
    struct Model : Concept {
        T func;
        Model(T f) : func(std::move(f)) {}
        void invoke() const override { func(); }
    };
    
    std::unique_ptr<Concept> ptr;
public:
    template<typename T>
    AnyCallable(T f) : ptr(std::make_unique<Model<T>>(std::move(f))) {}
    
    void operator()() const { ptr->invoke(); }
};

int main() {
    AnyCallable a([] { std::cout << "Type erased callable!\n"; });
    a();
    return 0;
}
```

**Test Case**:
```cpp
int main() {
    AnyCallable a([] { std::cout << "Hello from Type Erasure!\n"; });
    a();
    return 0;
}
```

---

### 3. **Performance Considerations**
- **Overhead**: Dynamic polymorphism introduces indirection (vtable lookup), while static polymorphism is resolved at compile time.
- **Type Erasure**: Adds memory overhead (heap allocation) and runtime cost (virtual calls).

**Benchmark Example**:
```cpp
#include <chrono>
#include <iostream>

void static_dispatch() {
    for (int i = 0; i < 1000000; ++i) {
        make_sound(Dog{}); // Static
    }
}

void dynamic_dispatch() {
    Animal* animal = new Dog();
    for (int i = 0; i < 1000000; ++i) {
        animal->speak(); // Dynamic
    }
    delete animal;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    static_dispatch();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Static: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "μs\n";
    
    start = std::chrono::high_resolution_clock::now();
    dynamic_dispatch();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Dynamic: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "μs\n";
    return 0;
}
```

---

### 6. **Summary**
- **Static Polymorphism**: Fast, compile-time resolution, no runtime overhead. Use templates.
- **Dynamic Polymorphism**: Flexible, runtime resolution. Use inheritance/virtual functions.
- **Bridging**: Combine both paradigms using `std::function`, bridge interfaces, or type erasure for flexibility and efficiency.

**Key Takeaways**:
- Use `std::function` for simple callbacks.
- Prefer templates for compile-time polymorphism.
- Use bridge interfaces/type erasure when dynamic dispatch is unavoidable but type flexibility is needed.

**Final Test Case**:
```cpp
int main() {
    // Static polymorphism
    make_sound(Dog{}); // Dog::speak()
    
    // Dynamic polymorphism
    Animal* animal = new Dog();
    animal->speak(); // Dog::speak()
    delete animal;
    
    // Type erasure
    AnyCallable a([] { std::cout << "Type erased!\n"; });
    a();
    
    return 0;
}
```

This chapter highlights how C++ templates can interoperate with dynamic polymorphism to leverage the best of both worlds. Experiment with these patterns to write robust and efficient code!