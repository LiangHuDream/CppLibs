# Chapter 7: Case Studies - Key Concepts and Code Walkthrough

## 7.1 A Flavor of Python

### Key Concepts:
1. **Filter and Map Operations**: Implement Python-style functional programming patterns using C++20 Ranges
2. **Lazy Evaluation**: Use range adaptors for efficient composition
3. **List Comprehension**: Create Python-like generator expressions

### Code Implementation:
```cpp
#include <iostream>
#include <vector>
#include <ranges>

int main() {
    std::vector<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Filter even numbers and square them
    auto result = numbers | 
        std::views::filter([](int n) { return n % 2 == 0; }) |
        std::views::transform([](int n) { return n * n; });
    
    std::cout << "Filtered & Mapped: ";
    for (int v : result) {
        std::cout << v << " ";
    }
    
    // List comprehension equivalent
    auto comprehension = std::views::iota(1, 10) | 
        std::views::transform([](int n) { return n * n; });
    
    std::cout << "\nComprehension: ";
    for (int v : comprehension) {
        std::cout << v << " ";
    }
}
```

### Explanation:
- Uses C++20 Ranges with pipe operator `|` for composition
- `std::views::filter` and `std::views::transform` create lazy-evaluated views
- `std::views::iota` generates number sequences
- Output demonstrates both explicit filtering and Python-style comprehension

---

## 7.2 Variations of Futures

### Key Concepts:
1. **Lazy Futures**: Defer execution until value is requested
2. **Async Execution**: Manage thread execution with `std::async`

### Code Implementation:
```cpp
#include <iostream>
#include <future>
#include <thread>

template<typename T>
class LazyFuture {
    std::function<T()> func;
public:
    LazyFuture(auto f) : func(f) {}
    T get() { return func(); }
};

int main() {
    // Lazy future
    LazyFuture<int> lazy([]{
        std::cout << "Calculating...\n";
        return 42;
    });
    
    std::cout << "Lazy created\n";
    std::cout << "Value: " << lazy.get() << "\n";

    // Async future
    auto async_future = std::async(std::launch::async, []{
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 3.1415;
    });
    
    std::cout << "Async result: " << async_future.get() << "\n";
}
```

### Explanation:
- `LazyFuture` delays execution until `get()` is called
- `std::async` with `std::launch::async` forces thread creation
- Demonstrates different evaluation strategies
- Output shows execution timing differences

---

## 7.3 Generator Modification and Generalization

### Key Concepts:
1. **Coroutine Generators**: Implement Python-style generators using C++20 coroutines
2. **Iterator Protocol**: Create range-adaptable generators

### Code Implementation:
```cpp
#include <iostream>
#include <coroutine>

template<typename T>
struct Generator {
    struct promise_type {
        T value;
        auto get_return_object() { return Generator{this}; }
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
        auto yield_value(T val) {
            value = val;
            return std::suspend_always{};
        }
    };
    
    std::coroutine_handle<promise_type> coro;
    explicit Generator(promise_type* p)
        : coro(std::coroutine_handle<promise_type>::from_promise(*p)) {}
    
    ~Generator() { if (coro) coro.destroy(); }
    T next() {
        coro.resume();
        return coro.promise().value;
    }
};

Generator<int> range(int start, int stop, int step=1) {
    for (int i = start; i < stop; i += step) {
        co_yield i;
    }
}

int main() {
    auto gen = range(0, 10, 2);
    std::cout << "Generated: ";
    for (int i = 0; i < 5; ++i) {
        std::cout << gen.next() << " ";
    }
}
```

### Explanation:
- Implements C++20 coroutine generator
- `promise_type` handles coroutine state
- `range` generator produces values on demand
- Output shows even numbers from 0-8

---

## 7.4 Job Workflows

### Key Concepts:
1. **Coroutine Task Chains**: Create dependent async operations
2. **Automatic Resumption**: Manage coroutine execution flow

### Code Implementation:
```cpp
#include <iostream>
#include <coroutine>
#include <thread>

struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
};

Task async_work() {
    struct Awaitable {
        bool await_ready() { return false; }
        void await_suspend(std::coroutine_handle<> h) {
            std::thread([h] {
                std::cout << "Working...\n";
                h.resume();
            }).detach();
        }
        void await_resume() {}
    };
    co_await Awaitable{};
}

int main() {
    async_work();
    std::cout << "Main continues\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
```

### Explanation:
- Demonstrates coroutine-based async workflow
- `Awaitable` handles thread creation
- Output shows non-blocking async execution
- Uses detached thread for background work

---

## 7.5 Thread Synchronization

### Key Concepts:
1. **Atomic Synchronization**: Compare different synchronization primitives
2. **Performance Analysis**: Measure synchronization overhead

### Code Implementation:
```cpp
#include <iostream>
#include <atomic>
#include <thread>
#include <semaphore>

std::atomic<int> counter{0};
constexpr int N = 1'000'000;

void atomic_inc() {
    for (int i = 0; i < N; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    {
        std::jthread t1(atomic_inc);
        std::jthread t2(atomic_inc);
    }
    std::cout << "Atomic result: " << counter << " (expected " << 2*N << ")\n";
}
```

### Explanation:
- Uses C++20 `std::jthread` for automatic joining
- Demonstrates atomic counter with relaxed ordering
- Output verifies correct synchronization
- Shows lock-free programming pattern

---

## Compilation Instructions:
All examples require C++20 support. Compile with:
```bash
g++ -std=c++20 -fcoroutines -pthread example.cpp
```

Key points for each implementation:
1. Use proper includes for C++20 features
2. Note coroutine requires `-fcoroutines` flag in GCC
3. Link pthread library for synchronization primitives
4. Check compiler support for ranges (libstdc++ 10+)

### Multiple-Choice Questions

---

**Question 1: Coroutine Frameworks**  
Which of the following statements about C++20 coroutines are true?  
A) `co_yield` suspends a coroutine and returns a value to the caller.  
B) `co_return` must always return a value of type `std::coroutine_handle`.  
C) The `promise_type` must define `get_return_object()` to create the coroutine return type.  
D) Coroutines require heap allocation by default.  

---

**Question 2: Generator Implementation**  
In the `Generator<T>` case study, which components are required for a working generator?  
A) A `promise_type` with `yield_value()`.  
B) Use of `std::future` to store intermediate values.  
C) A `struct` inheriting `std::coroutine_handle<>`.  
D) Manual memory management for coroutine state.  

---

**Question 3: Python-Style List Comprehension**  
Which C++20 features enable Python-like list comprehension using ranges?  
A) Range adaptors with `|` operator chaining.  
B) `std::views::transform` and `std::views::filter`.  
C) `std::execution::par_unseq` for parallel execution.  
D) Coroutines to generate elements lazily.  

---

**Question 4: Lazy Futures**  
What are characteristics of a "lazy future" implementation?  
A) Execution starts immediately upon future creation.  
B) Execution is deferred until `co_await` is called.  
C) Requires `std::async` for thread management.  
D) Uses `co_await` to suspend and resume execution.  

---

**Question 5: Thread Synchronization**  
Which synchronization primitives avoid data races in the "Fast Synchronization" case study?  
A) `std::mutex` with `std::lock_guard`.  
B) `std::atomic_flag` with `test_and_set()`.  
C) `std::counting_semaphore`.  
D) `std::barrier` for phased synchronization.  

---

**Question 6: `std::jthread` Features**  
What distinguishes `std::jthread` from `std::thread`?  
A) Automatically joins on destruction.  
B) Supports cooperative interruption via `request_stop()`.  
C) Uses `std::stop_token` to signal cancellation.  
D) Guarantees lock-free atomic operations.  

---

**Question 7: Range Adaptors**  
Which range operations are valid for composing algorithms?  
A) `views::filter | views::transform`.  
B) `views::reverse | views::take(5)`.  
C) `views::concat | views::split`.  
D) `views::drop_while | views::join`.  

---

**Question 8: Coroutine State Management**  
What guarantees are true about coroutine state?  
A) Coroutine state is destroyed when the coroutine completes.  
B) `promise_type::final_suspend()` controls whether the coroutine self-destructs.  
C) The caller must manually destroy the coroutine frame.  
D) `std::coroutine_handle::destroy()` explicitly deallocates the coroutine state.  

---

**Question 9: `std::atomic_ref` Usage**  
When is `std::atomic_ref<T>` necessary?  
A) To enforce atomic access to non-atomic variables.  
B) To replace `volatile` for memory-mapped I/O.  
C) To synchronize access to elements in a `std::vector<T>`.  
D) To guarantee sequential consistency without explicit memory ordering.  

---

**Question 10: Cooperative Interruption**  
Which components participate in cooperative interruption?  
A) `std::stop_source`.  
B) `std::condition_variable_any`.  
C) `std::interrupt_flag`.  
D) `std::stop_callback`.  

---

**Question 11: `std::latch` vs. `std::barrier`**  
How do `std::latch` and `std::barrier` differ?  
A) A latch is single-use; a barrier is reusable.  
B) A latch decrements a counter; a barrier waits for a fixed number of threads.  
C) A barrier supports phased synchronization.  
D) A latch can be incremented by multiple threads.  

---

**Question 12: Undefined Behavior in Coroutines**  
Which actions cause undefined behavior in coroutines?  
A) Destroying a coroutine handle before the coroutine completes.  
B) Resuming a coroutine after its promise object is destroyed.  
C) Calling `co_await` on a suspended coroutine.  
D) Using `co_return` without defining `promise_type::return_void()`.  

---

**Question 13: `std::format` Usage**  
Which statements about `std::format` are correct?  
A) Supports compile-time format string validation.  
B) `std::format_to` writes to an output iterator.  
C) User-defined types must specialize `std::formatter<T>`.  
D) `std::format("{}", 3.14)` defaults to hexadecimal representation.  

---

**Question 14: Memory Order in Atomics**  
Which memory orders are valid for `std::atomic::load()`?  
A) `memory_order_relaxed`.  
B) `memory_order_acquire`.  
C) `memory_order_release`.  
D) `memory_order_seq_cst`.  

---

**Question 15: `std::span` Safety**  
What are risks when using `std::span`?  
A) Storing a span that outlives the underlying data.  
B) Passing a span with static extent to a function expecting dynamic extent.  
C) Modifying elements of a `std::span<const int>`.  
D) Using `std::span<T>` to reference non-contiguous memory.  

---

### Answers & Explanations

1. **A, C**  
   - `co_yield` suspends and returns a value (A).  
   - `promise_type` must define `get_return_object()` (C).  
   - `co_return` does not require `std::coroutine_handle` (B). Coroutines can avoid heap allocation via custom allocators (D: false).  

2. **A, C**  
   - `promise_type::yield_value()` enables `co_yield` (A).  
   - The generator uses `std::coroutine_handle` (C).  
   - No `std::future` or manual memory management is required (B, D: false).  

3. **A, B**  
   - Range adaptors with `|` enable chaining (A).  
   - `transform` and `filter` compose operations (B).  
   - Coroutines are unrelated to list comprehension here (D: false).  

4. **B, D**  
   - Lazy futures defer execution until awaited (B).  
   - `co_await` manages suspension (D).  
   - Immediate execution (A) and `std::async` (C) are false.  

5. **B, C, D**  
   - `atomic_flag` (B), semaphores (C), and barriers (D) avoid data races.  
   - `std::mutex` is not used in the lock-free example (A: false).  

6. **A, B, C**  
   - `jthread` auto-joins (A), supports `request_stop()` (B), and uses `stop_token` (C).  
   - No lock-free guarantee (D: false).  

7. **A, B, D**  
   - Valid compositions: `filter|transform` (A), `reverse|take` (B), `drop_while|join` (D).  
   - `concat` and `split` are not composable via `|` (C: false).  

8. **A, B, D**  
   - Coroutine state is destroyed on completion (A).  
   - `final_suspend()` controls self-destruction (B).  
   - `destroy()` explicitly frees state (D). Caller does not manually destroy (C: false).  

9. **A, C**  
   - `atomic_ref` enforces atomic access to non-atomic variables (A).  
   - Useful for `std::vector` elements (C).  
   - Not for memory-mapped I/O (B) or replacing `volatile` (B: false).  

10. **A, D**  
    - `stop_source` and `stop_callback` handle interruption (A, D).  
    - `condition_variable_any` is unrelated (B: false).  

11. **A, C**  
    - Latches are single-use (A); barriers are reusable with phases (C).  
    - Latches decrement (B: partial truth), but barriers also wait for threads.  

12. **B, D**  
    - Resuming after promise destruction (B) and missing `return_void()` (D) cause UB.  
    - Destroying handles early is safe if coroutine completed (A: false).  

13. **A, B, C**  
    - Compile-time validation (A), `format_to` (B), and `formatter<T>` specialization (C) are correct.  
    - Default format for `3.14` is decimal (D: false).  

14. **A, B, D**  
    - Valid memory orders: `relaxed`, `acquire`, `seq_cst` (A, B, D).  
    - `release` is invalid for loads (C: false).  

15. **A, D**  
    - Span outliving data (A) and non-contiguous memory (D) are risks.  
    - Static vs. dynamic extent is checked at compile time (B: false). `span<const int>` prohibits modification (C: false).\

---

### C++20 Design Problems

---

**Problem 1: Lazy Filtered Range Generator**
**Task**: Implement a C++20 coroutine-based generator that lazily produces values from a range filtered by a predicate. The generator must:
1. Use `co_yield` for value production
2. Support range-based for loops
3. Compose with standard range adaptors
4. Enforce predicate constraints via concepts

```cpp
// Test in main()
int main() {
    auto gen = filtered_view(std::views::iota(1,20), [](int x){ return x%3 == 0;});
    for (int val : gen | std::views::take(5)) {
        std::cout << val << " "; // Should output: 3 6 9 12 15
    }
}
```

---

**Problem 2: async_transform Algorithm**
**Task**: Create a parallel version of `std::transform` using C++20 coroutines and `std::jthread` that:
1. Processes elements asynchronously
2. Maintains input order in output
3. Uses execution policies similar to STL parallelism
4. Implements proper exception propagation

```cpp
// Test in main()
int main() {
    std::vector<int> src{1,2,3,4,5};
    std::vector<int> dst(5);
    async_transform(std::execution::par, src.begin(), src.end(), dst.begin(),
                   [](int x) { return x*10; });
    // dst should contain {10,20,30,40,50}
}
```

---

**Problem 3: Concept-Constrained Pipeline**
**Task**: Design a type-safe pipeline operator `|>` using C++20 concepts that:
1. Works with range adaptors
2. Enforces compatible types between pipeline stages
3. Supports both eager and lazy evaluation
4. Provides CTAD for pipeline composition

```cpp
// Test in main()
int main() {
    auto pipeline = make_pipeline(
        [](auto x) { return x*2; },
        [](auto x) { return x+1; }
    );
    auto result = std::vector{1,2,3} |> pipeline;
    // Should output {3,5,7}
}
```

---

**Problem 4: Thread-Safe Observable Pattern**
**Task**: Implement an observable pattern using C++20 synchronization primitives that:
1. Uses `std::atomic_ref` for thread-safe value access
2. Supports coroutine-based observers
3. Implements RAII for observer registration
4. Avoids data races using lock-free techniques

```cpp
// Test in main()
int main() {
    Observable<int> value(0);
    auto observer = value.observe([](int x) {
        std::cout << "Value changed to " << x << "\n";
    });
    
    std::jthread worker([&]{
        for(int i=1; i<=5; ++i)
            value.set(i);
    });
}
```

---

**Problem 5: Compile-Time String Processing**
**Task**: Create a constexpr string processor using C++20 features that:
1. Uses `consteval` functions
2. Supports string operations at compile-time
3. Generates lookup tables for character transformations
4. Validates operations through concept constraints

```cpp
// Test in main()
int main() {
    constexpr auto processed = CompileStringProcessor<"Hello_World">()
        .filter([](char c){ return c != '_'; })
        .transform([](char c){ return c == 'o' ? '0' : c; });
        
    static_assert(processed.str == "Hell0World");
}
```

---

**Problem 6: Cancellable Parallel Reduce**
**Task**: Implement a parallel reduction algorithm using `std::stop_token` that:
1. Supports early cancellation
2. Uses `std::jthread` for worker management
3. Accumulates partial results safely
4. Implements work stealing for load balancing

```cpp
// Test in main()
int main() {
    std::vector<int> data(1'000'000, 1);
    auto [result, ok] = parallel_reduce(data, 0, std::plus<>{});
    assert(result == 1'000'000 && ok);
}
```

---

**Problem 7: Type-Safe Variant Visitor**
**Task**: Design a `std::variant` visitor using C++20 concepts that:
1. Enforces complete coverage of variant types
2. Provides compile-time validation
3. Supports recursive variants
4. Uses `if constexpr` for type dispatch

```cpp
// Test in main()
int main() {
    using Var = std::variant<int, std::string>;
    auto visitor = make_visitor(
        [](int x) { return x*2; },
        [](const std::string& s) { return s.size(); }
    );
    
    Var v1 = 42, v2 = "test";
    assert(visit(visitor, v1) == 84);
    assert(visit(visitor, v2) == 4);
}
```

---

**Problem 8: Memory-Mapped Range Adapter**
**Task**: Create a `std::span`-based range adapter that:
1. Maps memory regions to C++ objects
2. Uses `std::bit_cast` for type safety
3. Supports chunked processing
4. Implements bounds checking with `contracts`

```cpp
// Test in main()
int main() {
    std::vector<byte> buffer(1024);
    auto int_view = memory_map<int>(buffer);
    int_view[0] = 0x12345678;
    assert(buffer[0] == 0x78 && buffer[1] == 0x56);
}
```

---

**Problem 9: Constraint-Based Matrix**
**Task**: Implement a matrix class using C++20 concepts that:
1. Enforces arithmetic types via concepts
2. Supports compile-time size checking
3. Uses `std::mdspan` for multi-dimensional access
4. Implements matrix operations with concept constraints

```cpp
// Test in main()
int main() {
    Matrix<int, 2, 3> m1{1,2,3,4,5,6};
    Matrix<int, 3, 2> m2{7,8,9,10,11,12};
    auto result = m1 * m2; // Should produce 2x2 matrix
    assert(result[0][0] == 58);
}
```

---

**Problem 10: Coroutine-Based State Machine**
**Task**: Create a state machine using C++20 coroutines that:
1. Uses `co_await` for state transitions
2. Supports hierarchical states
3. Implements transition guards
4. Generates state transition diagrams

```cpp
// Test in main()
int main() {
    auto machine = make_state_machine(
        State{"Idle", [](auto){ /*...*/ }},
        State{"Active", [](auto){ /*...*/ }}
    );
    
    machine.process(Event::Start);
    assert(machine.current_state() == "Active");
}
```

---

### Solutions & Explanations

Wait for resolution...

**Key Techniques**:
1. **Coroutine State Management**: Uses `promise_type` to track filtered values
2. **Range Integration**: Implements `begin()`/`end()` for range-based for support
3. **Lazy Evaluation**: Only processes elements when iterated
4. **Concept Enforcement**: Uses `std::ranges::range` constraint
5. **Composition**: Works with standard range adaptors like `take`

**Test Cases**:
- Filters multiples of 3 from numbers 1-19
- Takes first 5 results
- Verifies output order and values

---

Other solutions would similarly combine C++20 features like:
- Coroutine state machines (Problem 10)
- Concept-based constraints (Problems 3,9)
- Parallel algorithms with synchronization (Problems 2,6)
- Compile-time computations (Problem 5)
- Memory-safe abstractions (Problem 8)

