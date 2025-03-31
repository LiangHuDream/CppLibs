
#### 1. Parameter Packs
What: A template parameter that accepts zero or more arguments.  
Syntax:  
```cpp
template<typename... Args> // Template parameter pack
void func(Args... args) {  // Function parameter pack
    // ...
}
```

Example: Simple recursive print function.  
```cpp
#include <iostream>

// Base case: Terminates recursion
void print() {}

template<typename T, typename... Args>
void print(T first, Args... rest) {
    std::cout << first << " ";
    print(rest...); // Recursive call with remaining arguments
}

// Test case
int main() {
    print(42, "hello", 3.14); // Output: 42 hello 3.14
    return 0;
}
```

---

#### 2. Fold Expressions (C++17)
What: Simplify iteration over parameter packs using operators.  
Syntax:  
- `(args + ...)`: Unary right fold.  
- `(... + args)`: Unary left fold.  
- `(0 + ... + args)`: Binary left fold (initial value `0`).  

Example: Summing all arguments with a fold:  
```cpp
#include <iostream>

// 处理空参数包的情况，重载 sum 函数
template<>
auto sum() {
    return 0;
}

template<typename... Args>
auto sum(Args... args) {
    return (args + ...); // Unary right fold
}

int main() {
    std::cout << sum(1, 2, 3.5); // Output: 6.5
    std::cout << sum(); // 现在可以处理空参数包调用
    return 0;
}
    
```

Handling Empty Packs:  
```cpp
#include <iostream>

template<typename... Args>
auto safe_sum(Args... args) {
    return (args + ... + 0); // Binary fold with initial value 0
}

int main() {
    std::cout << safe_sum(); // Output: 0 (no error)
    return 0;
}
```

---

#### 3. Perfect Forwarding with Variadics
Why: Preserve value categories (lvalue/rvalue) when forwarding arguments.  
How: Use `std::forward` with `Args&&...`.  

Example: Generic factory function:  
```cpp
#include <iostream>
#include <string>
#include <utility>

template<typename T, typename... Args>
T create(Args&&... args) {
    return T(std::forward<Args>(args)...);
}

int main() {
    auto s = create<std::string>(3, 'A'); // "AAA"
    std::cout << s; // Output: AAA
    return 0;
}
    
```

---

#### 4. Variadic Class Templates
Use Case: Classes with variable template parameters (e.g., `std::tuple`).  

Example: Simplified tuple implementation:  
```cpp
#include <iostream>
#include <string>

template<typename... Types>
class Tuple;

template<> // Base case: Empty tuple
class Tuple<> {};

template<typename T, typename... Rest>
class Tuple<T, Rest...> : public Tuple<Rest...> {
public:
    T value;
    Tuple(T v, Rest... vs) : Tuple<Rest...>(vs...), value(v) {}
    T get_first() { return value; }
};

int main() {
    Tuple<int, std::string> t(10, "test");
    std::cout << t.get_first(); // Output: 10
    return 0;
}
```

---

### Multiple-Choice Questions

#### Questions 1-5  
Q1: What is the output of `print(sizeof...(args))` if `args` is `(1, "hello", 3.14)`?  
A) 1  
B) 3  
C) Compile error  
D) Runtime error  

Q2: Which syntax correctly expands a parameter pack `args` in a function call?  
A) `func(args...)`  
B) `func(...args)`  
C) `func->args...`  
D) `func...args`  

Q3: What happens if the base case is omitted in a recursive variadic template?  
A) Runtime error  
B) Compilation error  
C) Undefined behavior  
D) Works if the pack is empty  

Q4: Which fold expression sums all elements with an initial value of `0`?  
A) `(0 + ... + args)`  
B) `(args + ...)`  
C) `(... + args)`  
D) `(args + ... + 0)`  

Q5: What is the type of `args` in `template<typename... Args> void f(Args... args)`?  
A) `std::tuple<Args...>`  
B) A parameter pack  
C) `std::vector<Args...>`  
D) `auto`  

---

#### Questions 6-10  
Q6: Which code correctly forwards a parameter pack?  
A) `std::move(args)...`  
B) `std::forward<Args>(args)...`  
C) `std::forward(args)...`  
D) `args...`  

Q7: What does `std::index_sequence_for<Args...>` generate?  
A) A sequence of indices for the pack  
B) A tuple of indices  
C) A compile-time error  
D) A runtime array  

Q8: Which line causes a compilation error?  
```cpp
template<typename... Args>
void foo(Args... args) {
    int arr[sizeof...(args)] = {args...}; // Line 1
    (std::cout << ... << args);           // Line 2
}
```  
A) Line 1  
B) Line 2  
C) Both  
D) Neither  

Q9: What is the result of `sum(1, "hello")` if `sum` uses `(args + ...)`?  
A) Compile error  
B) Runtime error  
C) Undefined behavior  
D) `1hello`  

Q10: Which code creates a `std::tuple` from `args`?  
A) `std::tuple(args...)`  
B) `std::make_tuple(args...)`  
C) `std::tuple<Args...> t(args)`  
D) `std::tuple<Args...> t(args...)`  

---

### Answers & Explanations  
1. B (`sizeof...` returns 3).  
2. A (`func(args...)`).  
3. B (Missing base case causes compilation failure).  
4. A (Binary left fold with initial value `0`).  
5. B (Parameter pack).  
6. B (`std::forward<Args>(args)...` preserves value categories).  
7. A (Generates indices like `0,1,2`).  
8. A (Cannot initialize `int[]` with mixed types).  
9. A (Cannot add `int` and `const char*`).  
10. B (`std::make_tuple(args...)`).  

### Design Questions

#### Questions 1-5:
1. Implement a function `sum_all` that returns the sum of all arguments using a fold expression.  
   ```cpp  
   template<typename... Args>  
   auto sum_all(Args&&... args) {  
       return (... + args);  
   }  
   ```  
   Test Case:  
   ```cpp  
   int main() {  
       std::cout << sum_all(1, 2.5, 3, 4.2); // Output: 10.7  
   }  
   ```  

2. Create a recursive `print_all` function without fold expressions.  
   ```cpp  
   void print_all() {} // Base case  
   template<typename T, typename... Args>  
   void print_all(T first, Args... rest) {  
       std::cout << first << " ";  
       print_all(rest...);  
   }  
   ```  
   Test Case:  
   ```cpp  
   int main() {  
       print_all(42, "hello", 3.14); // Output: 42 hello 3.14  
   }  
   ```  

3. Design a class `Tuple` that can hold any number of elements of any type.  
   ```cpp  
   template<typename... Types>  
   class Tuple;  
   template<>  
   class Tuple<> {}; // Empty tuple  
   template<typename T, typename... Rest>  
   class Tuple<T, Rest...> : public Tuple<Rest...> {  
       T value;  
   public:  
       Tuple(T v, Rest... vs) : Tuple<Rest...>(vs...), value(v) {}  
       T get() { return value; }  
   };  
   ```  
   Test Case:  
   ```cpp  
   int main() {  
       Tuple<int, std::string, double> t(10, "test", 3.14);  
       std::cout << t.get(); // Output: 10  
   }  
   ```  

4. Write a function `make_tuple` that forwards all arguments to construct a `std::tuple`.  
   ```cpp  
   template<typename... Args>  
   auto make_tuple(Args&&... args) {  
       return std::tuple<Args...>(std::forward<Args>(args)...);  
   }  
   ```  
   Test Case:  
   ```cpp  
   int main() {  
       auto t = make_tuple(42, "hello", 3.14);  
       std::cout << std::get<1>(t); // Output: hello  
   }  
   ```  

5. Implement `for_each_arg` that applies a functor to each argument.  
   ```cpp  
   template<typename Func, typename... Args>  
   void for_each_arg(Func f, Args&&... args) {  
       (f(std::forward<Args>(args)), ...); // Fold with comma operator  
   }  
   ```  
   Test Case:  
   ```cpp  
   int main() {  
       for_each_arg([](auto x) { std::cout << x << " "; }, 1, 2.5, "hello");  
       // Output: 1 2.5 hello  
   }  
   ```  

---

### Summary:
These exercises reinforce variadic template mechanics, including pack expansion, recursion, fold expressions, and perfect forwarding. The test cases validate correct behavior for different argument types and counts.
