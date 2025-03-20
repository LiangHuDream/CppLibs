Key Concept:  
- Templates allow creating generic classes parameterized by types/values
- Syntax: `template<typename T> class Stack { ... };`

Code Example:
```cpp
template<typename T>
class Stack {
private:
    std::vector<T> elems;  // Elements stored in vector
public:
    void push(T const&);   // Push element
    T pop();               // Pop element
};
```
Explanation:  
- `template<typename T>` declares a type parameter `T`
- `Stack<T>` becomes a distinct type for each template argument (e.g., `Stack<int>` vs `Stack<string>`)

---

### 2. Member Function Implementation
Key Concept:  
- Member functions must also be templates
- Out-of-class definitions require template parameters

Code Example:
```cpp
template<typename T>
void Stack<T>::push(T const& elem) {
    elems.push_back(elem);  // Append copy of elem
}

template<typename T>
T Stack<T>::pop() {
    if (elems.empty()) throw std::out_of_range("Stack<>::pop(): empty");
    T elem = elems.back();
    elems.pop_back();
    return elem;
}
```
Why It Matters:  
- `Stack<T>::` qualifies the member function to the class template
- Each specialization (e.g., `Stack<int>`) has its own member functions

---

### 3. Using Class Templates
Key Concept:  
- Must specify template arguments when instantiating
- Example: `Stack<int> intStack;`

Common Pitfall:  
- Unlike function templates, class templates cannot deduce arguments from constructor parameters in C++14/C++17 (until C++17 deduction guides)

---

### 4. Partial Usage of Class Templates
Key Insight:  
- A class template can be used even if not all operations are supported for a type
- Example: `Stack<NoCopyType>` works if you never call `pop()` (which requires copyability)

Implication:  
- Templates are lazily instantiated; only used member functions are compiled

---

### 5. Friends & Specialization
Friend Function Syntax:
```cpp
template<typename U>
friend std::ostream& operator<<(std::ostream&, Stack<U> const&);
```
Specialization:
```cpp
// Full specialization for std::string
template<>
class Stack<std::string> {
    // Custom implementation for strings
};
```
Partial Specialization:
```cpp
// Partial specialization for pointer types
template<typename T>
class Stack<T*> {
    // Special handling for pointers
};
```

Why Important:  
- Full specialization replaces all members
- Partial specialization allows custom behavior for categories of types

---

### 6. Default Template Arguments
Syntax:
```cpp
template<typename T, typename Cont = std::vector<T>>
class Stack {
    Cont elems;  // Custom container (default: vector)
};
```
Usage:  
`Stack<int>` uses `std::vector<int>`, while `Stack<double, std::deque<double>>` uses a deque.

---

### 7. Type Aliases (C++11)
Using `using`:
```cpp
template<typename T>
using DequeStack = Stack<T, std::deque<T>>;
```
Equivalent to:  
`typedef Stack<T, std::deque<T>> DequeStack<T>;` but more flexible.

---

### 8. Class Template Argument Deduction (C++17)
Deduction Guides:
```cpp
// Deduce template argument from constructor
Stack(const char*) -> Stack<std::string>;
```
Usage:  
`Stack s{"Hello"};` deduces `s` as `Stack<std::string>`.

---

### 9. Templatized Aggregates

Key Point:  
- A class template can be an aggregate if it has no user-declared constructors, private/protected members, etc.
- Allows brace initialization:
```cpp
template<typename T>
struct Point {
    T x, y;
};
Point<int> p{1, 2};  // Aggregate initialization
```

### 10 Multiple-Choice Questions (Answers & Explanations Below)

---

#### 1. When are member functions of a class template instantiated?
A) When the class template is declared.  
B) When the class template is explicitly instantiated.  
C) Only when the member function is called.  
D) When the program starts execution.  

Answer:  
C) Only when the member function is called.  
Member functions of class templates are instantiated on-demand (when called). This avoids unnecessary code generation.

---

#### 2. Which syntax correctly declares a partial specialization for `Stack<T*>`?
A) `template<typename T> class Stack<T*> { ... };`  
B) `template<> class Stack<T*> { ... };`  
C) `template<typename U> class Stack<U*> { ... };`  
D) `template<class T> class Stack<T*> { ... };`  

Answer:  
C) `template<typename U> class Stack<U*> { ... };`  
Partial specializations retain a parameter list. Here, `U*` matches any pointer type, with `U` as the template parameter.

---

#### 3. What is the output of `std::cout << sizeof(Stack<int>);` if `Stack` uses `std::vector` internally?  
A) Size of `std::vector<int>`.  
B) 1 (empty class optimization).  
C) Compilation error.  
D) Depends on the compiler.  

Answer:  
A) Size of `std::vector<int>`.  
 `Stack<int>` contains a `std::vector<int>` member. The size reflects the vector’s internal state (pointers, size, capacity).

---

#### 4. Which declaration allows `Stack<std::string>` to use a `std::deque` container by default?  
A) `template<typename T, typename Cont = std::deque<T>> class Stack;`  
B) `template<typename T, typename Cont = std::vector<T>> class Stack;`  
C) `template<typename T = std::deque> class Stack;`  
D) `template<typename T, Cont = std::deque> class Stack;`  

Answer:  
A) `template<typename T, typename Cont = std::deque<T>> class Stack;`  
 Default template arguments must specify all type dependencies. `std::deque<T>` is correct.

---

#### 5. What is the purpose of a deduction guide for `Stack(const char*)`?  
A) Handle string literals as `const char[]`.  
B) Prevent implicit conversion to `std::string`.  
C) Allow CTAD for C-style strings.  
D) All of the above.  

Answer:  
D) All of the above.  
Deduction guides resolve ambiguities when initializing with string literals, ensuring the correct type (`std::string` or `const char*`) is deduced.

---

#### 6. Which code snippet correctly declares a type alias for `Stack<int>` using C++11 syntax?  
A) `typedef Stack<int> IntStack;`  
B) `using IntStack = Stack<int>;`  
C) `#define IntStack Stack<int>`  
D) Both A & B.  

Answer:  
D) Both A & B.  
`typedef` (legacy) and `using` (modern) are both valid. `using` is preferred for aliases.

---

#### 7. What happens if a class template’s member function is never called?  
A) It is still compiled.  
B) It is not instantiated.  
C) It causes a linker error.  
D) The program crashes.  

Answer:  
B) It is not instantiated.  
Unused member functions are not instantiated, saving compile time and binary size.

---

#### 8. Which code snippet defines a templatized aggregate?  
A)  
```cpp
template<typename T>
struct Point { T x; T y; };
```
B)  
```cpp
template<typename T>
class Point { 
public: T x; T y; 
    Point(T a, T b) : x(a), y(b) {}
};
```
C) Both.  
D) None.  

Answer:  
A) Templatized aggregates have no user-provided constructors. Option B has a constructor, making it non-aggregate.

---

#### 9. What is the output of the following code?  
```cpp
Stack<const char*> s = "Hello";
std::cout << s.top();
```
Assume `Stack` uses `std::vector` and has no deduction guides.  
A) Hello  
B) Compilation error (ambiguous deduction).  
C) Undefined behavior.  
D) Empty output.  

Answer:  
B) Compilation error.
Without a deduction guide, `Stack(const char*)` cannot deduce `T = std::string` automatically. A guide like `Stack(const char*) -> Stack<std::string>` is needed.  

---

#### 10. Which code correctly declares a friend function for `operator<<` in `Stack`?  
A)  
```cpp
template<typename T>
class Stack {
    friend std::ostream& operator<<(std::ostream&, const Stack&);
};
```
B)  
```cpp
template<typename T>
class Stack {
    template<typename U>
    friend std::ostream& operator<<(std::ostream&, const Stack<U>&);
};
```
C) Both.  
D) None.  

Answer:  
B) The friend declaration must match the template signature. Option A declares a non-template friend, which only works for the current instantiation (`Stack<T>`). Option B makes the operator a template friend for all `Stack<U>`.

---

### 11. Design Questions (Solutions & Test Cases Below)

---

#### 1. Implement `Stack<T>` with `std::vector` and a partial specialization for `Stack<T*>` that deletes pointers on destruction.
```cpp
#include <vector>
#include <iostream>

// Primary template
template<typename T>
class Stack {
    std::vector<T> elems;
public:
    void push(const T& elem) { elems.push_back(elem); }
    T pop() {
        T elem = elems.back();
        elems.pop_back();
        return elem;
    }
};

// Partial specialization for pointers
template<typename T>
class Stack<T*> {
    std::vector<T*> elems;
public:
    void push(T* elem) { elems.push_back(elem); }
    T* pop() {
        T* elem = elems.back();
        elems.pop_back();
        return elem;
    }
    ~Stack() { 
        for (auto ptr : elems) delete ptr; 
    }
};

// Test Case
int main() {
    Stack<int> intStack;
    intStack.push(42);
    std::cout << intStack.pop() << "\n"; // 42

    Stack<int*> ptrStack;
    ptrStack.push(new int(100));
    int* val = ptrStack.pop();
    std::cout << *val << "\n"; // 100
    delete val;
}
```

---

#### 2. Design a `MathVector` class template supporting CTAD for `std::initializer_list`.  
```cpp
#include <vector>
#include <iostream>

template<typename T>
class MathVector {
    std::vector<T> data;
public:
    MathVector(std::initializer_list<T> ilist) : data(ilist) {}
    // Deduction guide (not needed in C++20)
};

// CTAD guide (C++17)
template<typename T> MathVector(T) -> MathVector<T>;

// Test Case
int main() {
    MathVector vec = {1, 2, 3}; // Deduces MathVector<int>
    for (auto x : vec.data) std::cout << x << " "; // 1 2 3
}
```

---

#### 3. Create a templatized aggregate `Pair` with CTAD support for mixed types.  
```cpp
template<typename T, typename U>
struct Pair {
    T first;
    U second;
};

// Deduction guide for mixed types
template<typename T, typename U>
Pair(T, U) -> Pair<T, U>;

// Test Case
int main() {
    Pair p1{5, 3.14};       // Pair<int, double>
    Pair p2{"Hello", 'A'};  // Pair<const char*, char>
    std::cout << p1.first << " " << p1.second << "\n"; // 5 3.14
}
```

---

#### 4. Implement a `SmartPointer` class template with a specialization for arrays.  
```cpp
#include <iostream>

// Primary template
template<typename T>
class SmartPointer {
    T* ptr;
public:
    explicit SmartPointer(T* p) : ptr(p) {}
    ~SmartPointer() { delete ptr; }
    T& operator*() const { return *ptr; }
};

// Specialization for arrays
template<typename T>
class SmartPointer<T[]> {
    T* ptr;
public:
    explicit SmartPointer(T* p) : ptr(p) {}
    ~SmartPointer() { delete[] ptr; }
    T& operator[](size_t idx) { return ptr[idx]; }
};

// Test Case
int main() {
    SmartPointer<int> sp(new int(42));
    std::cout << *sp << "\n"; // 42

    SmartPointer<int[]> arr(new int[3]{1,2,3});
    std::cout << arr[1] << "\n"; // 2
}
```

---

#### 5. Design a `Matrix` class template with a partial specialization for `Matrix<T, 1>` (a vector).  
```cpp
#include <iostream>
#include <vector>

// Primary template for N-dimensional matrix
template<typename T, size_t N>
class Matrix {
    std::vector<T> data;
    size_t dimensions[N];
public:
    Matrix(std::initializer_list<size_t> dims) { /*...*/ }
};

// Partial specialization for 1D (vector)
template<typename T>
class Matrix<T, 1> {
    std::vector<T> data;
public:
    Matrix(size_t size) : data(size) {}
    T& operator[](size_t idx) { return data[idx]; }
};

// Test Case
int main() {
    Matrix<int, 1> vec(3);
    vec[0] = 10;
    vec[1] = 20;
    std::cout << vec[1] << "\n"; // 20
}
```

---
