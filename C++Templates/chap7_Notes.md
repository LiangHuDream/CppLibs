
@[TOC](By Value or by Reference?)

---
### Key Concepts & Challenges:
---

**1. Passing by Value**
**When to Use:**
- Small, cheap-to-copy types (e.g., primitives).
- When you need a copy anyway.

**Code Example:**
```cpp
#include <iostream>

template<typename T>
void byValue(T val) {
    std::cout << "Value: " << val << std::endl;
}

int main() {
    int x = 42;
    byValue(x);       // OK: copies x
    byValue(100);     // OK: temporary
    return 0;
}
```
Output:
```
Value: 42
Value: 100
```
- Test Case: Works for both lvalues and rvalues. Copies occur.

---

**1. Passing by Reference**
**2.1 Const References**
```cpp
template<typename T>
void byConstRef(const T& ref) {
    std::cout << "Const Ref: " << ref << std::endl;
}

int main() {
    std::string s = "hello";
    byConstRef(s);        // No copy
    byConstRef("world");  // OK: binds to const&
    return 0;
}
```
Output:
```
Const Ref: hello
Const Ref: world
```
- Avoids copies but cannot modify the original.

**2.2 Non-Const References**
```cpp
template<typename T>
void modifyRef(T& ref) {
    ref += 10;
}

int main() {
    int x = 5;
    modifyRef(x);
    std::cout << "Modified: " << x << std::endl; // 15
    // modifyRef(5); // ERROR: cannot bind non-const lvalue ref to rvalue
    return 0;
}
```
- Only accepts lvalues. Fails for temporaries.

---

**3. Forwarding References (`T&&`)**
**Key Idea:**
- `T&&` binds to *both* lvalues and rvalues.
- Use with `std::forward` for perfect forwarding.

**Code Example:**
```cpp
#include <utility>

template<typename T>
void forwardExample(T&& arg) {
    // Forward preserves value category
    process(std::forward<T>(arg));
}

void process(int& x) { std::cout << "Lvalue: " << x << "\n"; }
void process(int&& x) { std::cout << "Rvalue: " << x << "\n"; }

int main() {
    int x = 10;
    forwardExample(x);       // Lvalue
    forwardExample(20);      // Rvalue
    return 0;
}
```
Output:
```
Lvalue: 10
Rvalue: 20
```
- Test Case: Correctly forwards lvalues/rvalues.

---

**4. `std::ref` and `std::cref`**
 **Purpose:**
- Wrap references to pass as values (e.g., to templates expecting copies).

**Code Example:**
```cpp
#include <functional>

template<typename T>
void useRef(T val) {
    val.get() = 99; // Modify through reference wrapper
}

int main() {
    int x = 42;
    useRef(std::ref(x));
    std::cout << "x after ref: " << x << "\n"; // 99
    return 0;
}
```
Output:
```
x after ref: 99
```
- Note: `std::ref` creates a `reference_wrapper`, allowing mutation.

---

**5. Handling String Literals/Raw Arrays**
**Problem:**
- Arrays decay to pointers; templates may not behave as expected.

**Solution:**
- Use type traits or specialization.

```cpp
#include <type_traits>
#include <iostream>

template<typename T>
void checkArray(T&& arr) {
    if constexpr (std::is_array_v<std::remove_reference_t<T>>) {
        std::cout << "Array size: " << std::extent_v<std::remove_reference_t<T>> << "\n";
    } else {
        std::cout << "Not an array\n";
    }
}

int main() {
    char str[] = "test";
    checkArray(str);       // Array size: 5 (includes null terminator)
    checkArray("hello");   // Array size: 6
    int x = 5;
    checkArray(x);         // Not an array
    return 0;
}
```
Output:
```
Array size: 5
Array size: 6
Not an array
```
- Test Case: Correctly identifies arrays vs. non-arrays.

---

### Multiple-Choice Questions

**Question 1: Perfect Forwarding**  
Which statements about `std::forward` are correct?  
A) It preserves the value category of the argument.  
B) It should only be used with forwarding references (`T&&`).  
C) It can replace `std::move` in all scenarios.  
D) It is unnecessary if the argument is passed by value.  

**Question 2: Reference Collapsing**  
Which combinations result in an lvalue reference after reference collapsing?  
A) `T& &`  
B) `T& &&`  
C) `T&& &`  
D) `T&& &&`  

**Question 3: `std::ref` Usage**  
When is `std::ref` necessary?  
A) To pass a reference to a template function expecting a value.  
B) To prevent implicit copying of large objects.  
C) To bind a non-const reference to a temporary.  
D) To modify an argument passed to a lambda by reference.  

**Question 4: String Literals and Arrays**  
Which techniques correctly handle string literals in templates?  
A) Using `std::decay_t` to convert to `const char*`.  
B) Specializing the template for array types.  
C) Passing by value to force type decay.  
D) Using `std::string_view` to avoid copies.  

**Question 5: Performance Tradeoffs**  
When is passing by value preferred over reference?  
A) For small trivially copyable types (e.g., `int`).  
B) When the function needs to modify the original object.  
C) For types with expensive copy constructors (e.g., `std::vector`).  
D) When perfect forwarding is required.  

**Question 6: Forwarding References**  
Which declarations are forwarding references?  
A) `template<typename T> void f(T&& param)`  
B) `void f(const int&& param)`  
C) `template<typename T> void f(std::vector<T>&& param)`  
D) `template<typename T> void f(T& param)`  

**Question 7: `std::cref` Behavior**  
What happens when `std::cref` is used with a mutable lambda?  
A) The lambda can modify the referenced object.  
B) The lambda cannot modify the referenced object.  
C) It creates a compile-time error.  
D) It creates a `const` reference wrapper.  

**Question 8: SFINAE and Parameter Passing**  
Which scenarios trigger SFINAE?  
A) A template parameter cannot deduce `T&` from an rvalue.  
B) A substitution failure in the return type.  
C) A mismatch in the number of template arguments.  
D) A `static_assert` failure in the function body.  

**Question 9: Move Semantics**  
Which statements about `std::move` are true?  
A) It converts an lvalue to an rvalue.  
B) It transfers ownership of resources.  
C) It should be used in conjunction with `std::forward`.  
D) It guarantees no copies are made.  

**Question 10: Type Deduction**  
Which parameter types allow deducing `T` as a reference?  
A) `template<typename T> void f(T param)`  
B) `template<typename T> void f(T& param)`  
C) `template<typename T> void f(T&& param)`  
D) `template<typename T> void f(const T param)`  

---

### Design Questions

**Design Question 1: Perfect Forwarding Factory**  
Create a template function `make_resource` that perfectly forwards arguments to a constructor of a class `Resource`. Ensure it handles lvalues, rvalues, and preserves `const`-correctness. Include test cases for `int`, `std::string`, and a temporary `Resource` object.

**Design Question 2: `std::ref`-Based Wrapper**  
Design a class `RefHolder` that stores a reference to any type using `std::ref` and allows mutation. Ensure it works with both lvalues and rvalues. Test with `int`, `std::string`, and a temporary object.

**Design Question 3: Array Type Detector**  
Implement a template function `is_array` that detects raw arrays (not `std::array`) and returns their size using type traits. Test with `int[5]`, `char[]`, and non-array types.

**Design Question 4: SFINAE-Enabled Function**  
Write a function `process` that is enabled only for arithmetic types (integers/floats). Use `std::enable_if` and test with `int`, `double`, and `std::string`.

**Design Question 5: Const-Correct Wrapper**  
Create a template class `ConstWrapper` that accepts any type and ensures it cannot modify the wrapped object, even if the original is non-`const`. Test with mutable and `const` objects.

---

### Answers & Explanations (Multiple-Choice)

**Question 1**  
Correct: A, B  
Explanation:  
- `std::forward` preserves value categories (A).  
- It is only meaningful with forwarding references (B).  
- `std::move` unconditionally casts to rvalue (C is false).  

**Question 2**  
Correct: A, B  
Explanation:  
- `T& &` → `T&` (A).  
- `T& &&` → `T&` (B).  
- `T&& &` → `T&` (not listed here).  
- `T&& &&` → `T&&` (D is rvalue).  

**Question 3**  
Correct: A, D  
Explanation:  
- `std::ref` wraps references for templates expecting values (A).  
- Lambdas capturing by reference need `std::ref` (D).  

**Question 4**  
Correct: B, D  
Explanation:  
- Specializing for arrays (B) and `std::string_view` (D) avoid decay issues.  

**Question 5**  
Correct: A, D  
Explanation:  
- Small types (A) and perfect forwarding (D) favor by-value passing.  

**Question 6**  
Correct: A  
Explanation:  
- Only `T&&` without qualifiers is a forwarding reference (A).  

**Question 7**  
Correct: B, D  
Explanation:  
- `std::cref` creates a `const` wrapper (D), preventing modification (B).  

**Question 8**  
Correct: A, B  
Explanation:  
- SFINAE occurs during substitution (A, B). `static_assert` is a hard error (D).  

**Question 9**  
Correct: A, B  
Explanation:  
- `std::move` casts to rvalue (A) and enables moves (B).  

**Question 10**  
Correct: B, C  
Explanation:  
- `T&` (B) and `T&&` (C) can deduce references.  

---

### Answers & Explanations (Design Questions)

**Design Question 1**  
Solution:  
```cpp 
#include <utility>

class Resource {
public:
    Resource(int x, const std::string& s) {}
    Resource(const Resource&) = delete;
    Resource(Resource&&) = default;
};

template<typename... Args>
Resource make_resource(Args&&... args) {
    return Resource(std::forward<Args>(args)...);
}

int main() {
    int x = 10;
    auto r1 = make_resource(x, "test"); // lvalue
    auto r2 = make_resource(20, std::string("temp")); // rvalue
}
```

**Design Question 2**  
Solution:  
```cpp
#include <functional>

template<typename T>
class RefHolder {
    std::reference_wrapper<T> ref;
public:
    RefHolder(T& t) : ref(t) {}
    void set(const T& value) { ref.get() = value; }
};

int main() {
    int x = 5;
    std::string s = "hello";
    RefHolder<int> rh1(x);
    rh1.set(10); // x = 10
    RefHolder<std::string> rh2(s);
    rh2.set("world");
}
```

**Design Question 3**  
Solution:  
```cpp
#include <type_traits>

template<typename T>
void is_array() {
    if constexpr (std::is_array_v<T>) {
        std::cout << "Array size: " << std::extent_v<T> << '\n';
    } else {
        std::cout << "Not an array\n";
    }
}

int main() {
    is_array<int[5]>();  // Size: 5
    is_array<char[]>();  // Size: 0 (incomplete)
    is_array<int>();     // Not an array
}
```

**Design Question 4**  
Solution:  
```cpp
#include <type_traits>

template<typename T>
typename std::enable_if_t<std::is_arithmetic_v<T>, void>
process(T t) {
    std::cout << "Processing arithmetic type\n";
}

int main() {
    process(10);        // OK
    process(3.14);      // OK
    // process("test"); // Error
}
```

**Design Question 5**  
Solution:  
```cpp
template<typename T>
class ConstWrapper {
    const T& ref;
public:
    ConstWrapper(const T& t) : ref(t) {}
    // No non-const member functions
};

int main() {
    int x = 5;
    const int y = 10;
    ConstWrapper<int> w1(x); // x is treated as const
    ConstWrapper<const int> w2(y);
}
```

### Summary of Key Points:
1. By Value: Simple but may copy. Use for small types.
2. By Reference: Avoid copies; use `const T&` for read-only, `T&` for modification.
3. Forwarding References: Enable perfect forwarding with `T&&` and `std::forward`.
4. `std::ref`/`std::cref`: Pass references when templates expect values.
5. Arrays/Strings: Use type traits to handle decay correctly.
