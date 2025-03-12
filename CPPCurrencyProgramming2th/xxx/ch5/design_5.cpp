#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <functional>
#include <stdexcept>
#include <memory>

// 链表节点结构体
template<typename K, typename V>
struct Node {
    K key;
    V value;
    std::atomic<Node*> next;

    Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
};

// 线程安全的哈希表类
template<typename K, typename V>
class ThreadSafeHashTable {
private:
    // 桶结构体，包含链表头指针
    struct Bucket {
        std::atomic<Node<K, V>*> head;

        Bucket() : head(nullptr) {}
        ~Bucket() {
            Node<K, V>* current = head.load(std::memory_order_relaxed);
            while (current != nullptr) {
                Node<K, V>* next = current->next.load(std::memory_order_relaxed);
                delete current;
                current = next;
            }
        }
    };

    std::vector<std::unique_ptr<Bucket>> buckets;
    size_t numBuckets;
    std::hash<K> hashFunction;

    // 获取键对应的桶索引
    size_t getBucketIndex(const K& key) const {
        return hashFunction(key) % numBuckets;
    }

    // 标记指针帮助处理并发删除
    static constexpr uintptr_t MARKED = 0x1;

    // 检查节点是否被标记
    bool is_marked(Node<K, V>* node) {
        return (reinterpret_cast<uintptr_t>(node) & MARKED) != 0;
    }

    // 获取未标记的节点指针
    Node<K, V>* get_unmarked(Node<K, V>* node) {
        return reinterpret_cast<Node<K, V>*>(
            reinterpret_cast<uintptr_t>(node) & ~MARKED);
    }

    // 获取标记的节点指针
    Node<K, V>* get_marked(Node<K, V>* node) {
        return reinterpret_cast<Node<K, V>*>(
            reinterpret_cast<uintptr_t>(node) | MARKED);
    }

public:
    // 构造函数
    ThreadSafeHashTable(size_t bucketsCount = 64) : numBuckets(bucketsCount) {
        buckets.reserve(numBuckets);
        for (size_t i = 0; i < numBuckets; ++i) {
            buckets.emplace_back(std::make_unique<Bucket>());
        }
    }

    // 插入操作
    bool insert(const K& key, const V& value) {
        size_t index = getBucketIndex(key);
        try {
            Node<K, V>* newNode = new Node<K, V>(key, value);
            while (true) {
                Node<K, V>* currentHead = buckets[index]->head.load(std::memory_order_relaxed);
                newNode->next.store(currentHead, std::memory_order_relaxed);

                if (buckets[index]->head.compare_exchange_weak(
                        currentHead, newNode,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    return true;
                }
            }
        } catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed in insert: " << e.what() << std::endl;
            return false;
        }
    }

    // 查找操作
    bool find(const K& key, V& result) {
        size_t index = getBucketIndex(key);
        Node<K, V>* current = buckets[index]->head.load(std::memory_order_relaxed);

        while (current != nullptr) {
            Node<K, V>* unmarkedCurrent = get_unmarked(current);
            if (unmarkedCurrent->key == key && !is_marked(current)) {
                result = unmarkedCurrent->value;
                return true;
            }
            current = unmarkedCurrent->next.load(std::memory_order_relaxed);
        }
        return false;
    }

    // 删除操作
    bool remove(const K& key) {
        size_t index = getBucketIndex(key);
        while (true) {
            Node<K, V>* current = buckets[index]->head.load(std::memory_order_relaxed);
            Node<K, V>* prev = nullptr;
            Node<K, V>* unmarkedCurrent;

            // 遍历链表查找目标节点
            while (true) {
                if (current == nullptr) return false;

                unmarkedCurrent = get_unmarked(current);
                if (unmarkedCurrent->key == key) break;

                prev = unmarkedCurrent;
                current = unmarkedCurrent->next.load(std::memory_order_relaxed);
            }

            // 尝试标记要删除的节点
            Node<K, V>* next = unmarkedCurrent->next.load(std::memory_order_relaxed);
            Node<K, V>* expectedNext = next;
            Node<K, V>* desiredNext = get_marked(next);

            if (!unmarkedCurrent->next.compare_exchange_weak(
                    expectedNext, desiredNext,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                continue; // CAS失败，重试
            }

            // 尝试物理删除节点
            if (prev == nullptr) {
                if (buckets[index]->head.compare_exchange_strong(
                        current, next,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    delete unmarkedCurrent;
                    return true;
                }
            } else {
                if (prev->next.compare_exchange_strong(
                        current, next,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    delete unmarkedCurrent;
                    return true;
                }
            }
        }
    }
};

// 测试插入操作的线程函数
template<typename K, typename V>
void insertTest(ThreadSafeHashTable<K, V>& hashTable, const std::vector<std::pair<K, V>>& data) {
    for (const auto& pair : data) {
        hashTable.insert(pair.first, pair.second);
    }
}

// 测试查找操作的线程函数
template<typename K, typename V>
void findTest(ThreadSafeHashTable<K, V>& hashTable, const std::vector<K>& keys) {
    V result;
    for (const auto& key : keys) {
        if (hashTable.find(key, result)) {
            std::cout << "Found key: " << key << ", value: " << result << std::endl;
        } else {
            std::cout << "Key: " << key << " not found." << std::endl;
        }
    }
}

// 测试删除操作的线程函数
template<typename K, typename V>
void removeTest(ThreadSafeHashTable<K, V>& hashTable, const std::vector<K>& keys) {
    for (const auto& key : keys) {
        if (hashTable.remove(key)) {
            std::cout << "Removed key: " << key << std::endl;
        } else {
            std::cout << "Key: " << key << " not found for removal." << std::endl;
        }
    }
}

int main() {
    ThreadSafeHashTable<int, int> hashTable;

    // 插入测试数据
    std::vector<std::pair<int, int>> insertData = {{1, 100}, {2, 200}, {3, 300}};
    std::thread insertThread(insertTest<int, int>, std::ref(hashTable), insertData);

    // 查找测试数据
    std::vector<int> findKeys = {1, 2, 3, 4};
    std::thread findThread(findTest<int, int>, std::ref(hashTable), findKeys);

    // 删除测试数据
    std::vector<int> removeKeys = {2, 3};
    std::thread removeThread(removeTest<int, int>, std::ref(hashTable), removeKeys);

    // 等待所有线程完成
    insertThread.join();
    findThread.join();
    removeThread.join();

    return 0;
}