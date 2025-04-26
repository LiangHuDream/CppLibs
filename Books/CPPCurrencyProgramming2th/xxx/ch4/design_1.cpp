#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <sstream>
#include <stdexcept>

// 字符串转换工具
namespace StringConverter {

    // 检测是否存在 toString 成员函数
    template <typename T, typename = void>
    struct has_to_string : std::false_type {};

    template <typename T>
    struct has_to_string<T, std::void_t<decltype(std::declval<T>().toString())>>
        : std::true_type {};

    // 检测是否支持 operator<<
    template <typename T, typename = void>
    struct has_output_operator : std::false_type {};

    template <typename T>
    struct has_output_operator<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
        : std::true_type {};

    // 主转换函数
    template <typename T>
    std::string toString(const T& value) {
        if constexpr (has_to_string<T>::value) {
            return value.toString();
        } else if constexpr (has_output_operator<T>::value || std::is_arithmetic_v<T>) {
            std::stringstream ss;
            ss << value;
            return ss.str();
        } else {
            throw std::runtime_error("Type not convertible to string");
        }
    }

    // 特化处理 C 字符串
    template <>
    std::string toString<const char*>(const char* const& value) {
        return value;
    }

} // namespace StringConverter

// 线程安全的缓存类
template<typename Key, typename Value>
class SafeCache {
    std::shared_mutex mtx;
    std::unordered_map<Key, Value> cache;
    std::unordered_map<Key, std::shared_future<Value>> pending;

    Value compute(const Key& key) {
        // 模拟耗时计算
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return StringConverter::toString(key) + "_value";
    }

public:
    Value get(const Key& key) {
        std::shared_lock lock(mtx);
        if (auto it = cache.find(key); it != cache.end()) {
            return it->second;
        }

        lock.unlock();
        std::unique_lock uniqueLock(mtx);

        if (auto it = pending.find(key); it != pending.end()) {
            auto fut = it->second;
            uniqueLock.unlock();
            return fut.get();
        }

        auto promise = std::make_shared<std::promise<Value>>();
        auto fut = promise->get_future().share();
        pending.emplace(key, fut);

        uniqueLock.unlock();

        try {
            auto res = compute(key);
            std::unique_lock finalLock(mtx);
            cache.emplace(key, res);
            pending.erase(key);
            promise->set_value(res);
        } catch (...) {
            std::unique_lock finalLock(mtx);
            pending.erase(key);
            promise->set_exception(std::current_exception());
        }

        return fut.get();
    }
};

// 测试用例
void test_cache() {
    SafeCache<std::string, std::string> cache;
    auto task = [&](std::string key) {
        return cache.get(key);
    };

    auto fut1 = std::async(std::launch::async, task, "A");
    auto fut2 = std::async(std::launch::async, task, "A");

    std::cout << fut1.get() << std::endl; // A_value
    std::cout << fut2.get() << std::endl; // A_value (仅一次计算)
}

void test_cache_2() {
    SafeCache<int, std::string> cache;
    auto task = [&](int key) {
        return cache.get(key);
    };

    auto fut1 = std::async(std::launch::async, task, 1);
    auto fut2 = std::async(std::launch::async, task, 2);

    std::cout << fut1.get() << std::endl; // 1_value
    std::cout << fut2.get() << std::endl; // 2_value
}

int main() {
    test_cache();
    test_cache_2();
    return 0;
}