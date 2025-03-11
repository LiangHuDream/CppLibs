## 1. 等待事件或等待其他条件

**std::condition_variable**：是 C++ 标准库提供的用于线程间同步的工具，它允许一个或多个线程等待某个条件满足后再继续执行。通常与 std::mutex 一起使用，std::mutex 用于保护共享数据，std::condition_variable 用于线程间的通知机制。
**虚假唤醒** ：指线程在没有收到明确通知的情况下从 wait 状态中被唤醒。为避免虚假唤醒，wait 函数通常会接受一个谓词（可调用对象）作为参数，线程被唤醒后会检查该谓词的返回值，如果为 false 则会继续等待。

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void worker() {
    std::unique_lock<std::mutex> lock(mtx);
    // 等待 ready 变为 true
    cv.wait(lock, [] { return ready; });
    std::cout << "Worker thread is working." << std::endl;
}

void prepare_work() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    // 通知一个等待的线程
    cv.notify_one();
    std::cout << "Work is prepared." << std::endl;
}

int main() {
    std::thread t1(worker);
    std::thread t2(prepare_work);

    t1.join();
    t2.join();

    return 0;
}
/*
1. worker 线程首先获取 std::unique_lock 锁定互斥量 mtx，然后调用 cv.wait(lock, [] { return ready; })。wait 函数会检查 ready 条件，如果不满足，它会释放锁并使线程进入等待状态。
2. prepare_work 线程获取锁，将 ready 置为 true，释放锁后调用 cv.notify_one() 通知一个等待的线程。
3. 当 worker 线程被通知后，它会重新获取锁并继续执行。
*/
```

## 2. 基于条件变量的生产者 - 消费者模型

- 核心概念
  生产者 - 消费者模型是一种常见的并发编程模式，其中生产者线程负责生成数据并将其放入共享缓冲区，消费者线程负责从缓冲区中取出数据并进行处理。使用条件变量可以实现生产者和消费者之间的同步，避免数据竞争和缓冲区溢出。
- 代码

```cpp
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

std::queue<int> data_queue;
std::mutex mtx;
std::condition_variable data_cond;

void producer() {
    for (int i = 0; i < 5; ++i) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            data_queue.push(i);
        }
        // 通知消费者有新数据
        data_cond.notify_one();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        // 等待队列中有数据
        data_cond.wait(lock, [] { return!data_queue.empty(); });
        int value = data_queue.front();
        data_queue.pop();
        lock.unlock();
        std::cout << "Consumed: " << value << std::endl;
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    // 这里可以添加适当的机制来停止消费者线程
    // 例如使用一个标志位
    return 0;
}
/*
1. producer 线程每隔 1 秒向队列中添加一个数据，并调用 data_cond.notify_one() 通知消费者线程。
2. consumer 线程在 data_cond.wait(lock, [] { return!data_queue.empty(); }) 处等待，直到队列中有数据。当有数据时，它会取出数据并处理。
*/
```

## 3. 超时等待

- 核心概念
  在某些情况下，线程不能无限期地等待某个条件满足，需要设置一个超时时间。std::condition_variable 提供了 wait_for 和 wait_until 方法来实现超时等待。
- 代码

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void worker() {
    std::unique_lock<std::mutex> lock(mtx);
    // 等待 2 秒
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    if (cv.wait_until(lock, timeout, [] { return ready; })) {
        std::cout << "Worker thread is working after being notified." << std::endl;
    } else {
        std::cout << "Worker thread timed out." << std::endl;
    }
}

void prepare_work() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one();
    std::cout << "Work is prepared." << std::endl;
}

int main() {
    std::thread t1(worker);
    std::thread t2(prepare_work);

    t1.join();
    t2.join();

    return 0;
}
/*
- worker 线程使用 cv.wait_until(lock, timeout, [] { return ready; }) 等待 ready 条件满足，最多等待 2 秒。
- prepare_work 线程在 3 秒后才将 ready 置为 true 并通知 worker 线程，因此 worker 线程会超时。
*/
```

## 4. std::future 和 std::promise

### 核心概念

**std::future**：表示一个异步操作的结果，它可以用来获取异步任务的返回值。std::future 对象可以通过 get() 方法来阻塞等待异步操作完成并获取结果。
**std::promise**：是一个可以存储值或异常的对象，它可以与 std::future 关联，用于在不同线程之间传递结果。std::promise 对象可以通过 set_value() 方法设置结果，或通过 set_exception() 方法设置异常。

### 代码

```cpp
#include <iostream>
#include <thread>
#include <future>

int calculate_sum(int a, int b) {
    return a + b;
}

int main() {
    // 创建一个 std::promise 对象
    std::promise<int> prom;
    // 获取与 promise 关联的 future
    std::future<int> fut = prom.get_future();

    std::thread t([&prom]() {
        int result = calculate_sum(3, 5);
        // 设置 promise 的值
        prom.set_value(result);
    });

    // 获取 future 的值
    int sum = fut.get();
    std::cout << "The sum is: " << sum << std::endl;

    t.join();
    return 0;
}
/*
- 创建 std::promise<int> prom 和与之关联的 std::future<int> fut。
- 启动一个新线程，在该线程中调用 calculate_sum 函数计算结果，并使用 prom.set_value(result) 设置 promise 的值。
- 在主线程中，使用 fut.get() 获取异步操作的结果。
*/
```

## 5. std::packaged_task 和 std::async

### 核心概念

- **std::packaged_task**: 包装一个可调用对象（如函数、函数对象或 lambda 表达式），并将其结果与 std::future 关联。可以通过调用 std::packaged_task 对象来执行包装的可调用对象，并通过关联的 std::future 获取结果。
- **std::async**:是一个更高级的异步操作启动方式，它可以自动管理线程和返回结果。std::async 接受一个可调用对象和其参数，返回一个 std::future 对象，用于获取异步操作的结果。
- 示例代码:

```cpp
#include <iostream>
#include <thread>
#include <future>

int add(int a, int b) {
    return a + b;
}

int main() {
    // 创建一个 packaged_task 对象
    std::packaged_task<int(int, int)> task(add);
    // 获取与 packaged_task 关联的 future
    std::future<int> fut = task.get_future();

    std::thread t(std::move(task), 3, 5);

    // 获取 future 的值
    int result = fut.get();
    std::cout << "The result is: " << result << std::endl;

    t.join();
    return 0;
}
/*
- 创建 std::packaged_task<int(int, int)> task(add) 包装 add 函数，并获取与之关联的 std::future<int> fut。
- 启动一个新线程，将 task 移动到线程中执行，并传递参数 3 和 5。
- 在主线程中，使用 fut.get() 获取异步操作的结果。
*/
```

## 6. 多选题目

1. 关于 std::condition_variable，以下说法正确的是（ ）  
   A. std::condition_variable 必须与 std::mutex 一起使用  
   B. 调用 wait 方法时，如果不使用谓词，可能会发生虚假唤醒  
   C. notify_one 方法会唤醒所有等待的线程  
   D. wait_for 和 wait_until 方法可以实现超时等待  

2. 在生产者 - 消费者模型中，以下哪些做法可以提高性能和安全性（ ）  
   A. 生产者和消费者使用同一个互斥量保护共享队列  
   B. 生产者在添加数据后及时通知消费者  
   C. 消费者在处理数据时长时间持有锁  
   D. 使用谓词来避免消费者的虚假唤醒  
   
3. 关于 std::future 和 std::promise，以下说法正确的是（ ）  
   A. std::future 可以用来获取异步操作的结果  
   B. std::promise 可以存储值或异常  
   C. 一个 std::promise 只能与一个 std::future 关联  
   D. std::future 的 get 方法可以多次调用  

4. 以下哪些情况可能导致线程的虚假唤醒（ ）  
   A. 操作系统的调度策略  
   B. 硬件中断  
   C. 信号处理  
   D. 调用 notify_one 或 notify_all 方法  

5. 在使用 std::condition_variable 时，以下做法正确的是（ ）  
   A. 在调用 wait 方法前先锁定互斥量  
   B. 在调用 notify_one 或 notify_all 方法时持有互斥量  
   C. 使用谓词来避免虚假唤醒  
   D. 在 wait 方法返回后再次检查条件  
## 6. 多选题目答案
1. 答案：ABD
   解释：std::condition_variable 依赖 std::mutex 来保护共享数据，所以必须与 std::mutex 一起使用，A 正确；不使用谓词时，wait 方法可能会因操作系统或硬件的原因发生虚假唤醒，B 正确；notify_one 方法只会唤醒一个等待的线程，notify_all 才会唤醒所有等待的线程，C 错误；wait_for 和 wait_until 方法可以让线程在等待一定时间后自动返回，实现超时等待，D 正确。
2. 答案：ABD
   解释：生产者和消费者使用同一个互斥量保护共享队列可以避免数据竞争，保证线程安全，A 正确；生产者在添加数据后及时通知消费者可以减少消费者的等待时间，提高性能，B 正确；消费者在处理数据时长时间持有锁会导致其他线程无法访问共享队列，降低并发性能，C 错误；使用谓词来避免消费者的虚假唤醒可以确保消费者只在队列中有数据时才进行处理，提高程序的正确性和性能，D 正确。
3. 答案：ABC
   解释：std::future 主要用于获取异步操作的结果，A 正确；std::promise 可以存储一个值或异常，并通过关联的 std::future 传递给其他线程，B 正确；一个 std::promise 只能与一个 std::future 关联，C 正确；std::future 的 get 方法只能调用一次，多次调用会导致未定义行为，D 错误。
4. 答案：ABC
   解释：操作系统的调度策略、硬件中断和信号处理等因素都可能导致线程的虚假唤醒，A、B、C 正确；调用 notify_one 或 notify_all 方法是正常的通知机制，不会导致虚假唤醒，D 错误。
5. 答案：ACD
    解释：在调用 wait 方法前先锁定互斥量是必要的，因为 wait 方法会在等待时自动释放锁，在被唤醒后重新获取锁，A 正确；调用 notify_one 或 notify_all 方法时不需要持有互斥量，B 错误；使用谓词可以避免虚假唤醒，确保线程只在条件满足时才继续执行，C 正确；在 wait 方法返回后再次检查条件可以进一步确保程序的正确性，D 正确。

## 7. 设计题目

### 7.1 设计一个带有优先级和超时机制的多生产者多消费者任务队列

要求：

- 任务队列中的任务具有不同的优先级，高优先级的任务优先被处理。
- 生产者可以向队列中添加任务，消费者从队列中取出任务进行处理。
- 消费者在取出任务时，如果队列中没有任务，最多等待指定的超时时间。
- 支持多线程并发操作，确保线程安全。

### 7.2 实现一个异步文件读写任务调度器

要求：

- 可以提交文件读取和写入任务，任务调度器会异步执行这些任务。
- 每个任务可以设置回调函数，当任务完成时调用回调函数。
- 支持任务的优先级调度，高优先级的任务优先执行。
  处理文件读写过程中可能出现的异常，如文件不存在、权限不足等。

### 7.3 设计一个分布式缓存系统的本地缓存模块

要求：

- 本地缓存模块负责缓存数据，减少对远程缓存或数据库的访问。
- 支持缓存数据的过期策略，当数据过期时自动从缓存中移除。
- 当本地缓存中没有所需数据时，从远程缓存或数据库中获取数据并更新本地缓存。
- 支持多线程并发访问本地缓存，确保线程安全。
- 提供缓存数据的统计信息，如缓存命中率、缓存大小等。

### 7.4 实现一个基于事件驱动的异步网络服务器

要求：

- 服务器可以监听多个端口，处理客户端的连接请求。
- 采用事件驱动的方式处理网络事件，如连接建立、数据接收、数据发送等。
- 支持异步 I/O 操作，提高服务器的并发性能。
- 可以动态添加和删除监听端口。
- 处理网络异常，如连接断开、超时等。

### 7.5 设计一个线程池，支持任务的分组和优先级调度

要求：

- 线程池可以管理多个线程，执行提交的任务。
- 任务可以分组，不同组的任务可以有不同的执行策略，如并行执行、顺序执行等。
- 任务具有不同的优先级，高优先级的任务优先执行。
- 支持动态调整线程池的大小。
- 提供任务执行状态的查询接口，如任务是否完成、是否出错等。

### 7.1 答案

### 7.2 答案

### 7.3 答案

### 7.4 答案

### 7.5 答案
