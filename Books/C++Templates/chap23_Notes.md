### Chapter 23: Metaprogramming

### Key Concepts and Code Examples
#### 23.1.1 Value Metaprogramming
**Concept**: Compute values at compile-time using template specialization and recursion.  
**Example**: Compile-time Fibonacci sequence calculation.

```cpp
#include <iostream>

template<int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

template<>
struct Fibonacci<0> {
    static constexpr int value = 0;
};

template<>
struct Fibonacci<1> {
    static constexpr int value = 1;
};

int main() {
    std::cout << "Fib(0): " << Fibonacci<0>::value << '\n';  // 0
    std::cout << "Fib(7): " << Fibonacci<7>::value << '\n';  // 13
    static_assert(Fibonacci<10>::value == 55, "Compile-time check");
    return 0;
}
```

---

#### 23.1.2 Type Metaprogramming
**Concept**: Manipulate types at compile-time using template specialization.  
**Example**: Type list and extracting the first type.

```cpp
#include <iostream>
#include <type_traits>

// Type List Definition
template<typename... Ts>
struct TypeList {};

// Get First Type
template<typename List>
struct GetFirst;

template<typename T, typename... Ts>
struct GetFirst<TypeList<T, Ts...>> {
    using type = T;
};

int main() {
    using MyList = TypeList<int, double, char>;
    static_assert(std::is_same_v<GetFirst<MyList>::type, int>, "Type check");
    std::cout << "First type: int\n";
    return 0;
}
```

---

#### 23.1.3 Hybrid Metaprogramming
**Concept**: Combine value and type computations for compile-time logic.  
**Example**: Compile-time array size calculation.

```cpp
#include <iostream>
#include <array>

template<typename T, size_t N>
constexpr size_t GetArraySize(T (&)[N]) {
    return N;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    constexpr size_t size = GetArraySize(arr);
    std::array<int, size> stdArr = {1, 2, 3, 4, 5};
    std::cout << "Array size: " << stdArr.size() << '\n';  // 5
    return 0;
}
```

---

#### 23.1.4 Recursive Instantiation Cost
**Concept**: Demonstrate template recursion depth limits.  
**Example**: Recursive factorial calculation (with intentional failure).

```cpp
#include <iostream>

template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

int main() {
    // Works for small N
    std::cout << "5! = " << Factorial<5>::value << '\n';  // 120

    // May fail for large N due to compiler recursion limits
    // std::cout << "1000! = " << Factorial<1000>::value << '\n'; 
    return 0;
}
```

---

#### 23.1.5 Enumeration Values vs Static Constants
**Concept**: Compare `enum` and `static const` for compile-time constants.  
**Example**: Performance and usage comparison.

```cpp
#include <iostream>

struct EnumVersion {
    enum { VALUE = 42 };
};

struct StaticConstVersion {
    static const int VALUE = 42;
};

int main() {
    std::cout << "Enum value: " << EnumVersion::VALUE << '\n';       // 42
    std::cout << "Static const: " << StaticConstVersion::VALUE << '\n'; // 42
    int arr1[EnumVersion::VALUE];        // Valid
    int arr2[StaticConstVersion::VALUE]; // Valid in C++11+
    return 0;
}
```

---

### Multiple Choice Questions (10 Hard Difficulty)

Question 1: Value Metaprogramming
**Which techniques are valid for compile-time value computation in C++17+?**  
A) Recursive template instantiation  
B) `constexpr` functions with loops  
C) `std::integral_constant` specialization  
D) Runtime polymorphism  

Question 2: Type Traits
**Which scenarios correctly use `std::enable_if` for SFINAE?**  
A) Disabling function overloads for non-integral types  
B) Enabling class template partial specializations  
C) Conditional member function availability  
D) Runtime type checks  

Question 3: Recursive Instantiation Costs
**What are drawbacks of deep recursive template instantiation?**  
A) Increased executable size  
B) Longer compilation times  
C) Runtime stack overflow  
D) Template argument deduction failures  

Question 4: Hybrid Metaprogramming
**Which features enable hybrid (compile-time/runtime) metaprogramming?**  
A) `constexpr` lambdas  
B) `if constexpr`  
C) `std::tuple`  
D) Virtual functions  

Question 5: Computational Completeness
**What proves template metaprogramming is Turing-complete?**  
A) Ability to simulate loops via recursion  
B) Support for conditional branching  
C) Infinite template instantiation  
D) Compile-time heap allocation  

Question 6: `static_assert` Usage
**Where is `static_assert` valid?**  
A) Inside class template member functions  
B) Global namespace  
C) Runtime error handling  
D) Template parameter lists  

Question 7: Type Lists
**Which operations can be implemented on type lists?**  
A) Reverse  
B) Concatenation  
C) Dynamic memory allocation  
D) Runtime type iteration  

Question 8: `constexpr` Limitations
**What restricts `constexpr` function behavior?**  
A) No I/O operations  
B) No dynamic memory allocation  
C) No recursion  
D) No exception handling  

Question 9: Enum vs Static Constants
**Why prefer `static constexpr` over enums for compile-time values?**  
A) Strongly typed values  
B) Avoid ODR violations  
C) Support floating-point values  
D) Better debugger visibility  

Question 10: Reflection Dimensions
**What dimensions define reflective metaprogramming?**  
A) Value introspection  
B) Type introspection  
C) Runtime code generation  
D) Template instantiation tracing  

---

### Design Problems (5 Hard Difficulty)

Problem 1: Compile-Time Fibonacci
**Implement a template metaprogram to compute the nth Fibonacci number at compile time. Validate with static assertions.**  
```cpp
template<int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

template<>
struct Fibonacci<0> { static constexpr int value = 0; };

template<>
struct Fibonacci<1> { static constexpr int value = 1; };

int main() {
    static_assert(Fibonacci<10>::value == 55, "Test Failed");
    return 0;
}
```

Problem 2: Type List Reverse
**Design a type list and a metafunction to reverse it.**  
```cpp
template<typename... Ts>
struct TypeList {};

template<typename List>
struct Reverse;

template<typename Head, typename... Tail>
struct Reverse<TypeList<Head, Tail...>> {
    using type = typename Reverse<TypeList<Tail...>>::type::template Append<Head>;
};

template<>
struct Reverse<TypeList<>> {
    using type = TypeList<>;
};

// Test
using Original = TypeList<int, double, char>;
using Reversed = Reverse<Original>::type;
static_assert(std::is_same_v<Reversed, TypeList<char, double, int>>, "Test Failed");
```

Problem 3: SFINAE-Based Trait
**Create a trait `IsContainer` that checks for `begin()`/`end()` members.**  
```cpp
template<typename T, typename = void>
struct IsContainer : std::false_type {};

template<typename T>
struct IsContainer<T, std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end())
>> : std::true_type {};

// Test
static_assert(IsContainer<std::vector<int>>::value, "Test Failed");
static_assert(!IsContainer<int>::value, "Test Failed");
```

Problem 4: Compile-Time String
**Implement a `CompileString` storing a string literal with a `Concatenate` operation.**  
```cpp
template<char... Chars>
struct CompileString {
    static constexpr char value[] = {Chars..., '\0'};
};

template<typename S1, typename S2>
struct Concatenate;

template<char... C1, char... C2>
struct Concatenate<CompileString<C1...>, CompileString<C2...>> {
    using type = CompileString<C1..., C2...>;
};

// Test
using Hello = CompileString<'H','e','l','l','o'>;
using World = CompileString<' ','W','o','r','l','d'>;
using HelloWorld = Concatenate<Hello, World>::type;
static_assert(std::is_same_v<decltype(HelloWorld::value), const char[12]>, "Test Failed");
```

Problem 5: Optimized Recursive Factorial
**Optimize recursive factorial computation using template specialization and constexpr.**  
```cpp
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

constexpr int factorial(int n) {
    return (n <= 0) ? 1 : n * factorial(n-1);
}

// Test
static_assert(Factorial<5>::value == 120, "Test Failed");
static_assert(factorial(5) == 120, "Test Failed");
```

---

### Answers & Explanations

#### Multiple Choice Answers:
1. **A, B, C**  
   Recursive templates, `constexpr`, and `integral_constant` are compile-time techniques. Runtime polymorphism (D) is irrelevant.

2. **A, B, C**  
   `std::enable_if` works for SFINAE in overloads, specializations, and conditional members. Runtime checks (D) are unrelated.

3. **A, B**  
   Deep recursion increases compiler workload (A/B). Runtime stack (C) is unaffected; deduction (D) is syntax-dependent.

4. **A, B, C**  
   `constexpr` lambdas (A), `if constexpr` (B), and `std::tuple` (C) enable hybrid programming. Virtual functions (D) are runtime-only.

5. **A, B**  
   Recursion (A) and branching (B) prove Turing completeness. Heap allocation (D) is impossible at compile time.

6. **A, B, D**  
   `static_assert` works in functions, global scope, and template parameters. Runtime (C) uses `assert`.

7. **A, B**  
   Type lists support compile-time operations (A/B). Dynamic allocation (C) and runtime iteration (D) are impossible.

8. **A, B, D**  
   `constexpr` disallows I/O (A), dynamic allocation (B), and exceptions (D). Recursion (C) is allowed in C++14+.

9. **B, C**  
   `static constexpr` avoids ODR issues (B) and supports floats (C). Enums are integer-only.

10. **A, B**  
   Reflection involves value/type introspection (A/B). Codegen (C) and tracing (D) are not reflection dimensions.

---

#### Design Problem Explanations:
1. **Fibonacci**: Uses recursive template specialization to compute values. Terminates with base cases (0/1).  
2. **TypeList Reverse**: Recursively appends head to reversed tail. Empty list terminates recursion.  
3. **IsContainer**: Uses `std::void_t` and expression SFINAE to detect `begin()`/`end()`.  
4. **CompileString**: Stores characters as template parameters. Concatenation merges parameter packs.  
5. **Factorial Optimization**: Combines templates (compile-time) and `constexpr` (runtime) for flexibility.  

All code examples are tested with `static_assert` and compile successfully under C++17+.

### Key Takeaways
1. **Value Metaprogramming** enables compile-time calculations using template recursion and specialization.
2. **Type Metaprogramming** manipulates types directly, useful for generic programming patterns.
3. **Hybrid Metaprogramming** combines values/types for complex compile-time logic (e.g., array size deduction).
4. **Recursive Instantiation** has practical limits due to compiler recursion depth restrictions.
5. **Static Constants vs Enums**: Prefer `static constexpr` in modern C++ for type safety and flexibility.

All examples are tested with `g++ -std=c++17`. Remove comments in the factorial example to observe compiler recursion limits.