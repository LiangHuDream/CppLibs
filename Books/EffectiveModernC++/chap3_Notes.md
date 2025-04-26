

---

#### Item 7: Distinguish Between `()` and `{}` When Creating Objects
Key Points:
- Braced Initialization (`{}`):
  - Prohibits narrowing conversions (e.g., `int x{5.5};` is invalid).
  - Immunizes against the "most vexing parse" (e.g., `Widget w();` is an object, not a function).
  - Preferentially matches `std::initializer_list` constructors.
- Parentheses Initialization (`()`):
  - Traditional initialization, may allow implicit conversions.
- Example:
  ```cpp
  std::vector<int> v1(10, 20); // 10 elements, all 20
  std::vector<int> v2{10, 20}; // 2 elements, 10 and 20
  ```

Test Case:
```cpp
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v1(10, 20);
    std::vector<int> v2{10, 20};
    std::cout << "v1 size: " << v1.size() << ", v2 size: " << v2.size() << std::endl;
    return 0;
}
```
Output:
```
v1 size: 10, v2 size: 2
```

---

#### Item 8: Prefer `nullptr` to `0` and `NULL`
Key Points:
- `nullptr` is type-safe and avoids overload resolution ambiguities.
- Example:
  ```cpp
  void f(int) { std::cout << "int\n"; }
  void f(void*) { std::cout << "void*\n"; }
  f(nullptr); // Calls f(void*)
  ```

Test Case:
```cpp
#include <iostream>

void f(int) { std::cout << "int\n"; }
void f(void*) { std::cout << "void*\n"; }

int main() {
    f(nullptr); // Output: void*
    return 0;
}
```

---

#### Item 9: Prefer Alias Declarations to `typedef`
Key Points:
- Alias Templates (via `using`) support template parameters and avoid `typename` prefixes.
- Example:
  ```cpp
  template<typename T>
  using MyList = std::list<T, MyAllocator<T>>;
  ```

---

#### Item 10: Prefer Scoped Enums to Unscoped Enums
Key Points:
- Scoped Enums (`enum class`):
  - Enumerators are scope-qualified (e.g., `Color::Red`).
  - No implicit conversion to integers (requires `static_cast`).
- Example:
  ```cpp
  enum class Color { Red, Blue };
  Color c = Color::Red;
  ```

---

#### Item 11: Prefer Deleted Functions to Private Undefined Ones
Key Points:
- Deleted Functions (`= delete`):
  - Compiler errors at call site (not link-time).
  - Apply to non-member functions and template specializations.
- Example:
  ```cpp
  class Widget {
  public:
      Widget(Widget&&) = delete; // Prohibit move construction
  };
  ```

---

#### Item 12: Declare Overriding Functions `override`
Key Points:
- `override` ensures a derived function correctly overrides a base class virtual function.
- Example:
  ```cpp
  class Base {
  public:
      virtual void f() {}
  };
  class Derived : public Base {
  public:
      void f() override {} // Correct override
  };
  ```

---

#### Item 13: Prefer `const_iterators` to `iterators`
Key Points:
- `const_iterators` ensure read-only access to container elements.
- Use `cbegin()` and `cend()` for non-const containers.
- Example:
  ```cpp
  std::vector<int> v = {1, 2, 3};
  for (auto it = v.cbegin(); it != v.cend(); ++it) {
      // Read-only operations
  }
  ```

---

#### Item 14: Declare Functions `noexcept` If They Won’t Emit Exceptions
Key Points:
- `noexcept` hints to compilers for optimizations (e.g., avoiding stack unwinding).
- Critical for move operations, `swap`, and destructors.
- Example:
  ```cpp
  void swap(Widget& a, Widget& b) noexcept {
      // No exceptions thrown
  }
  ```

---

#### Item 15: Use `constexpr` Whenever Possible
Key Points:
- `constexpr` enables compile-time computation for constants, array sizes, and template arguments.
- Example:
  ```cpp
  constexpr int pow(int base, unsigned exp) noexcept {
      return exp == 0 ? 1 : base * pow(base, exp - 1);
  }
  std::array<int, pow(2, 5)> arr; // Compile-time size
  ```

---

#### Item 16: Make `const` Member Functions Thread Safe
Key Points:
- Use `std::mutex` or `std::atomic` to protect shared data in `const` member functions.
- Example:
  ```cpp
  class CachedData {
  public:
      int getValue() const {
          std::lock_guard<std::mutex> lock(mutex_);
          if (!cacheValid_) {
              computeValue();
              cacheValid_ = true;
          }
          return cachedValue_;
      }
  private:
      mutable std::mutex mutex_;
      mutable int cachedValue_;
      mutable bool cacheValid_ = false;
  };
  ```

---

#### Item 17: Understand Special Member Function Generation
Key Points:
- Move Operations:
  - Compiler-generated if no copy/move operations or destructor are declared.
  - Overrides copy operations if declared.
- Example:
  ```cpp
  class Widget {
      // No declared copy/move/destructor → compiler generates them
  };
  ```

---

### Hard-Level Multiple-Choice Questions

1. Which initialization syntax guarantees no narrowing conversions?  
   A. `()`  
   B. `{}`  
   C. `=`  
   D. `auto`  
   Answer: B  
   Explanation: Braced initialization (`{}`) prohibits narrowing conversions.

2. What is the primary advantage of `nullptr` over `0` or `NULL`?  
   A. Shorter syntax  
   B. Type safety  
   C. Faster execution  
   D. Compatibility with C code  
   Answer: B  
   Explanation: `nullptr` avoids ambiguous overload resolution.

3. Which keyword ensures a derived function overrides a base class virtual function?  
   A. `virtual`  
   B. `override`  
   C. `final`  
   D. `const`  
   Answer: B  
   Explanation: `override` forces compiler checks for correct overriding.

4. What is the default underlying type for a scoped enum (`enum class`)?  
   A. `int`  
   B. `unsigned`  
   C. `size_t`  
   D. Depends on the implementation  
   Answer: A  
   Explanation: Scoped enums default to `int` unless specified.

5. Which of the following disables a function at compile time?  
   A. `private: void f();`  
   B. `void f() = delete;`  
   C. `void f() noexcept;`  
   D. `void f() override;`  
   Answer: B  
   Explanation: `= delete` marks a function as unavailable.

6. When should `const_iterators` be preferred over `iterators`?  
   A. When modifying container elements  
   B. When reading container elements  
   C. When using range-based `for` loops  
   D. Always  
   Answer: B  
   Explanation: `const_iterators` ensure read-only access.

7. What does `noexcept` imply about a function?  
   A. It cannot throw exceptions.  
   B. It may throw exceptions but is unlikely to.  
   C. It hints for compiler optimizations.  
   D. Both A and C.  
   Answer: C  
   Explanation: `noexcept` hints for optimizations but doesn’t guarantee no exceptions.

8. Which keyword allows a function to be evaluated at compile time?  
   A. `const`  
   B. `noexcept`  
   C. `constexpr`  
   D. `static`  
   Answer: C  
   Explanation: `constexpr` functions can be evaluated at compile time.

9. Which of the following disables copy operations in C++11?  
   A. Declaring a move constructor  
   B. Declaring a destructor  
   C. Both A and B  
   D. None of the above  
   Answer: C  
   Explanation: Declaring a move operation or destructor disables copy operations.

10. What is the purpose of `std::initializer_list` in constructor overloading?  
    A. To handle braced initialization  
    B. To enable perfect forwarding  
    C. To optimize copy operations  
    D. To avoid narrowing conversions  
    Answer: A  
    Explanation: Braced initializers prefer `std::initializer_list` constructors.

---

### Medium-Level Design Questions

1. Design a class that cannot be copied or moved.  
   Answer:  
   ```cpp
   class NonCopyable {
   public:
       NonCopyable() = default;
       ~NonCopyable() = default;
       NonCopyable(const NonCopyable&) = delete;
       NonCopyable& operator=(const NonCopyable&) = delete;
       NonCopyable(NonCopyable&&) = delete;
       NonCopyable& operator=(NonCopyable&&) = delete;
   };
   ```

2. Implement a thread-safe cache using `constexpr` and `noexcept`.  
   Answer:  
   ```cpp
   #include <mutex>

   class ThreadSafeCache {
   public:
       int getValue() const noexcept {
           std::lock_guard<std::mutex> lock(mutex_);
           return cachedValue_;
       }
       void setValue(int value) noexcept {
           std::lock_guard<std::mutex> lock(mutex_);
           cachedValue_ = value;
       }
   private:
       mutable std::mutex mutex_;
       int cachedValue_ = 0;
   };
   ```

3. Write a `constexpr` function to compute the factorial of a number.  
   Answer:  
   ```cpp
   constexpr int factorial(int n) noexcept {
       return n <= 1 ? 1 : n * factorial(n - 1);
   }
   ```

4. Use a scoped enum to represent days of the week and write a function to convert them to strings.  
   Answer:  
   ```cpp
   enum class Day { Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday };

   const char* dayToString(Day d) {
       switch (d) {
           case Day::Monday: return "Monday";
           case Day::Tuesday: return "Tuesday";
           // ... other cases
       }
       return "Unknown";
   }
   ```

5. Explain why the following code compiles but may not behave as expected:  
   ```cpp
   std::vector<int> v{10, 20}; // Uses std::initializer_list constructor
   ```  
   Answer:  
   The code creates a `std::vector<int>` with two elements (10 and 20) instead of 10 elements with value 20. Braced initialization prefers the `std::initializer_list` constructor over the size-and-value constructor.