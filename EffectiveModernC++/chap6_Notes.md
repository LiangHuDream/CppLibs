### Chapter 6: Lambda Expressions — Key Points and Code Explanations

---

#### Item 31: Avoid Default Capture Modes
Key Points:
- Default captures (`[=]` or `[&]`) can lead to dangling references or unintended behavior if the lambda outlives the captured variables.
- Implicit captures may inadvertently capture `this` (leading to dangling pointers) or large objects (inefficient copies).

Code Example:
```cpp
#include <functional>
#include <iostream>

std::function<void()> createDangerousLambda() {
    int localVar = 42;
    // Danger: Captures localVar by reference, which will be destroyed
    return [&]() { std::cout << localVar << std::endl; };
}

int main() {
    auto lambda = createDangerousLambda();
    lambda(); // Undefined behavior: localVar no longer exists
}
```

Test Case:
- Output is unpredictable (e.g., garbage value or crash).

---

#### Item 32: Use Init Capture to Move Objects
Key Points:
- C++14’s *init capture* (`[var = std::move(var)]`) allows moving objects into closures.
- Essential for move-only types (e.g., `std::unique_ptr`).

Code Example:
```cpp
#include <memory>
#include <iostream>

int main() {
    auto ptr = std::make_unique<int>(99);
    auto lambda = [capturedPtr = std::move(ptr)]() {
        std::cout << *capturedPtr << std::endl;
    };
    // ptr is now null
    lambda(); // Output: 99
}
```

Test Case:
- `ptr` is `nullptr` after the move; lambda retains ownership.

---

#### Item 33: Use `decltype` on `auto&&` for Perfect Forwarding
Key Points:
- Generic lambdas (`auto&&` parameters) require `decltype` to preserve value category.
- Use `std::forward<decltype(param)>(param)` for perfect forwarding.

Code Example:
```cpp
#include <utility>
#include <iostream>

void process(int& x) { std::cout << "Lvalue: " << x << std::endl; }
void process(int&& x) { std::cout << "Rvalue: " << x << std::endl; }

int main() {
    auto forwarder = [](auto&& arg) {
        process(std::forward<decltype(arg)>(arg));
    };

    int x = 10;
    forwarder(x);   // Lvalue: 10
    forwarder(20); // Rvalue: 20
}
```

Test Case:
- Correctly forwards lvalues/rvalues to the appropriate overload.

---

#### Item 34: Prefer Lambdas to `std::bind`
Key Points:
- Lambdas are more readable, support inlining, and avoid evaluation order issues.
- `std::bind` may leak resources or misorder arguments.

Code Example:
```cpp
#include <functional>
#include <iostream>

void log(int a, int b, int c) {
    std::cout << a << ", " << b << ", " << c << std::endl;
}

int main() {
    // Lambda: Clear and efficient
    auto lambda = [](int a, int b, int c) { log(a, b, c); };
    lambda(1, 2, 3); // Output: 1, 2, 3

    // std::bind: Opaque and risky
    auto binder = std::bind(log, std::placeholders::_3, std::placeholders::_1, 9);
    binder(4, 5, 6); // Output: 6, 4, 9 (confusing order)
}
```

Test Case:
- Lambda preserves argument order; `std::bind` does not.

---

### Multiple-Choice Questions

1. What happens when a lambda captures a local variable by reference and is called after the variable is destroyed?
   - A) Compile error
   - B) Runtime crash
   - C) Undefined behavior
   - D) The lambda retains a copy

   Answer: C) Undefined behavior  
   Explanation: The reference becomes dangling, leading to UB.

---

2. Which syntax correctly moves a `unique_ptr` into a lambda?
   - A) `[ptr](){}`
   - B) `[=](){}`
   - C) `[ptr = std::move(ptr)](){}`
   - D) `[&ptr](){}`

   Answer: C)  
   Explanation: Init capture with `std::move` transfers ownership.

---

3. What does `decltype(x)` return for `auto&& x` in a generic lambda?
   - A) `T`
   - B) `T&`
   - C) `T&&` for rvalues, `T&` for lvalues
   - D) `T*`

   Answer: C)  
   Explanation: `decltype(x)` preserves the value category.

---

4. Why should lambdas be preferred over `std::bind`?
   - A) Better performance
   - B) Clearer syntax
   - C) No argument order issues
   - D) All of the above

   Answer: D)  
   Explanation: Lambdas excel in all these areas.

---

5. Which capture mode is safest for avoiding dangling references?
   - A) `[=]`
   - B) `[&]`
   - C) Init capture with move
   - D) Default capture

   Answer: C)  
   Explanation: Moving ensures ownership without references.

---

### Design Questions

1. Implement a lambda to forward arguments to `std::make_unique`.  
   Solution:
   ```cpp
   auto makeUniqueWrapper = [](auto&&... args) {
       return std::make_unique<std::decay_t<decltype(args)>>...(
           std::forward<decltype(args)>(args)...
       );
   };
   auto ptr = makeUniqueWrapper(42); // Creates unique_ptr<int>
   ```

---

2. Convert a `std::bind` expression to a lambda:  
   `auto f = std::bind(func, _2, _1, 42);`  
   Solution:
   ```cpp
   auto lambda = [](auto a, auto b) { return func(b, a, 42); };
   ```

---

3. Capture a `vector` by moving and print its size.  
   Solution:
   ```cpp
   std::vector<int> vec{1, 2, 3};
   auto lambda = [v = std::move(vec)]() {
       std::cout << v.size(); // Output: 3
   };
   ```

---

4. Write a lambda to filter even numbers from a captured list.  
   Solution:
   ```cpp
   auto filterEven = [even = std::list<int>{}](int x) mutable {
       if (x % 2 == 0) even.push_back(x);
       return even;
   };
   ```

---

5. Design a lambda factory returning lambyas with unique IDs.  
   Solution:
   ```cpp
   auto makeIdLambda = []() {
       static int id = 0;
       return [id++]() { std::cout << "ID: " << id; };
   };
   auto l1 = makeIdLambda(); // ID: 1
   auto l2 = makeIdLambda(); // ID: 2
   ```

---

### Test Cases for All Code Blocks
Example for Item 32:
```cpp
int main() {
    auto ptr = std::make_unique<int>(99);
    auto lambda = [capturedPtr = std::move(ptr)]() {
        std::cout << (capturedPtr ? "Alive" : "Dead");
    };
    lambda(); // Alive
    std::cout << (ptr ? "Alive" : "Dead"); // Dead
}
```

Output:  
```
Alive
Dead
```

---

This breakdown ensures mastery of lambda mechanics, pitfalls, and modern practices in C++14.