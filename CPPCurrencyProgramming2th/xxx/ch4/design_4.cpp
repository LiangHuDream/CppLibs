#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <memory> // 引入智能指针头文件

// 模拟数据库连接类
class DatabaseConnection {
public:
    DatabaseConnection() {
        std::cout << "Database connection created." << std::endl;
    }

    ~DatabaseConnection() {
        std::cout << "Database connection destroyed." << std::endl;
    }

    void executeQuery(const std::string& query) {
        std::cout << "Executing query: " << query << std::endl;
    }
};

// 数据库连接池类
class DatabaseConnectionPool {
private:
    std::queue<std::unique_ptr<DatabaseConnection>> connections;
    std::mutex mtx;
    std::condition_variable cv;
    size_t maxConnections;
    std::chrono::time_point<std::chrono::steady_clock> lastUsed;

    // 自动回收线程函数（此处简化处理，未使用智能回收策略）
    void autoReclaim() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::minutes(1)); // 每分钟检查一次
            {
                std::unique_lock<std::mutex> lock(mtx);
                auto now = std::chrono::steady_clock::now();
                while (!connections.empty()) {
                    auto& connection = connections.front();
                    // 假设可以通过某种方式获取连接的空闲时间（此处简化处理）
                    auto idleTime = std::chrono::duration_cast<std::chrono::minutes>(now - lastUsed);
                    if (idleTime.count() >= 2) {
                        connections.pop();
                    } else {
                        break;
                    }
                }
            }
        }
    }

public:
    DatabaseConnectionPool(size_t maxConns) : maxConnections(maxConns) {
        for (size_t i = 0; i < maxConns; ++i) {
            connections.push(std::make_unique<DatabaseConnection>());
        }
        lastUsed = std::chrono::steady_clock::now();
        std::thread(&DatabaseConnectionPool::autoReclaim, this).detach();
    }

    ~DatabaseConnectionPool() {
        // 无需手动删除连接，unique_ptr 会自动管理生命周期
    }

    // 获取数据库连接
    std::unique_ptr<DatabaseConnection> getConnection() {
        std::unique_lock<std::mutex> lock(mtx);
        if (cv.wait_for(lock, std::chrono::milliseconds(500), [this] { return!connections.empty(); })) {
            auto connection = std::move(connections.front());
            connections.pop();
            lastUsed = std::chrono::steady_clock::now();
            return connection;
        }
        return nullptr; // 超时返回 nullptr
    }

    // 释放数据库连接
    void releaseConnection(std::unique_ptr<DatabaseConnection> connection) {
        std::unique_lock<std::mutex> lock(mtx);
        connections.push(std::move(connection));
        lastUsed = std::chrono::steady_clock::now();
        cv.notify_one();
    }
};

// 测试用例
void testConnectionPool() {
    DatabaseConnectionPool pool(3);

    auto task = [&pool]() {
        auto conn = pool.getConnection();
        if (conn) {
            conn->executeQuery("SELECT * FROM users");
            pool.releaseConnection(std::move(conn));
        } else {
            std::cout << "Failed to get a connection (timeout)." << std::endl;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(task);
    }

    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    testConnectionPool();
    return 0;
}