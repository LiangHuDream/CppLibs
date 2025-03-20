### Chapter 1 Key Concepts and Difficulties

#### 1. Function Template Basics
- Definition: Function templates enable the creation of a single function that can operate on multiple data types.
- Example:
  ```cpp
  template<typename T>
  T max(T a, T b) {
      return (a > b) ? a : b;
  }
  ```
  Explanation: The `max` function template takes two parameters of the same type `T` and returns the larger one. The `typename` keyword declares a type parameter.
  Test Case:
  ```cpp
  int main() {
      std::cout << max(5, 10) << std::endl;   // Output: 10
      std::cout << max(3.14, 2.71) << std::endl; // Output: 3.14
      return 0;
  }
  ```

#### 2. Template Argument Deduction
- Automatic Deduction: The compiler infers the template parameter `T` from the function arguments.
- Restrictions:
  - No implicit conversions for reference parameters.
  - Value parameters allow decay (e.g., arrays to pointers).
- Example:
  ```cpp
  template<typename T>
  void print(T value) {
      std::cout << value << std::endl;
  }
  ```
  Test Case:
  ```cpp
  int main() {
      int arr[] = {1, 2, 3};
      print(arr);  // T deduced as int* (array decays to pointer)
      return 0;
  }
  ```

#### 3. Multiple Template Parameters
- Multi - Type Parameters: Function templates can have multiple type parameters.
- Return Type Deduction:
  - C++14: Use `auto` for the return type.
  - C++11: Use `decltype` with a trailing return type.
- Example:
  ```cpp
  template<typename T1, typename T2>
  auto max(T1 a, T2 b) {
      return (a > b) ? a : b;
  }
  ```
  Test Case:
  ```cpp
  int main() {
      auto result = max(5, 3.14);  // T1 = int, T2 = double
      std::cout << result << std::endl;  // Output: 5
      return 0;
  }
  ```

#### 4. Default Template Arguments
- Syntax: Default values can be provided for template parameters.
- Example:
  ```cpp
  template<typename T = int>
  T add(T a, T b) {
      return a + b;
  }
  ```
  Test Case:
  ```cpp
  int main() {
      std::cout << add(3, 5) << std::endl;       // T = int (deduced)
      std::cout << add<>(3.1, 4.2) << std::endl; // T = double (default)
      return 0;
  }
  ```

#### 5. Overloading Function Templates
- Rules:
  - Non - template functions take precedence over template functions.
  - More specialized templates are preferred.
- Example:
  ```cpp
  int max(int a, int b) {
      return a > b ? a : b;
  }
  template<typename T>
  T max(T a, T b) {
      return (a > b) ? a : b;
  }
  ```
  Test Case:
  ```cpp
  int main() {
      std::cout << max(5, 10) << std::endl;  // Calls non - template (int)
      std::cout << max(5.0, 10.0) << std::endl; // Calls template (double)
      return 0;
  }
  ```

#### 6. Passing by Value vs. Reference
- Value Semantics:
  - Simple but involves copying.
  - Decays arrays/functions to pointers.
- Reference Semantics:
  - Avoids copying but requires care with `const`.
- Example:
  ```cpp
  template<typename T>
  void printByValue(T value) { /* ... */ }
  
  template<typename T>
  void printByRef(const T& value) { /* ... */ }
  ```

#### 7. Inline and constexpr
- Inline: Advises the compiler to expand the function inline.
- constexpr: Enables compile - time evaluation.
- Example:
  ```cpp
  template<typename T>
  inline constexpr T max(T a, T b) {
      return (a > b) ? a : b;
  }
  ```

---

### 10 Hard Multiple - Choice Questions

1. Question: What is the output of the following code?
   ```cpp
   template<typename T>
   T max(T a, T b) { return b < a ? a : b; }
   int max(int a, int b) { return a > b ? a : b; }
   int main() {
       std::cout << max(5, 10) << " ";
       std::cout << max<>(5, 10) << std::endl;
       return 0;
   }
   ```
   - A. `10 10`
   - B. `10 5`
   - C. `5 10`
   - D. `5 5`

2. Question: Which of the following is invalid for template parameters?
   - A. `template<typename T, int N>`
   - B. `template<typename T, T value>`
   - C. `template<typename T, class U>`
   - D. `template<typename T, auto value>`

3. Question: What happens if you call `max(3.0, 5)` with the basic `max` template?
   - A. Compiles and returns `5`
   - B. Compiles and returns `3.0`
   - C. Compilation error
   - D. Linker error

4. Question: Which keyword is used to explicitly specify template arguments?
   - A. `template`
   - B. `typename`
   - C. `class`
   - D. `<>`

5. Question: What is the return type of `max(3.5, 5)` in the following code?
   ```cpp
   template<typename T1, typename T2>
   auto max(T1 a, T2 b) { return b < a ? a : b; }
   ```
   - A. `T1`
   - B. `T2`
   - C. `double`
   - D. `int`

6. Question: Which of the following is a valid function template declaration?
   - A. `template<typename T> T add(T a, T b = 0);`
   - B. `template<typename T> auto add(T a, T b) -> T;`
   - C. `template<typename T> T add(T a, T b) { return a + b; }`
   - D. All of the above

7. Question: What is the output of the following code?
   ```cpp
   template<typename T>
   T max(T a, T b) { return b < a ? a : b; }
   int main() {
       std::cout << max('a', 'b') << std::endl;
       return 0;
   }
   ```
   - A. `a`
   - B. `b`
   - C. `97`
   - D. `98`

8. Question: Which of the following correctly forwards arguments using perfect forwarding?
   - A. `template<typename T> void f(T arg) { g(arg); }`
   - B. `template<typename T> void f(T&& arg) { g(std::forward<T>(arg)); }`
   - C. `template<typename T> void f(T& arg) { g(std::move(arg)); }`
   - D. `template<typename T> void f(T* arg) { g(*arg); }`

9. Question: Which of the following is true about default template arguments?
   - A. They can only be used for type parameters.
   - B. They can be omitted if the compiler can deduce the type.
   - C. They must be specified in the function call.
   - D. They can be used in any order.

10. Question: What is the output of the following code?
    ```cpp
    template<typename T>
    T max(T a, T b) { return b < a ? a : b; }
    int main() {
        std::cout << max(5, 3) << std::endl;
        return 0;
    }
    ```
    - A. `3`
    - B. `5`
    - C. Compilation error
    - D. Linker error

---

### Answers and Detailed Solutions

1. Answer: B  
   Explanation: The first call `max(5, 10)` uses the non - template function (int), returning `10`. The second call `max<>(5, 10)` explicitly uses the template, returning `5`.

2. Answer: B  
   Explanation: Template parameters of type `T` cannot be used as nontype parameters unless `T` is a literal type (C++17).

3. Answer: C  
   Explanation: `max(3.0, 5)` has mismatched types (`double` and `int`), causing a deduction error.

4. Answer: D  
   Explanation: `<>` is used to explicitly specify template arguments, e.g., `max<double>(3, 5)`.

5. Answer: C  
   Explanation: The return type is deduced as `double` (from `3.5`).

6. Answer: D  
   Explanation: All are valid function template declarations.

7. Answer: B  
   Explanation: The function returns `'b'` (ASCII value 98 is greater than 97).

8. Answer: B  
   Explanation: `std::forward<T>` preserves the value category of the argument.

9. Answer: B  
   Explanation: Default template arguments can be omitted if the compiler infers the type.

10. Answer: B  
    Explanation: The template returns `5` as `5 > 3`.

---

### 5 Medium Design Questions

1. Design: Create a variadic template `sum` that returns the sum of any number of arguments.  
   Test Case: `sum(1, 2, 3)` should output `6`.

2. Design: Implement a template `contains` that checks if a container holds a value.  
   Test Case: `std::vector<int> v = {1, 2, 3}; contains(v, 2)` should return `true`.

3. Design: Create a template `apply` that calls a function with given arguments.  
   Test Case: `apply(add, 3, 5)` should return `8` (assuming `int add(int, int)`).

4. Design: Implement a template `printArray` to print array elements.  
   Test Case: `int arr[] = {1, 2, 3}; printArray(arr)` should output `1 2 3`.

5. Design: Create a template `transform` that applies a function to each element of a container.  
   Test Case: `transform({1, 2, 3}, [](int x) { return x * 2; })` should return `{2, 4, 6}`.

---

### Design Question Answers

1. Solution:
   ```cpp
   template<typename T>
   T sum(T a) {
       return a;
   }
   template<typename T, typename... Args>
   T sum(T a, Args... args) {
       return a + sum(args...);
   }
   int main() {
       std::cout << sum(1, 2, 3) << std::endl;  // Output: 6
       return 0;
   }
   ```

2. Solution:
   ```cpp
   #include <vector>
   #include <algorithm>

   template<typename Container, typename T>
   bool contains(Container& container, T value) {
       return std::find(container.begin(), container.end(), value) != container.end();
   }

   int main() {
       std::vector<int> v = {1, 2, 3};
       std::cout << std::boolalpha << contains(v, 2) << std::endl;  // Output: true
       return 0;
   }
   ```

3. Solution:
   ```cpp
   template<typename Func, typename... Args>
   decltype(auto) apply(Func func, Args... args) {
       return func(args...);
   }

   int add(int a, int b) {
       return a + b;
   }

   int main() {
       std::cout << apply(add, 3, 5) << std::endl;  // Output: 8
       return 0;
   }
   ```

4. Solution:
   ```cpp
   template<typename T, size_t N>
   void printArray(T (&arr)[N]) {
       for (size_t i = 0; i < N; ++i) {
           std::cout << arr[i] << " ";
       }
       std::cout << std::endl;
   }

   int main() {
       int arr[] = {1, 2, 3};
       printArray(arr);  // Output: 1 2 3
       return 0;
   }
   ```

5. Solution:
   ```cpp
   #include <vector>
   #include <algorithm>

   template<typename Container, typename Func>
   auto transform(Container container, Func func) {
       std::transform(container.begin(), container.end(), container.begin(), func);
       return container;
   }

   int main() {
       std::vector<int> v = {1, 2, 3};
       auto doubled = transform(v, [](int x) { return x * 2; });
       for (int x : doubled) {
           std::cout << x << " ";
       }  // Output: 2 4 6
       return 0;
   }
   ```