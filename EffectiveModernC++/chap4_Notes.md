### Chapter 4: Smart Pointers – Key Points and Code Explanations

---

#### Item 18: `std::unique_ptr` for Exclusive Ownership
Key Points:
- Exclusive Ownership: Only one `unique_ptr` owns the resource. Move-only semantics (no copying).
- Custom Deleters: Supported but affect the type (unlike `shared_ptr`).
- Incomplete Types: Requires destructor definition in implementation files (critical for Pimpl Idiom).

Code Example:
```cpp
// Factory function returning unique_ptr
std::unique_ptr<Widget> createWidget() {
    return std::make_unique<Widget>();
}

// Custom deleter (e.g., for a FILE*)
auto fileDeleter = [](FILE* fp) { fclose(fp); };
std::unique_ptr<FILE, decltype(fileDeleter)> filePtr(fopen("test.txt", "r"), fileDeleter);
```

Test Case:
```cpp
#include <memory>
#include <iostream>

struct Widget { ~Widget() { std::cout << "Widget destroyed\n"; } };

int main() {
    auto ptr = createWidget(); // Widget created
    // ptr moved to another unique_ptr; no copies allowed
    auto ptr2 = std::move(ptr); 
} // Output: "Widget destroyed"
```

---

#### Item 19: `std::shared_ptr` for Shared Ownership
Key Points:
- Shared Ownership: Multiple `shared_ptr`s manage the same resource. Uses a control block for reference counting.
- Cyclic References: Can cause leaks; solved with `weak_ptr`.
- Custom Deleters: Do not affect the type (type-erased).

Code Example:
```cpp
auto shared1 = std::make_shared<Widget>(); // Control block created
auto shared2 = shared1; // Ref count = 2

// Cyclic reference (leak)
struct Node {
    std::shared_ptr<Node> next;
};
auto node1 = std::make_shared<Node>();
auto node2 = std::make_shared<Node>();
node1->next = node2;
node2->next = node1; // Ref counts never drop to 0!
```

Test Case:
```cpp
#include <memory>
#include <iostream>

struct Widget { ~Widget() { std::cout << "Widget destroyed\n"; } };

int main() {
    std::shared_ptr<Widget> shared1 = std::make_shared<Widget>();
    {
        auto shared2 = shared1; // Ref count = 2
    } // Ref count = 1
} // Output: "Widget destroyed"
```

---

#### Item 20: `std::weak_ptr` for Non-Owning References
Key Points:
- Break Cycles: `weak_ptr` observes a `shared_ptr` without affecting ref count.
- Check Validity: Use `expired()` or `lock()` to access the resource.

Code Example:
```cpp
auto shared = std::make_shared<Widget>();
std::weak_ptr<Widget> weak = shared;

if (!weak.expired()) {
    auto locked = weak.lock(); // Creates a new shared_ptr
}
```

Test Case:
```cpp
#include <memory>
#include <iostream>

struct Widget { ~Widget() { std::cout << "Widget destroyed\n"; } };

int main() {
    std::weak_ptr<Widget> weak;
    {
        auto shared = std::make_shared<Widget>();
        weak = shared; // Ref count remains 1
    } // Resource destroyed
    std::cout << weak.expired() << std::endl; // Output: 1 (true)
}
```

---

#### Item 21: `make_unique` and `make_shared`
Key Points:
- Advantages: 
  - Exception safety (e.g., `processWidget(std::shared_ptr<Widget>(new Widget), mayThrow())` is unsafe).
  - Single memory allocation (for `shared_ptr` control block + object).
- Limitations: Cannot use custom deleters or brace initialization.

Code Example:
```cpp
auto unique = std::make_unique<std::vector<int>>(5, 0); // vector of 5 zeros
auto shared = std::make_shared<Widget>(/*args*/);
```

Test Case:
```cpp
#include <memory>
#include <vector>

int main() {
    auto vec = std::make_unique<std::vector<int>>(3, 42); // [42, 42, 42]
    auto sharedVec = std::make_shared<std::vector<int>>(2, 10); // [10, 10]
}
```

---

#### Item 22: Pimpl Idiom with `unique_ptr`
Key Points:
- Pimpl (Pointer to Implementation): Hide implementation details in a `.cpp` file.
- Special Members: Destructor, move/copy operations must be defined in the implementation file (due to incomplete type).

Code Example:
```cpp
// Widget.h
class Widget {
public:
    Widget();
    ~Widget(); // Declared here, defined in Widget.cpp
    Widget(Widget&&); // Move constructor
    Widget& operator=(Widget&&); // Move assignment
private:
    struct Impl; // Forward declaration
    std::unique_ptr<Impl> pImpl;
};

// Widget.cpp
struct Widget::Impl { /*...*/ };
Widget::~Widget() = default; // Destructor defined after Impl is complete
Widget::Widget(Widget&&) = default;
Widget& Widget::operator=(Widget&&) = default;
```

Test Case:
```cpp
#include "Widget.h"

int main() {
    Widget w1;
    Widget w2 = std::move(w1); // Move works due to explicit definition
}
```

---

### Multiple-Choice Questions

1. What happens when a `shared_ptr` is copied?
   - A) The object is deep-copied.
   - B) The reference count increments.
   - C) A new control block is created.
   - D) The deleter is copied.
   Answer: B  
   Explanation: Copying `shared_ptr` increments the ref count in the same control block.

---

2. Which smart pointer allows custom deleters without changing its type?
   - A) `unique_ptr`  
   - B) `shared_ptr`  
   - C) Both  
   - D) Neither  
   Answer: B  
   Explanation: `shared_ptr` type-erases the deleter.

---

3. What is the output?
   ```cpp
   auto p = std::make_shared<int>(42);
   std::weak_ptr<int> wp = p;
   p.reset();
   if (wp.expired()) std::cout << "Expired";
   ```
   - A) Expired  
   - B) Nothing  
   - C) Compile error  
   Answer: A  
   Explanation: `wp` expires when the last `shared_ptr` (`p`) is reset.

---

4. Why is `make_shared` more efficient than `new` + `shared_ptr`?
   - A) Avoids control block allocation.  
   - B) Combines object and control block in one allocation.  
   - C) Uses less memory.  
   - D) All of the above.  
   Answer: B  
   Explanation: `make_shared` allocates the object and control block together.

---

5. What is missing in this Pimpl class?
   ```cpp
   // Widget.h
   class Widget {
       struct Impl;
       std::unique_ptr<Impl> pImpl;
   public:
       Widget();
   };
   ```
   - A) Move operations  
   - B) Destructor definition in `.cpp`  
   - C) Copy operations  
   - D) All of the above  
   Answer: B  
   Explanation: The destructor must be defined after `Impl` is complete.

---

6. Which code compiles?
   ```cpp
   // 1
   std::unique_ptr<int> p1 = new int(5);
   // 2
   std::unique_ptr<int> p2(std::make_unique<int>(5));
   ```
   - A) 1  
   - B) 2  
   - C) Both  
   - D) Neither  
   Answer: B  
   Explanation: `unique_ptr` cannot be assigned from raw pointer directly (no implicit conversion).

---

7. What is the ref count after `weak.lock()`?
   ```cpp
   auto shared = std::make_shared<int>(10);
   auto weak = shared;
   auto locked = weak.lock();
   ```
   - A) 1  
   - B) 2  
   - C) 3  
   - D) 0  
   Answer: B  
   Explanation: `locked` is a new `shared_ptr`; ref count increments to 2.

---

8. What is the type of `auto p = std::make_shared<std::vector<int>>();`?
   - A) `shared_ptr<vector>`  
   - B) `shared_ptr<vector<int>>`  
   - C) `unique_ptr<vector<int>>`  
   - D) `weak_ptr<vector<int>>`  
   Answer: B  

---

9. Which is true about `weak_ptr`?
   - A) Can directly access the object.  
   - B) Increases ref count.  
   - C) Must check `expired()` before use.  
   - D) Cannot be copied.  
   Answer: C  

---

10. What is the output?
    ```cpp
    struct Cycle {
        std::shared_ptr<Cycle> next;
    };
    auto a = std::make_shared<Cycle>();
    a->next = a;
    ```
    - A) Memory leak  
    - B) Crash  
    - C) No leak  
    Answer: A  
    Explanation: Cyclic references with `shared_ptr` cause leaks.

---

### Medium-Level Design Questions

1. Implement a Pimpl class `Logger` hiding a `std::ofstream`.  
   Solution:
   ```cpp
   // Logger.h
   #include <memory>
   class Logger {
       struct Impl;
       std::unique_ptr<Impl> pImpl;
   public:
       Logger();
       ~Logger();
       void log(const std::string& msg);
   };

   // Logger.cpp
   #include "Logger.h"
   #include <fstream>
   struct Logger::Impl {
       std::ofstream file{"log.txt"};
   };
   Logger::Logger() : pImpl(std::make_unique<Impl>()) {}
   Logger::~Logger() = default;
   void Logger::log(const std::string& msg) { pImpl->file << msg; }
   ```

---

2. Use `weak_ptr` to break a cyclic reference between `Parent` and `Child`.  
   Solution:
   ```cpp
   struct Child;
   struct Parent {
       std::weak_ptr<Child> child; // Use weak_ptr instead of shared_ptr
   };
   struct Child {
       std::shared_ptr<Parent> parent;
   };
   ```

---

3. Write a factory function returning `unique_ptr<Base>` with a `Derived` object.  
   Solution:
   ```cpp
   class Base { public: virtual ~Base() = default; };
   class Derived : public Base {};
   std::unique_ptr<Base> create() {
       return std::make_unique<Derived>();
   }
   ```

---

4. Demonstrate exception safety with `make_shared` vs `new`.  
   Solution:
   ```cpp
   void unsafe() {
       process(std::shared_ptr<Widget>(new Widget), mayThrow()); // May leak if mayThrow() throws
   }
   void safe() {
       process(std::make_shared<Widget>(), mayThrow()); // No leak
   }
   ```

---

5. Add a custom deleter to `shared_ptr` to log destruction.  
   Solution:
   ```cpp
   auto loggerDeleter = [](Widget* w) {
       std::cout << "Deleting Widget\n";
       delete w;
   };
   std::shared_ptr<Widget> sp(new Widget, loggerDeleter);
   ```

---

### Test Cases for All Code Snippets
Test Case for Pimpl:
```cpp
#include "Logger.h"
int main() {
    Logger logger;
    logger.log("Test message"); // Writes to log.txt
}
```

Test Case for Cyclic Reference Fix:
```cpp
auto parent = std::make_shared<Parent>();
auto child = std::make_shared<Child>();
parent->child = child;
child->parent = parent; // No leak; ref counts drop to 0 when out of scope.
```

Test Case for Factory Function:
```cpp
auto basePtr = create(); // Returns unique_ptr<Base> pointing to Derived
```

---

This breakdown ensures mastery of smart pointers, their use cases, and pitfalls like cyclic references and incomplete types in the Pimpl Idiom.