@[TOC](The Concurrency API)

---

#### Core Concepts and Code Explanations

---

##### Item 35: Prefer Task-Based Programming to Thread-Based
- Key Idea: Use `std::async` (tasks) instead of `std::thread` for better resource management and exception safety.
- Code Example:
  ```cpp
  #include <future>
  #include <iostream>

  int compute() { return 42; }

  int main() {
      auto future = std::async(compute); // Task-based
      std::cout << future.get(); // Output: 42
  }
  ```
- Test Case:
  ```cpp
  // Thread-based (not preferred)
  std::thread t([] { std::cout << "Thread running\n"; });
  if (t.joinable()) t.join(); // Manual management
  ```

---

##### Item 36: Specify `std::launch::async` for Asynchronicity
- Key Idea: Default `std::async` launch policy (`std::launch::async | std::launch::deferred`) may defer execution. Force async with `std::launch::async`.
- Code Example:
  ```cpp
  auto future = std::async(std::launch::async, [] { return 1 + 1; });
  ```
- Test Case:
  ```cpp
  auto deferred_future = std::async(std::launch::deferred, [] { return 2; });
  bool is_deferred = deferred_future.wait_for(std::chrono::seconds(0)) == std::future_status::deferred;
  std::cout << is_deferred; // Output: 1 (true)
  ```

---

##### Item 37: Make `std::thread`s Unjoinable on All Paths
- Key Idea: RAII wrapper to ensure threads are joined/detected.
- Code Example:
  ```cpp
  class ThreadGuard {
      std::thread t;
  public:
      explicit ThreadGuard(std::thread t_) : t(std::move(t_)) {}
      ~ThreadGuard() { if (t.joinable()) t.join(); }
  };

  int main() {
      ThreadGuard guard(std::thread([] { std::cout << "Thread\n"; }));
  } // Automatically joined
  ```

---

##### Item 38: Thread Handle Destructor Behavior
- Key Idea: Futures normally don’t block on destruction unless they’re the last reference to a deferred task’s shared state.
- Test Case:
  ```cpp
  std::future<void> f = std::async(std::launch::async, [] { std::this_thread::sleep_for(std::chrono::seconds(1)); });
  // f's destructor blocks if the task hasn't completed (depends on implementation)
  ```

---

##### Item 39: Void Futures for One-Shot Events
- Key Idea: Use `std::promise<void>` and `std::future<void>` for simple thread signaling.
- Code Example:
  ```cpp
  std::promise<void> p;
  std::future<void> f = p.get_future();

  std::thread t([&f] { f.wait(); std::cout << "Event signaled\n"; });
  p.set_value(); // Signal the event
  t.join();
  ```

---

##### Item 40: `std::atomic` vs. `volatile`
- Key Idea: `std::atomic` for thread-safe data; `volatile` for memory-mapped I/O.
- Test Case:
  ```cpp
  std::atomic<int> atomic_var(0);
  volatile int volatile_var = 0;

  std::thread t1([&] { atomic_var++; });
  std::thread t2([&] { atomic_var++; });
  t1.join(); t2.join();
  std::cout << atomic_var; // Output: 2 (guaranteed)
  ```

---

### 10 Multiple-Choice Questions

1. What happens if a `std::thread` object is destroyed while still joinable?
   - A) It detaches automatically.
   - B) The program terminates.
   - C) It joins automatically.
   - D) Undefined behavior.

2. Which launch policy ensures a task runs in a new thread?
   - A) `std::launch::async`
   - B) `std::launch::deferred`
   - C) Both
   - D) None

3. What is the return type of `std::async`?
   - A) `std::thread`
   - B) `std::future`
   - C) `std::promise`
   - D) `std::packaged_task`

4. Which is thread-safe for concurrent increments?
   - A) `volatile int`
   - B) `std::atomic<int>`
   - C) `int` with a mutex
   - D) Both B and C

5. When does a `std::future`’s destructor block?
   - A) Always
   - B) Never
   - C) Only for deferred tasks
   - D) When it’s the last reference to a deferred task’s shared state

6. What does `std::promise::set_value()` do?
   - A) Signals an event
   - B) Stores a value in the shared state
   - C) Both
   - D) None

7. Which is true about `volatile`?
   - A) It ensures atomicity.
   - B) It prevents compiler optimizations.
   - C) It is used for inter-thread communication.
   - D) All of the above.

8. What is the purpose of `std::future_status::deferred`?
   - A) The task is already completed.
   - B) The task will run on a new thread.
   - C) The task is not yet scheduled.
   - D) The task is invalid.

9. Which RAII wrapper is standard in C++20 for threads?
   - A) `std::jthread`
   - B) `std::thread_guard`
   - C) `std::async_guard`
   - D) None

10. What is the output of incrementing `volatile_var` with two threads?
    - A) 0
    - B) 1
    - C) 2
    - D) Undefined

---

### Answers to Multiple-Choice Questions

1. B – A joinable thread’s destructor calls `std::terminate`.
2. A – `std::launch::async` guarantees a new thread.
3. B – `std::async` returns a `std::future`.
4. D – Both `std::atomic` and mutex-protected `int` are thread-safe.
5. D – Blocks only if it’s the last reference to a deferred task.
6. C – Sets a value and signals readiness.
7. B – `volatile` prevents compiler optimizations (not thread-safe).
8. C – Deferred tasks are not yet scheduled.
9. A – `std::jthread` automatically joins in C++20.
10. D – `volatile` does not ensure atomicity; UB due to data races.

---

### 5 Design Questions

1. Implement a thread-safe queue using `std::mutex` and `std::condition_variable`.  
   Answer:
   ```cpp
   template<typename T>
   class ThreadSafeQueue {
       std::queue<T> queue;
       mutable std::mutex mtx;
       std::condition_variable cv;
   public:
       void push(T item) {
           std::lock_guard<std::mutex> lock(mtx);
           queue.push(std::move(item));
           cv.notify_one();
       }
       T pop() {
           std::unique_lock<std::mutex> lock(mtx);
           cv.wait(lock, [this] { return !queue.empty(); });
           T item = std::move(queue.front());
           queue.pop();
           return item;
       }
   };
   ```

2. Design a `scoped_thread` class that joins on destruction.  
   Answer:
   ```cpp
   class ScopedThread {
       std::thread t;
   public:
       explicit ScopedThread(std::thread t_) : t(std::move(t_)) {
           if (!t.joinable()) throw std::logic_error("No thread");
       }
       ~ScopedThread() { t.join(); }
   };
   ```

3. Use `std::async` to compute the sum of a vector asynchronously.  
   Answer:
   ```cpp
   #include <vector>
   #include <future>
   #include <numeric>

   int async_sum(const std::vector<int>& vec) {
       auto future = std::async(std::launch::async, [&vec] {
           return std::accumulate(vec.begin(), vec.end(), 0);
       });
       return future.get();
   }
   ```

4. Fix the data race in the following code:
   ```cpp
   int counter = 0;
   void increment() { ++counter; }
   // Called by multiple threads
   ```
   Answer:
   ```cpp
   std::atomic<int> counter(0); // Use atomic instead of raw int
   ```

5. Implement a one-shot event using `std::promise` and `std::future`.  
   Answer:
   ```cpp
   class OneShotEvent {
       std::promise<void> promise;
       std::future<void> future;
   public:
       OneShotEvent() : future(promise.get_future()) {}
       void wait() { future.wait(); }
       void signal() { promise.set_value(); }
   };
   ```

---

### Test Cases for Design Questions

Test Case for Thread-Safe Queue:
```cpp
ThreadSafeQueue<int> q;
std::thread producer([&q] { q.push(42); });
std::thread consumer([&q] { std::cout << q.pop(); });
producer.join(); consumer.join(); // Output: 42
```

Test Case for ScopedThread:
```cpp
ScopedThread t(std::thread([] { std::cout << "Scoped thread\n"; }));
// Automatically joins when t goes out of scope
```

This structured approach ensures mastery of C++ concurrency concepts, their practical use, and common pitfalls.