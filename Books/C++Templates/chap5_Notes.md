### Step-by-Step Explanation of Chapter 5: Tricky Basics

#### 1. Keyword `typename`
Purpose: Disambiguate dependent type names in templates.  
Use Case: Required when a name depends on a template parameter and represents a type.  
Code Example:
```cpp
template <typename T>
class MyClass {
    typename T::SubType* ptr; // "typename" required for dependent type
};

struct X {
    using SubType = int;
};

int main() {
    MyClass<X> obj; // Valid: T::SubType is int
    return 0;
}
```
Test Case: Without `typename`, the compiler would treat `T::SubType` as a static member, causing a syntax error.

---

#### 2. Zero Initialization
Purpose: Ensure built-in types are initialized to zero in templates.  
Code Example:
```cpp
template <typename T>
void init() {
    T val{}; // Zero-initialize (0 for int, nullptr for pointers)
    std::cout << "Value: " << val << std::endl;
}

int main() {
    init<int>();       // Output: Value: 0
    init<double>();    // Output: Value: 0
    init<int*>();      // Output: Value: 0 (nullptr as 0)
    return 0;
}
```

---

#### 3. Using `this->` in Templates
Purpose: Access members inherited from dependent base classes.  
Code Example:
```cpp
template <typename T>
class Base {
public:
    void foo() { std::cout << "Base::foo\n"; }
};

template <typename T>
class Derived : public Base<T> {
public:
    void bar() {
        this->foo(); // "this->" required to access Base<T>::foo
    }
};

int main() {
    Derived<int> d;
    d.bar(); // Output: Base::foo
    return 0;
}
```
Test Case: Without `this->`, `foo` would not be found during name lookup.

---

#### 4. Templates for Raw Arrays
Handling Arrays: Avoid pointer decay by using references.  
Code Example:
```cpp
template <typename T, int N>
void printArray(T (&arr)[N]) { // Pass array by reference
    for (int i=0; i<N; ++i) 
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

int main() {
    int arr[] = {1, 2, 3};
    printArray(arr); // Output: 1 2 3
    return 0;
}
```

---

#### 5. Member Templates
Purpose: Define templates inside classes (e.g., for conversions).  
Code Example:
```cpp
class MyContainer {
public:
    template <typename U>
    void assign(const U& val) { // Member function template
        data = static_cast<T>(val);
    }
private:
    T data;
};

int main() {
    MyContainer<int> c;
    c.assign(3.14); // Converts double to int
    return 0;
}
```

---

#### 6. Variable Templates (C++14)
Purpose: Define type-generic constants.  
Code Example:
```cpp
template <typename T>
constexpr T pi = T(3.1415926535897932385L);

int main() {
    std::cout << pi<int> << std::endl;     // Output: 3
    std::cout << pi<double> << std::endl;  // Output: 3.14159
    return 0;
}
```

---

#### 7. Template Template Parameters
Purpose: Pass a template as a parameter.  
Code Example:
```cpp
template <template <typename> class Container, typename T>
class Adapter {
    Container<T> data;
public:
    void add(const T& val) { data.push_back(val); }
    void print() {
        for (const auto& x : data) 
            std::cout << x << " ";
        std::cout << std::endl;
    }
};

int main() {
    Adapter<std::vector, int> adapter;
    adapter.add(42);
    adapter.print(); // Output: 42
    return 0;
}
```
Key Point: Ensure the template template parameter’s signature matches (e.g., `template <typename> class`, not `template <typename, typename>`).

---

### Summary of Tests
1. `typename`: Validates dependent type resolution.  
2. Zero Init: Ensures uninitialized variables have predictable values.  
3. `this->`: Confirms access to inherited members.  
4. Array Handling: Prevents pointer decay in templates.  
5. Member Templates: Tests cross-type assignments.  
6. Variable Templates: Checks type-specific constant values.  
7. Template Template: Validates container adaptation behavior.



### Multiple-Choice Questions

1. Which scenarios require the use of `typename` keyword in template programming?  
A) Declaring a template type parameter  
B) Accessing a nested dependent type in a template  
C) Referring to a static member of a template class  
D) Qualifying a base class member in a derived class template  

2. Which statements about member templates are correct?  
A) Member function templates cannot be virtual  
B) Member templates can be partially specialized  
C) A member template can have a default template argument  
D) Member templates are only valid in class templates  

3. What is the correct syntax for a template template parameter?  
A) `template<typename T> class Container`  
B) `template<template<typename> class Container> class MyClass`  
C) `template<typename T, template<typename> class Container> class MyClass`  
D) `template<template<typename> typename Container> class MyClass`  

4. Which code snippets correctly implement zero-initialization for a template type `T`?  
A) `T obj = T();`  
B) `T obj{};`  
C) `T obj(0);`  
D) `T obj;`  

5. What is true about template argument deduction for function templates?  
A) It works with explicit specialization  
B) It considers implicit type conversions for deduced parameters  
C) It can deduce template arguments from default function arguments  
D) It fails if the template parameter is used in a non-deduced context  

6. Which scenarios are valid uses of variable templates?  
A) `template<typename T> constexpr T pi = T(3.1415926535);`  
B) `template<typename T> T globalVar;`  
C) `template<auto N> constexpr auto square = N * N;`  
D) `template<typename T> using Vec = std::vector<T>;`  

7. What is the output of the following code?  
```cpp
template<typename T> void foo(T) { std::cout << "1"; }
template<typename T> void foo(T*) { std::cout << "2"; }
int main() {
    int* p = nullptr;
    foo(p);
    foo<int>(p);
}
```  
A) 11  
B) 21  
C) 12  
D) 22  

8. Which statements about `template` disambiguator are correct?  
A) It is required when accessing a dependent template member  
B) It is used to indicate a template specialization  
C) It helps the parser distinguish between comparison operators and nested templates  
D) It is mandatory when using member templates in lambda expressions  

9. What is true about raw arrays and string literals in template functions?  
A) They decay to pointers when passed by value  
B) `template<typename T, size_t N> void foo(T (&arr)[N])` can capture array size  
C) `const char*` specialization handles string literals better than raw arrays  
D) Deduction guides cannot prevent array-to-pointer decay  

10. Which code snippets correctly use `std::enable_if` for SFINAE?  
A)  
```cpp
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void foo(T) {}
```  
B)  
```cpp
template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, void> foo(T) {}
```  
C)  
```cpp
template<typename T>
void foo(T) requires std::is_class_v<T> {}
```  
D)  
```cpp
template<typename T>
void foo(T) noexcept(std::is_nothrow_copy_constructible_v<T>) {}
```

---

### Design Questions

1. Implement a `Stack` class template with a nested iterator member template  
Requirements:  
- Support `begin()`, `end()`, and reverse iterators  
- Iterator must work with range-based for loops  
- Provide const and non-const iterator versions  

2. Design a `Matrix` template using template template parameters  
Requirements:  
- Use `template<template<typename> class Container> class Matrix`  
- Matrix dimensions must be compile-time constants (non-type template parameters)  
- Support element-wise operations via operator overloading  

3. Create a type trait `IsCallableWithArgs` using SFINAE  
Requirements:  
- Check if a type `F` can be invoked with arguments of types `Args...`  
- Handle function objects, lambdas, and function pointers  
- Provide a `value` static member indicating true/false  

4. Implement a `Tuple` class template with recursive variadic templates  
Requirements:  
- Support `get<N>(tuple)` with compile-time index checking  
- Implement element storage without using std::tuple  
- Provide a `makeTuple` function with deduction guides  

5. Optimize a `Vector` template using expression templates  
Requirements:  
- Avoid temporary objects in expressions like `vec3 = vec1 + vec2`  
- Use CRTP for expression templates  
- Support lazy evaluation of arithmetic operations  

---

### Answers & Explanations

#### Multiple-Choice Answers

1. B, D  
   `typename` is required for accessing nested dependent types (B) and qualifying base class members in derived templates (D). A uses `template` syntax; C accesses static members without `typename`.

2. A, C  
   Member templates cannot be virtual (A). Default template arguments are allowed (C). Partial specialization is not allowed for member templates (B). Member templates exist in non-template classes too (D错).

3. B, C, D  
   Valid syntaxes for template template parameters. A is a regular template declaration.

4. A, B  
   `T()` and `T{}` perform value-initialization. C fails if `T` has no constructor accepting 0. D leaves `obj` uninitialized.

5. B, D  
   Deduction allows trivial conversions (B). Non-deduced contexts (e.g., `typename Identity<T>::type`) cause failure (D). Default arguments don't participate in deduction (C错).

6. A, B, C  
   Variable templates can have types (A), non-const globals (B), and non-type params (C). D is an alias template.

7. B  
   `foo(p)` deduces `T=int` (overload 1). `foo<int>(p)` explicitly selects `T=int`, matching overload 2 (`T*` is `int*`).

8. A, C  
   `template` disambiguates dependent templates (A) and helps parser (C). Not needed for specialization (B错) or lambdas (D错).

9. A, B  
   Arrays decay when passed by value (A). Reference to array captures size (B). Specializing for `const char*` doesn't handle all arrays (C错). Deduction guides can prevent decay (D错).

10. A, B, C  
   All are valid SFINAE patterns. D checks noexcept but doesn't disable overloads.

---

#### Design Question Solutions

1. Stack with Nested Iterator  
```cpp
template<typename T>
class Stack {
    std::vector<T> elems;
public:
    template<typename IterTag>
    class Iterator {
        using iterator_type = typename std::vector<T>::iterator;
        iterator_type it;
    public:
        using iterator_category = IterTag;
        // ... iterator members
    };
    using iterator = Iterator<std::random_access_iterator_tag>;
    iterator begin() { return {elems.begin()}; }
    iterator end() { return {elems.end()}; }
};

// Test
int main() {
    Stack<int> s;
    s.push(1); s.push(2);
    for (auto it = s.begin(); it != s.end(); ++it)
        std::cout << *it << ' '; // 1 2
}
```

2. Matrix with Template Template Parameter  
```cpp
template<template<typename> class Container, typename T, size_t Rows, size_t Cols>
class Matrix {
    Container<Container<T>> data;
public:
    Matrix() : data(Rows, Container<T>(Cols)) {}
    // ... operator+ etc.
};

// Test
int main() {
    Matrix<std::vector, float, 3, 3> mat;
    // Use mat
}
```

3. `IsCallableWithArgs` Trait  
```cpp
template<typename F, typename... Args>
struct IsCallableWithArgs {
    template<typename U>
    static auto test(U*) -> decltype(std::declval<U>()(std::declval<Args>()...), std::true_type{});
    static auto test(...) -> std::false_type;
    static constexpr bool value = decltype(test(static_cast<F*>(nullptr)))::value;
};

// Test
static_assert(IsCallableWithArgs<std::function<void(int)>, int>::value);
```

4. Custom Tuple Implementation  
```cpp
template<typename... Ts>
class Tuple;
template<typename T, typename... Ts>
class Tuple<T, Ts...> : Tuple<Ts...> {
    T value;
public:
    Tuple(T v, Ts... vs) : Tuple<Ts...>(vs...), value(v) {}
    // ... get<N> via inheritance
};
template<size_t N, typename T>
auto get(Tuple<T>& t) { /* ... */ }

// Test
auto t = makeTuple(42, "hello");
std::cout << get<0>(t); // 42
```

5. Vector with Expression Templates  
```cpp
template<typename E>
struct VecExpression {
    auto operator[](size_t i) const { return static_cast<const E&>(*this)[i]; }
    size_t size() const { return static_cast<const E&>(*this).size(); }
};
template<typename E1, typename E2>
struct VecSum : VecExpression<VecSum<E1, E2>> {
    // ... evaluate lazily
};
template<typename T>
class Vector : public VecExpression<Vector<T>> {
    // ... CRTP implementation
};

// Test
Vector<int> v1, v2, v3;
v3 = v1 + v2; // No temporaries
```

---
