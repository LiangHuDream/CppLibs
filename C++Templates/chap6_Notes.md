### Step-by-Step Guide to Mastering Chapter 6: Concurrency and Multithreading in C++

---

### 1. Key Concepts & Code Explanations

#### 1.1 Thread Management  
What: Threads enable concurrent execution. Use `std::thread` to create and manage threads.  
Pitfalls: Data races, undefined behavior.  

Example: Basic thread creation and joining.  
```cpp
#include <iostream>
#include <thread>

void print_hello() {
    std::cout << "Hello from thread!\n";
}

int main() {
    std::thread t(print_hello); // Create thread
    t.join(); // Wait for thread to finish
    std::cout << "Main thread exits.\n";
    // Output (order may vary):
    // Hello from thread!
    // Main thread exits.
}
```

---

#### 1.2 Mutexes and Locks  
What: Prevent data races by synchronizing access to shared data.  
Key Classes: `std::mutex`, `std::lock_guard`, `std::unique_lock`.  

Example: Safe counter with `std::mutex`.  
```cpp
#include <mutex>
#include <thread>

struct Counter {
    int value = 0;
    std::mutex mtx;

    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        ++value;
    }
};

int main() {
    Counter cnt;
    std::thread t1([&]() { cnt.increment(); });
    std::thread t2([&]() { cnt.increment(); });
    t1.join(); t2.join();
    std::cout << cnt.value; // Output: 2
}
```

---

#### 1.3 Condition Variables  
What: Synchronize threads based on conditions (e.g., producer-consumer).  
Key Class: `std::condition_variable`.  

Example: Producer-consumer with a bounded buffer.  
```cpp
#include <queue>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> buffer;
const int MAX_SIZE = 10;

void producer() {
    for (int i = 0; i < 20; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return buffer.size() < MAX_SIZE; });
        buffer.push(i);
        lock.unlock();
        cv.notify_one();
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty(); });
        int val = buffer.front();
        buffer.pop();
        lock.unlock();
        cv.notify_one();
        if (val == 19) break;
    }
}

int main() {
    std::thread p(producer), c(consumer);
    p.join(); c.join();
}
```

---

#### 1.4 Futures and Promises  
What: Asynchronous operations with `std::future` and `std::promise`.  

Example: Calculate sum asynchronously.  
```cpp
#include <future>

int sum(int a, int b) {
    return a + b;
}

int main() {
    std::future<int> result = std::async(std::launch::async, sum, 10, 20);
    std::cout << result.get(); // Output: 30
}
```

---

#### 1.5 Atomic Operations  
What: Lock-free thread-safe operations using `std::atomic<T>`.  

Example: Atomic counter.  
```cpp
#include <atomic>
#include <thread>

std::atomic<int> counter(0);

void increment() {
    for (int i = 0; i < 1000; ++i) {
        ++counter;
    }
}

int main() {
    std::thread t1(increment), t2(increment);
    t1.join(); t2.join();
    std::cout << counter; // Output: 2000
}
```

---

### 2. Multiple-Choice Questions (10)

#### Questions 1-5  
Q1: What happens if a thread is destroyed without `join()` or `detach()`?  
A) Compile error  
B) Runtime error (`std::terminate`)  
C) Undefined behavior  
D) The thread continues running  

Q2: Which lock automatically releases the mutex when it goes out of scope?  
A) `std::mutex`  
B) `std::lock_guard`  
C) `std::unique_lock`  
D) Both B and C  

Q3: What is the output?  
```cpp
std::atomic<int> x(0);
x.fetch_add(3, std::memory_order_relaxed);
std::cout << x.load();  
```  
A) 0  
B) 3  
C) Undefined  
D) Compile error  

Q4: Which function is used to retrieve the result of a `std::async` task?  
A) `wait()`  
B) `get()`  
C) `fetch()`  
D) `result()`  

Q5: What is a deadlock?  
A) A race condition  
B) Two threads waiting for each other indefinitely  
C) A memory leak  
D) An exception  

---

#### Questions 6-10  
Q6: What does `std::condition_variable::notify_all()` do?  
A) Wakes all waiting threads  
B) Wakes one thread  
C) Locks the mutex  
D) Unlocks the mutex  

Q7: Which memory order guarantees sequential consistency?  
A) `std::memory_order_relaxed`  
B) `std::memory_order_consume`  
C) `std::memory_order_seq_cst`  
D) `std::memory_order_acquire`  

Q8: What is the output?  
```cpp
std::promise<int> p;
auto f = p.get_future();
p.set_value(42);
std::cout << f.get();  
```  
A) 0  
B) 42  
C) Undefined  
D) Compile error  

Q9: Which code causes a data race?  
A)  
```cpp  
std::mutex mtx;  
int x = 0;  
void inc() { std::lock_guard<std::mutex> lock(mtx); ++x; }  
```  
B)  
```cpp  
int x = 0;  
void inc() { ++x; }  
```  
C)  
```cpp  
std::atomic<int> x(0);  
void inc() { ++x; }  
```  
D) None  

Q10: What does `std::async(std::launch::deferred, func)` do?  
A) Runs `func` asynchronously  
B) Runs `func` on the main thread  
C) Runs `func` lazily on the calling thread  
D) Compile error  

---

### Answers & Explanations  
1. B (Runtime error: Thread destructor calls `std::terminate`).  
2. D (`lock_guard` and `unique_lock` are RAII wrappers).  
3. B (Atomic fetch_add is thread-safe).  
4. B (`get()` blocks until the result is ready).  
5. B (Deadlock: mutual waiting).  
6. A (Wakes all threads waiting on the condition variable).  
7. C (`seq_cst` enforces global order).  
8. B (Promise sets 42, future retrieves it).  
9. B (No synchronization for `++x`).  
10. C (Deferred execution runs on the calling thread when `get()` is called).  

---

### 3. Design Questions (5)

#### Question 1: Thread-Safe Queue  
Task: Implement a thread-safe queue using `std::mutex` and `std::condition_variable`.  
```cpp
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;
public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(item);
        cv.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty(); });
        T item = queue.front();
        queue.pop();
        return item;
    }
};

int main() {
    ThreadSafeQueue<int> q;
    q.push(42);
    std::cout << q.pop(); // Output: 42
}
```

---

#### Question 2: Parallel Sum with `std::async`  
Task: Compute the sum of a vector using multiple threads.  
```cpp
#include <vector>
#include <future>

int parallel_sum(const std::vector<int>& vec, int start, int end) {
    int sum = 0;
    for (int i = start; i < end; ++i) {
        sum += vec[i];
    }
    return sum;
}

int main() {
    std::vector<int> vec(1000, 1); // 1000 elements of 1
    auto f1 = std::async(std::launch::async, parallel_sum, std::ref(vec), 0, 500);
    auto f2 = std::async(std::launch::async, parallel_sum, std::ref(vec), 500, 1000);
    std::cout << f1.get() + f2.get(); // Output: 1000
}
```

---

#### Question 3: Singleton with Thread Safety  
Task: Implement a thread-safe singleton using `std::call_once`.  
```cpp
#include <mutex>

class Singleton {
    static Singleton* instance;
    static std::once_flag flag;
    Singleton() {} // Private constructor
public:
    static Singleton* get() {
        std::call_once(flag, []() { instance = new Singleton(); });
        return instance;
    }
};

Singleton* Singleton::instance = nullptr;
std::once_flag Singleton::flag;

int main() {
    Singleton* s1 = Singleton::get();
    Singleton* s2 = Singleton::get();
    std::cout << (s1 == s2); // Output: 1 (same instance)
}
```

---

#### Question 4: Producer-Consumer with Bounded Buffer  
Task: Implement a producer-consumer pattern with a fixed-size buffer.  
```cpp
// (Reuse the code from the "Condition Variables" example in Section 1.3)
// Test case in main() runs producer and consumer threads.
```

---

#### Question 5: Atomic Spinlock  
Task: Implement a spinlock using `std::atomic_flag`.  
```cpp
#include <atomic>

class Spinlock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {}
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

int main() {
    Spinlock lock;
    lock.lock();
    // Critical section
    lock.unlock();
}
```

---

### Summary  
This guide covers thread management, synchronization (mutexes, condition variables), asynchronous operations (futures/promises), and atomic operations. Test cases validate thread safety, deadlock avoidance, and correct concurrency patterns. The exercises reinforce practical applications like thread-safe queues, parallel algorithms, and singleton initialization.