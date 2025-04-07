# Chapter 12: Concurrent Programming - Key Concepts and Code Walkthrough

## 1. Processes vs. Threads vs. I/O Multiplexing
Key Concepts:
- **Processes**: Heavyweight, isolated memory spaces
- **Threads**: Lightweight, shared memory within same process
- **I/O Multiplexing**: Single process handling multiple I/O channels (`select`/`poll`)

Code Example: Thread Creation
```c
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2

void* thread_func(void* arg) {
    int tid = *(int*)arg;
    printf("Thread %d created\n", tid);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int tids[NUM_THREADS] = {1, 2};
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, &tids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("All threads completed\n");
    return 0;
}
```
Compile & Test:
```bash
gcc -pthread thread_create.c -o thread_create && ./thread_create
```

---

## 2. Shared Variables & Race Conditions
Key Concepts:
- **Shared Variables**: Global variables accessible by multiple threads
- **Race Condition**: Undefined behavior from unsynchronized access

Code Example: Unsafe Counter
```c
#include <stdio.h>
#include <pthread.h>

#define NUM_OPS 100000
int counter = 0;

void* increment(void* arg) {
    for (int i = 0; i < NUM_OPS; i++) {
        counter++;
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Expected: %d\nActual: %d\n", 2*NUM_OPS, counter);
    return 0;
}
```
Compile & Test:
```bash
gcc -pthread unsafe_counter.c -o unsafe_counter && ./unsafe_counter
```
**Output will show incorrect value due to race condition**

---

## 3. Semaphores for Synchronization
Key Concepts:
- **Binary Semaphores**: Mutex locks for mutual exclusion
- **Counting Semaphores**: Resource management

Code Example: Safe Counter with Mutex
```c
#include <stdio.h>
#include <pthread.h>

#define NUM_OPS 100000
int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* increment(void* arg) {
    for (int i = 0; i < NUM_OPS; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Expected: %d\nActual: %d\n", 2*NUM_OPS, counter);
    return 0;
}
```
Compile & Test:
```bash
gcc -pthread safe_counter.c -o safe_counter && ./safe_counter
```

---

## 4. Producer-Consumer Problem
Key Concepts:
- **Bounded Buffer**: Shared buffer with limited capacity
- **Semaphore Trio**: mutex + empty + full

Code Example: Bounded Buffer
```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty, full;
pthread_mutex_t mutex;

void* producer(void* arg) {
    for (int i = 0; i < 10; i++) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        
        buffer[in] = i;
        printf("Produced %d\n", i);
        in = (in + 1) % BUFFER_SIZE;
        
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < 10; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        
        int item = buffer[out];
        printf("Consumed %d\n", item);
        out = (out + 1) % BUFFER_SIZE;
        
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    return NULL;
}

int main() {
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);
    
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    return 0;
}
```
Compile & Test:
```bash
gcc -pthread prod_cons.c -o prod_cons && ./prod_cons
```

---

## 5. Deadlock Conditions
Key Concepts (4 Requirements):
1. Mutual Exclusion
2. Hold and Wait
3. No Preemption
4. Circular Wait

Code Example: Deadlock Scenario
```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1(void* arg) {
    pthread_mutex_lock(&lock1);
    sleep(1);  // Ensure deadlock
    pthread_mutex_lock(&lock2);
    printf("Thread 1\n");
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    return NULL;
}

void* thread2(void* arg) {
    pthread_mutex_lock(&lock2);
    sleep(1);
    pthread_mutex_lock(&lock1);
    printf("Thread 2\n");
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
```
Compile & Test:
```bash
gcc -pthread deadlock.c -o deadlock && ./deadlock
```
**Program will hang indefinitely**

---

## 6. Thread Safety & Reentrancy
Key Concepts:
- **Thread-Safe**: Functions safe for concurrent access
- **Reentrant**: No static/global data, no non-local returns

Code Example: Unsafe vs Safe
```c
// UNSAFE VERSION
char* strtok_unsafe(char* str) {
    static char* saved;  // Shared state
    /* ... */
}

// SAFE VERSION (POSIX)
char* strtok_r(char* str, const char* delim, char** saveptr) {
    /* Uses caller-provided saveptr */
}
```

---

## 7. Prethreaded Server Pattern
Key Concepts:
- Worker thread pool
- Producer-consumer with requests

Code Skeleton:
```c
#define THREAD_POOL_SIZE 10
#define MAX_REQUESTS 100

typedef struct {
    int connfd;
    // Other request data
} request_t;

request_t req_buffer[MAX_REQUESTS];
int buf_cnt = 0;
pthread_mutex_t buf_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t slots, items;

// Worker threads
void* worker(void* arg) {
    while(1) {
        sem_wait(&items);
        pthread_mutex_lock(&buf_lock);
        
        request_t req = req_buffer[--buf_cnt];
        
        pthread_mutex_unlock(&buf_lock);
        sem_post(&slots);
        
        process_request(req.connfd);
    }
}

// Main thread
int main() {
    // Initialize semaphores
    // Create thread pool
    // Accept connections and add to buffer
}
```

---

## Testing Methodology
1. **Stress Testing**: Run with high NUM_OPS (100000+)
2. **Valgrind Checks**:
```bash
valgrind --tool=helgrind ./safe_counter
```
3. **Deadlock Detection**: Use `-pthread` and proper locking order
4. **Performance Metrics**: Compare threaded vs process-based

## Common Pitfalls
1. **Forgotten Unlock**: Always pair lock/unlock
2. **Priority Inversion**: Use priority inheritance mutexes
3. **Spurious Wakeups**: Use while loops with condition variables
4. **Resource Leaks**: Destroy mutexes/semaphores

### Multiple-Choice Questions

---

**Question 1: (Thread Safety & Reentrancy)**  
Which of the following are true about thread-safe functions?  
A. All reentrant functions are thread-safe.  
B. Using global variables without synchronization guarantees thread safety.  
C. A function that relies only on local variables and parameters is always thread-safe.  
D. Thread-safe functions can still produce race conditions if not properly synchronized.  

---

**Question 2: (Semaphores & Synchronization)**  
Which scenarios correctly use semaphores to enforce mutual exclusion?  
A. Initialize a semaphore to 0 and call `P()` before the critical section.  
B. Initialize a semaphore to 1 and call `P()` before the critical section, `V()` after.  
C. Use two semaphores to implement a barrier for thread synchronization.  
D. Use a semaphore initialized to `N` to allow up to `N` threads into the critical section.  

---

**Question 3: (Deadlock Conditions)**  
Which are necessary conditions for deadlock to occur?  
A. Mutual exclusion.  
B. No preemption of resources.  
C. Circular wait.  
D. Starvation of low-priority threads.  

---

**Question 4: (Race Conditions)**  
Which code snippets contain a potential race condition?  
A. `if (counter > 0) { counter--; }`  
B. `counter += 1;` (without locks)  
C. `int local = counter; local++; counter = local;` (without locks)  
D. `pthread_mutex_lock(&m); counter++; pthread_mutex_unlock(&m);`  

---

**Question 5: (Concurrency Models)**  
Which statements about threading vs. processes are true?  
A. Threads share the same virtual memory space; processes do not.  
B. Processes are more lightweight than threads.  
C. Context switching between threads is faster than between processes.  
D. Processes require explicit shared memory mechanisms (e.g., pipes) for communication.  

---

**Question 6: (I/O Multiplexing)**  
Which are limitations of I/O multiplexing with `select()`?  
A. It cannot handle file descriptor numbers larger than 1024.  
B. It scales poorly with a large number of file descriptors.  
C. It supports asynchronous I/O notifications.  
D. It requires separate event loops for read and write operations.  

---

**Question 7: (Signal Handling in Threads)**  
Which statements about signals in threaded programs are correct?  
A. A signal sent to a process is delivered to a single arbitrary thread.  
B. Each thread has its own signal mask.  
C. Signal handlers are shared among all threads in a process.  
D. `pthread_kill()` sends a signal to a specific thread.  

---

**Question 8: (Thread-Local Storage)**  
Which mechanisms implement thread-local storage in C?  
A. The `static` keyword.  
B. The `__thread` qualifier (GCC).  
C. Dynamically allocated memory with `malloc()`.  
D. Using `pthread_setspecific()` and `pthread_getspecific()`.  

---

**Question 9: (Atomic Operations)**  
Which operations are atomic on x86-64 for aligned `int` variables?  
A. `x++` (without locks).  
B. `x = 42;` (assignment).  
C. `x += y;` (where `y` is a local variable).  
D. Reading a 64-bit integer on a 64-bit architecture.  

---

**Question 10: (Memory Ordering)**  
Which statements about memory reordering are true?  
A. Compilers may reorder instructions to optimize performance.  
B. CPUs always execute instructions in program order.  
C. Memory barriers enforce ordering of loads and stores.  
D. `volatile` variables prevent all compiler and hardware reordering.  

---

**Question 11: (Producer-Consumer Problem)**  
Which are valid solutions for the producer-consumer problem?  
A. Two semaphores (empty/full slots) and a mutex.  
B. A single counting semaphore.  
C. A condition variable paired with a mutex.  
D. A spinlock for buffer access.  

---

**Question 12: (Thread Pools)**  
Which are advantages of thread pools?  
A. Reducing thread creation/destruction overhead.  
B. Limiting resource usage by capping active threads.  
C. Automatically scaling to utilize all CPU cores.  
D. Guaranteeing FIFO execution order for tasks.  

---

**Question 13: (Lock-Free Programming)**  
Which are challenges of lock-free programming?  
A. ABA problems.  
B. Ensuring progress guarantees (e.g., wait-free vs. lock-free).  
C. Simplicity of debugging and testing.  
D. Higher memory overhead compared to mutexes.  

---

**Question 14: (Futures & Promises)**  
Which describe futures/promises correctly?  
A. Futures represent asynchronous computation results.  
B. Promises are used to set the value of a future.  
C. Futures block indefinitely until a result is available.  
D. They require explicit thread management by the programmer.  

---

**Question 15: (Debugging Concurrency Bugs)**  
Which tools/techniques help detect concurrency bugs?  
A. Valgrind’s Helgrind tool.  
B. Print statements with timestamps.  
C. Static analysis for race conditions.  
D. Using `gdb` to inspect thread states.  

---

### Answers & Explanations

1. **A, D**  
   - Reentrant functions (A) are thread-safe because they avoid static data. Thread-safe functions (D) may still have race conditions if synchronization is missing.  
   - (B) is false: global variables need synchronization. (C) is false if local variables point to shared data.  

2. **B, C**  
   - (B) is the standard mutex pattern. (C) uses semaphores for barriers.  
   - (A) deadlocks (semaphore starts at 0). (D) allows multiple threads, not mutual exclusion.  

3. **A, B, C**  
   - Deadlock requires all four conditions: Mutual exclusion, hold-and-wait, no preemption, and circular wait. (D) relates to scheduling, not deadlock.  

4. **A, B, C**  
   - (A) and (B) lack atomicity. (C) has a read-modify-write race. (D) is correctly synchronized.  

5. **A, C, D**  
   - Threads share memory (A), faster context switches (C). Processes need IPC (D). (B) is false: processes are heavier.  

6. **A, B**  
   - `select()` has FD limits (A) and poor scalability (B). (C) is false (synchronous). (D) is false: `select()` handles multiple events.  

7. **B, C, D**  
   - Threads have individual masks (B) but shared handlers (C). (D) is true. (A) is OS-dependent.  

8. **B, D**  
   - (B) GCC’s TLS; (D) POSIX-specific. (A) and (C) are not thread-local.  

9. **B, D**  
   - Aligned 64-bit reads/writes are atomic (B, D). (A) and (C) require atomic instructions.  

10. **A, C**  
   - Compilers (A) and CPUs reorder; barriers enforce order (C). (B) is false. `volatile` (D) doesn’t prevent hardware reordering.  

11. **A, C**  
   - (A) uses semaphores + mutex; (C) uses condition variables. (B) lacks buffer management. (D) causes busy-waiting.  

12. **A, B**  
   - Pools reduce overhead (A) and limit threads (B). (C) depends on pool size. (D) is not guaranteed.  

13. **A, B**  
   - ABA problems (A) and progress guarantees (B) are challenges. (C) is false; (D) is false (lock-free uses less memory).  

14. **A, B**  
   - Futures (A) and promises (B) are correct. (C) may block with timeouts. (D) is false (managed by the runtime).  

15. **A, C**  
   - Helgrind (A) and static analysis (C) detect races. (B) and (D) are manual and less reliable.

**After you are done, it is highly recommended to do the exercises from the original book**