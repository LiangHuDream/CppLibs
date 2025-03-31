@[TOC](Rvalue References, Move Semantics, and Perfect Forwarding)

---



### Mastering Chapter 5 of *Effective Modern C++*: Key Points and Difficulties

---

#### Item 23: Understand `std::move` and `std::forward`
Key Points:
- `std::move` unconditionally casts its argument to an rvalue (no runtime effect).
- `std::forward` conditionally casts to an rvalue only if the argument was initialized with an rvalue.
- Example:
  ```cpp
  template<typename T>
  void f(T&& param) {
      otherFunc(std::forward<T>(param)); // Forward param's value category
  }
  ```

Test Case:
```cpp
#include <iostream>

void process(int& x) { std::cout << "lvalue\n"; }
void process(int&& x) { std::cout << "rvalue\n"; }

template<typename T>
void forwarder(T&& param) {
    process(std::forward<T>(param));
}

int main() {
    int a = 0;
    forwarder(a);   // Output: lvalue
    forwarder(0);   // Output: rvalue
    return 0;
}
```

---

#### Item 24: Distinguish Universal References from Rvalue References
Key Points:
- Universal References (`T&&`):
  - Deduced as lvalue references if initialized with lvalues.
  - Deduced as rvalue references if initialized with rvalues.
- Rvalue References: Explicitly declared as `T&&` without type deduction.
- Example:
  ```cpp
  template<typename T>
  void f(T&& param); // Universal reference
  void g(int&& param); // Rvalue reference
  ```

---

#### Item 25: Use `std::move` on Rvalue References, `std::forward` on Universal References
Key Points:
- Rvalue References: Use `std::move` to enable move operations.
- Universal References: Use `std::forward` to preserve value category.
- Example:
  ```cpp
  class Widget {
  public:
      Widget(Widget&& rhs) : data(std::move(rhs.data)) {} // Move ctor
      template<typename T>
      void setData(T&& newData) { data = std::forward<T>(newData); } // Perfect forwarding
  };
  ```

---

#### Item 26: Avoid Overloading on Universal References
Key Points:
- Universal reference overloads can unexpectedly match all argument types.
- Example:
  ```cpp
  void logAndAdd(const std::string& name); // Safe
  template<typename T>
  void logAndAdd(T&& name); // Dangerous (may match unintended types)
  ```

---

#### Item 27: Familiarize Yourself with Alternatives to Overloading on Universal References
Key Points:
- Alternatives:
  - Tag dispatch (using `std::true_type`/`std::false_type`).
  - Constraining templates with `std::enable_if`.
- Example:
  ```cpp
  template<typename T>
  void logAndAdd(T&& name, std::false_type) { /* ... */ }
  void logAndAdd(int idx, std::true_type) { /* ... */ }
  ```

---

#### Item 28: Understand Reference Collapsing
Key Points:
- Reference Collapsing Rules:
  - `T& &&` → `T&`
  - `T&& &&` → `T&&`
- Example:
  ```cpp
  template<typename T>
  void f(T&& param) {
      // param's type is lvalue/rvalue reference based on T
  }
  ```

---

#### Item 29: Assume That Move Operations Are Not Present, Not Cheap, and Not Used
Key Points:
- Move operations may not be noexcept or cheaper than copies.
- Use `std::move` judiciously.

---

#### Item 30: Familiarize Yourself with Perfect Forwarding Failure Cases
Key Points:
- Perfect Forwarding Fails For:
  - Braced initializers.
  - `0`/`NULL` as null pointers.
  - Overloaded function names.
- Example:
  ```cpp
  template<typename T>
  void f(T&& param) { /* ... */ }
  f({1, 2, 3}); // Error: Braced initializer not forwarded
  ```

---

### Hard-Level Multiple-Choice Questions

1. Which of the following correctly forwards an rvalue reference?  
   A. `std::move`  
   B. `std::forward`  
   C. Both A and B  
   D. None  
   Answer: A  
   Explanation: `std::move` casts to rvalue unconditionally.

2. When does `T&&` become a universal reference?  
   A. When `T` is deduced.  
   B. When `T` is explicitly specified.  
   C. Always.  
   D. Never.  
   Answer: A  
   Explanation: Universal references require type deduction.

3. What is the result of `decltype((x))` for `int x`?  
   A. `int`  
   B. `int&`  
   C. `int&&`  
   D. `const int&`  
   Answer: B  
   Explanation: `(x)` is an lvalue expression.

4. Which of the following is a valid use of `std::forward`?  
   A. `std::forward<int>(x)` where `x` is an lvalue.  
   B. `std::forward<int>(x)` where `x` is an rvalue.  
   C. Both A and B.  
   D. None.  
   Answer: C  
   Explanation: `std::forward` preserves value category.

5. What happens if you overload a function with a universal reference?  
   A. It matches all argument types.  
   B. It causes compile errors.  
   C. It is more efficient.  
   D. It disables copy operations.  
   Answer: A  
   Explanation: Universal references match nearly all argument types.

6. Which keyword disables a function template for specific types?  
   A. `delete`  
   B. `override`  
   C. `enable_if`  
   D. `constexpr`  
   Answer: C  
   Explanation: `std::enable_if` conditionally disables templates.

7. What is the output of the following code?  
   ```cpp
   int x = 0;
   auto&& y = x;
   decltype(y) z = x;
   z = 10;
   std::cout << x << std::endl;
   ``` 
   A. `0`  
   B. `10`  
   C. Compile error  
   D. Undefined behavior  
   Answer: B  
   Explanation: `y` is an lvalue reference, so `z` is `int&`, modifying `x`.

8. Which of the following is a perfect forwarding failure case?  
   A. Braced initializers  
   B. `0` as a null pointer  
   C. Overloaded function names  
   D. All of the above  
   Answer: D  
   Explanation: All listed cases fail perfect forwarding.

9. When should you use `std::move` on a local variable?  
   A. When returning it from a function.  
   B. When passing it to another function and no longer using it.  
   C. Always.  
   D. Never.  
   Answer: B  
   Explanation: Use `std::move` only if the variable is no longer needed.

10. What is the purpose of reference collapsing?  
    A. To avoid compiler errors for reference-to-reference.  
    B. To enable universal references.  
    C. Both A and B.  
    D. None.  
    Answer: C  
    Explanation: Reference collapsing resolves reference-to-reference and enables universal references.

---

### Medium-Level Design Questions

1. Implement a function that forwards arguments to another function, preserving their value category.  
   Answer:  
   ```cpp
   template<typename F, typename... Args>
   auto forward_call(F&& f, Args&&... args) {
       return std::forward<F>(f)(std::forward<Args>(args)...);
   }
   ```

2. Design a class with move semantics that logs move operations.  
   Answer:  
   ```cpp
   class MoveLogger {
   public:
       MoveLogger() = default;
       MoveLogger(const MoveLogger&) { std::cout << "Copied\n"; }
       MoveLogger(MoveLogger&&) noexcept { std::cout << "Moved\n"; }
   };
   ```

3. Explain why the following code is problematic:  
   ```cpp
   template<typename T>
   void f(T&& param) { /* ... */ }
   f(0); // T is int
   f(nullptr); // T is std::nullptr_t
   ``` 
   Answer:  
   The function accepts `nullptr` as `std::nullptr_t`, which may not be intended. Use `std::enable_if` to constrain types.

4. Use tag dispatch to handle integral and non-integral arguments separately.  
   Answer:  
   ```cpp
   template<typename T>
   void handle(T&& arg, std::true_type) { /* Integral */ }
   template<typename T>
   void handle(T&& arg, std::false_type) { /* Non-integral */ }

   template<typename T>
   void wrapper(T&& arg) {
       handle(std::forward<T>(arg), std::is_integral<T>());
   }
   ```

5. Implement a perfect-forwarding constructor for a class.  
   Answer:  
   ```cpp
   class Widget {
   public:
       template<typename T>
       Widget(T&& data) : data_(std::forward<T>(data)) {}
   private:
       std::string data_;
   };
   ```