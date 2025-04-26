
### Chapter 6: Move Semantics and enable_if<>

---

### Key points and difficulties of Move Semantics and enable_if<>

**1. Perfect Forwarding**
Purpose: Preserve the value category (lvalue/rvalue) of arguments when passing them through wrapper functions.  
Key Tool: `std::forward<T>()`.

Code Example
```cpp
#include <iostream>
#include <utility>

// Wrapper function using perfect forwarding
template<typename T>
void wrapper(T&& arg) {
    std::cout << "Forwarding: ";
    worker(std::forward<T>(arg)); // Preserve value category
}

// Worker functions
void worker(int& x) { std::cout << "lvalue\n"; }
void worker(int&& x) { std::cout << "rvalue\n"; }

// Test Cases
int main() {
    int a = 5;
    wrapper(a);       // lvalue
    wrapper(10);      // rvalue
    wrapper(std::move(a)); // rvalue
}
```

Output:
```
Forwarding: lvalue
Forwarding: rvalue
Forwarding: rvalue
```

Explanation:
- `wrapper` uses a forwarding reference (`T&&`).
- `std::forward<T>` ensures the original value category (lvalue/rvalue) is preserved when calling `worker`.

---

**2. Special Member Function Templates**
Problem: Template constructors/assignment operators may override compiler-generated defaults, causing unexpected behavior.

Code Example (Problem Case)
```cpp
#include <iostream>

class Widget {
public:
    // Compiler-generated default constructor
    Widget() = default;

    // Templated constructor (problematic!)
    template<typename T>
    Widget(const T&) { 
        std::cout << "Templated constructor\n"; 
    }
};

// Test Case
int main() {
    Widget w1;           // OK: default constructor
    Widget w2(w1);      // Uses templated constructor, NOT copy constructor!
}
```

Output:
```
Templated constructor
```

Explanation:
- The templated constructor matches `Widget(const Widget&)` better than the compiler-generated copy constructor, causing unexpected behavior.

---

**3. `enable_if<>` to Conditionally Disable Templates**
Purpose: Disable template instantiations that would cause errors or unwanted behavior.  
Key Tool: SFINAE (Substitution Failure Is Not An Error).

Code Example (Solution)
```cpp
#include <iostream>
#include <type_traits>

class Widget {
public:
    Widget() = default;

    // Enable this constructor only if T is not Widget
    template<typename T, typename = std::enable_if_t<!std::is_same_v<T, Widget>>>
    Widget(const T&) { 
        std::cout << "Templated constructor\n"; 
    }

    // Compiler-generated copy constructor
    Widget(const Widget&) = default;
};

// Test Cases
int main() {
    Widget w1;           // Default constructor
    Widget w2(w1);      // Copy constructor (now works!)
    Widget w3(42);       // Templated constructor
}
```

Output:
```
Templated constructor
```

Explanation:
- `std::enable_if_t` disables the templated constructor when `T = Widget`.
- The compiler-generated copy constructor is now correctly prioritized.

---

Key Takeaways
1. Perfect Forwarding:
   - Use `T&&` (forwarding reference) and `std::forward<T>`.
   - Preserves move semantics in wrapper functions.

2. Special Member Templates:
   - May override compiler-generated defaults.
   - Use `enable_if<>` to avoid conflicts.

3. `enable_if<>`:
   - Disables invalid template instantiations via SFINAE.
   - Critical for template metaprogramming and API design.

### Test Case Summary
| Test Case                   | Behavior                          |
|-----------------------------|-----------------------------------|
| `wrapper(a)`                | Correctly forwards lvalue        |
| `wrapper(10)`               | Correctly forwards rvalue        |
| `Widget w2(w1)` (fixed)     | Uses copy constructor            |
| `Widget w3(42)`             | Uses templated constructor       |

---

### Multiple-Choice Questions

**Question 1: Perfect Forwarding**
Which statements about `std::forward` are true?  
A) It preserves the value category (lvalue/rvalue) of arguments.  
B) It converts any argument to an rvalue.  
C) It is only useful in template functions with forwarding references (`T&&`).  
D) It can be replaced with `std::move` in generic code without breaking functionality.  

**Question 2: Forwarding References**  
Which of the following are forwarding references?  
A) `template<typename T> void f(T&& arg)`  
B) `template<typename T> void f(const T&& arg)`  
C) `void f(int&& arg)`  
D) `template<typename T> void f(std::vector<T>&& arg)`  

**Question 3: Special Member Function Templates**  
Why might a templated constructor cause unexpected behavior?  
A) It overrides the compiler-generated copy constructor.  
B) It is more specialized than the default copy constructor.  
C) It cannot accept lvalue arguments.  
D) It prevents implicit type conversions.  

**Question 4: `enable_if` and SFINAE**  
Which scenarios correctly use `enable_if`?  
A) Disabling a template when `T` is not integral.  
B) Enforcing runtime type checks.  
C) Conditionally overloading a function based on traits.  
D) Preventing template instantiation for polymorphic classes.  

**Question 5: Move Semantics**  
Which code snippets correctly use move semantics?  
A) `std::string s1 = std::move(s2);`  
B) `void f(int&& x) { int y = x; }`  
C) `template<typename T> void f(T&& arg) { T copy = std::forward<T>(arg); }`  
D) `std::vector<int> v = std::move(other_vec);`  

**Question 6: Reference Collapsing**  
Which types result from reference collapsing?  
A) `T& &` → `T&`  
B) `T&& &` → `T&`  
C) `T&& &&` → `T&&`  
D) `T& &&` → `T&&`  

**Question 7: SFINAE Principles**  
Which examples demonstrate SFINAE?  
A) A template substitution error that removes a candidate from the overload set.  
B) A compiler error due to invalid syntax.  
C) Using `enable_if` to disable a template overload.  
D) A runtime error caused by invalid type usage.  

**Question 8: `enable_if` Placement**  
Where can `enable_if` be applied?  
A) In template parameter lists.  
B) As a function parameter default argument.  
C) In the function return type.  
D) Inside the function body.  

**Question 9: Move Constructor**  
When is a move constructor called?  
A) When an object is returned by value.  
B) When an object is copied.  
C) When `std::move` is explicitly used.  
D) When an object is thrown in an exception.  

**Question 10: `std::forward` Mechanics**  
What does `std::forward<T>(arg)` do?  
A) Casts `arg` to `T&&` unconditionally.  
B) Casts `arg` to `T&` if `T` is an lvalue reference.  
C) Preserves the original value category of `arg`.  
D) Only works with rvalue references.  

---

### Design Questions

**Design Question 1: Perfect Forwarding Wrapper**  
Design a function `forward_wrapper` that perfectly forwards arguments to `std::make_shared<T>`. Ensure it works for both lvalues and rvalues. Include test cases in `main` to verify forwarding.  

**Design Question 2: `enable_if`-Based Constructor**  
Create a class `Resource` with a templated constructor that is enabled only if the input is a `std::string` or `const char*`. Disable it for other types using `enable_if`. Test with `int`, `std::string`, and `const char*` arguments.  

**Design Question 3: SFINAE-Based Overload**  
Implement a function `log` that:  
- Calls `log(int)` if the input is an integral type.  
- Calls `log(double)` if the input is a floating-point type.  
Use `enable_if` and type traits. Test with `int`, `double`, and `std::string`.  

**Design Question 4: Move-Aware Container**  
Design a class `Buffer` with move semantics:  
- Move constructor/assignment transfers ownership of a dynamic array.  
- Copy operations are disabled.  
Include test cases to validate moves and prevent copies.  

**Design Question 5: Variadic Perfect Forwarding**  
Create a template function `emplace_back_wrapper` that accepts variadic arguments and forwards them to `emplace_back` of a `std::vector`. Test with lvalues, rvalues, and mixed arguments.  

---

### Answers & Explanations (Multiple-Choice)

**Question 1**  
Correct: A, C  
Explanation:  
- `std::forward` preserves value categories (A).  
- It requires a forwarding reference (`T&&`) to work correctly (C).  
- Using `std::move` would always convert to rvalue, breaking lvalue cases (D is false).  

**Question 2**  
Correct: A  
Explanation:  
- Only `T&&` without `const` is a forwarding reference (A).  
- `const T&&` (B) and `std::vector<T>&&` (D) are rvalue references, not forwarding references.  

**Question 3**  
Correct: A, B  
Explanation:  
- Templated constructors can override compiler-generated ones (A).  
- They are more specialized, so the compiler prefers them (B).  

**Question 4**  
Correct: A, C  
Explanation:  
- `enable_if` disables templates based on type traits (A, C).  
- It is a compile-time mechanism (B and D are unrelated).  

**Question 5**  
Correct: A, D  
Explanation:  
- `std::move` transfers ownership (A, D).  
- Using `x` after `std::forward` in (C) may cause undefined behavior.  

**Question 6**  
Correct: A, B, C, D  
Explanation:  
- Reference collapsing rules apply to all combinations (A-D are all valid).  

**Question 7**  
Correct: A, C  
Explanation:  
- SFINAE removes invalid candidates (A).  
- `enable_if` uses SFINAE to disable templates (C).  

**Question 8**  
Correct: A, B, C  
Explanation:  
- `enable_if` can be used in template parameters (A), function parameters (B), or return types (C).  

**Question 9**  
Correct: A, C, D  
Explanation:  
- Move constructors are called for rvalues (A, C, D).  

**Question 10**  
Correct: B, C  
Explanation:  
- `std::forward` casts to `T&` if `T` is an lvalue reference (B).  
- It preserves the original value category (C).  

---

### Answers & Explanations (Design Questions)

**Design Question 1**  
Solution:  
```cpp
#include <memory>
#include <utility>

template<typename T, typename... Args>
auto forward_wrapper(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// Test in main
int main() {
    auto p1 = forward_wrapper<std::string>("test"); // rvalue
    std::string s = "hello";
    auto p2 = forward_wrapper<std::string>(s);      // lvalue
}
```

**Design Question 2**  
Solution:  
```cpp
#include <type_traits>
#include <string>

class Resource {
public:
    template<typename T, typename = std::enable_if_t<
        std::is_same_v<T, std::string> || std::is_same_v<T, const char*>
    >>
    Resource(T&& arg) {}

    // Test in main
int main() {
    Resource r1("test");          // OK
    Resource r2(std::string("s"));// OK
    // Resource r3(42);           // Fails (disabled)
}
```

**Design Question 3**  
Solution:  
```cpp
#include <type_traits>

void log(int) {}
void log(double) {}

template<typename T>
auto log(T x) -> std::enable_if_t<std::is_integral_v<T>> {
    log(static_cast<int>(x));
}

template<typename T>
auto log(T x) -> std::enable_if_t<std::is_floating_point_v<T>> {
    log(static_cast<double>(x));
}

// Test in main
int main() {
    log(10);     // Calls log(int)
    log(3.14);   // Calls log(double)
    // log("s"); // Fails (no overload)
}
```

**Design Question 4**  
Solution:  
```cpp
class Buffer {
    int* data;
public:
    Buffer() : data(new int[100]) {}
    ~Buffer() { delete[] data; }

    Buffer(Buffer&& other) : data(other.data) { other.data = nullptr; }
    Buffer& operator=(Buffer&& other) {
        delete[] data;
        data = other.data;
        other.data = nullptr;
        return *this;
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
};

// Test in main
int main() {
    Buffer b1;
    Buffer b2 = std::move(b1); // Move OK
    // Buffer b3 = b1;         // Error (copy deleted)
}
```

**Design Question 5**  
Solution:  
```cpp
#include <vector>
#include <utility>

template<typename T, typename... Args>
void emplace_back_wrapper(std::vector<T>& vec, Args&&... args) {
    vec.emplace_back(std::forward<Args>(args)...);
}

// Test in main
int main() {
    std::vector<std::string> vec;
    std::string s = "temp";
    emplace_back_wrapper(vec, "test");    // rvalue
    emplace_back_wrapper(vec, s);         // lvalue
    emplace_back_wrapper(vec, std::move(s)); // rvalue
}
```

These examples and tests reinforce the chapter’s key concepts. Each design question addresses a real-world use case, and the MCQs test deep understanding of mechanics.