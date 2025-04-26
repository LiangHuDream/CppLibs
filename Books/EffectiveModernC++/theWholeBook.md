
### 20 Multiple-Choice Questions on Modern C++  
Based on Key Concepts from "Effective Modern C++"  

---

#### 1. Universal References  
What is the deduced type of `param` in `template<typename T> void f(T&& param);` when `f(42)` is called?  
A) `int`  
B) `int&`  
C) `const int&`  
D) `int&&`  

---

#### 2. `std::move` vs. `std::forward`  
Which function is used to preserve the value category (lvalue/rvalue) of a forwarded argument?  
A) `std::move`  
B) `std::forward`  
C) Both  
D) Neither  

---

#### 3. Smart Pointer Ownership  
What happens when a `std::unique_ptr` is copied?  
A) The pointer is shallow-copied.  
B) The program compiles but crashes at runtime.  
C) The program fails to compile.  
D) The pointer is deep-copied.  

---

#### 4. Lambda Captures  
Which lambda capture clause ensures a variable is copied by value?  
A) `[&]`  
B) `[=]`  
C) `[x]`  
D) `[this]`  

---

#### 5. `constexpr` Functions  
Which statement about `constexpr` functions is false?  
A) They can be evaluated at compile time.  
B) They cannot modify global variables.  
C) They must return `void`.  
D) They can use loops.  

---

#### 6. Move Semantics  
When is a move constructor not generated implicitly by the compiler?  
A) If the class has a user-declated destructor.  
B) If the class has a user-declared copy constructor.  
C) If the class has a user-declared move assignment operator.  
D) Never; it is always generated.  

---

#### 7. `noexcept` Specifier  
What is the primary purpose of marking a function `noexcept`?  
A) To improve runtime performance.  
B) To enforce compile-time checks.  
C) To disable exceptions entirely.  
D) To document intent for exception safety.  

---

#### 8. Template Type Deduction  
Given `template<typename T> void f(T param);` and `const int x = 0;`, what is `T` when `f(x)` is called?  
A) `const int`  
B) `int`  
C) `const int&`  
D) `int&`  

---

#### 9. `std::initializer_list`  
Which constructor is prioritized when a class has both a constructor taking `std::initializer_list` and one with parameter(s) of the same type?  
A) The `std::initializer_list` constructor.  
B) The constructor with matching parameters.  
C) The compiler throws an error.  
D) It depends on the argument types.  

---

#### 10. `std::shared_ptr` Control Block  
Which operation does not create a new control block for a `std::shared_ptr`?  
A) `std::make_shared`  
B) Constructing from a raw pointer.  
C) Copying an existing `std::shared_ptr`.  
D) Using `std::shared_ptr::reset()`.  

---

#### 11. Perfect Forwarding Failure  
Which argument type cannot be perfectly forwarded using `std::forward`?  
A) Braced initializers (`{1, 2, 3}`).  
B) `nullptr`.  
C) Overloaded function names.  
D) Bitfields.  

---

#### 12. RAII and Mutexes  
Why should `std::lock_guard` be preferred over manual `lock()`/`unlock()` calls?  
A) It avoids deadlocks.  
B) It ensures exception safety.  
C) It supports recursive locking.  
D) It is faster.  

---

#### 13. `std::async` Launch Policy  
What is the default launch policy for `std::async`?  
A) `std::launch::async`  
B) `std::launch::deferred`  
C) `std::launch::async | std::launch::deferred`  
D) Platform-dependent.  

---

#### 14. Type Traits  
Which type trait checks if a type can be copy-constructed?  
A) `std::is_copy_constructible`  
B) `std::is_copy_assignable`  
C) `std::is_trivial`  
D) `std::is_pod`  

---

#### 15. PIMPL Idiom  
Why must the destructor of a PIMPL class be defined in the implementation file?  
A) To avoid incomplete type errors.  
B) To enable move semantics.  
C) To ensure exception safety.  
D) To reduce code duplication.  

---

#### 16. `decltype(auto)`  
What is the primary use of `decltype(auto)`?  
A) To deduce the type like `auto` but preserve references.  
B) To enforce copy semantics.  
C) To replace `auto` in all contexts.  
D) To disable template argument deduction.  

---

#### 17. Move-Only Types  
Which type is not move-only?  
A) `std::unique_ptr`  
B) `std::thread`  
C) `std::shared_ptr`  
D) `std::fstream`  

---

#### 18. `volatile` Keyword  
Which scenario correctly uses `volatile`?  
A) Multi-threaded access to a counter.  
B) Memory-mapped hardware registers.  
C) Cache optimization.  
D) Atomic operations.  

---

#### 19. `std::variant` vs. `union`  
What advantage does `std::variant` have over a raw `union`?  
A) Type safety.  
B) Smaller memory footprint.  
C) Support for non-trivial types.  
D) Faster access times.  

---

#### 20. `const` Member Functions  
What does a `const` member function imply about `this`?  
A) `this` is a `const` pointer.  
B) `this` is a pointer to `const` data.  
C) The function cannot modify the object’s state.  
D) All of the above.  

---

### Answers & Detailed Explanations  

1. D) `int&&`  
   - Universal reference `T&&` deduces to `int&&` for rvalue `42`.  

2. B) `std::forward`  
   - `std::forward` preserves the original value category (lvalue/rvalue).  

3. C) The program fails to compile.  
   - `std::unique_ptr` is move-only; copying is disabled.  

4. C) `[x]`  
   - `[x]` captures `x` by value explicitly.  

5. C) They must return `void`.  
   - `constexpr` functions can return any type, not just `void`.  

6. B) If the class has a user-declared copy constructor.  
   - User-declared copy operations suppress move constructor generation.  

7. A) To improve runtime performance.  
   - `noexcept` allows optimizations (e.g., avoiding stack unwinding).  

8. B) `int`  
   - Template type deduction strips `const` and references for by-value parameters.  

9. A) The `std::initializer_list` constructor.  
   - Braced initializers prioritize `std::initializer_list` constructors.  

10. C) Copying an existing `std::shared_ptr`.  
    - Copies share the same control block.  

11. A) Braced initializers.  
    - Braced initializers cannot deduce `std::initializer_list` in templates.  

12. B) It ensures exception safety.  
    - `std::lock_guard` releases the mutex automatically via RAII.  

13. C) `std::launch::async | std::launch::deferred`  
    - Default policy allows deferred execution.  

14. A) `std::is_copy_constructible`  
    - Checks if `T` can be copy-constructed.  

15. A) To avoid incomplete type errors.  
    - The destructor must see the complete `Impl` type.  

16. A) To preserve references.  
    - `decltype(auto)` uses `decltype` rules to retain value categories.  

17. C) `std::shared_ptr`  
    - `std::shared_ptr` is copyable.  

18. B) Memory-mapped hardware registers.  
    - `volatile` prevents compiler optimizations for hardware access.  

19. A) Type safety.  
    - `std::variant` ensures type-safe access via visitors.  

20. C) The function cannot modify the object’s state.  
    - `const` member functions cannot modify non-`mutable` members.  


---
## 10 Design Questions
### 1. Thread-Safe Singleton with Double-Checked Locking  
Question: Implement a thread-safe singleton using modern C++ features, avoiding the pitfalls of double-checked locking.  

Answer:  
```cpp  
#include <mutex>  
#include <atomic>  

class Singleton {  
public:  
    static Singleton& instance() {  
        static Singleton inst;  
        return inst;  
    }  
    // Delete copy/move  
    Singleton(const Singleton&) = delete;  
    Singleton& operator=(const Singleton&) = delete;  
private:  
    Singleton() = default;  
};  

// Test Case  
int main() {  
    auto& s1 = Singleton::instance();  
    auto& s2 = Singleton::instance();  
    assert(&s1 == &s2); // Same instance  
}  
```
Explanation:  
- Uses Meyers' Singleton (Item 47), which is thread-safe due to C++11 guarantees.  
- Avoids double-checked locking entirely by leveraging `static` initialization.  
- Key Points: Static local variables are initialized once, even in concurrent code.  

---

### 2. Generic Factory Function with Perfect Forwarding  
Question: Design a factory function that creates objects of any type using variadic templates and perfect forwarding.  

Answer:  
```cpp  
template<typename T, typename... Args>  
T create(Args&&... args) {  
    return T(std::forward<Args>(args)...);  
}  

// Test Case  
struct Widget {  
    Widget(int a, double b) {}  
};  

int main() {  
    auto w = create<Widget>(42, 3.14); // Perfectly forwarded  
}  
```
Explanation:  
- Perfect Forwarding (Item 25) preserves the value category (lvalue/rvalue) of arguments.  
- Variadic Templates (Item 3) allow any number of arguments.  

---

### 3. RAII File Handle with `unique_ptr`  
Question: Create a RAII wrapper for a C-style file handle using `unique_ptr` and a custom deleter.  

Answer:  
```cpp  
#include <memory>  
#include <cstdio>  

struct FileDeleter {  
    void operator()(FILE* fp) const {  
        if (fp) fclose(fp);  
    }  
};  

using FileHandle = std::unique_ptr<FILE, FileDeleter>;  

FileHandle openFile(const char* path, const char* mode) {  
    return FileHandle(fopen(path, mode));  
}  

// Test Case  
int main() {  
    auto file = openFile("test.txt", "w");  
    if (file) fprintf(file.get(), "Hello");  
} // File closed automatically  
```
Explanation:  
- Custom Deleter (Item 18) ensures `fclose` is called.  
- `unique_ptr` (Item 18) manages exclusive ownership.  

---

### 4. Type-Erased Container for Callables  
Question: Implement a container that stores any callable with signature `int(int)`.  

Answer:  
```cpp  
#include <functional>  
#include <vector>  

using Callable = std::function<int(int)>;  

class CallableContainer {  
    std::vector<Callable> callables;  
public:  
    void add(Callable func) {  
        callables.push_back(std::move(func));  
    }  
    int runAll(int x) {  
        int result = 0;  
        for (const auto& f : callables) result += f(x);  
        return result;  
    }  
};  

// Test Case  
int main() {  
    CallableContainer cc;  
    cc.add([](int x) { return x * 2; });  
    cc.add([](int x) { return x + 1; });  
    assert(cc.runAll(3) == (6 + 4)); // 3*2 + (3+1) = 10  
}  
```
Explanation:  
- `std::function` (Item 34) enables type erasure for callables.  

---

### 5. Thread Pool with `std::async` and Futures  
Question: Implement a thread pool that executes tasks asynchronously and returns futures.  

Answer:  
```cpp  
#include <vector>  
#include <future>  
#include <functional>  

class ThreadPool {  
    std::vector<std::future<void>> futures;  
public:  
    template<typename Func, typename... Args>  
    void submit(Func&& func, Args&&... args) {  
        futures.emplace_back(  
            std::async(std::launch::async,  
                std::forward<Func>(func),  
                std::forward<Args>(args)...  
            )  
        );  
    }  
    ~ThreadPool() {  
        for (auto& f : futures) f.wait();  
    }  
};  

// Test Case  
int main() {  
    ThreadPool pool;  
    pool.submit([] { std::cout << "Task 1\n"; });  
    pool.submit([] { std::cout << "Task 2\n"; });  
}  
```
Explanation:  
- `std::async` (Item 35) launches tasks asynchronously.  
- Futures (Item 38) manage task results.  

---

### 6. Compile-Time Fibonacci with `constexpr`  
Question: Write a `constexpr` function to compute Fibonacci numbers at compile time.  

Answer:  
```cpp  
constexpr int fibonacci(int n) {  
    return (n <= 1) ? n : fibonacci(n - 1) + fibonacci(n - 2);  
}  

// Test Case  
int main() {  
    static_assert(fibonacci(10) == 55); // Compile-time check  
}  
```
Explanation:  
- `constexpr` (Item 15) enables compile-time evaluation.  

---

### 7. PIMPL Idiom with `unique_ptr`  
Question: Implement the PIMPL idiom for a class `Widget`, ensuring safe move operations.  

Answer:  
Widget.h:  
```cpp  
class Widget {  
    struct Impl;  
    std::unique_ptr<Impl> pImpl;  
public:  
    Widget();  
    ~Widget();  
    Widget(Widget&&) noexcept;  
    Widget& operator=(Widget&&) noexcept;  
};  
```
Widget.cpp:  
```cpp  
#include "Widget.h"  
struct Widget::Impl { int data; };  

Widget::Widget() : pImpl(std::make_unique<Impl>()) {}  
Widget::~Widget() = default;  
Widget::Widget(Widget&&) noexcept = default;  
Widget& Widget::operator=(Widget&&) noexcept = default;  
```
Explanation:  
- Move Operations (Item 17) must be explicitly defined due to `unique_ptr`.  
- Incomplete Type (Item 22) requires destructor definition in the implementation file.  

---

### 8. Type Trait for Hashable Types  
Question: Create a type trait `is_hashable` to check if a type can be used in `std::unordered_map`.  

Answer:  
```cpp  
#include <type_traits>  
#include <unordered_map>  

template<typename T, typename = void>  
struct is_hashable : std::false_type {};  

template<typename T>  
struct is_hashable<T, std::void_t<decltype(std::hash<T>{}(std::declval<T>()))>>  
    : std::true_type {};  

// Test Case  
static_assert(is_hashable<int>::value);  
static_assert(!is_hashable<std::vector<int>>::value);  
```
Explanation:  
- SFINAE (Item 27) checks for valid `std::hash` instantiation.  

---

### 9. Custom `any` Container Using Variant Pattern  
Question: Implement a type-safe `any` container without `std::any` or `std::variant`.  

Answer:  
```cpp  
#include <stdexcept>  
#include <typeinfo>  

class Any {  
    struct Base {  
        virtual ~Base() = default;  
        virtual Base* clone() const = 0;  
    };  
    template<typename T>  
    struct Derived : Base {  
        T value;  
        Derived(T v) : value(v) {}  
        Base* clone() const override { return new Derived(value); }  
    };  
    Base* ptr = nullptr;  
public:  
    template<typename T>  
    Any(T&& val) : ptr(new Derived<std::decay_t<T>>(std::forward<T>(val))) {}  
    ~Any() { delete ptr; }  

    template<typename T>  
    T cast() const {  
        if (auto d = dynamic_cast<Derived<T>*>(ptr)) {  
            return d->value;  
        }  
        throw std::bad_cast();  
    }  
};  

// Test Case  
int main() {  
    Any a = 42;  
    assert(a.cast<int>() == 42);  
}  
```
Explanation:  
- Type Erasure (Item 34) hides the underlying type.  
- Virtual Cloning ensures safe copying.  

---

### 10. Optimize `push_back` with `emplace_back`  
Question: Replace a `push_back` loop with `emplace_back` and measure performance improvements.  

Answer:  
```cpp  
#include <vector>  
#include <string>  

int main() {  
    std::vector<std::string> vec;  
    // Before  
    for (int i = 0; i < 1000; ++i) {  
        vec.push_back(std::to_string(i));  
    }  
    // After (optimized)  
    vec.clear();  
    for (int i = 0; i < 1000; ++i) {  
        vec.emplace_back(std::to_string(i));  
    }  
}  
```
Explanation:  
- `emplace_back` (Item 42) avoids creating a temporary `std::string`.  
- Move Semantics (Item 29) reduce copies in C++11.  

---

These questions and answers solidify understanding of Modern C++ by combining practical implementation with deep conceptual insights.