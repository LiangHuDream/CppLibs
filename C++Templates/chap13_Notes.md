@[TOC](C++20 Modules)

Focus Areas: Module declarations, partitions, interface/implementation units, and compatibility with legacy code.

---

### Key Concepts & Code Explanations

#### 1. Problems with Traditional Headers

Why Modules?: Headers cause slow compilation (reparsing), macros leaking, and ODR violations. Modules encapsulate code and reduce dependencies.

#### 2. Basic Module Structure

Example: A module `math` exporting functions.

```cpp
// math.ixx (module interface)  
export module math;  

export namespace math {  
    int add(int a, int b) { return a + b; }  
    double sqrt(double x); // Declaration only  
}  

// math_impl.ixx (module implementation)  
module math;  
export double math::sqrt(double x) { /* ... */ }  
```

Test Case:

```cpp
// main.cpp  
import math;  

int main() {  
    math::add(3, 4); // OK  
    math::sqrt(2.0); // OK  
}  
```

---

#### 3. Module Partitions

Use Case: Split large modules into logical partitions.

```cpp
// math-core.ixx (partition interface)  
export module math:core;  
export int multiply(int a, int b) { return a * b; }  

// math-advanced.ixx (partition interface)  
export module math:advanced;  
export double exp(double x);  

// math.ixx (primary interface)  
export module math;  
export import :core;  
export import :advanced;  
```

Test Case:

```cpp
import math;  
int main() {  
    multiply(3, 4); // From core partition  
    exp(2.0);      // From advanced partition  
}  
```

---

#### 4. Legacy Header Compatibility

Global Module Fragment: Include headers before module declarations.

```cpp
// legacy.ixx  
module;  
#include <vector> // Legacy header  
export module legacy;  

export template<typename T>  
void process(const std::vector<T>& vec) { /* ... */ }  
```

Test Case:

```cpp
import legacy;  
int main() {  
    std::vector<int> vec = {1, 2, 3};  
    process(vec);  
}  
```

---

### 10 Hard Multiple-Choice Questions

#### Questions 1-5

1. What is the primary benefit of modules over headers?
    A) Faster runtime
    B) Faster compilation
    C) Smaller binaries
    D) Better exception safety
2. Which keyword exports a module’s interface? 
    A) `export`
    B) `import`
    C) `module`
    D) `namespace`
3. What is the file extension for module interfaces in MSVC?
    A) `.cpp`
    B) `.hpp`
    C) `.ixx`
    D) `.module`
4. Which code correctly imports a module?
   A) `#include <math>`
   B) `import math;`
   C) `using namespace math;`
   D) `export import math;`
5. What is a module partition?
    A) A way to split modules across files
    B) A deprecated feature
    C) A type of namespace
    D) A build system flag

---

#### Questions 6-10

6. Which code is valid in the global module fragment?
    A) `export module math;`
    B) `#include <vector>`
    C) `import std.core;`
    D) `export int add(int a, int b);`
7. What is the purpose of `export import`?
    A) Re-export a module’s interface
    B) Hide a module’s implementation
    C) Link to legacy code
    D) Disable macros
8. Which statement about module partitions is true?
   A) Partitions can be imported independently
   B) Partitions must have unique names across modules
   C) Partitions replace namespaces
   D) Partitions are declared with `module math.partition;`
9. What happens if a module interface includes `using namespace std;`?
    A) `std` is exported to users
    B) `std` is hidden from users
    C) Compile error
    D) UB
10. Which code mixes modules and headers safely?
    A)

    ```cpp
    module;  
    #include <vector>  
    export module data;  
    ```

    B)

    ```cpp
    import <vector>;  
    export module data;  
    ```

    C)

    ```cpp
    export module data;  
    #include <vector>  
    ```

    D)

    ```cpp
    export module data;  
    import std.vector;  
    ```

---

### 1-10 Answers & Explanations

1. B (Modules reduce redundant parsing).
2. A (`export` exposes entities).
3. C (`.ixx` in MSVC).
4. B (`import math;`).
5. A (Partitions split modules into files).
6. B (Global fragment allows `#include`).
7. A (`export import` re-exports dependencies).
8. B (Partition names must be unique within a module).
9. A (Exported declarations leak `std`).
10. A (Global fragment includes headers safely).

---

### 10 Hard Design Questions

#### Question 1: Module for a Math Library

Task: Design a module `math` exporting `add`, `subtract`, and `PI`.

```cpp
// math.ixx  
export module math;  

export namespace math {  
    constexpr double PI = 3.14159;  
    int add(int a, int b) { return a + b; }  
    int subtract(int a, int b) { return a - b; }  
}  
```

Test Case:

```cpp
import math;  
int main() {  
    math::add(10, math::subtract(5, 3)); // 10 + (5-3) = 12  
}  
```

---

#### Question 2: Module Partition for Logging

Task: Split a module `logger` into core (`:core`) and file I/O (`:file`) partitions.

```cpp
// logger-core.ixx  
export module logger:core;  
export void log(const std::string& msg);  

// logger-file.ixx  
export module logger:file;  
import :core;  
export void logToFile(const std::string& msg);  

// logger.ixx  
export module logger;  
export import :core;  
export import :file;  
```

Test Case:

```cpp
import logger;  
int main() {  
    log("Hello");  
    logToFile("Debug");  
}  
```

---

#### Question 3: Legacy Compatibility with Modules

Task: Wrap a legacy C-style API in a module.

```cpp
// legacy.ixx  
module;  
#include "legacy.h" // void legacy_init();  
export module legacy;  

export void init() { legacy_init(); }  
```

Test Case:

```cpp
import legacy;  
int main() {  
    init(); // Calls legacy_init()  
}  
```

---

#### Question 4: Module Interface Unit with Templates

Task: Export a template class `Stack` in a module.

```cpp
// stack.ixx  
export module stack;  

export template<typename T>  
class Stack {  
    T* data;  
public:  
    void push(T val);  
    T pop();  
};  

template<typename T>  
void Stack<T>::push(T val) { /* ... */ }  

template<typename T>  
T Stack<T>::pop() { /* ... */ }  
```

Test Case:

```cpp
import stack;  
int main() {  
    Stack<int> s;  
    s.push(42);  
    s.pop();  
}  
```

---

#### Question 5: Optimizing Builds with Modules

Task: Convert a header-only library (`utils.h`) into a module.

```cpp
// utils.ixx  
export module utils;  

export template<typename T>  
T clamp(T val, T min, T max) {  
    return (val < min) ? min : (val > max) ? max : val;  
}  
```

Test Case:

```cpp
import utils;  
int main() {  
    clamp(10, 0, 5); // Returns 5  
}  
```

---

#### Question 6: Math Module with Partitions  
Task: Split a `math` module into `:core` (add/subtract) and `:advanced` (sqrt).  
Solution:  
```cpp  
// math-core.ixx  
export module math:core;  
export int add(int a, int b) { return a + b; }  
export int subtract(int a, int b) { return a - b; }  

// math-advanced.ixx  
export module math:advanced;  
export double sqrt(double x) { /* ... */ }  

// math.ixx  
export module math;  
export import :core;  
export import :advanced;  
```
Test Case:  
```cpp  
import math;  
int main() {  
    add(15, 3);      // Output usage  
    sqrt(25.0);     // Output usage  
}  
```

---

#### Question 7: Legacy Header Wrapper  
Task: Wrap a legacy C header (`legacy.h`) in a module.  
Solution:  
```cpp  
// legacy.ixx  
module;  
#include "legacy.h" // void legacy_init();  
export module legacy;  
export void init() { legacy_init(); }  
```
Test Case:  
```cpp  
import legacy;  
int main() {  
    init(); // Calls legacy_init()  
}  
```

---

#### Question 8: Module for a Template Class  
Task: Export a generic `Stack` template class.  
Solution:  
```cpp  
// stack.ixx  
export module stack;  

export template<typename T>  
class Stack {  
    T* data;  
public:  
    void push(T val) { /* ... */ }  
    T pop() { /* ... */ }  
};  
```
Test Case:  
```cpp  
import stack;  
int main() {  
    Stack<int> s;  
    s.push(10);  
    s.pop();  
}  
```

---

#### Question 9: Module with `constexpr` Functions  
Task: Create a `utils` module exporting `constexpr` functions.  
Solution:  
```cpp  
// utils.ixx  
export module utils;  

export constexpr int clamp(int val, int min, int max) {  
    return (val < min) ? min : (val > max) ? max : val;  
}  
```
Test Case:  
```cpp  
import utils;  
int main() {  
    static_assert(clamp(10, 0, 15) == 15);  
}  
```

---

#### Question 10: Module Partition for Logging  
Task: Implement a `logger` module with `:file` and `:network` partitions.  
Solution:  
```cpp  
// logger-file.ixx  
export module logger:file;  
export void logToFile(const std::string& msg) { /* ... */ }  

// logger-network.ixx  
export module logger:network;  
export void logToNetwork(const std::string& msg) { /* ... */ }  

// logger.ixx  
export module logger;  
export import :file;  
export import :network;  
```
Test Case:  
```cpp  
import logger;  
int main() {  
    logToFile("test.log");  
    logToNetwork("http://example.com");  
}  
```

---

### Summary  
This guide covers:  
1. Module Syntax: Declaring interfaces, partitions, and exports.  
2. Legacy Code: Safely integrating headers via the global module fragment.  
3. Design Patterns: Partitioning modules for scalability.  
4. Testing: Validating modules with compile-time assertions and runtime tests.  

Test cases ensure modules function correctly and adhere to C++20 specifications.
