### Step-by-Step Guide to Mastering Chapter 5: Move Semantics and Smart Pointers in C++

---

### 1. Key Concepts & Code Explanations

#### 1.1 Move Semantics  
Why: Optimize resource management (e.g., memory, files) by transferring ownership instead of copying.  
Key Terms:  
- Rvalue References: `T&&` binds to temporary objects.  
- Move Constructor/Assignment: Transfers resources from a source object.  
- `std::move`: Casts an lvalue to an rvalue.  

Example: Custom `String` class with move semantics.  
```cpp
class String {
    char* data;
public:
    // Move constructor
    String(String&& src) noexcept : data(src.data) {
        src.data = nullptr; // Reset source to avoid double-free
    }

    // Move assignment
    String& operator=(String&& src) noexcept {
        if (this != &src) {
            delete[] data;
            data = src.data;
            src.data = nullptr;
        }
        return *this;
    }

    ~String() { delete[] data; }
};

int main() {
    String s1("Hello");
    String s2 = std::move(s1); // Invokes move constructor
}
```

---

#### 1.2 `std::unique_ptr`  
What: Exclusive ownership; cannot be copied. Use `std::move` to transfer ownership.  
Example:  
```cpp
#include <memory>

int main() {
    auto ptr1 = std::make_unique<int>(42);
    auto ptr2 = std::move(ptr1); // ptr1 is now nullptr
    if (!ptr1) std::cout << "ptr1 is empty\n"; // Output: ptr1 is empty
}
```

---

#### 1.3 `std::shared_ptr`  
What: Shared ownership with reference counting.  
Pitfall: Circular references cause memory leaks.  
Example:  
```cpp
struct Node {
    std::shared_ptr<Node> next;
};

int main() {
    auto n1 = std::make_shared<Node>();
    auto n2 = std::make_shared<Node>();
    n1->next = n2;
    n2->next = n1; // Circular reference (memory leak)
}
```

---

#### 1.4 `std::weak_ptr`  
What: Breaks circular references by observing a `shared_ptr` without ownership.  
Example:  
```cpp
struct Node {
    std::weak_ptr<Node> next; // Use weak_ptr to break cycle
};

int main() {
    auto n1 = std::make_shared<Node>();
    auto n2 = std::make_shared<Node>();
    n1->next = n2;
    n2->next = n1; // No memory leak
}
```

---

### 2. Multiple-Choice Questions (10)

#### Questions 1-5  
Q1: What is the result of `std::move` on an lvalue?  
A) Copies the value  
B) Transfers ownership  
C) Converts it to an rvalue  
D) Destroys the object  

Q2: When is a move constructor called?  
A) When copying a temporary object  
B) When passing by value  
C) When using `std::move`  
D) Both A and C  

Q3: What happens if a moved-from object is used?  
A) Undefined behavior  
B) It is guaranteed to be empty  
C) It retains its original value  
D) Compile error  

Q4: Why use `std::make_unique`?  
A) Avoids manual `new`/`delete`  
B) Ensures exception safety  
C) Both A and B  
D) Improves performance  

Q5: What is the output?  
```cpp
auto p1 = std::make_shared<int>(42);
auto p2 = p1;
std::cout << p1.use_count();
```  
A) 1  
B) 2  
C) 0  
D) Undefined  

---

#### Questions 6-10  
Q6: Which smart pointer should you use for shared ownership?  
A) `unique_ptr`  
B) `weak_ptr`  
C) `shared_ptr`  
D) `auto_ptr`  

Q7: What is the purpose of `weak_ptr::lock()`?  
A) Destroys the managed object  
B) Creates a `shared_ptr` from the `weak_ptr`  
C) Resets the `weak_ptr`  
D) Checks if the object exists  

Q8: Which code causes a memory leak?  
A) `std::shared_ptr<int> p(new int(42));`  
B) `std::unique_ptr<int> p(new int(42));`  
C) `auto p = std::make_shared<int>(42);`  
D) None  

Q9: What is the output?  
```cpp
std::unique_ptr<int> p1(new int(5));
std::unique_ptr<int> p2 = p1;
```  
A) Compiles and runs  
B) Compile error  
C) Runtime error  
D) Undefined behavior  

Q10: How to fix a circular reference?  
A) Use `unique_ptr`  
B) Use `weak_ptr`  
C) Avoid smart pointers  
D) Manually delete objects  

---

### Answers & Explanations  
1. C (`std::move` casts to rvalue; no transfer until move constructor/assignment).  
2. D (Used for temporaries or with `std::move`).  
3. B (Moved-from objects are in a valid but unspecified state).  
4. C (Safer and exception-safe).  
5. B (Reference count = 2).  
6. C (`shared_ptr` allows shared ownership).  
7. B (Creates a `shared_ptr` to access the object).  
8. A (Raw `new` without `make_shared` isn’t exception-safe).  
9. B (`unique_ptr` cannot be copied).  
10. B (`weak_ptr` breaks cycles).  

---

### 3. Design Questions (5)

#### Question 1: Implement a Move-Aware `Vector`  
Task: Design a `Vector` class with move constructor/assignment.  
```cpp
class Vector {
    int* data;
    size_t size;
public:
    Vector(Vector&& src) noexcept : data(src.data), size(src.size) {
        src.data = nullptr;
        src.size = 0;
    }

    Vector& operator=(Vector&& src) noexcept {
        if (this != &src) {
            delete[] data;
            data = src.data;
            size = src.size;
            src.data = nullptr;
            src.size = 0;
        }
        return *this;
    }

    ~Vector() { delete[] data; }
};

int main() {
    Vector v1(10);
    Vector v2 = std::move(v1); // Move constructor
}
```

---

#### Question 2: Factory Function with `unique_ptr`  
Task: Create a factory function returning `unique_ptr<Base>`.  
```cpp
class Base {
public:
    virtual ~Base() = default;
};

class Derived : public Base {};

std::unique_ptr<Base> create() {
    return std::make_unique<Derived>();
}

int main() {
    auto obj = create(); // Returns unique_ptr<Base>
}
```

---

#### Question 3: Fix Circular References  
Task: Refactor the following code to avoid memory leaks.  
```cpp
struct A {
    std::shared_ptr<A> other;
};

int main() {
    auto a1 = std::make_shared<A>();
    auto a2 = std::make_shared<A>();
    a1->other = a2;
    a2->other = a1; // Circular reference
}
```

Solution:  
```cpp
struct A {
    std::weak_ptr<A> other; // Use weak_ptr
};
```

---

#### Question 4: Safe Resource Handling  
Task: Use `shared_ptr` with a custom deleter for a file handle.  
```cpp
#include <cstdio>

void file_deleter(FILE* f) { if (f) fclose(f); }

int main() {
    std::shared_ptr<FILE> file(fopen("test.txt", "r"), file_deleter);
    if (file) {
        // Read from file
    }
    // File closed automatically
}
```

---

#### Question 5: Move-Only Class  
Task: Design a `ThreadPool` class that can only be moved, not copied.  
```cpp
class ThreadPool {
    std::vector<std::thread> workers;
public:
    ThreadPool() = default;
    ThreadPool(ThreadPool&&) = default; // Move constructor
    ThreadPool& operator=(ThreadPool&&) = default; // Move assignment
    ThreadPool(const ThreadPool&) = delete; // No copy
};

int main() {
    ThreadPool pool1;
    ThreadPool pool2 = std::move(pool1); // OK
}
```

---

### Summary  
This guide reinforces move semantics (transferring ownership, `std::move`), smart pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`), and their pitfalls (circular references, undefined behavior). Test cases validate resource management and ownership transfers.