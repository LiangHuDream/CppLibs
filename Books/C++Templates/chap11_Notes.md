### Step-by-Step Guide to Chapter 11: Generic Libraries

1. **Callables and Wrapping Function Calls**
**Key Concepts**:  
Callables include function pointers, functors, lambdas, and member functions. The challenge is to handle these uniformly.

**Code Example**:
```cpp
#include <iostream>
#include <functional>

// Wrapper to invoke any callable
template<typename Callable, typename... Args>
auto invoke(Callable&& op, Args&&... args) {
    return std::invoke(std::forward<Callable>(op), 
                      std::forward<Args>(args)...);
}

struct Foo {
    void bar(int x) { std::cout << "Foo::bar: " << x << '\n'; }
};

int main() {
    // Test with lambda
    invoke([](int x) { std::cout << "Lambda: " << x << '\n'; }, 42);

    // Test with function
    invoke(static_cast<void(*)(int)>([](int x) { 
        std::cout << "Function: " << x << '\n'; 
    }), 100);

    // Test with member function
    Foo foo;
    invoke(&Foo::bar, foo, 200); // foo.bar(200)
}
```
**Output**:
```
Lambda: 42
Function: 100
Foo::bar: 200
```

**Explanation**:  
- `std::invoke` handles all callable types uniformly.
- Member functions require an object instance as the first argument.

---

2. **Type Traits and `std::declval`**
**Key Concepts**:  
Type traits inspect/modify types at compile time. `std::declval` creates "fake" instances for type deduction.

**Code Example**:
```cpp
#include <type_traits>
#include <iostream>

template<typename T>
void check_type() {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "Integral type\n";
    } else if (std::is_class_v<T>) {
        std::cout << "Class type\n";
    }
}

// Using std::declval to get return type
template<typename T>
using AddResult = decltype(std::declval<T>() + std::declval<T>());

int main() {
    check_type<int>();       // Output: Integral type
    check_type<std::string>(); // Output: Class type

    static_assert(std::is_same_v<AddResult<int>, int>, "int + int = int");
    static_assert(std::is_same_v<AddResult<std::string>, std::string>, 
                  "string + string = string");
}
```

**Explanation**:  
- `std::is_integral_v<T>` checks if `T` is an integer type.
- `std::declval<T>()` allows type deduction without constructor calls.

---

3. **Perfect Forwarding with `std::forward`**
**Key Concepts**:  
Preserve value category (lvalue/rvalue) when passing arguments through templates.

**Code Example**:
```cpp
#include <iostream>
#include <string>

template<typename T>
void wrapper(T&& arg) {
    std::cout << "Arg is "
              << (std::is_lvalue_reference_v<decltype(arg)> ? "lvalue" : "rvalue")
              << '\n';
}

int main() {
    std::string s = "test";
    wrapper(s);             // lvalue
    wrapper(std::move(s));  // rvalue
    wrapper(42);            // rvalue (literal is prvalue)
}
```
**Output**:
```
Arg is lvalue
Arg is rvalue
Arg is rvalue
```

**Explanation**:  
- `T&&` becomes a universal reference (deduces to `T&` for lvalues, `T&&` for rvalues).
- `std::forward<T>` preserves the original value category.

---

4. **SFINAE with `std::enable_if`**
**Key Concepts**:  
Enable/disable template overloads based on type conditions.

**Code Example**:
```cpp
#include <type_traits>
#include <iostream>

// Enabled only for integral types
template<typename T, 
         typename = std::enable_if_t<std::is_integral_v<T>>>
void process(T x) {
    std::cout << "Processing integral: " << x << '\n';
}

// Fallback for non-integral types
template<typename T>
void process(T x) {
    std::cout << "Processing non-integral: " << x << '\n';
}

int main() {
    process(42);           // Integral
    process(3.14);          // Non-integral
    process("hello");       // Non-integral
}
```
**Output**:
```
Processing integral: 42
Processing non-integral: 3.14
Processing non-integral: hello
```

**Explanation**:  
- `std::enable_if_t<condition>` removes the function from overload resolution if `condition` is false.
- The second overload acts as a fallback.

---

5. **Variable Templates and `constexpr`**
**Key Concepts**:  
Compile-time computation using variable templates and `constexpr`.

**Code Example**:
```cpp
#include <iostream>

template<typename T>
constexpr bool is_pointer_v = false;

template<typename T>
constexpr bool is_pointer_v<T*> = true;

int main() {
    std::cout << std::boolalpha;
    std::cout << is_pointer_v<int> << '\n';    // false
    std::cout << is_pointer_v<int*> << '\n';   // true
}
```
**Output**:
```
false
true
```

**Explanation**:  
- Variable templates simplify type checks compared to classic type traits.
- Specializations handle specific cases (e.g., pointers).

---


### Multiple-Choice Questions (Hard Difficulty)

Question 1:
Which techniques are valid for supporting member functions as callables in generic code?
A) Use `std::bind` with placeholders  
B) Leverage `std::invoke` with an object instance and member function pointer  
C) Overload for member function pointers explicitly  
D) Use lambda captures to bind the object instance  

Question 2:
What ensures perfect forwarding of arguments in a generic wrapper?
A) `std::forward<Args>(args)...`  
B) `decltype(auto)` return type  
C) Universal references in the template parameter list  
D) `auto&&` for parameter types  

Question 3:
Which scenarios require `std::ref`/`std::cref` when passing arguments to templates?
A) Preserving lvalue references across template boundaries  
B) Forwarding temporaries to functions expecting const references  
C) Storing references in containers  
D) Avoiding unnecessary copies when passing callable objects  

Question 4:
What are valid uses of `std::declval` in type traits?
A) Accessing member types of incomplete types  
B) Forming expressions in `decltype` without object construction  
C) Creating temporary values for SFINAE checks  
D) Implementing `noexcept` specifiers  

Question 5:
Which rules govern reference collapsing in template argument deduction?
A) `T&& &&` collapses to `T&&`  
B) `T& &&` collapses to `T&`  
C) `T&& &` collapses to `T&`  
D) `T& &` collapses to `T&`  

Question 6:
What are valid approaches to delay evaluation of template code?
A) Use alias templates with `typename`  
B) Leverage `std::void_t` in SFINAE contexts  
C) Encapsulate expressions in `decltype`  
D) Defer instantiation via inheritance  

Question 7:
Which scenarios cause dangling references when using reference template parameters?
A) Storing references to temporaries in a `tuple`  
B) Returning references to local variables in template functions  
C) Binding `auto&&` to rvalues  
D) Capturing references in lambda expressions  

Question 8:
What correctly implements a type-safe variadic argument wrapper?
A) `template<typename... Args> void log(Args&&... args) { /*...*/ }`  
B) `std::tuple<Args...>` with `std::forward`  
C) Fold expressions with `std::cout`  
D) `std::initializer_list` capture  

Question 9:
Which techniques avoid instantiation errors in SFINAE contexts?
A) Using `if constexpr` for conditional compilation  
B) Partial template specialization  
C) Expression SFINAE with `decltype`  
D) `enable_if` on template parameters  

Question 10:
What are valid uses of `std::addressof` in generic code?
A) Overloading `operator&` safely  
B) Getting the true address of an object  
C) Bypassing custom `operator new`  
D) Checking pointer validity  

---

### Detailed Design Questions (Hard Difficulty)

Question 1:
**Design a generic `CallWrapper` that can invoke any callable (including member functions with additional arguments) while preserving value categories.**  
**Requirements:**  
- Support free functions, lambdas, and member functions  
- Handle perfect forwarding of arguments  
- Allow invoking member functions with an object instance and extra parameters  

```cpp
// Example usage:
struct Widget { void paint(int, std::string); };
Widget w;
CallWrapper wrapper{&Widget::paint, &w, 42};
wrapper("blue"); // Invokes w.paint(42, "blue")
```

Question 2:
**Implement a `type_switch` mechanism that dispatches based on type identity without RTTI.**  
**Requirements:**  
- Use template specialization and variadic templates  
- Match types exactly (no implicit conversions)  
- Provide a default case handler  

```cpp
// Example usage:
type_switch<int, float>(value)
    .case_<int>([](int x) { /*...*/ })
    .case_<float>([](float y) { /*...*/ })
    .default_([](auto z) { /*...*/ });
```

Question 3:
**Create a `constexpr`-enabled `TypeList` with operations (e.g., `filter`, `transform`) using C++17 fold expressions.**  
**Requirements:**  
- Support type lists like `TypeList<int, float, char>`  
- Implement `filter` to remove types based on a predicate  
- Enable compile-time transformations (e.g., `add_const`)  

```cpp
using MyList = TypeList<int, float, char>;
using Filtered = MyList::filter<std::is_integral>; // TypeList<int, char>
using Transformed = MyList::transform<std::add_const>; // TypeList<const int, ...>
```

Question 4:
**Design a `ScopeGuard` template that executes a lambda on scope exit, with support for conditional rollback and exception safety.**  
**Requirements:**  
- Use RAII for resource management  
- Allow conditional commit/dismissal  
- Handle exceptions during cleanup  

```cpp
{
    FILE* f = fopen("data.txt", "r");
    ScopeGuard onExit([&] { if (f) fclose(f); });
    // ... use file; if exception thrown, file is closed
    onExit.dismiss(); // Commit to keep file open
}
```

Question 5:
**Build a `ThreadSafeQueue` template that supports wait-free enqueue and blocking dequeue with type-safe variadic arguments.**  
**Requirements:**  
- Use mutexes and condition variables  
- Perfect-forward arguments to emplace elements  
- Provide thread-safe `push` and `pop` operations  

```cpp
ThreadSafeQueue<std::string> queue;
queue.emplace("Hello", 42); // Constructs std::string{"Hello42"}
std::string value;
queue.pop(value); // Blocks until element is available
```

---

### Answers & Explanations

#### Multiple-Choice Answers:
1. **B, C**  
   `std::invoke` handles member functions when passed an object instance and member pointer. Explicit overloading can also detect member pointers.

2. **A, C**  
   `std::forward` preserves value categories, and universal references (T&&) enable perfect forwarding.

3. **A, D**  
   `std::ref` preserves references across templates. It’s used to avoid copies when passing callables.

4. **B, C**  
   `std::declval` creates fake instances for `decltype` and SFINAE without constructing objects.

5. **B, C, D**  
   Reference collapsing rules: Only `T&& &&` becomes `T&&`; others collapse to `T&` or `T&& &` → `T&`.

6. **B, C**  
   `std::void_t` defers evaluation for SFINAE. `decltype` delays expression checking.

7. **A, B**  
   Storing temporaries in tuples or returning local references causes dangling references.

8. **A, B**  
   Perfect-forwarding variadic args with `Args&&` and `std::tuple` correctly preserves types.

9. **C, D**  
   Expression SFINAE and `enable_if` prevent invalid instantiations.

10. **A, B**  
    `std::addressof` bypasses overloaded `operator&` to get the true address.

---

#### Detailed Design Answers

**Question 1: `CallWrapper` Implementation**

```cpp
template<typename Callable, typename Obj, typename... Args>
class CallWrapper {
    Callable callable_;
    Obj obj_;
    std::tuple<Args...> stored_args_;

public:
    CallWrapper(Callable&& c, Obj&& o, Args&&... args)
        : callable_(std::forward<Callable>(c)),
          obj_(std::forward<Obj>(o)),
          stored_args_(std::forward<Args>(args)...) {}

    template<typename... ExtraArgs>
    decltype(auto) operator()(ExtraArgs&&... extra_args) {
        return std::apply([&](auto&&... args) {
            return std::invoke(callable_, obj_,
                std::forward<decltype(args)>(args)...,
                std::forward<ExtraArgs>(extra_args)...);
        }, stored_args_);
    }
};

// Test case:
struct Widget { void paint(int a, std::string b) { 
    std::cout << a << " " << b << std::endl; 
}};
Widget w;
CallWrapper wrapper{&Widget::paint, &w, 42};
wrapper("blue"); // Output: 42 blue
```

**Explanation:**  
- Stores the member function pointer, object instance, and prefetched arguments.  
- Uses `std::apply` to unpack stored arguments and appends extra args via perfect forwarding.  
- `std::invoke` handles member function invocation correctly.

**Question 2: `type_switch` Implementation**

```cpp
template<typename... Ts>
class TypeSwitch {
    std::variant<Ts...> value_;

    template<typename T, typename Case>
    auto try_case(Case&& case_func) {
        if constexpr (std::is_same_v<decltype(value_), T>) {
            return case_func(std::get<T>(value_));
        }
    }

public:
    TypeSwitch(auto&& value) : value_(std::forward<decltype(value)>(value)) {}

    template<typename Case>
    auto case_(Case&& case_func) {
        return try_case<Case>(std::forward<Case>(case_func));
    }

    template<typename Default>
    auto default_(Default&& default_func) {
        return std::visit([&](auto&& val) {
            using T = decltype(val);
            if constexpr (!(std::is_same_v<T, Ts> || ...)) {
                default_func(val);
            }
        }, value_);
    }
};

// Test case:
std::variant<int, float> v = 3.14f;
TypeSwitch<int, float> ts(v);
ts.case_<int>([](int x) { std::cout << "int: " << x; })
  .case_<float>([](float y) { std::cout << "float: " << y; })
  .default_([](auto z) { std::cout << "unknown"; });
// Output: float: 3.14
```

**Explanation:**  
- Uses `std::variant` to hold possible types.  
- `try_case` uses `if constexpr` to check type matches.  
- `std::visit` handles variant dispatching.

**Question 3: `TypeList` Implementation**

```cpp
template<typename... Ts>
struct TypeList {
    template<template<typename> class Predicate>
    using filter = decltype(filter_impl<Predicate>(std::index_sequence_for<Ts...>{}));

    template<template<typename> class Transformer>
    using transform = TypeList<Transformer<Ts>...>;

private:
    template<template<typename> class Predicate, std::size_t... Is>
    static auto filter_impl(std::index_sequence<Is...>) {
        return TypeList<std::conditional_t<Predicate<std::tuple_element_t<Is, std::tuple<Ts...>>>::value,
            std::tuple_element_t<Is, std::tuple<Ts...>>, void>...>{};
    }
};

// Test case:
using MyList = TypeList<int, float, char>;
using Filtered = MyList::filter<std::is_integral>; // Contains int, char
using Transformed = MyList::transform<std::add_const>; // TypeList<const int, ...>
```

**Explanation:**  
- `filter_impl` uses `std::conditional_t` and index sequences to filter types.  
- `transform` directly applies the transformer to each type.

**Question 4: `ScopeGuard` Implementation**

```cpp
class ScopeGuard {
    std::function<void()> cleanup_;
    bool active_ = true;

public:
    template<typename Func>
    ScopeGuard(Func&& f) : cleanup_(std::forward<Func>(f)) {}

    void dismiss() { active_ = false; }

    ~ScopeGuard() { 
        if (active_) cleanup_(); 
    }
};

// Test case:
{
    FILE* f = fopen("test.txt", "w");
    ScopeGuard guard([&] { if (f) fclose(f); });
    // ... use file
    guard.dismiss(); // File remains open
} // File closed only if not dismissed
```

**Explanation:**  
- RAII ensures cleanup on scope exit.  
- `dismiss()` prevents cleanup when called.

**Question 5: `ThreadSafeQueue` Implementation**

```cpp
template<typename T>
class ThreadSafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;

public:
    template<typename... Args>
    void emplace(Args&&... args) {
        {
            std::lock_guard lock(mtx_);
            queue_.emplace(std::forward<Args>(args)...);
        }
        cv_.notify_one();
    }

    void pop(T& value) {
        std::unique_lock lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        value = std::move(queue_.front());
        queue_.pop();
    }
};

// Test case:
ThreadSafeQueue<std::string> q;
std::thread producer([&] { q.emplace("Hello", 3); });
std::string result;
std::thread consumer([&] { q.pop(result); });
producer.join(); consumer.join();
// result == "Hello3"
```

**Explanation:**  
- `emplace` perfect-forwards arguments to construct elements in-place.  
- `pop` uses condition variables to block until data is available.


### Summary of Key Points
1. **Uniform Callable Handling**: Use `std::invoke` to abstract function pointers, lambdas, and member functions.
2. **Type Traits**: Leverage `std::is_*` and `std::declval` for compile-time type introspection.
3. **Perfect Forwarding**: Preserve value categories with `std::forward` to avoid unnecessary copies.
4. **SFINAE**: Use `std::enable_if` to conditionally enable template overloads.
5. **Compile-Time Computation**: Utilize `constexpr` and variable templates for efficient metaprogramming.