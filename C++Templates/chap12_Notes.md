@[TOC](Advanced Template Metaprogramming and Type Traits)

---

### 1. Key Concepts & Code Explanations

#### 1.1 SFINAE (Substitution Failure Is Not An Error)  
What: Enable/disable template overloads based on type properties using `std::enable_if`.  
Use Case: Restrict function templates to specific types.  
```cpp  
#include <type_traits>  

// Overload for arithmetic types  
template<typename T>  
typename std::enable_if<std::is_arithmetic<T>::value, void>::type  
print(T val) {  
    std::cout << val << "\n";  
}  

// Overload for non-arithmetic types  
template<typename T>  
typename std::enable_if<!std::is_arithmetic<T>::value, void>::type  
print(T) {  
    std::cout << "Non-arithmetic type\n";  
}  

int main() {  
    print(10);      // Output: 10  
    print("hello"); // Output: Non-arithmetic type  
}  
```

---

#### 1.2 Type Traits  
What: Inspect or modify type properties at compile time (e.g., `std::is_pointer`, `std::remove_reference`).  
Use Case: Enforce type constraints with `static_assert`.  
```cpp  
template<typename T>  
void process(T val) {  
    static_assert(std::is_integral<T>::value, "T must be integral");  
    // ...  
}  

int main() {  
    process(42); // OK  
    // process(3.14); // Compile error  
}  
```

---

#### 1.3 `constexpr` and Compile-Time Computation  
What: Evaluate expressions at compile time.  
Use Case: Compute factorials during compilation.  
```cpp  
constexpr int factorial(int n) {  
    return (n <= 1) ? 1 : n * factorial(n - 1);  
}  

int main() {  
    constexpr int result = factorial(5);  
    static_assert(result == 120, "Factorial of 5 must be 120");  
}  
```

---

#### 1.4 Variadic Templates  
What: Templates accepting a variable number of arguments.  
Use Case: Type-safe `printf` replacement.  
```cpp  
void log() {} // Base case  

template<typename T, typename... Args>  
void log(T first, Args... rest) {  
    std::cout << first << " ";  
    log(rest...);  
}  

int main() {  
    log("Error:", 404, "Not Found"); // Output: Error: 404 Not Found  
}  
```

---

#### 1.5 C++20 Concepts  
What: Define constraints on template parameters using readable syntax.  
Use Case: Ensure a type supports `operator+`.  
```cpp  
#include <concepts>  

template<typename T>  
concept Addable = requires(T a, T b) {  
    { a + b } -> std::same_as<T>;  
};  

template<Addable T>  
T sum(T a, T b) { return a + b; }  

int main() {  
    std::cout << sum(3, 4); // Output: 7  
    // sum("a", "b"); // Error: constraints not satisfied  
}  
```

---

### 2. Multiple-Choice Questions

#### Questions 1-5  
1. What is the primary use of `std::enable_if`?  
   A) Runtime type checking  
   B) Compile-time overload selection  
   C) Memory allocation  
   D) Exception handling  

2. Which code snippet uses SFINAE correctly?  
   A)  
   ```cpp  
   template<typename T>  
   void func(T val) { static_assert(std::is_integral<T>::value); }  
   ```
   B)  
   ```cpp  
   template<typename T>  
   typename std::enable_if<std::is_integral<T>::value>::type func(T val) {}  
   ```
   C) Both  
   D) Neither  

3. What does `std::remove_reference<T>::type` do?  
   A) Removes `const` from `T`  
   B) Strips references from `T`  
   C) Checks if `T` is a reference  
   D) Adds a reference to `T`  

4. Which C++ feature replaces SFINAE for type constraints?  
   A) `constexpr`  
   B) `noexcept`  
   C) Concepts  
   D) `decltype`  

5. What is the output of:  
   ```cpp  
   template<typename... Args>  
   int count(Args... args) { return sizeof...(Args); }  
   std::cout << count(1, "a", 3.14);  
   ```
   A) 3  
   B) 1  
   C) Compile error  
   D) Runtime error  

---

#### Questions 6-10  
6. Which type trait checks if a type is a pointer?  
   A) `std::is_pointer`  
   B) `std::is_reference`  
   C) `std::is_array`  
   D) `std::is_function`  

7. What is the purpose of `std::declval`?  
   A) Declare a variable  
   B) Create a value of type `T` in unevaluated contexts  
   C) Delete a value  
   D) Allocate memory  

8. Which code calculates `2^3` at compile time?  
   A)  
   ```cpp  
   constexpr int pow(int base, int exp) { return (exp == 0) ? 1 : base * pow(base, exp - 1); }  
   constexpr int result = pow(2, 3);  
   ```
   B)  
   ```cpp  
   int pow(int base, int exp) { return base ^ exp; }  
   const int result = pow(2, 3);  
   ```
   C) Both  
   D) Neither  

9. What does `std::void_t` do?  
   A) Always returns `void`  
   B) Causes substitution failure for invalid types  
   C) Checks if a type is `void`  
   D) Converts a type to `void`  

10. Which syntax correctly defines a variadic template?  
    A) `template<typename T, ...>`  
    B) `template<typename T, typename... Args>`  
    C) `template<typename T, typename Args...>`  
    D) `template<typename... T>`  

---

#### 1 - 10 Answers & Explanations  
1. B (SFINAE enables/disables overloads at compile time).  
2. B (Uses `std::enable_if` to conditionally enable the function).  
3. B (Strips references, e.g., `int&` → `int`).  
4. C (Concepts provide cleaner syntax for constraints).  
5. A (`sizeof...(Args)` returns 3).  
6. A (`std::is_pointer` checks pointer types).  
7. B (`std::declval` creates a value in unevaluated contexts like `decltype`).  
8. A (Compile-time recursion with `constexpr`).  
9. B (`std::void_t` triggers substitution failure for invalid types).  
10. B (`typename... Args` declares a parameter pack).  

#### Questions 11-15  
11. What is the purpose of `std::enable_if`?  
   A) To enable runtime type checks  
   B) To conditionally remove function overloads during template substitution  
   C) To allocate memory for templates  
   D) To disable exception handling  

12. What does `std::decay<T>::type` do?  
   A) Removes `const` and references from `T`  
   B) Converts `T` to its "natural" type (e.g., array to pointer, function to function pointer)  
   C) Adds `const` to `T`  
   D) Checks if `T` is a primitive type  

13. Which code snippet correctly uses SFINAE to enable a function for integral types?  
   A)  
   ```cpp  
   template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>  
   void func(T val) {}  
   ```
   B)  
   ```cpp  
   template<typename T>  
   void func(T val) requires std::is_integral_v<T> {}  
   ```
   C) Both A and B  
   D) Neither  

14. What is the result of `std::is_same_v<std::remove_const_t<const int>, int>`?  
   A) `true`  
   B) `false`  
   C) Compile error  
   D) Undefined  

15. Which syntax defines a variadic template parameter pack?  
   A) `template<typename... T>`  
   B) `template<typename T, typename... Args>`  
   C) `template<typename T, typename Args...>`  
   D) Both A and B  

---

#### Questions 16-20  
16. What does `std::declval<T>()` allow you to do?  
   A) Declare a variable of type `T`  
   B) Use `T` in unevaluated contexts (e.g., `decltype`) without constructing it  
   C) Allocate memory for `T`  
   D) Check if `T` is default-constructible  

17. Which code computes `3!` (6) at compile time?  
   A)  
   ```cpp  
   constexpr int factorial(int n) { return n <= 11 ? 11 : n * factorial(n - 11); }  
   constexpr int result = factorial(3);  
   ```
   B)  
   ```cpp  
   int factorial(int n) { return n <= 11 ? 11 : n * factorial(n - 11); }  
   const int result = factorial(3);  
   ```
   C) Both  
   D) Neither  

18. What is the output of:  
   ```cpp  
   template<typename... Args>  
   int countArgs(Args...) { return sizeof...(Args); }  
   std::cout << countArgs(11, "a", 3.14);  
   ```
   A) 3  
   B) 11  
   C) Compile error  
   D) Runtime error  

19. Which C++20 feature simplifies type constraints over SFINAE?  
   A) `constexpr`  
   B) `noexcept`  
   C) Concepts  
   D) `decltype`  

20. What does `std::void_t<T>` check for?  
    A) If `T` is `void`  
    B) If a valid type `T` exists (triggers substitution failure if not)  
    C) Converts `T` to `void`  
    D) Checks if `T` is a function  

---

#### 11-20 Answers & Explanations  
11. B (SFINAE removes invalid overloads during substitution).  
12. B (`std::decay` converts arrays to pointers, functions to pointers, etc.).  
13. C (Both use valid SFINAE/concepts syntax).  
14. A (`remove_const_t<const int>` is `int`).  
15. D (Both A and B are valid syntax for parameter packs).  
16. B (`std::declval` is used in unevaluated contexts like `decltype`).  
17. A (`constexpr` ensures compile-time computation).  
18. A (`sizeof...(Args)` returns 3).  
19. C (Concepts replace SFINAE with cleaner syntax).  
20. B (`std::void_t` triggers substitution failure for invalid types).  

---

### 3. Design Questions 

#### Question 1: Compile-Time Type List  
Task: Create a type list that stores types and provides a `size` member.  
```cpp  
template<typename... Ts>  
struct TypeList {  
    static constexpr size_t size = sizeof...(Ts);  
};  

// Test Case  
static_assert(TypeList<int, double, char>::size == 3, "Size should be 3");  
```

---

#### Question 2: CRTP (Curiously Recurring Template Pattern)  
Task: Implement a base class using CRTP to count object instances.  
```cpp  
template<typename Derived>  
class InstanceCounter {  
protected:  
    static int count;  
public:  
    InstanceCounter() { ++count; }  
    ~InstanceCounter() { --count; }  
    static int getCount() { return count; }  
};  

template<typename Derived>  
int InstanceCounter<Derived>::count = 0;  

class Widget : public InstanceCounter<Widget> {};  

// Test Case  
int main() {  
    Widget w1, w2;  
    std::cout << Widget::getCount(); // Output: 2  
}  
```

---

#### Question 3: Fold Expression for Sum  
Task: Compute the sum of all arguments using a fold expression.  
```cpp  
template<typename... Args>  
auto sum(Args... args) {  
    return (... + args); // Unary right fold  
}  

// Test Case  
int main() {  
    std::cout << sum(1, 2, 3.5); // Output: 6.5  
}  
```

---

#### Question 4: Type-Safe `printf` with Variadic Templates  
Task: Implement a `printf` that checks format specifiers against argument types.  
```cpp  
void safe_printf(const char* s) {  
    while (*s) {  
        if (*s == '%' && *(++s) != '%')  
            throw std::runtime_error("Invalid format");  
        std::cout << *s++;  
    }  
}  

template<typename T, typename... Args>  
void safe_printf(const char* s, T val, Args... args) {  
    while (*s) {  
        if (*s == '%' && *(++s) != '%') {  
            if (*s == 'd' && !std::is_integral<T>::value)  
                throw std::runtime_error("Expected integer");  
            std::cout << val;  
            safe_printf(++s, args...);  
            return;  
        }  
        std::cout << *s++;  
    }  
}  

// Test Case  
int main() {  
    safe_printf("%d %s", 10, "hello"); // Throws if types don't match  
}  
```

---

#### Question 5: Concept-Based `sort` Function  
Task: Use C++20 concepts to constrain a `sort` function to random-access containers.  
```cpp  
#include <concepts>  
#include <vector>  
#include <list>  

template<typename Container>  
concept RandomAccess = requires(Container c) {  
    { c.begin() } -> std::random_access_iterator;  
};  

template<RandomAccess Container>  
void sort(Container& c) {  
    std::sort(c.begin(), c.end());  
}  

// Test Case  
int main() {  
    std::vector<int> vec = {3, 1, 4};  
    sort(vec); // OK  
    std::list<int> lst = {3, 1, 4};  
    // sort(lst); // Error: Not random-access  
}  
```

---
#### Question 6: Custom Type Trait for Member Function Check  
Task: Create a trait `has_member_foo` to check if a type has a member function `void foo()`.  
```cpp  
#include <type_traits>  

template<typename T, typename = void>  
struct has_member_foo : std::false_type {};  

template<typename T>  
struct has_member_foo<T, std::void_t<decltype(std::declval<T>().foo())>>  
    : std::true_type {};  

// Test Case  
struct Test { void foo() {} };  
struct Empty {};  

int main() {  
    static_assert(has_member_foo<Test>::value, "Test has foo()");  
    static_assert(!has_member_foo<Empty>::value, "Empty does not have foo()");  
}  
```

---

#### Question 7: Compile-Time Factorial with Variadic Templates  
Task: Compute the product of a parameter pack at compile time.  
```cpp  
template<int... Args>  
constexpr int product() {  
    return (Args * ...); // Fold expression  
}  

// Test Case  
int main() {  
    static_assert(product<2, 3, 4>() == 24, "2*3*4=24");  
}  
```

---

#### Question 8: CRTP for Static Polymorphism  
Task: Use CRTP to add `serialize()` to derived classes.  
```cpp  
template<typename Derived>  
struct Serializable {  
    std::string serialize() const {  
        return static_cast<const Derived*>(this)->serialize_impl();  
    }  
};  

class Data : public Serializable<Data> {  
    int value = 42;  
    friend class Serializable<Data>;  
    std::string serialize_impl() const { return std::to_string(value); }  
};  

// Test Case  
int main() {  
    Data d;  
    std::cout << d.serialize(); // Output: "42"  
}  
```

---

#### Question 9: Concept for Iterators  
Task: Use C++20 concepts to constrain a function to accept iterators.  
```cpp  
#include <iterator>  
#include <vector>  
#include <list>  

template<typename It>  
concept Iterator = requires(It it) {  
    { *it } -> std::same_as<typename std::iterator_traits<It>::reference>;  
    { ++it } -> std::same_as<It&>;  
};  

template<Iterator It>  
void iterate(It begin, It end) {  
    for (; begin != end; ++begin) std::cout << *begin << " ";  
}  

// Test Case  
int main() {  
    std::vector<int> vec = {11, 2, 3};  
    iterate(vec.begin(), vec.end()); // Output: 11 2 3  
}  
```

---

#### Question 10: Tuple Class with Variadic Templates  
Task: Implement a basic tuple using variadic templates.  
```cpp  
template<typename... Ts>  
class Tuple {};  

template<typename T, typename... Ts>  
class Tuple<T, Ts...> : public Tuple<Ts...> {  
    T value;  
public:  
    Tuple(T v, Ts... vs) : Tuple<Ts...>(vs...), value(v) {}  
    T getHead() const { return value; }  
};  

// Test Case  
int main() {  
    Tuple<int, std::string, double> t(10, "test", 3.14);  
    std::cout << t.getHead(); // Output: 10  
}  
```

---

### Summary  
This guide reinforces:  
1. SFINAE and type traits for compile-time type introspection.  
2. Variadic templates and fold expressions for parameter pack operations.  
3. C++20 concepts for readable type constraints.  
4. CRTP for static polymorphism.  