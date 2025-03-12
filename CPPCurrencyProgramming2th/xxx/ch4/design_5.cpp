#include <iostream>
#include <fstream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <chrono>
#include <atomic>
#include <vector>

// 定义日志级别
enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class AsyncLogger {
private:
    std::queue<std::string> logQueue;
    std::mutex mtx;
    std::condition_variable cv;
    std::ofstream logFile;
    std::thread loggerThread;
    std::atomic<bool> stopLogging;
    std::atomic<bool> isSyncMode;
    static constexpr size_t MAX_QUEUE_SIZE = 1000;

    std::unique_ptr<std::ofstream> logFile; // 使用智能指针管理文件流
    LogLevel minLogLevel; // 新增成员变量，用于日志级别过滤

    // 日志线程函数
    void logger() {
        std::vector<std::string> buffer;
        while (!stopLogging) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait_for(lock, std::chrono::milliseconds(100), [this] { return!logQueue.empty() || stopLogging; });
                while (!logQueue.empty()) {
                    buffer.push_back(std::move(logQueue.front()));
                    logQueue.pop();
                }
            }
            for (const auto& msg : buffer) {
                try {
                    logFile << msg << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Error writing log: " << e.what() << std::endl;
                }
            }
            buffer.clear();
        }
        // 处理剩余的日志
        {
            std::unique_lock<std::mutex> lock(mtx);
            while (!logQueue.empty()) {
                try {
                    logFile << logQueue.front() << std::endl;
                    logQueue.pop();
                } catch (const std::exception& e) {
                    std::cerr << "Error writing remaining log: " << e.what() << std::endl;
                }
            }
        }
        try {
            logFile.flush();
        } catch (const std::exception& e) {
            std::cerr << "Error flushing log file: " << e.what() << std::endl;
        }
    }

    // 格式化日志消息，添加日志级别和时间戳
    std::string formatLogMessage(LogLevel level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::string timeStr = std::ctime(&currentTime);
        timeStr.pop_back(); // 去掉换行符
        std::string levelStr;
        switch (level) {
            case LogLevel::DEBUG:
                levelStr = "DEBUG";
                break;
            case LogLevel::INFO:
                levelStr = "INFO";
                break;
            case LogLevel::WARN:
                levelStr = "WARN";
                break;
            case LogLevel::ERROR:
                levelStr = "ERROR";
                break;
        }
        return "[" + timeStr + "] [" + levelStr + "] " + message;
    }

public:
    AsyncLogger(const std::string& logFileName) : stopLogging(false), isSyncMode(false) {
        try {
            logFile.open(logFileName, std::ios::app);
            if (!logFile.is_open()) {
                std::cerr << "Failed to open log file: " << logFileName << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error opening log file: " << e.what() << std::endl;
        }
        loggerThread = std::thread(&AsyncLogger::logger, this);
    }

    ~AsyncLogger() {
        stopLogging = true;
        cv.notify_one();
        if (loggerThread.joinable()) {
            loggerThread.join();
        }
        try {
            logFile.close();
        } catch (const std::exception& e) {
            std::cerr << "Error closing log file: " << e.what() << std::endl;
        }
    }

    void setMinLogLevel(LogLevel level) {
        minLogLevel = level;
    }

    // 写入日志
    void log(LogLevel level, const std::string& message) {
        if (level < minLogLevel) {
            return; // 日志级别低于最小日志级别，直接返回
        }

        std::string formattedMessage = formatLogMessage(level, message);
        {
            std::unique_lock<std::mutex> lock(mtx);
            if (logQueue.size() >= MAX_QUEUE_SIZE) {
                isSyncMode = true;
            } else {
                isSyncMode = false;
            }
        }

        if (isSyncMode) {
            // 同步模式，直接写入日志
            std::lock_guard<std::mutex> lock(mtx);
            try {
                logFile << formattedMessage << std::endl;
                logFile.flush();
            } catch (const std::exception& e) {
                std::cerr << "Error writing log in sync mode: " << e.what() << std::endl;
            }
        } else {
            // 异步模式，将日志消息放入队列
            {
                std::lock_guard<std::mutex> lock(mtx);
                logQueue.push(formattedMessage);
            }
            cv.notify_one();
        }
    }

    // 刷盘操作
    void flush() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return logQueue.empty(); });
        try {
            if (logFile.is_open()) {
                logFile.flush();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error flushing log during flush operation: " << e.what() << std::endl;
        }
    }
};

// 测试用例
void testAsyncLogger() {
    AsyncLogger logger("test.log");

    // 模拟大量日志写入
    for (int i = 0; i < 2000; ++i) {
        logger.log(LogLevel::INFO, "Log message " + std::to_string(i));
        std::cout << "Log message " << i << std::endl;  // 输出到控制台
    }

    // 刷盘操作
    logger.flush();
}

int main() {
    testAsyncLogger();
    return 0;
}