#include <iostream>
#include <future>
#include <atomic>
#include <chrono>
#include <functional>
#include <stdexcept>

// 可取消任务管理类
template <typename Result>
class CancellableTaskManager {
public:
    // 使用工厂方法创建任务
    template <typename Func>
    static CancellableTaskManager createTask(Func func) {
        auto cancelFlag = std::make_shared<std::atomic<bool>>(false);
        auto future = std::async(std::launch::async, [func, cancelFlag]() -> Result { // 这里是怎么传入cancelFlag
            return func(cancelFlag); // 将取消标志传递给任务函数
        });
        return CancellableTaskManager(std::move(future), cancelFlag);
    }

    // 获取任务的 future 对象（移除const，使用移动语义）
    std::future<Result> getFuture() {
        return std::move(future_);
    }

    // 取消任务
    void cancel() {
        cancelFlag_->store(true);
    }

private:
    // 私有构造函数，仅通过工厂方法创建实例
    CancellableTaskManager(std::future<Result> future, std::shared_ptr<std::atomic<bool>> cancelFlag)
        : future_(std::move(future)), cancelFlag_(std::move(cancelFlag)) {}

    std::future<Result> future_;
    std::shared_ptr<std::atomic<bool>> cancelFlag_;
};

// 示例任务函数，现在接受取消标志参数
int exampleTask(std::shared_ptr<std::atomic<bool>> cancelFlag) {
    for (int i = 0; i < 5; ++i) {
        // 检查是否已取消
        if (cancelFlag->load()) {
            throw std::runtime_error("Task cancelled during execution");
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Task is working: " << i << std::endl;
    }
    return 42;
}

int main() {
    // 创建可取消的任务，使用lambda适配函数签名
    auto taskManager = CancellableTaskManager<int>::createTask(
        [](std::shared_ptr<std::atomic<bool>> flag) { return exampleTask(flag); }
    );
    // 获取任务的 future 对象
    auto future = taskManager.getFuture();

    // 模拟一段时间后取消任务
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Cancelling the task..." << std::endl;
    taskManager.cancel();

    try {
        // 尝试获取任务的结果
        int result = future.get();
        std::cout << "Task result: " << result << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}