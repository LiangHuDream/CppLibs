### Chapter 3: Nontype Template Parameters - Key Points

To master the key concepts of Chapter 3 "Nontype Template Parameters," let's break down the essential components through detailed explanations and code examples:

---

### 1. Definition and Syntax
Non-type template parameters allow values (not types) as template arguments. They must be compile-time constants.

Syntax Example:
```cpp
template<int Size>       // int non-type parameter
class Buffer {
    int data[Size];     // Size determines array size at compile-time
};
Buffer<1024> buf;       // Creates buffer with 1024 elements
```

---

### 2. Allowed Non-Type Parameter Types
C++ supports these non-type parameter categories:
- Integral types (`int`, `char`, etc.)
- Enumerations
- Pointers/References to objects/functions
- `nullptr`
- Floating-point (C++20)
- Literal class types with `constexpr` constructor (C++20)

Example with Pointer:
```cpp
template<const char* Msg>
void print() { std::cout << Msg; }

// Requires Msg to have external linkage
extern const char hello[] = "Hello";
print<hello>();        // Uses pointer to compile-time constant
```

---

### 3. Compile-Time Constants Requirement
Non-type parameters must be determinable at compile-time.

Invalid Example:
```cpp
int size = 10;
Buffer<size> buf;      // ERROR: 'size' isn't a compile-time constant
```

Valid Example:
```cpp
constexpr int size = 10;
Buffer<size> buf;      // OK: constexpr guarantees compile-time value
```

---

### 4. Auto Deduction for Non-Type Parameters (C++17)
Using `auto` lets the compiler deduce the parameter type.

Example:
```cpp
template<auto Value>
void printValue() { std::cout << Value << '\n'; }

printValue<42>();      // Deduces int
printValue<'A'>();     // Deduces char
```

---

### 5. Function Templates with Non-Type Parameters
Non-type parameters enable compile-time configurations.

Example: Array Creator
```cpp
template<typename T, int Size>
T* createArray() {
    return new T[Size];  // Allocates array of Size elements
}
auto arr = createArray<double, 5>();  // Creates double[5]
```

---

### 6. Template Specialization
Specialize templates for specific non-type values.

Example: Factorial Calculation
```cpp
template<int N>
struct Factorial {
    static const int value = N * Factorial<N-1>::value;
};

template<>
struct Factorial<0> {  // Specialization for N=0
    static const int value = 1;
};
std::cout << Factorial<5>::value;  // 120 (5! = 120)
```

---

### 7. Practical Applications
Non-type parameters are essential in:
- Fixed-size containers (`std::array<T, N>`)
- Compile-time computations (math, validations)
- Policy-based design (tuning algorithms at compile-time)

Example: Compile-Time Array Size Checking
```cpp
template<typename T, int MaxSize>
class Stack {
    T elements[MaxSize];
    int count = 0;
public:
    void push(const T& val) {
        static_assert(MaxSize > 0, "Invalid stack size");
        if (count >= MaxSize) throw std::overflow_error("Stack full");
        elements[count++] = val;
    }
};
Stack<int, 100> stack;  // Guaranteed fixed-size stack
```

---

### 8. C++20 Enhancements
- Class Types as Non-Type Parameters:
  ```cpp
  struct Point {
      int x, y;
      constexpr Point(int x, int y) : x(x), y(y) {}
  };

  template<Point P>
  void plot() { /* Plot point P at compile time */ }
  plot<Point(3,4)>();
  ```
- Floating-Point Support:
  ```cpp
  template<double Tolerance>
  bool isApprox(double a, double b) {
      return std::abs(a - b) < Tolerance;
  }
  isApprox<0.001>(1.0, 1.0001);  // Returns true
  ```

---

### Key Takeaways:
1. Compile-Time Determination: Non-type parameters must be resolvable during compilation.
2. Type Restrictions: Know allowed types (integral, pointer, etc.) and C++20 expansions.
3. Use Cases: Fixed-size containers, compile-time computations, policy-based design.
4. Modern Features: Leverage `auto` (C++17) and class types (C++20) for flexibility.

---

### 5 Multiple-Choice Questions

1. Which of these is NOT a valid nontype template parameter?
   a) `int`
   b) `const char*`
   c) `size_t`
   d) `float` (C++20)
   Answer: b) `const char*`  
   Explanation: String literals can't be used directly as template arguments due to internal linkage.

2. What's the output of this code?
   ```cpp
   template <auto N>
   void print() { std::cout << N; }
   
   int main() {
       print<42>();
       print<'A'>();
   }
   ```
   a) 42A  
   b) Compile error  
   c) 65  
   d) 42 65  
   Answer: a) 42A  
   Explanation: `auto` deduces `int` and `char` types correctly.

3. Which template declaration is valid?
   a) `template <int& ref>`
   b) `template <std::string s>`
   c) `template <double d>`
   d) `template <auto x = nullptr>`  
   Answer: a) and d) (C++17/20)  
   Clarification: c) valid in C++20, b) invalid as class type.

4. What does this code do?
   ```cpp
   template <size_t N, typename T>
   constexpr T power(T base) {
       T result = 1;
       for(size_t i=0; i<N; ++i) result *= base;
       return result;
   }
   ```
   a) Runtime exponent calculation  
   b) Compile-time exponent calculation  
   c) Both  
   Answer: c) Both  
   Explanation: `constexpr` allows compile-time evaluation when possible.

5. Which array initialization is valid?
   ```cpp
   template <int(&arr)[3]>
   struct Wrapper {};
   
   int main() {
       int arr[] = {1,2,3};
       Wrapper<arr> w1;      // A
       Wrapper<{1,2,3}> w2;  // B
   }
   ```
   a) A only  
   b) B only  
   c) Both  
   d) Neither  
   Answer: a) A only  
   Explanation: Brace-enclosed lists can't be template arguments.

---

### 5 Design Problems

1. Fixed-Size Array Template  
Implement a `FixedArray` template class using a nontype size parameter:
```cpp
template <typename T, size_t N>
class FixedArray {
    T data[N];
public:
    constexpr size_t size() const { return N; }
    // Add element access, iterators, etc.
};

// Test case:
FixedArray<int, 5> arr;
static_assert(arr.size() == 5);
```

2. Compile-Time Fibonacci  
Create a template calculating Fibonacci numbers at compile-time:
```cpp
template <size_t N>
constexpr size_t fibonacci = fibonacci<N-1> + fibonacci<N-2>;

template <>
constexpr size_t fibonacci<0> = 0;

template <>
constexpr size_t fibonacci<1> = 1;

// Test case:
static_assert(fibonacci<10> == 55);
```

3. Matrix Dimensions  
Design a matrix class with compile-time dimensions:
```cpp
template <typename T, size_t Rows, size_t Cols>
class Matrix {
    T data[Rows][Cols];
public:
    constexpr size_t rows() const { return Rows; }
    constexpr size_t cols() const { return Cols; }
    // Add operator() for element access
};

// Test case:
Matrix<double, 3, 4> mat;
static_assert(mat.rows() == 3 && mat.cols() == 4);
```

4. Policy-Based Design  
Implement a counter with policy-based increment:
```cpp
template <typename T, typename IncrementPolicy>
class Counter {
    T value;
public:
    void inc() { IncrementPolicy::apply(value); }
};

struct DoubleIncrement {
    template <typename T>
    static void apply(T& v) { v += 2; }
};

// Test case:
Counter<int, DoubleIncrement> c;
c.inc();  // Should increase by 2
```

5. String Literal Template (C++17)  
Create a template storing string literals using `auto` parameter:
```cpp
template <auto... Chars>
struct FixedString {
    static constexpr char value[] = {Chars..., '\0'};
};

// Usage:
using Hello = FixedString<'H','e','l','l','o'>;
static_assert(Hello::value[0] == 'H');
```

---

### Test Cases (Main Function Examples)

Fibonacci Test:
```cpp
int main() {
    std::cout << "Fib(10): " << fibonacci<10> << "\n";  // 55
    return 0;
}
```

Matrix Test:
```cpp
int main() {
    Matrix<int, 2, 3> mat;
    std::cout << "Matrix size: " << mat.rows() << "x" << mat.cols() << "\n";
    return 0;
}
```

Counter Test:
```cpp
int main() {
    Counter<int, DoubleIncrement> c;
    c.inc();
    c.inc();
    std::cout << "Counter value: " << c.value << "\n";  // Assume value is public
    return 0;
}
```

These exercises reinforce understanding of nontype parameters, compile-time computations, and template specialization. Each solution emphasizes practical applications of chapter concepts with verifiable test cases.