@[TOC](Deducing Types)
#### 1. Template Type Deduction (Item 1)
- Three Cases:
  1. `ParamType` is a reference/pointer (not universal reference):
     - Ignore references in `expr`; match remaining type to `ParamType`.
     ```cpp
     template<typename T>
     void f(T& param);
     const int cx = 0;
     f(cx); // T = const int, param = const int&
     ```
  2. `ParamType` is a universal reference (`T&&`):
     - Lvalues deduce `T` as lvalue reference; rvalues deduce `T` as non-reference.
     ```cpp
     template<typename T>
     void f(T&& param);
     int x = 0;
     f(x);  // T = int&, param = int& (lvalue)
     f(27); // T = int, param = int&& (rvalue)
     ```
  3. `ParamType` is by-value (neither pointer nor reference):
     - Strip `const`/`volatile`; decay arrays/functions to pointers.
     ```cpp
     template<typename T>
     void f(T param);
     const char arr[] = "test";
     f(arr); // T = const char* (array decays to pointer)
     ```

---

#### 2. Auto Type Deduction (Item 2)
- Follows template deduction rules except for braced initializers:
  ```cpp
  auto x = {1, 2, 3}; // x = std::initializer_list<int>
  template<typename T> void f(T param);
  f({1, 2, 3}); // Error: cannot deduce T
  ```

---

#### 3. Decltype (Item 3)
- `decltype` returns the declared type of an entity or expression:
  ```cpp
  int x = 0;
  decltype(x) y = x;    // y = int
  decltype((x)) z = y;  // z = int& (expression (x) is lvalue)
  ```

---

#### 4. Viewing Deduced Types (Item 4)
- Use `typeid` (may strip references) or `Boost.TypeIndex`:
  ```cpp
  #include <boost/type_index.hpp>
  template<typename T>
  void printType() {
      std::cout << boost::typeindex::type_id_with_cvr<T>().pretty_name();
  }
  ```

---

### Multiple-Choice Questions

1. What is the type of `param` in `template<typename T> void f(T&& param)` when called with `int x = 0; f(x);`?
   - A) `int`
   - B) `int&`
   - C) `int&&`
   - D) `const int&`
   
   Answer: B) `int&`  
   Explanation: `x` is an lvalue; universal reference deduces `T` as `int&`.

---

2. Given `const char name[] = "Hello"; auto arr = name;`, what is `arr`'s type?
   - A) `const char*`
   - B) `const char[6]`
   - C) `const char(&)[6]`
   - D) `char*`
   
   Answer: A) `const char*`  
   Explanation: Arrays decay to pointers in by-value `auto` deduction.

---

3. Which code causes a compilation error?
   ```cpp
   // 1: auto x = {1}; 
   // 2: template<typename T> void f(T param); f({1});
   ```
   - A) 1
   - B) 2
   - C) Both
   - D) Neither
   
   Answer: B) 2  
   Explanation: Braced initializers cannot deduce `T` in templates.

---

4. What is `decltype((x))` if `int x = 0;`?
   - A) `int`
   - B) `int&`
   - C) `int&&`
   - D) `const int`
   
   Answer: B) `int&`  
   Explanation: `(x)` is an lvalue expression; `decltype` yields `int&`.

---

5. Which code deduces `T` as `int[3]`?
   ```cpp
   int arr[3];
   // 1: template<typename T> void f(T& param); f(arr);
   // 2: template<typename T> void f(T param); f(arr);
   ```
   - A) 1
   - B) 2
   - C) Both
   - D) Neither
   
   Answer: A) 1  
   Explanation: References preserve array types; by-value decays to pointer.

---

### Design Questions

1. Write a template function `arraySize` returning the size of an array.
   ```cpp
   template<typename T, size_t N>
   constexpr size_t arraySize(T (&)[N]) { return N; }
   
   int main() {
       int arr[] = {1, 2, 3};
       static_assert(arraySize(arr) == 3);
   }
   ```

---

2. Use `decltype` to declare a reference variable `y` bound to `x` without `auto&`.
   ```cpp
   int x = 10;
   decltype((x)) y = x; // y = int&
   ```

---

3. Fix the error in `print({1, 2, 3})`:
   ```cpp
   // Original (error):
   template<typename T> void print(T param) {}
   print({1, 2, 3});
   
   // Fixed:
   void print(std::initializer_list<int> param) {}
   ```

---

4. Design a `forwarder` to perfectly forward arguments.
   ```cpp
   template<typename F, typename... Args>
   auto forwarder(F&& func, Args&&... args) {
       return std::forward<F>(func)(std::forward<Args>(args)...);
   }
   ```

---

5. Create a type trait `IsReference` to detect references.
   ```cpp
   template<typename T>
   struct IsReference : std::false_type {};
   
   template<typename T>
   struct IsReference<T&> : std::true_type {};
   
   template<typename T>
   struct IsReference<T&&> : std::true_type {};
   ```

---

### Test Cases

#### Test 1: Array Decay
```cpp
#include <iostream>
#include <type_traits>

template<typename T>
void checkArray(T param) {
    if (std::is_array_v<T>)
        std::cout << "Array preserved!\n";
    else
        std::cout << "Array decayed to pointer.\n";
}

int main() {
    int arr[3];
    checkArray(arr); // Output: "Array decayed to pointer."
}
```

#### Test 2: Universal Reference Deduction
```cpp
#include <iostream>
#include <type_traits>

template<typename T>
void checkRef(T&& param) {
    if (std::is_lvalue_reference_v<T&&>)
        std::cout << "Lvalue reference\n";
    else
        std::cout << "Rvalue reference\n";
}

int main() {
    int x = 0;
    checkRef(x);  // Lvalue reference
    checkRef(27); // Rvalue reference
}
```

#### Test 3: decltype Behavior
```cpp
#include <iostream>
#include <type_traits>

int main() {
    int x = 0;
    decltype((x)) y = x; // y = int&
    static_assert(std::is_same_v<decltype(y), int&>);
}
```

---

### Answers and Explanations

#### Multiple-Choice Answers
1. B - Universal reference with lvalue deduces to lvalue reference.
2. A - By-value `auto` decays arrays to pointers.
3. B - Braced initializers cannot deduce template types.
4. B - `decltype` on lvalue expression yields reference.
5. A - References preserve array types.

#### Design Answers
1. `arraySize` uses reference to deduce array size.
2. `decltype((x))` captures lvalue reference.
3. Explicit `std::initializer_list` fixes template deduction.
4. `forwarder` uses perfect forwarding.
5. `IsReference` specializes for `T&` and `T&&`.