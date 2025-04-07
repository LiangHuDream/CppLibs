
@[TOC](Historical Context)
Key Focus Areas:
1. **C++ Standards Evolution**  
   - C++98 introduced templates/STL, C++11 revolutionized with move semantics/type traits, C++17 added parallel STL/filesystem.
2. **TR1's Role**  
   - Bridged legacy C++ to Modern C++ by introducing Boost-inspired libraries (smart pointers, regex).
3. **C++20 "Big Four"**  
   - Concepts (template constraints), Modules (header replacement), Ranges (functional-style algorithms), Coroutines (async control flow).
4. **Standardization Process**  
   - WG21 structure: Core/Library Working Groups (Stage 3), Evolution Groups (Stage 2), Domain-Specific Study Groups (Stage 1).
5. **Language/Concurrency Improvements**  
   - `<=>` operator, designated initializers, atomic smart pointers, semaphores/latches.

---

### Multiple-Choice Questions

**Question 1**  
Which C++ standards introduced the following features?  
A) Move semantics  
B) Filesystem library  
C) TR1 technical report  
D) Concepts  
Options:  
1. C++11  
2. C++17  
3. C++20  
4. C++03  

**Question 2**  
What are the primary goals of C++20 Modules?  
A) Eliminate header files  
B) Replace the preprocessor entirely  
C) Improve compile times  
D) Enable cross-platform thread management  

**Question 3**  
Which statements about **Concepts** are true?  
A) They replace `typename` in template parameters.  
B) They enable compiler-enforced semantic constraints.  
C) They simplify SFINAE techniques.  
D) They require runtime type checks.  

**Question 4**  
Identify valid C++20 syntax for **Coroutines**:  
A) `co_return 42;`  
B) `co_await future;`  
C) `co_yield value;`  
D) `co_parallel algorithm;`  

**Question 5**  
Which features are part of the **Ranges Library**?  
A) Pipeline operator `|` for algorithm composition  
B) Lazy evaluation of infinite sequences  
C) Built-in GPU acceleration  
D) Direct container operations without iterators  

**Question 6**  
What does the **Three-Way Comparison Operator (`<=>`)** provide?  
A) Default comparison for all class members  
B) Automatic generation of `==`, `!=`, `<`, etc.  
C) Compile-time type equivalence checks  
D) A single function to define all relational operators  

**Question 7**  
Which statements about **Designated Initialization** are correct?  
A) It allows initializing struct members by name.  
B) It works with aggregate types only.  
C) Members must be initialized in declaration order.  
D) It supports non-static class members.  

**Question 8**  
What are valid uses of **`std::span`**?  
A) Wrapping a C-style array  
B) Owning a contiguous sequence of elements  
C) Providing bounds-checked access  
D) Replacing `std::vector` in all scenarios  

**Question 9**  
Which **Concurrency** features were added in C++20?  
A) `std::jthread`  
B) `std::atomic_ref`  
C) `std::mutex`  
D) `std::latch`  

**Question 10**  
What is true about **`constinit`** and **`consteval`**?  
A) `constinit` ensures constant initialization.  
B) `consteval` functions can be called at runtime.  
C) `constinit` applies to dynamic storage duration.  
D) `consteval` functions are implicitly `constexpr`.  

---

### Answers & Explanations

**Answer 1**  
- **Correct**: A(1), B(2), C(4), D(3)  
- **Explanation**:  
  - Move semantics (C++11), Filesystem (C++17), TR1 (published in 2005 as a precursor to C++11), Concepts (C++20).

**Answer 2**  
- **Correct**: A, C  
- **Explanation**:  
  - Modules aim to replace headers (A) and improve compilation (C), but the preprocessor remains for macros (B). Thread management is unrelated (D).

**Answer 3**  
- **Correct**: B, C  
- **Explanation**:  
  - Concepts enforce constraints (B) and reduce SFINAE complexity (C). They don’t replace `typename` entirely (A) and are compile-time (D is false).

**Answer 4**  
- **Correct**: A, B, C  
- **Explanation**:  
  - `co_return`, `co_await`, and `co_yield` are coroutine keywords. `co_parallel` is invalid (D).

**Answer 5**  
- **Correct**: A, B, D  
- **Explanation**:  
  - Ranges use `|` for composition (A), lazy evaluation (B), and direct container ops (D). GPU support is not part of C++20 (C).

**Answer 6**  
- **Correct**: B, D  
- **Explanation**:  
  - `<=>` generates relational operators (B) and reduces boilerplate (D). It doesn’t auto-generate `==` unless `= default` (A partial). No type checks (C).

**Answer 7**  
- **Correct**: A, B, C  
- **Explanation**:  
  - Designated initialization requires aggregates (B), named members (A), and declaration order (C). Non-static members are allowed (D is true but irrelevant).

**Answer 8**  
- **Correct**: A, C  
- **Explanation**:  
  - `std::span` wraps existing data (A) with optional bounds checks (C). It doesn’t own data (B) or replace `vector` (D).

**Answer 9**  
- **Correct**: A, B, D  
- **Explanation**:  
  - `jthread` (A), `atomic_ref` (B), and `latch` (D) are C++20. `mutex` (C) predates C++20.

**Answer 10**  
- **Correct**: A, D  
- **Explanation**:  
  - `constinit` ensures static initialization (A). `consteval` functions are `constexpr` (D) but can’t run at runtime (B false). `constinit` applies to static storage (C false).