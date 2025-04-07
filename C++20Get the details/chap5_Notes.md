@[TOC](Chapter 5: The Standard Library)

---

### 5.1 The Ranges Library

**Key Concepts**
1. **Ranges**: Replace iterator pairs with _contiguous sequences_ (containers, views).
2. **Views**: Non-owning, lazily evaluated ranges (e.g., `filter`, `transform`).
3. **Range Adaptors**: Pipelined operations using `|` operator.
4. **Lazy Evaluation**: Operations are deferred until iteration.

**Code Example: Filter & Transform**
```cpp
#include <iostream>
#include <vector>
#include <ranges>

int main() {
    std::vector<int> nums{1, 2, 3, 4, 5};
    
    // Define a pipeline: filter even numbers, then square them
    auto result = nums 
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::transform([](int n) { return n * n; });
    
    for (int v : result) {
        std::cout << v << " "; // Output: 4 16
    }
}
```
- **Explanation**:
  - `filter` retains even numbers.
  - `transform` squares each element.
  - Operations are lazily evaluated when iterated.

---

### 5.2 `std::span`

**Key Concepts**
1. **Non-Owowing View**: Represents a contiguous sequence (array, vector).
2. **Static vs. Dynamic Extent**: Fixed size (`std::span<int, 5>`) or runtime size (`std::span<int>`).
3. **Safety**: Bounds checking (optional via `at()` in debug builds).

**Code Example: Modifying Elements**
```cpp
#include <iostream>
#include <span>
#include <vector>

void print(std::span<int> s) {
    for (int v : s) {
        std::cout << v << " ";
    }
}

int main() {
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::span<int> s(vec);
    
    s[2] = 99; // Modify the third element
    print(s);  // Output: 1 2 99 4 5
}
```
- **Explanation**:
  - `span` references `vec`’s data.
  - Modifications via `span` affect the original container.

---

### 5.3 Container Improvements

**Key Features**
1. **`contains()` for Associative Containers**:
   ```cpp
   #include <iostream>
   #include <set>

   int main() {
       std::set<int> s{1, 2, 3};
       if (s.contains(2)) {
           std::cout << "Found!"; // Output: Found!
       }
   }
   ```
   - Replaces `s.find(2) != s.end()`.

2. **`std::erase/erase_if` for Sequence Containers**:
   ```cpp
   #include <vector>
   #include <algorithm>

   int main() {
       std::vector<int> v{1, 2, 3, 4, 5};
       std::erase_if(v, [](int n) { return n % 2 == 0; });
       // v becomes [1, 3, 5]
   }
   ```

---

### 5.4 Arithmetic Utilities

**Safe Integer Comparisons**
```cpp
#include <utility>
#include <iostream>

int main() {
    int a = -1;
    unsigned b = 0;
    
    // Safe comparison avoids integer overflow
    if (std::cmp_less(a, b)) {
        std::cout << "-1 < 0 (safely compared)";
    }
}
```
- **Explanation**:
  - `std::cmp_less` safely compares signed/unsigned values.

**Math Constants**
```cpp
#include <numbers>
#include <iostream>

int main() {
    std::cout << std::numbers::pi; // Output: 3.14159...
}
```

---

### 5.5 Formatting Library

**`std::format` Basics**
```cpp
#include <format>
#include <iostream>

int main() {
    std::cout << std::format("Hello, {}! π ≈ {:.2f}", "C++20", 3.14159);
    // Output: Hello, C++20! π ≈ 3.14
}
```

**Custom Type Formatting**
```cpp
#include <format>
#include <iostream>

struct Point { int x, y; };

template <>
struct std::formatter<Point> {
    auto parse(auto& ctx) { return ctx.begin(); }
    auto format(const Point& p, auto& ctx) {
        return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
    }
};

int main() {
    Point p{10, 20};
    std::cout << std::format("{}", p); // Output: (10, 20)
}
```

---

### 5.6 Calendar & Time Zones

**Date & Time Example**
```cpp
#include <chrono>
#include <iostream>

using namespace std::chrono;

int main() {
    auto now = system_clock::now();
    std::cout << "Current time: " << now << "\n"; // ISO 8601 format

    auto date = 2023y/September/15;
    std::cout << "Date: " << date << "\n"; // Output: 2023-09-15
}
```

**Time Zone Conversion**
```cpp
#include <chrono>
#include <iostream>

int main() {
    auto utc_time = std::chrono::system_clock::now();
    auto ny_tz = std::chrono::locate_zone("America/New_York");
    auto ny_time = std::chrono::zoned_time(ny_tz, utc_time);
    std::cout << "NY time: " << ny_time << "\n";
}
```

---

### 5.7 Further Improvements

**`std::bind_front`**
```cpp
#include <functional>
#include <iostream>

void print(int a, int b) { std::cout << a << ", " << b << "\n"; }

int main() {
    auto f = std::bind_front(print, 42);
    f(10); // Output: 42, 10
}
```

**`std::is_constant_evaluated`**
```cpp
#include <iostream>

constexpr int compute() {
    if (std::is_constant_evaluated()) {
        return 42; // Compile-time
    } else {
        return 0;  // Runtime
    }
}

int main() {
    constexpr int v1 = compute(); // 42
    int v2 = compute();           // 0
    std::cout << v1 << " " << v2; // Output: 42 0
}
```

---

### Compilation Notes
- Use `-std=c++20` (GCC/Clang) or `/std:c++latest` (MSVC).
- Link against `<ranges>`, `<chrono>`, and other C++20 headers.
- Ensure your compiler fully supports C++20 features (e.g., GCC 10+, Clang 12+).
---

### Multiple-Choice Questions
---

**Questions 1-25**

**1. Which statements about `std::ranges::views` are true?**  
A) Views own the data they operate on.  
B) Views are lazily evaluated.  
C) `std::views::transform` modifies the original container.  
D) Views can be composed using the `|` operator.  

**2. What is true about `std::span`?**  
A) It owns the underlying data.  
B) It can represent a runtime-sized range.  
C) `std::span<int, 5>` has a static extent.  
D) Accessing elements with `operator[]` always performs bounds checking.  

**3. Which C++20 features allow safe integer comparisons?**  
A) `std::cmp_less`  
B) `std::ssize`  
C) `std::midpoint`  
D) `std::in_range`  

**4. Select valid ways to use `std::format`:**  
A) `std::format("{}", 3.14)`  
B) `std::format("{:.2f}", 3.14159)`  
C) `std::format("{:%Y-%m-%d}", std::chrono::system_clock::now())`  
D) `std::format("{0} {0}", "C++")`  

**5. Which operations are valid for `std::jthread`?**  
A) Automatically joins on destruction.  
B) Supports cooperative interruption.  
C) Can replace `std::thread` without code changes.  
D) Requires manual `join()` calls.  

**6. What applies to `std::semaphore`?**  
A) Supports timed waits.  
B) Maximum count is fixed at compile time.  
C) `acquire()` decrements the counter.  
D) `release(n)` increments the counter by `n`.  

**7. Which `std::chrono` types are valid?**  
A) `std::chrono::day`  
B) `std::chrono::time_of_day`  
C) `std::chrono::year_month_weekday`  
D) `std::chrono::time_zone`  

**8. What describes `std::is_constant_evaluated()`?**  
A) Returns `true` during compile-time evaluation.  
B) Used to optimize runtime performance.  
C) Valid only in `constexpr` functions.  
D) Requires the `<type_traits>` header.  

**9. Which statements about `std::ranges` algorithms are true?**  
A) They accept iterator pairs.  
B) They always return iterators.  
C) `std::ranges::sort` can sort a `std::vector` directly.  
D) They require explicit projection parameters.  

**10. What is true about `std::bind_front`?**  
A) Binds arguments to the front of a callable.  
B) Supports partial function application.  
C) Requires `<functional>` header.  
D) Replaces `std::bind` entirely.  

**11. Which operations are valid for `std::atomic<std::shared_ptr<T>>`?**  
A) `load()`  
B) `store()`  
C) `exchange()`  
D) `wait()`  

**12. What applies to `std::latch`?**  
A) Counts down to zero.  
B) Reusable after reaching zero.  
C) Threads block until the count reaches zero.  
D) Supports `arrive_and_wait()`.  

**13. Which are valid C++20 container improvements?**  
A) `std::vector::contains()`  
B) `std::erase_if(std::list, predicate)`  
C) `std::map::merge()`  
D) `std::string::starts_with()`  

**14. What describes `std::source_location`?**  
A) Captures `__FILE__` and `__LINE__` at compile time.  
B) Requires C++20 or later.  
C) Defaults to the call site when used as a default argument.  
D) Replaces the `assert` macro.  

**15. Which statements about coroutines are true?**  
A) `co_await` suspends execution.  
B) `co_return` must return a value.  
C) Coroutines require heap allocation.  
D) `co_yield` resumes execution immediately.  

**16. What applies to `std::format` for user-defined types?**  
A) Requires specializing `std::formatter<T>`.  
B) Uses `format_as()` function.  
C) Supports custom format specifiers.  
D) Requires inheritance from `std::formattable`.  

**17. Which are valid `std::chrono` literals?**  
A) `10ns`  
B) `3h`  
C) `2023_y`  
D) `Sunday`  

**18. What is true about `std::jthread` interruption?**  
A) Uses `request_stop()`.  
B) Checks `get_stop_token().stop_requested()`.  
C) Automatically interrupts running threads.  
D) Requires `std::stop_callback`.  

**19. Which statements about `std::span` are true?**  
A) `std::span<int>` has a dynamic extent.  
B) `subspan()` returns a smaller span.  
C) `size_bytes()` returns the size in bytes.  
D) Cannot be constructed from a `std::array`.  

**20. What describes `std::ranges::to`?**  
A) Converts a range to a container.  
B) Requires explicit template arguments.  
C) Part of C++20.  
D) Works with all standard containers.  

**21. Which are valid uses of `std::views`?**  
A) `std::views::iota(0, 10)`  
B) `std::views::split("hello", 'l')`  
C) `std::views::reverse`  
D) `std::views::common`  

**22. What applies to `std::atomic_ref`?**  
A) Provides atomic access to non-atomic objects.  
B) Requires aligned memory.  
C) Lifetime must not exceed the referenced object.  
D) Supports all arithmetic types.  

**23. Which statements about `std::barrier` are true?**  
A) Reusable after completion.  
B) Supports a completion function.  
C) Threads block until all arrive.  
D) Part of `<barrier>` header.  

**24. What is true about `std::ranges::join`?**  
A) Flattens a range of ranges.  
B) Eagerly evaluates the input range.  
C) Requires a bidirectional range.  
D) Returns a view.  

**25. Which describe `std::expected` (C++23 preview)?**  
A) Represents a value or an error.  
B) Part of C++20.  
C) Similar to `std::optional` with error handling.  
D) Requires `<expected>` header.  

---

### **Answers & Explanations**

**1. B, D**  
- Views are non-owning and lazy. Composition uses `|`.

**2. B, C**  
- `std::span` can have dynamic or static extent. Bounds checking is debug-only.

**3. A, D**  
- `std::cmp_less` and `std::in_range` handle mixed signedness safely.

**4. A, B, C, D**  
- All are valid: floating-point, date formatting, positional arguments.

**5. A, B**  
- `std::jthread` auto-joins and supports stop tokens.

**6. A, C, D**  
- Semaphores allow timed waits, `acquire()` reduces count, `release(n)` adds `n`.

**7. A, C, D**  
- `time_of_day` is valid; `time_zone` is part of the chrono library.

**8. A, B**  
- Returns `true` during compile-time evaluation. Usable in non-`constexpr` contexts.

**9. A, C**  
- Ranges accept containers directly. Algorithms like `sort` work on ranges.

**10. A, B, C**  
- `bind_front` binds leading arguments; requires `<functional>`.

**11. A, B, C, D**  
- Atomic smart pointers support all atomic operations.

**12. A, C, D**  
- Latches count down to zero, block until zero, and are single-use.

**13. B, D**  
- `std::erase_if` and `starts_with` are C++20 additions.

**14. A, B, C**  
- `source_location` captures compile-time context; defaults to call site.

**15. A**  
- `co_await` suspends. `co_return` can return `void`. Heap allocation is optional.

**16. A, C**  
- Specialize `std::formatter<T>` and define custom specifiers.

**17. A, B, C**  
- Valid literals: `10ns`, `3h`, `2023y` (C++20). `Sunday` is a type, not a literal.

**18. A, B**  
- Interruption uses `request_stop()` and stop tokens.

**19. A, B, C**  
- `subspan()` and `size_bytes()` are valid. `std::span` can wrap `std::array`.

**20. A**  
- `std::ranges::to` (C++23) converts ranges to containers. Not part of C++20.

**21. A, C, D**  
- `iota`, `reverse`, and `common` are valid views. `split` requires a range.

**22. A, B, C**  
- `atomic_ref` references non-atomic objects with alignment requirements.

**23. B, C, D**  
- Barriers support completion functions and reuse. Part of `<barrier>`.

**24. A, D**  
- `join` flattens ranges and returns a view. Works with forward ranges.

**25. A, C**  
- `std::expected` (C++23) is value/error type. Not part of C++20.

---

### Design Challenges (C++20 Standard Library & Concurrency)

---

**Question 1: Ranges Pipeline Composition**
**Task:**  
Create a pipeline using `std::ranges` that filters even numbers from a vector, squares them, reverses the order, 
and returns only the first 3 elements. Ensure all operations are lazy-evaluated.

**Test Case:**  
Input: `{1, 2, 3, 4, 5, 6, 7, 8}`  
Expected Output: `64 36 16`

---

**Question 2: Custom Range View**
**Task:**  
Implement a custom view `split_by_half` that divides a range into two halves. Use this view with `std::ranges::join` to 
flatten the result. Handle odd-sized ranges by making the first half larger.

**Test Case:**  
Input: `{1, 2, 3, 4, 5}` → Split → `{{1,2,3}, {4,5}}` → Join → `{1,2,3,4,5}`

---

**Question 3: std::span Dynamic Bounds**
**Task:**  
Create a `std::span` over a C-style array and modify elements through the span. Implement bounds checking using 
`span::subspan` to safely access elements, throwing `std::out_of_range` if invalid.

**Test Case:**  
Array: `int arr[] = {10, 20, 30, 40}`  
Access indices 1-3 → Modify to `{25, 35}`. Handle index 4 with exception.

---

**Question 4: constexpr Vector Sorting**
**Task:**  
Use `std::vector` in a `constexpr` context to sort integers with `std::sort` (C++20 constexpr support). Verify the 
sorted result at compile time using `static_assert`.

**Test Case:**  
Input: `{5, 3, 4, 1, 2}` → Sorted: `{1, 2, 3, 4, 5}`

---

**Question 5: Atomic Smart Pointers**
**Task:**  
Implement a thread-safe shared counter using `std::atomic<std::shared_ptr<int>>`. Use `compare_exchange_weak` to 
atomically increment the counter across multiple threads.

**Test Case:**  
3 threads incrementing 1000 times each → Final value: 3000.

---

**Question 6: Custom Formatter for User-Defined Type**
**Task:**  
Create a `std::formatter` specialization for a `struct Point3D { int x, y, z; }` to display as `(x,y,z)`. Support 
format specifiers for hex (`%x`) and decimal (`%d`).

**Test Case:**  
`Point3D{15, 255, 16}` with format `"{:%x,%d,%x}"` → Output: `(f,255,10)`.

---

**Question 7: Time Zone Conversions**
**Task:**  
Convert a UTC timestamp to New York’s time zone accounting for daylight saving. Use `std::chrono::zoned_time` and print 
the local time in `YYYY-MM-DD HH:MM` format.

**Test Case:**  
UTC: `2023-07-01 12:00:00` → New York: `2023-07-01 08:00`.

---

**Question 8: Generator Coroutine with Exception Handling**
**Task:**  
Implement a coroutine `Generator<T>` that yields values until an error condition (e.g., negative value). Propagate 
exceptions from the coroutine to the caller using `std::rethrow_exception`.

**Test Case:**  
Input: `{5, 3, -1, 2}` → Output: `5 3` followed by `std::runtime_error`.

---

**Question 9: Semaphore-Based Producer-Consumer**
**Task:**  
Implement a producer-consumer queue using `std::counting_semaphore`. Producers block when the buffer is full; consumers 
block when empty. Use `std::binary_semaphore` for mutex.

**Test Case:**  
2 producers, 2 consumers, buffer size 3. Verify no data races.

---

**Question 10: Latch for Parallel Initialization**
**Task:**  
Use `std::latch` to synchronize 4 threads initializing different parts of a shared array. Each thread signals 
completion, and the main thread aggregates results after all are ready.

**Test Case:**  
Array initialized to `{1, 2, 3, 4}` by 4 threads → Final array: `[1,2,3,4]`.

---

**Question 11: Barrier for Cyclic Task Processing**
**Task:**  
Use `std::barrier` to implement 3 threads processing batches of data in phases. After each phase, threads wait at 
the barrier before proceeding to the next phase.

**Test Case:**  
3 phases with intermediate results printed after each barrier.

---

**Question 12: jthread with Cooperative Cancellation**
**Task:**  
Use `std::jthread` and `std::stop_token` to run a task that periodically checks for cancellation. Stop the thread 
after 5 iterations using `request_stop()`.

**Test Case:**  
Output: "Iteration 1"..."Iteration 5" then stops.

---

**Question 13: Synchronized Output Streams**
**Task:**  
Use `std::osyncstream` to synchronize output from multiple threads writing to `std::cout`. Ensure lines from different 
threads are not interleaved.

**Test Case:**  
4 threads each printing 100 lines. Verify no interleaving.

---

**Question 14: std::is_constant_evaluated**
**Task:**  
Implement a function `square()` that uses `std::is_constant_evaluated` to return a `constexpr` result when possible 
and a runtime result otherwise. Verify with `static_assert`.

**Test Case:**  
`static_assert(square(5) == 25);` and runtime `square(5) == 25`.

---

**Question 15: std::source_location Logging**
**Task:**  
Create a logging macro `LOG(msg)` that captures `std::source_location` (file, line, function). Print logs in the 
format `[file:line] function: msg`.

**Test Case:**  
`LOG("Error")` → Output: `[main.cpp:42] main: Error`.

---

### Answer Key & Detailed Solutions

---

**Question 1 Solution: Ranges Pipeline**
```cpp
#include <vector>
#include <ranges>
#include <iostream>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
    auto pipeline = v | std::views::filter([](int x) { return x % 2 == 0; })
                     | std::views::transform([](int x) { return x * x; })
                     | std::views::reverse
                     | std::views::take(3);
    for (int x : pipeline) std::cout << x << " ";
    // Output: 64 36 16
}
```
**Explanation:**  
The pipeline chains multiple range adaptors. `filter` removes odd numbers, `transform` squares, `reverse` reverses 
order, and `take` selects the first 3 elements. All operations are lazy.

---

**Question 2 Solution: Custom View**
```cpp
#include <ranges>
#include <vector>
#include <iostream>

struct split_by_half : std::ranges::view_interface<split_by_half> {
    std::vector<int> vec;
    split_by_half(std::vector<int> v) : vec(std::move(v)) {}
    auto begin() const { 
        auto mid = vec.begin() + (vec.size() + 1) / 2;
        return std::array{vec.begin(), mid, mid, vec.end()}.begin();
    }
    auto end() const { return begin() + 2; }
};

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto halves = split_by_half(v) | std::views::join;
    for (int x : halves) std::cout << x << " "; // 1 2 3 4 5
}
```
**Explanation:**  
The custom view `split_by_half` divides the vector into two halves. `std::views::join` flattens the split ranges.

---

**Question 3 Solution: std::span Bounds**
```cpp
#include <span>
#include <iostream>
#include <stdexcept>

void modify_span(std::span<int> s, size_t index, int value) {
    if (index >= s.size()) throw std::out_of_range("Invalid index");
    s.subspan(index, 1)[0] = value;
}

int main() {
    int arr[] = {10, 20, 30, 40};
    std::span s(arr);
    try {
        modify_span(s, 1, 25); // OK
        modify_span(s, 4, 50); // Throws
    } catch (const std::out_of_range& e) {
        std::cout << e.what(); // "Invalid index"
    }
    // arr becomes {10,25,30,40}
}
```
**Explanation:**  
`subspan` creates a subview with bounds checking. Accessing index 4 throws `std::out_of_range`.

---

**Question 4 Solution: constexpr Vector Sort**
```cpp
#include <vector>
#include <algorithm>
#include <cassert>

consteval std::vector<int> constexpr_sort() {
    std::vector<int> v = {5,3,4,1,2};
    std::sort(v.begin(), v.end());
    return v;
}

int main() {
    constexpr auto sorted = constexpr_sort();
    static_assert(sorted[0] == 1); // Compile-time check
    // Runtime check omitted for brevity
}
```
**Explanation:**  
C++20 allows `std::sort` in `consteval` functions. The sorted vector is verified at compile time.

---

**Question 5 Solution: Atomic Shared Pointer**
```cpp
#include <atomic>
#include <memory>
#include <thread>

struct Counter {
    std::atomic<std::shared_ptr<int>> ptr = std::make_shared<int>(0);
    void increment() {
        std::shared_ptr<int> current = ptr.load();
        std::shared_ptr<int> desired;
        do {
            desired = std::make_shared<int>(*current + 1);
        } while (!ptr.compare_exchange_weak(current, desired));
    }
};

int main() {
    Counter c;
    std::thread t1([&] { for (int i=0; i<1000; ++i) c.increment(); });
    std::thread t2([&] { for (int i=0; i<1000; ++i) c.increment(); });
    t1.join(); t2.join();
    std::cout << *c.ptr; // 2000 (assuming no data races)
}
```
**Explanation:**  
`compare_exchange_weak` atomically updates the shared pointer. Thread safety is ensured via atomic operations.

---

**Question 6 Solution: Custom Formatter**
```cpp
#include <format>
#include <iostream>

struct Point3D { int x, y, z; };

template<> struct std::formatter<Point3D> {
    char format = 'd';
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end()) format = *it++;
        return it;
    }
    auto format(const Point3D& p, std::format_context& ctx) const {
        if (format == 'x') 
            return std::format_to(ctx.out(), "({:x},{:x},{:x})", p.x, p.y, p.z);
        else
            return std::format_to(ctx.out(), "({},{},{})", p.x, p.y, p.z);
    }
};

int main() {
    Point3D p{15, 255, 16};
    std::cout << std::format("{:%x,%d,%x}", p); // (f,255,10)
}
```
**Explanation:**  
The `formatter` specialization handles hex and decimal formatting using a custom specifier.

---

**Question 7 Solution: Time Zone Conversion**
```cpp
#include <chrono>
#include <format>
#include <iostream>

int main() {
    using namespace std::chrono;
    auto utc_time = sys_days{2023y/July/1} + 12h;
    zoned_time ny_time{"America/New_York", utc_time};
    std::cout << std::format("{:%Y-%m-%d %H:%M}", ny_time); // 2023-07-01 08:00
}
```
**Explanation:**  
`zoned_time` converts UTC to New York time, adjusting for daylight saving.

---

**Question 8 Solution: Generator with Exceptions**
```cpp
#include <coroutine>
#include <stdexcept>
#include <iostream>

template<typename T>
struct Generator {
    struct promise_type {
        T value;
        std::exception_ptr eptr;
        auto yield_value(T val) { value = val; return suspend_always{}; }
        auto initial_suspend() { return suspend_never{}; }
        auto final_suspend() noexcept { return suspend_always{}; }
        void unhandled_exception() { eptr = std::current_exception(); }
        Generator get_return_object() { return Generator{handle_type::from_promise(*this)}; }
    };
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coro;
    explicit Generator(handle_type h) : coro(h) {}
    ~Generator() { if (coro) coro.destroy(); }
    T next() {
        coro.resume();
        if (coro.promise().eptr) std::rethrow_exception(coro.promise().eptr);
        return coro.promise().value;
    }
};

Generator<int> gen(std::vector<int> v) {
    for (int x : v) {
        if (x < 0) throw std::runtime_error("Negative value");
        co_yield x;
    }
}

int main() {
    try {
        auto g = gen({5, 3, -1, 2});
        std::cout << g.next() << " "; // 5
        std::cout << g.next() << " "; // 3
        g.next(); // Throws
    } catch (const std::exception& e) {
        std::cout << e.what(); // "Negative value"
    }
}
```
**Explanation:**  
Exceptions in coroutines are captured via `unhandled_exception` and rethrown using `std::rethrow_exception`.

---

**Question 9 Solution: Semaphore Queue**
```cpp
#include <semaphore>
#include <queue>
#include <thread>

template<typename T>
class SafeQueue {
    std::queue<T> queue;
    std::mutex mtx;
    std::counting_semaphore<> items{0};
    std::counting_semaphore<> spaces{3}; // Buffer size 3
public:
    void enqueue(T item) {
        spaces.acquire();
        {
            std::lock_guard lock(mtx);
            queue.push(item);
        }
        items.release();
    }
    T dequeue() {
        items.acquire();
        T item;
        {
            std::lock_guard lock(mtx);
            item = queue.front();
            queue.pop();
        }
        spaces.release();
        return item;
    }
};

int main() {
    SafeQueue<int> q;
    std::jthread producer([&] { for (int i=0; i<5; ++i) q.enqueue(i); });
    std::jthread consumer([&] { for (int i=0; i<5; ++i) q.dequeue(); });
}
```
**Explanation:**  
`counting_semaphore` controls buffer slots. Producers wait on `spaces`, consumers on `items`.

---

**Question 10 Solution: Latch Synchronization**
```cpp
#include <latch>
#include <thread>
#include <vector>

int main() {
    std::vector<int> data(4);
    std::latch latch(4);
    auto worker = [&](int idx) {
        data[idx] = idx + 1;
        latch.count_down();
    };
    std::jthread t1(worker, 0);
    std::jthread t2(worker, 1);
    std::jthread t3(worker, 2);
    std::jthread t4(worker, 3);
    latch.wait(); // Wait for all threads
    // data becomes [1,2,3,4]
}
```
**Explanation:**  
`std::latch` coordinates thread completion. `count_down` decrements the counter; `wait` blocks until zero.

---

**Question 11 Solution: Barrier Phases**
```cpp
#include <barrier>
#include <iostream>
#include <thread>

int main() {
    std::barrier bar(3, []() noexcept { std::cout << "Phase completed\n"; });
    auto task = [&](int id) {
        for (int phase=0; phase<3; ++phase) {
            std::cout << "Thread " << id << " phase " << phase << "\n";
            bar.arrive_and_wait();
        }
    };
    std::jthread t1(task, 1);
    std::jthread t2(task, 2);
    std::jthread t3(task, 3);
}
```
**Explanation:**  
`std::barrier` synchronizes threads at each phase. The completion function prints after each phase.

---

**Question 12 Solution: jthread Cancellation**
```cpp
#include <stop_token>
#include <thread>
#include <iostream>

void task(std::stop_token stoken) {
    int count = 0;
    while (!stoken.stop_requested() && count < 10) {
        std::cout << "Iteration " << ++count << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::jthread t(task);
    std::this_thread::sleep_for(std::chrono::milliseconds(550));
    t.request_stop(); // Stops after ~5 iterations
}
```
**Explanation:**  
`std::stop_token` checks for cancellation requests. `request_stop()` signals the thread to exit.

---

**Question 13 Solution: osyncstream**
```cpp
#include <syncstream>
#include <thread>

void log(int id) {
    for (int i=0; i<100; ++i)
        std::osyncstream(std::cout) << "Thread " << id << ": " << i << "\n";
}

int main() {
    std::jthread t1(log, 1);
    std::jthread t2(log, 2);
}
```
**Explanation:**  
`std::osyncstream` ensures atomic writes to `std::cout`, preventing interleaved lines.

---

**Question 14 Solution: is_constant_evaluated**
```cpp
#include <type_traits>

constexpr int square(int x) {
    if (std::is_constant_evaluated()) return x * x;
    else return x * x; // Runtime version (could differ)
}

int main() {
    static_assert(square(5) == 25); // Compile-time
    int x = square(5); // Runtime
}
```
**Explanation:**  
`std::is_constant_evaluated` allows branching between compile-time and runtime logic.

---

**Question 15 Solution: source_location Logging**
```cpp
#include <source_location>
#include <iostream>

#define LOG(msg) \
    std::cout << "[" << std::source_location::current().file_name() \
              << ":" << std::source_location::current().line() << "] " \
              << std::source_location::current().function_name() << ": " \
              << msg << "\n"

void demo() {
    LOG("Test message");
}

int main() {
    demo(); // Output: [main.cpp:10] demo: Test message
}
```
**Explanation:**  
`std::source_location` captures context (file, line, function) at the point of the macro expansion.

---

### Summary
These challenges cover C++20’s critical features, emphasizing the Standard Library and Concurrency. Each solution 
demonstrates idiomatic usage and highlights edge cases (e.g., exception handling, thread safety). Testing 
ensures correctness across compile-time/runtime scenarios.