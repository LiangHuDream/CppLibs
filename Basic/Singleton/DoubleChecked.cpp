#include <iostream>
#include <mutex>

class Singleton {
public:
    static Singleton* getInstance() {
        if (!instance) { // 第一次检查
            std::lock_guard<std::mutex> lock(mutex);
            if (!instance) { // 第二次检查（避免其他线程已创建）
                instance = new Singleton();
            }
        }
        return instance;
    }
private:
    Singleton() {}
    static Singleton* instance;
    static std::mutex mutex;
};
Singleton* Singleton::instance = nullptr;
std::mutex Singleton::mutex;
