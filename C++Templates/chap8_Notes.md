
@[TOC](Chapter 8：Compile-Time Programming)

---

### Key points and difficulties of Compile-Time Programming

#### 1. Template Metaprogramming Basics
Key Concept: Using template instantiation to perform computations at compile-time.

```cpp
#include <iostream>

// Primary template for factorial
template<int N>
struct Factorial {
    static const int value = N * Factorial<N-1>::value;
};

// Base case specialization
template<>
struct Factorial<0> {
    static const int value = 1;
};

int main() {
    std::cout << Factorial<5>::value << '\n';  // 120
    static_assert(Factorial<5>::value == 120, "Test Failed");
    return 0;
}
```
Test Output:
```
120
```
This demonstrates compile-time factorial calculation using template recursion.

---

#### 2. constexpr Functions
Key Concept: Using `constexpr` for compile-time evaluation with function syntax.

```cpp
constexpr int factorial(int n) {
    return (n <= 0) ? 1 : n * factorial(n-1);
}

int main() {
    constexpr int val = factorial(5);
    std::cout << val << '\n';                // 120
    static_assert(factorial(5) == 120, "Error");
    return 0;
}
```
Test Output:
```
120
```
Shows modern C++ alternative to template metaprogramming.

---

#### 3. Partial Specialization for Type Selection
Key Concept: Choosing implementations based on type traits.

```cpp
#include <type_traits>
#include <iostream>

// Primary template
template<typename T, bool = std::is_integral<T>::value>
struct ValueProcessor {
    static void process(T val) {
        std::cout << "Generic processing: " << val << '\n';
    }
};

// Specialization for integral types
template<typename T>
struct ValueProcessor<T, true> {
    static void process(T val) {
        std::cout << "Integer processing: " << val * 2 << '\n';
    }
};

int main() {
    ValueProcessor<double>::process(3.14);  // Generic
    ValueProcessor<int>::process(42);       // Integer
    return 0;
}
```
Test Output:
```
Generic processing: 3.14
Integer processing: 84
```

---

#### 4. SFINAE with enable_if
Key Concept: Enabling/disabling function overloads using type traits.

```cpp
#include <type_traits>
#include <iostream>

// Enabled for integral types
template<typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
process(T val) {
    std::cout << "SFINAE int: " << val << '\n';
}

// Fallback for other types
template<typename T>
typename std::enable_if<!std::is_integral<T>::value, void>::type
process(T val) {
    std::cout << "SFINAE generic: " << val << '\n';
}

int main() {
    process(10);         // int version
    process(3.14);       // generic version
    return 0;
}
```
Test Output:
```
SFINAE int: 10
SFINAE generic: 3.14
```

---

#### 5. Compile-Time if (C++17)
Key Concept: Conditional code elimination at compile-time.

```cpp
#include <iostream>
#include <type_traits>

template<typename T>
void handleValue(T val) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "Integral: " << val * 2 << '\n';
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "Float: " << val * 3.14 << '\n';
    } else {
        std::cout << "Unhandled type\n";
    }
}

int main() {
    handleValue(10);      // Integral
    handleValue(3.14);    // Float
    handleValue("test");  // Unhandled
    return 0;
}
```
Test Output:
```
Integral: 20
Float: 9.8596
Unhandled type
```

---

### Multiple-Choice Questions

**Design Question 1: Template Metaprogramming** 
Which statements about template recursion are true?  
A) It requires a base case specialization to terminate recursion.  
B) It can compute values at runtime.  
C) All template arguments must be types (not values).  
D) Results are evaluated during compilation.  

**Design Question 2: `constexpr` Functions**  
When is a `constexpr` function evaluated?  
A) Always at compile time.  
B) At compile time if its result is used in a constant expression.  
C) At runtime when passed non-const variables.  
D) Only for primitive types like `int` or `double`.  

**Design Question 3: SFINAE**  
Which scenarios trigger SFINAE?  
A) Substitution failure in a function template’s return type.  
B) A syntax error in the function body.  
C) Invalid template parameter deduction.  
D) A `static_assert` failure.  

**Design Question 4: `if constexpr`**  
What is the primary advantage of `if constexpr` over runtime `if`?  
A) It reduces executable size by eliminating dead code.  
B) It allows type-dependent branching without template specialization.  
C) It guarantees no runtime overhead.  
D) It works with non-constexpr conditions.  

**Design Question 5: Partial Specialization**  
Which declarations are valid partial specializations?  
A) `template<typename T> struct Foo<T, int> {};`  
B) `template<typename T> struct Foo<T*> {};`  
C) `template<typename T> struct Foo<T, T> {};`  
D) `template<typename U> struct Foo<U, float> {};`  

**Design Question 6: Type Traits**  
Which type traits correctly identify their target?  
A) `std::is_integral_v<float>` → `false`  
B) `std::is_pointer_v<int*>` → `true`  
C) `std::is_const_v<const int*>` → `true`  
D) `std::is_reference_v<int&&>` → `true`  

**Design Question 7: `enable_if` Usage**  
Where can `std::enable_if` be applied?  
A) In template parameter lists.  
B) As a default function argument.  
C) In the function return type.  
D) Inside a `static_assert`.  

**Design Question 8: Compile-Time Computations**  
Which techniques allow compile-time computation of Fibonacci numbers?  
A) Template recursion with specializations.  
B) `constexpr` functions.  
C) Preprocessor macros.  
D) Runtime recursion with memoization.  

**Design Question 9: Template Recursion Limitations**  
What are limitations of template recursion?  
A) Limited compiler recursion depth.  
B) Cannot handle floating-point calculations.  
C) Requires C++17 or later.  
D) Produces bloated binaries.  

**Design Question 10: `std::void_t`**  
What is the purpose of `std::void_t`?  
A) To trigger SFINAE in template parameter validation.  
B) To check if a type is `void`.  
C) To simplify partial specialization syntax.  
D) To enable type-dependent error messages.  

---

### Design Questions

**Design Question 1: Compile-Time Prime Checker**  
Implement a template metafunction `IsPrime<N>` that evaluates to `true` if `N` is a prime number. Test with `N = 5`, `N = 8`, and `N = 13`.

**Design Question 2: SFINAE-Based Overload**  
Create a function `print` that:  
- Uses `operator<<` for types that support it.  
- Prints "[unprintable]" for others.  
Use SFINAE with `decltype` and `std::void_t`. Test with `int`, `std::string`, and a custom `struct`.

**Design Question 3: `constexpr` Compile-Time Sorting**  
Write a `constexpr` function `Sort` that sorts an `std::array` at compile time. Test with `std::array<int, 4>{5, 3, 9, 1}`.

**Design Question 4: Type Traits for Polymorphic Types**  
Implement a type trait `IsPolymorphic<T>` that checks if `T` has virtual functions. Use `std::is_polymorphic` and validate with test cases.

**Design Question 5: `if constexpr`-Based JSON Serializer**  
Design a function `serialize` that uses `if constexpr` to handle:  
- Integers → `"number"`  
- Strings → `"string"`  
- Containers (e.g., `std::vector`) → `"array"`  
Test with `int`, `std::string`, and `std::vector<int>`.

---

### Answers & Explanations (Multiple-Choice)

**Design Question 1** 
Correct: A, D  
Explanation:  
- Template recursion requires a base case (A).  
- Computations happen at compile time (D).  
- Template arguments can be values (C is false).  

**Design Question 2** 
Correct: B, C  
Explanation:  
- `constexpr` functions can be evaluated at compile or runtime (B).  
- Non-const variables force runtime evaluation (C).  

**Design Question 3**  
Correct: A, C  
Explanation:  
- SFINAE applies to substitution failures (A, C).  
- Syntax errors and `static_assert` are hard errors (B, D).  

**Design Question 4**  
Correct: A, B  
Explanation:  
- `if constexpr` discards dead code (A) and avoids specialization (B).  

**Design Question 5**  
Correct: B, D  
Explanation:  
- Partial specialization must match primary template parameters (B, D).  

**Design Question 6**  
Correct: A, B, D  
Explanation:  
- `float` is not integral (A).  
- `int*` is a pointer (B).  
- `int&&` is a reference (D).  
- `const int*` is not const (C is false).  

**Design Question 7**  
Correct: A, B, C  
Explanation:  
- `enable_if` can be in parameters, return types, or default arguments.  

**Design Question 8**  
Correct: A, B  
Explanation:  
- Template recursion (A) and `constexpr` (B) work at compile time.  

**Design Question 9**  
Correct: A, B, D  
Explanation:  
- Compilers limit recursion depth (A).  
- Floating points are allowed in C++11+ (B is false).  

**Design Question 10**  
Correct: A, C  
Explanation:  
- `std::void_t` helps SFINAE (A) and simplifies specialization (C).  

---

### Answers & Explanations (Design Questions)

**Design Question 1**  
Solution:  
```cpp
template<unsigned N, unsigned D = N-1>
struct IsPrime {
    static const bool value = (N % D != 0) && IsPrime<N, D-1>::value;
};

template<unsigned N>
struct IsPrime<N, 1> {
    static const bool value = true;
};

int main() {
    static_assert(IsPrime<5>::value, "5 is prime");
    static_assert(!IsPrime<8>::value, "8 is not prime");
    static_assert(IsPrime<13>::value, "13 is prime");
}
```

**Design Question 2**  
Solution:  
```cpp
#include <iostream>
#include <type_traits>

template<typename T, typename = void>
struct is_printable : std::false_type {};

template<typename T>
struct is_printable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> 
    : std::true_type {};

template<typename T>
void print(T val) {
    if constexpr (is_printable<T>::value) {
        std::cout << val << '\n';
    } else {
        std::cout << "[unprintable]\n";
    }
}

struct Unprintable {};

int main() {
    print(42);              // 42
    print(std::string("test")); // test
    print(Unprintable{});   // [unprintable]
}
```

**Design Question 3**  
Solution:  
```cpp
#include <array>
#include <algorithm>

constexpr auto sort_array(const std::array<int, 4>& arr) {
    std::array<int, 4> sorted = arr;
    std::sort(sorted.begin(), sorted.end());
    return sorted;
}

int main() {
    constexpr std::array<int, 4> arr = {5, 3, 9, 1};
    constexpr auto sorted = sort_array(arr);
    static_assert(sorted[0] == 1 && sorted[3] == 9, "Sort failed");
}
```

**Design Question 4**  
Solution:  
```cpp
#include <type_traits>

class Polymorphic { virtual void foo(); };
class NonPolymorphic {};

static_assert(std::is_polymorphic<Polymorphic>::value, "Polymorphic failed");
static_assert(!std::is_polymorphic<NonPolymorphic>::value, "NonPolymorphic failed");
```

**Design Question 5**  
Solution:  
```cpp
#include <vector>
#include <type_traits>

template<typename T>
std::string serialize(const T& val) {
    if constexpr (std::is_integral_v<T>) {
        return "number";
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "string";
    } else if constexpr (requires { val.begin(); val.end(); }) {
        return "array";
    }
}

int main() {
    std::cout << serialize(10) << '\n';          // number
    std::cout << serialize(std::string("test")); // string
    std::cout << serialize(std::vector<int>{}); // array
}
```

### Key Differences Summary
| Technique          | When to Use                      | Pros                         | Cons                   |
|--------------------|----------------------------------|------------------------------|------------------------|
| Template Recursion | Pre-C++11, complex calculations | Zero runtime overhead        | Complex syntax         |
| constexpr          | C++11+ simple calculations       | Natural function syntax      | Limited complexity     |
| if constexpr       | C++17+ conditional logic         | Readable code organization   | Requires C++17         |
| SFINAE             | Overload selection               | Fine-grained control         | Complex error messages |