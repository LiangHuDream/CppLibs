### Chapter 4 Core Language: Key Concepts and Code Walkthrough

#### 4.1 Concepts
**Key Points**:
- Constraints for template parameters
- Improve error messages and enable semantic requirements
- Two approaches: type traits (legacy) vs. concepts (C++20)

**Code 1: Using Predefined Concepts**
```cpp
#include <concepts>
#include <iostream>

template<std::integral T>
T add(T a, T b) {
    return a + b;
}

int main() {
    std::cout << add(5, 3) << '\n';        // OK
    // std::cout << add(5.0, 3.0) << '\n'; // Compile error
}
```
**Test & Compile**:
```bash
g++ -std=c++20 -Wall -Wextra concepts.cpp
```

**Code 2: Custom Concept**
```cpp
#include <concepts>
#include <iostream>

template<typename T>
concept Even = requires(T t) {
    { t % 2 } -> std::same_as<T>;
};

template<Even T>
void print_even(T val) {
    std::cout << val << " is even\n";
}

int main() {
    print_even(4);     // OK
    // print_even(5);  // Fails concept check
}
```

---

#### 4.2 Modules
**Key Points**:
- Replace header files
- Export/import syntax
- Reduce compilation dependencies

**Code: Math Module**
```cpp
// math.ixx (MSVC) or math.cppm (GCC)
export module math;

export int add(int a, int b) {
    return a + b;
}
```

**Main File**:
```cpp
import math;
#include <iostream>

int main() {
    std::cout << add(20, 22) << '\n';  // 42
}
```
**Compilation Note**:  
MSVC: `/std:c++latest`, GCC: `-fmodules-ts`

---

#### 4.3 Three-Way Comparison (<=>)
**Key Points**:
- Compiler-generated comparison operators
- Return type: `std::strong_ordering`

**Code: Auto-Generated Operators**
```cpp
#include <compare>
#include <iostream>

struct Point {
    int x;
    int y;
    auto operator<=>(const Point&) const = default;
};

int main() {
    Point p1{1,2}, p2{3,4};
    std::cout << (p1 < p2) << '\n';  // true (compares x first)
}
```

---

#### 4.4 Designated Initialization
**Key Points**:
- Initialize struct members by name
- Must follow declaration order

**Code: Safe Initialization**
```cpp
struct Widget {
    int id;
    double price;
};

int main() {
    Widget w1{.id=42, .price=9.99};  // OK
    // Widget w2{.price=9.99, .id=42}; // Error: wrong order
}
```

---

#### 4.5 consteval/constinit
**Key Points**:
- `consteval`: Immediate functions (compile-time only)
- `constinit`: Compile-time initialization

**Code 1: consteval**
```cpp
consteval int square(int n) {
    return n * n;
}

int main() {
    constexpr int val = square(5);  // OK
    // int runtime = square(5);    // Error: not constexpr
}
```

**Code 2: constinit**
```cpp
constinit static int global = 42;  // Compile-time init

int main() {
    global += 5;  // Valid mutation
}
```

---

#### 4.6 Lambda Improvements
**Key Points**:
- Template parameters in lambdas
- [=, this] capture (C++20 fix)

**Code: Templated Lambda**
```cpp
#include <iostream>

int main() {
    auto lambda = []<typename T>(T a, T b) {
        return a + b;
    };
    
    std::cout << lambda(3, 4) << '\n';     // 7
    std::cout << lambda(3.1, 4.2) << '\n';// 7.3
}
```

---

#### 4.7 New Attributes
**Key Points**:
- `[[nodiscard]]`: Enforce return value usage
- `[[likely]]`/`[[unlikely]]`: Optimization hints

**Code: [[nodiscard]]**
```cpp
[[nodiscard]] int important_func() {
    return 42;
}

int main() {
    important_func();  // Warning: ignoring return value
}
```

---

### Testing Strategy
1. **Incremental Testing**: Validate each feature in isolation
2. **Compiler Flags**:  
   GCC: `-std=c++20 -Wall -Wextra -pedantic`  
   MSVC: `/std:c++latest /W4`
3. **Negative Testing**: Uncomment error cases to verify constraints

All code examples are complete and compilable when using C++20-compliant compilers. The examples demonstrate both successful execution cases and commented-out invalid usages to highlight language constraints.
### Multiple-Choice Questions 

---

**Question 1: Concepts**  
Which statements about C++20 concepts are **true**?  
A) Concepts can be used to constrain `auto` placeholder types.  
B) A concept must always be defined using `requires` expressions.  
C) The `requires` clause can directly replace SFINAE techniques.  
D) Abbreviated function templates implicitly generate template parameters.  

---

**Question 2: Modules**  
Which statements about C++20 modules are **correct**?  
A) A module interface unit must export all declarations.  
B) `import` statements replace `#include` directives for headers.  
C) Modules eliminate the need for forward declarations.  
D) Private module fragments are declared with `module :private;`.  

---

**Question 3: Three-Way Comparison**  
Which statements about `operator<=>` are **true**?  
A) Defaulting `operator<=>` automatically generates `operator==`.  
B) The return type must be `std::strong_ordering`.  
C) Rewriting expressions allow `a != b` to use `!(a == b)`.  
D) `operator<=>` can be used with floating-point types.  

---

**Question 4: Designated Initialization**  
Which are **valid** uses of designated initializers in C++20?  
A) `struct Point { int x; int y; }; Point p {.x = 1, .y = 2};`  
B) `struct Data { int a; int b; }; Data d {.b = 5, .a = 3};`  
C) `union U { int i; float f; }; U u {.f = 3.14};`  
D) `struct Nested { struct { int x; }; }; Nested n {.x = 10};`  

---

**Question 5: `consteval` and `constinit`**  
Which statements are **correct**?  
A) `consteval` functions can be called at runtime.  
B) `constinit` ensures constant initialization of static variables.  
C) `constinit` can be applied to non-static variables.  
D) `consteval` functions cannot have runtime side effects.  

---

**Question 6: Template Improvements**  
Which are **valid** C++20 template features?  
A) Non-type template parameters of class type.  
B) Conditionally explicit constructors using `requires`.  
C) Template parameter lists for lambda expressions.  
D) Partial specialization of variable templates.  

---

**Question 7: Lambda Improvements**  
Which lambda features are introduced in C++20?  
A) `[...args = std::move(args)]` capture with parameter packs.  
B) `[=, this]` implicit capture of `this` pointer.  
C) Stateless lambdas can be default-constructed.  
D) `consteval` lambdas for compile-time evaluation.  

---

**Question 8: Attributes**  
Which statements about C++20 attributes are **true**?  
A) `[[nodiscard("reason")]]` can provide a diagnostic message.  
B) `[[likely]]` and `[[unlikely]]` affect code generation optimizations.  
C) `[[no_unique_address]]` ensures a unique memory address.  
D) `[[deprecated]]` can replace `#pragma message`.  

---

**Question 9: `std::span`**  
Which are **true** about `std::span`?  
A) It can own the underlying contiguous memory.  
B) `std::span<int, 5>` has a static extent.  
C) Modifying elements through `span` affects the original container.  
D) `span` cannot be constructed from a `std::vector`.  

---

**Question 10: Coroutines**  
Which are **required** for a coroutine in C++20?  
A) A `promise_type` nested in the coroutine return type.  
B) The `co_await` operator to suspend execution.  
C) The coroutine body must use `co_return`.  
D) The `co_yield` keyword for generating values.  

---

**Question 11: `constexpr` Containers**  
Which containers support `constexpr` operations in C++20?  
A) `std::vector`  
B) `std::string`  
C) `std::map`  
D) `std::array`  

---

**Question 12: Comparison Categories**  
Which comparison categories are **valid** for `operator<=>`?  
A) `std::partial_ordering`  
B) `std::unordered_ordering`  
C) `std::weak_ordering`  
D) `std::exact_ordering`  

---

**Question 13: `requires` Expressions**  
Which are **valid** components of a `requires` expression?  
A) Nested requirements (`requires { ... }`).  
B) Type requirements (`typename T`).  
C) Compound requirements (`{ expr } -> Concept`).  
D) Return-type constraints for functions.  

---

**Question 14: `volatile` Improvements**  
Which statements about `volatile` in C++20 are **true**?  
A) `volatile` variables can be used in `constexpr` contexts.  
B) `volatile` member functions are deprecated.  
C) `std::atomic` replaces most `volatile` use cases.  
D) `volatile` applies to lambda captures implicitly.  

---

**Question 15: `[[likely]]` and `[[unlikely]]`**  
Which are **valid** uses of these attributes?  
A) Marking a loop branch as likely.  
B) Annotating `switch` cases.  
C) Applying to function declarations.  
D) Optimizing error-handling paths.  

---

**Question 16: `std::jthread`**  
Which features distinguish `std::jthread` from `std::thread`?  
A) Automatic joining on destruction.  
B) Support for cooperative interruption.  
C) A built-in `std::stop_token`.  
D) Thread-local storage guarantees.  

---

**Question 17: Aggregate Initialization**  
Which are **valid** C++20 aggregate initialization rules?  
A) Parentheses initialization is allowed.  
B) Private members can be initialized with designated initializers.  
C) Inherited members participate in aggregate initialization.  
D) Default member initializers override aggregate initializers.  

---

**Question 18: Lambda Template Parameters**  
Which are **valid** lambda template syntaxes in C++20?  
A) `[](auto x) { ... }`  
B) `[]<typename T>(T x) { ... }`  
C) `[]<Concept T>(T x) { ... }`  
D) `[]<int N>() { ... }`  

---

**Question 19: `std::is_constant_evaluated`**  
Which are **valid** uses of `std::is_constant_evaluated()`?  
A) Choosing between `constexpr` and runtime algorithms.  
B) Validating compile-time constants in `consteval` functions.  
C) Branching in `constinit` initializers.  
D) Enforcing constant evaluation in `constexpr` variables.  

---

**Question 20: Rewriting Expressions**  
Which expressions are **valid** under C++20 comparison rewriting?  
A) `a != b` → `!(a == b)`  
B) `a > b` → `(b <=> a) < 0`  
C) `a <= b` → `(a <=> b) <= 0`  
D) `a == b` → `(a <=> b) == 0`  

---

### Answers and Explanations

---

**Question 1**  
**Correct Answers**: A, C, D  
- A: Concepts can constrain `auto` (e.g., `Integral auto x = 5;`).  
- C: `requires` clauses replace SFINAE for constraints.  
- D: Abbreviated templates like `void f(Concept auto x)` generate templates.  
- B: Incorrect. Concepts can be defined with boolean expressions (e.g., `concept C = ...`).  

**Question 2**  
**Correct Answers**: B, C  
- B: `import` replaces `#include` for module interfaces.  
- C: Modules avoid header-style forward declarations.  
- A: Incorrect. Modules can have non-exported declarations.  
- D: Incorrect. Private fragments use `module :private;` syntax.  

**Question 3**  
**Correct Answers**: C, D  
- C: `a != b` rewrites to `!(a == b)` if `operator==` exists.  
- D: `operator<=>` works with `std::partial_ordering` for floats.  
- A: Incorrect. Defaulting `<=>` does **not** auto-generate `==`.  
- B: Incorrect. Return type can be `std::partial_ordering` or others.  

**Question 4**  
**Correct Answers**: A, C, D  
- A: Valid designated initialization.  
- C: Valid union initialization.  
- D: Valid for nested anonymous structs.  
- B: Invalid. Designators must follow declaration order.  

**Question 5**  
**Correct Answers**: B, D  
- B: `constinit` enforces constant initialization.  
- D: `consteval` functions (immediate functions) cannot have runtime side effects.  
- A: Incorrect. `consteval` functions are compile-time only.  
- C: Incorrect. `constinit` applies only to static storage duration variables.  

**Question 6**  
**Correct Answers**: B, C, D  
- B: `requires` enables conditional `explicit`.  
- C: C++20 allows templated lambdas (`[]<typename T>(T x) { ... }`).  
- D: Variable templates can be partially specialized.  
- A: Incorrect. Class-type NTTPs require structural equality (C++20).  

**Question 7**  
**Correct Answers**: A, C, D  
- A: Pack expansion in init-capture is allowed.  
- C: Stateless lambdas can be default-constructed.  
- D: `consteval` lambdas are valid.  
- B: Incorrect. `[=, this]` is redundant and deprecated; `[=]` captures `this` implicitly.  

**Question 8**  
**Correct Answers**: A, B  
- A: `[[nodiscard("reason")]]` provides a message.  
- B: `[[likely]]` guides branch prediction.  
- C: Incorrect. `[[no_unique_address]]` allows overlapping addresses.  
- D: Incorrect. `[[deprecated]]` is distinct from `#pragma message`.  

**Question 9**  
**Correct Answers**: B, C  
- B: `span<int, 5>` has fixed extent.  
- C: `span` provides a view, modifying elements affects the source.  
- A: Incorrect. `span` does not own memory.  
- D: Incorrect. `span` can be constructed from `vector` (via `data()` and `size()`).  

**Question 10**  
**Correct Answers**: A, B  
- A: The return type must define `promise_type`.  
- B: `co_await` is required for suspension.  
- C: Incorrect. `co_return` is optional (e.g., `void` coroutines).  
- D: Incorrect. `co_yield` is optional (used in generator coroutines).  

**Question 11**  
**Correct Answers**: A, B, D  
- A, B, D: `std::vector`, `std::string`, and `std::array` support `constexpr` in C++20.  
- C: `std::map` requires dynamic memory, not `constexpr`.  

**Question 12**  
**Correct Answers**: A, C  
- A: `std::partial_ordering` (e.g., for floats).  
- C: `std::weak_ordering` (e.g., case-insensitive strings).  
- B, D: Invalid. No such categories in C++20.  

**Question 13**  
**Correct Answers**: A, B, C  
- A: Nested requirements are allowed.  
- B: Type requirements check validity.  
- C: Compound requirements validate expressions.  
- D: Incorrect. Return-type constraints are part of compound requirements.  

**Question 14**  
**Correct Answers**: B, C  
- B: `volatile` member functions are deprecated.  
- C: `std::atomic` is preferred for concurrency.  
- A: Incorrect. `volatile` variables are not `constexpr`.  
- D: Incorrect. `volatile` must be explicit in captures.  

**Question 15**  
**Correct Answers**: A, B, D  
- A, B, D: Attributes apply to branches, `switch` cases, and error paths.  
- C: Incorrect. Attributes cannot annotate function declarations directly.  

**Question 16**  
**Correct Answers**: A, B, C  
- A, B, C: `jthread` auto-joins, supports interruption, and includes `stop_token`.  
- D: Incorrect. Thread-local storage is unrelated.  

**Question 17**  
**Correct Answers**: A, C  
- A: C++20 allows `Point p(1, 2);` for aggregates.  
- C: Inherited members participate in C++20 aggregates.  
- B: Incorrect. Designated initializers require public members.  
- D: Incorrect. Default initializers are used if no explicit value is provided.  

**Question 18**  
**Correct Answers**: B, C, D  
- B, C, D: Templated lambdas support type, concept, and NTTP parameters.  
- A: Valid but not a C++20-specific feature (generic lambdas since C++14).  

**Question 19**  
**Correct Answers**: A, C  
- A: Used to select compile-time vs. runtime logic.  
- C: Valid in `constinit` contexts.  
- B: Incorrect. `consteval` functions are always compile-time.  
- D: Incorrect. `constexpr` variables are already compile-time.  

**Question 20**  
**Correct Answers**: A, C, D  
- A: `a != b` uses `operator==` if available.  
- C, D: Valid rewrites for `<=` and `==`.  
- B: Incorrect. `a > b` rewrites to `(a <=> b) > 0`.  

--- 

### C++20 Design Problems

---

**Problem 1: Custom Concept for Sortable Types**
**Task**: Create a concept `Sortable` that checks if a type supports `<` comparison and default construction. Use it to constrain a `sortVector` template function.  
**Test Case**: Test with `std::vector<int>` and `std::vector<std::pair<int, std::string>>` (should fail compilation).

```cpp
#include <vector>
#include <concepts>
#include <algorithm>

template<typename T>
concept Sortable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    T(); // Default constructible
};

template<Sortable T>
void sortVector(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end());
}

// Test in main:
int main() {
    std::vector<int> intVec{3,1,4};
    sortVector(intVec); // OK

    struct NonSortable { int x; };
    std::vector<NonSortable> nsVec{{1}, {2}};
    // sortVector(nsVec); // Fails: no operator< and not default constructible
}
```

---

**Problem 2: Module Interface/Implementation Partition**
**Task**: Design a module `Math` that exports `add` and `multiply` functions. Split interface/implementation into separate files.  
**Test Case**: Import the module and verify calculations.

```cpp
// math.ixx (module interface)
export module Math;
export int add(int a, int b);
export int multiply(int a, int b);

// math.cpp (module implementation)
module Math;
int add(int a, int b) { return a + b; }
int multiply(int a, int b) { return a * b; }

// main.cpp
import Math;
#include <iostream>
int main() {
    std::cout << add(2,3) << "\n";      // 5
    std::cout << multiply(2,3) << "\n";// 6
}
```

---

**Problem 3: Three-Way Comparison for Custom Class**
**Task**: Implement `operator<=>` for a `Point3D` class (x,y,z coordinates). Support `std::partial_ordering` and test comparisons.

```cpp
#include <compare>
#include <iostream>

struct Point3D {
    int x, y, z;
    auto operator<=>(const Point3D&) const = default;
};

int main() {
    Point3D p1{1,2,3}, p2{1,2,4};
    std::cout << ((p1 < p2) ? "true" : "false");  // true
    std::cout << ((p1 == p2) ? "true" : "false"); // false
}
```

---

**Problem 4: Designated Initialization with Nested Structs**
**Task**: Use designated initializers for a nested struct `Person{Name{first, last}; int age;}`. Ensure initialization order matches declaration.

```cpp
struct Name { std::string first, last; };
struct Person { Name name; int age; };

int main() {
    Person p1{.name{.first = "John", .last = "Doe"}, .age = 30}; // OK
    // Person p2{.age = 30, .name{.first = "John"}}; // Error: wrong order
}
```

---

**Problem 5: consteval vs constexpr Functions**
**Task**: Create a `consteval` function `square` and a `constexpr` function `cube`. Test compile-time vs runtime execution.

```cpp
consteval int square(int n) { return n * n; }
constexpr int cube(int n) { return n * n * n; }

int main() {
    constexpr int s = square(4);  // OK: compile-time
    int x = 5;
    // int s2 = square(x);       // Error: non-constant argument
    int c = cube(x);             // OK: runtime
}
```

---

**Problem 6: Conditionally Explicit Constructor**
**Task**: Design a `Box<T>` class where the constructor is explicit if T is not integral.

```cpp
#include <concepts>

template<typename T>
class Box {
public:
    explicit(!std::integral<T>) Box(T val) : value(val) {}
private:
    T value;
};

int main() {
    Box<int> b1(42);       // OK: implicit
    Box<std::string> b2("hello"); // Requires explicit
}
```

---

**Problem 7: Lambda Template Parameters**
**Task**: Write a lambda with a template parameter to print elements of any container.

```cpp
#include <vector>
#include <list>
#include <iostream>

auto print = []<typename T>(const T& container) {
    for (const auto& x : container) std::cout << x << " ";
};

int main() {
    std::vector<int> v{1,2,3};
    std::list<double> l{4.5, 6.7};
    print(v); // 1 2 3
    print(l); // 4.5 6.7
}
```

---

**Problem 8: [[no_unique_address]] Optimization**
**Task**: Use `[[no_unique_address]]` to optimize memory layout of a `Widget` class containing empty allocator.

```cpp
struct EmptyAllocator {};
class Widget {
    int id;
    [[no_unique_address]] EmptyAllocator alloc;
public:
    Widget(int i) : id(i) {}
};

int main() {
    static_assert(sizeof(Widget) == sizeof(int)); // Should pass
}
```

---

**Problem 9: Rewriting Expressions with Operator==**
**Task**: Demonstrate operator rewriting for `operator==` and `operator!=` in a `StringWrapper` class.

```cpp
#include <string>
#include <iostream>

class StringWrapper {
    std::string s;
public:
    StringWrapper(const char* str) : s(str) {}
    bool operator==(const StringWrapper& other) const { return s == other.s; }
};

int main() {
    StringWrapper sw1("hello"), sw2("world");
    std::cout << (sw1 == "hello") << "\n"; // true (rewrites to sw1.operator==(StringWrapper("hello")))
    std::cout << (sw1 != sw2) << "\n";     // true (rewrites to !(sw1 == sw2))
}
```

---

**Problem 10: constinit for Static Initialization**
**Task**: Use `constinit` to ensure a static variable is initialized at compile-time.

```cpp
constinit static int global = 42; // Initialized at compile-time

int main() {
    static constinit int local = 100; // OK
    // constinit int x = 5; // Error: not static
    return global + local; // 142
}
```

---

### **Answers & Explanations**

---

**Problem 1 Explanation**
- **Concept Definition**: `Sortable` requires `<` operator and default constructor.
- **Compilation Failure**: `std::pair` lacks `<` unless specialized. Custom types without these traits will fail template substitution.

---

**Problem 2 Explanation**
- **Module Splitting**: Interface declares functions, implementation defines them.
- **Visibility**: Only exported entities are visible to importers.

---

**Problem 3 Explanation**
- **Auto-Generated Operator**: Defaulted `operator<=>` compares members lexicographically.
- **Ordering Category**: `int` members imply `std::strong_ordering`, but `auto` here deduces to default.

---

**Problem 4 Explanation**
- **Aggregate Initialization**: Designated initializers must follow declaration order.
- **Nested Initializers**: Subobjects are initialized recursively.

---

**Problem 5 Explanation**
- **consteval Enforcement**: Arguments must be constant expressions.
- **Runtime Usage**: `constexpr` functions can execute at runtime if arguments aren't constant.

---

**Problem 6 Explanation**
- **Explicit Control**: `explicit(!condition)` makes constructor conditionally explicit.
- **Template Argument Deduction**: `std::integral` checks type traits at compile-time.

---

**Problem 7 Explanation**
- **Lambda Templates**: C++20 allows template parameters in lambdas for generic handling.
- **Type Deduction**: Compiler deduces `T` from argument type.

---

**Problem 8 Explanation**
- **Empty Class Optimization**: `[[no_unique_address]]` allows overlapping with empty classes.
- **Size Assertion**: Compiler optimizes away empty allocator’s size.

---

**Problem 9 Explanation**
- **Operator Rewriting**: `a != b` becomes `!(a == b)`. Mixed-type comparisons use implicit conversions.

---

**Problem 10 Explanation**
- **constinit Guarantee**: Ensures static initialization avoids runtime overhead.
- **Lifetime Restrictions**: `constinit` variables must have static or thread storage duration.
