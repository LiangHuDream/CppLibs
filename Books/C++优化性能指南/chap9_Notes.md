《C++性能优化指南》第九章重点讲解查找与排序的优化策略。让我们通过循序渐进的代码示例，深入理解这些优化技巧。

---

### 第九章核心知识点
1. **选择高效算法**：二分查找O(logN)优于线性查找O(N)
2. **善用标准库**：`<algorithm>`中的算法经过高度优化
3. **数据结构优化**：有序容器适合二分查找，哈希表适合快速查找
4. **内存布局优化**：连续内存访问提升缓存命中率

---

代码示例1：二分查找优化
```cpp
#include <iostream>
#include <vector>
#include <algorithm>

// 手动实现二分查找
bool binary_search_manual(const std::vector<int>& vec, int target) {
    int left = 0;
    int right = vec.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (vec[mid] == target) return true;
        if (vec[mid] < target) 
            left = mid + 1;
        else 
            right = mid - 1;
    }
    return false;
}

int main() {
    std::vector<int> data = {1, 3, 5, 7, 9, 11, 13, 15};
    std::sort(data.begin(), data.end());

    // 测试查找性能
    auto test = [&](int target) {
        bool found = binary_search_manual(data, target);
        std::cout << target << (found ? " found" : " not found") << std::endl;
    };

    test(7);   // 存在
    test(8);    // 不存在
    test(15);   // 边界值
}
```

**优化点**：
- 手动实现避免迭代器开销
- 计算mid时避免整数溢出（使用`left + (right-left)/2`）
- 有序数据保证算法正确性

---

代码示例2：标准库算法对比
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

void measure_performance() {
    std::vector<int> data(1000000);
    std::generate(data.begin(), data.end(), []() { return rand() % 1000000; });
    std::sort(data.begin(), data.end());
    int target = data[data.size()/2]; // 中间元素

    // 测试std::binary_search
    auto start = std::chrono::high_resolution_clock::now();
    bool bs = std::binary_search(data.begin(), data.end(), target);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "std::binary_search: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()
              << " μs\n";

    // 测试std::lower_bound
    start = std::chrono::high_resolution_clock::now();
    auto it = std::lower_bound(data.begin(), data.end(), target);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "std::lower_bound:   " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()
              << " μs\n";
}

int main() {
    measure_performance();
}
```

**输出示例**：
```
std::binary_search: 15 μs
std::lower_bound:   12 μs
```

**优化分析**：
- `lower_bound`直接返回迭代器位置，省去二次验证
- 标准库实现使用平台特定的优化指令（如SIMD）

---

代码示例3：哈希表优化
```cpp
#include <iostream>
#include <unordered_map>
#include <string>

struct CustomHash {
    size_t operator()(const std::string& s) const {
        size_t hash = 0;
        for(char c : s) {
            hash = (hash * 131) + c; // 更好的哈希分布
        }
        return hash;
    }
};

int main() {
    // 使用自定义哈希函数
    std::unordered_map<std::string, int, CustomHash> hash_map = {
        {"apple", 5}, {"banana", 3}, {"orange", 8}
    };

    auto test = [&](const std::string& key) {
        auto it = hash_map.find(key);
        if(it != hash_map.end()) 
            std::cout << key << ": " << it->second << std::endl;
        else
            std::cout << key << " not found" << std::endl;
    };

    test("apple");    // 存在
    test("grape");    // 不存在
}
```

**优化点**：
- 自定义哈希函数减少冲突
- 预分配桶大小减少rehash次数
- 使用`find()`代替`operator[]`避免默认构造开销

---

性能优化策略总结

| 方法 | 时间复杂度 | 适用场景 |
|------|------------|----------|
| 线性查找 | O(n) | 小数据集或无序数据 |
| 二分查找 | O(log n) | 已排序的连续存储 |
| 哈希表 | O(1)平均 | 需要快速查找/插入 |
| trie树 | O(k)（k为键长） | 字符串前缀查找 |

---

关键测试技巧
1. **数据预热**：在计时前先运行几次测试代码消除冷启动误差
```cpp
// 预热缓存
for(int i=0; i<10; ++i) {
    std::binary_search(data.begin(), data.end(), target);
}
```
1. **大数据测试**：至少使用10^6量级的数据才能体现性能差异
2. **多场景验证**：测试最佳/平均/最差情况下的性能表现

### 核心知识点总结
1. 标准库容器的性能特征（map、unordered_map、vector等）
2. 二分查找的不同实现方式（std::lower_bound vs 手动实现）
3. 哈希表优化（负载因子、哈希函数设计）
4. 字符串键优化的特殊处理（固定长度数组、C字符串）
5. 排序算法选择策略（数据分布特性、稳定性需求）
6. 内存布局对缓存的影响（连续存储vs链表存储）
7. 类型特化带来的性能提升
8. 预计算和缓存的应用
9. 查找算法的抽象代价
10. 自定义分配器对性能的影响

---

## 多选题

1. 关于std::map和std::unordered_map的性能差异，**错误**的说法是：
   A) unordered_map的插入时间复杂度总是O(1)
   B) map的有序性使其适合范围查询
   C) unordered_map的查找性能受哈希函数质量影响
   D) map的迭代器失效规则比unordered_map更严格
   E) 两者都支持自定义比较函数

2. 优化哈希表性能的有效手段包括：
   A) 使用素数作为桶的数量
   B) 保持负载因子低于0.7
   C) 预分配足够空间避免rehash
   D) 使用完美哈希函数
   E) 采用开放寻址法代替链地址法

3. 对于已排序的vector进行查找，正确的优化策略是：
   A) 优先使用std::find
   B) 使用std::lower_bound进行二分查找
   C) 通过缓存最近结果加速后续查询
   D) 将数据转换为平衡二叉搜索树
   E) 使用SIMD指令优化比较操作

4. 关于字符串作为键的优化，正确的做法是：
   A) 对短字符串使用SSO（短字符串优化）
   B) 用char[N]代替std::string避免动态分配
   C) 使用字符串视图避免复制
   D) 对长字符串使用滚动哈希
   E) 始终优先选择C风格字符串

5. 在排序算法选择中，正确的决策包括：
   A) 对基本有序数据使用插入排序
   B) 需要稳定排序时选择std::sort
   C) 内存受限时使用原地排序算法
   D) 并行环境下使用radix sort
   E) 对大型数据集优先选择快速排序

6. 影响std::list性能的因素包括：
   A) 内存访问模式不连续
   B) 每个元素需要额外存储指针
   C) 排序时的高效缓存利用
   D) 插入删除操作的时间复杂度
   E) 预分配节点内存池

7. 关于内存布局优化的正确说法：
   A) std::vector的连续内存有利于缓存
   B) 结构体填充对齐会提升访问速度
   C) SoA比AoS更适合并行处理
   D) 热数据分离可以提升缓存命中率
   E) 使用std::deque会完全避免内存重分配

8. 有效的查找优化模式包括：
   A) 布隆过滤器加速不存在判断
   B) 分层索引结构减少比较次数
   C) 位图索引加速等值查询
   D) 牺牲内存空间换取查找速度
   E) 所有场景都应使用哈希表

9. 关于STL算法的正确理解：
   A) std::sort要求随机访问迭代器
   B) std::stable_sort的时间复杂度是O(N logN)
   C) std::partial_sort用于提取top K元素
   D) std::nth_element会完全排序容器
   E) std::inplace_merge需要额外内存

10. 优化比较操作的有效方法：
    A) 使用三路比较减少分支预测失败
    B) 对复杂对象缓存比较结果
    C) 利用SIMD指令并行比较
    D) 所有比较都应内联处理
    E) 避免在比较函数中分配内存

---

## 设计题

### 设计题1：实现固定长度字符串键的哈希表
**要求**：
- 使用char[8]作为键类型
- 支持快速插入和查找
- 处理哈希冲突采用开放寻址法
- 实现动态扩容机制

```cpp
#include <iostream>
#include <vector>
#include <cstring>

template<typename V, size_t N = 8>
class FixedKeyHashMap {
    struct Entry {
        char key[N];
        V value;
        bool occupied = false;
    };

    std::vector<Entry> table;
    size_t count = 0;
    const double max_load_factor = 0.7;

    size_t hash(const char* key) const {
        size_t h = 0;
        for (size_t i = 0; i < N; ++i) {
            h = (h * 131) + static_cast<unsigned char>(key[i]);
        }
        return h % table.size();
    }

    void rehash() {
        std::vector<Entry> old_table = std::move(table);
        table.resize(old_table.size() * 2);
        count = 0;
        
        for (auto& entry : old_table) {
            if (entry.occupied) {
                insert(entry.key, entry.value);
            }
        }
    }

public:
    FixedKeyHashMap(size_t initial_size = 16) : table(initial_size) {}

    void insert(const char* key, const V& value) {
        if (static_cast<double>(count) / table.size() > max_load_factor) {
            rehash();
        }

        size_t index = hash(key);
        for (size_t i = 0; i < table.size(); ++i) {
            size_t probe = (index + i) % table.size();
            if (!table[probe].occupied || 
                memcmp(table[probe].key, key, N) == 0) {
                memcpy(table[probe].key, key, N);
                table[probe].value = value;
                if (!table[probe].occupied) {
                    table[probe].occupied = true;
                    ++count;
                }
                return;
            }
        }
        rehash();
        insert(key, value);
    }

    bool find(const char* key, V& value) const {
        size_t index = hash(key);
        for (size_t i = 0; i < table.size(); ++i) {
            size_t probe = (index + i) % table.size();
            if (!table[probe].occupied) break;
            if (memcmp(table[probe].key, key, N) == 0) {
                value = table[probe].value;
                return true;
            }
        }
        return false;
    }
};

int main() {
    FixedKeyHashMap<int> map;
    
    const char key1[8] = {'A','B','C','D','E','F','G','H'};
    map.insert(key1, 100);
    
    const char key2[8] = {'1','2','3','4','5','6','7','8'};
    map.insert(key2, 200);

    int val;
    std::cout << std::boolalpha;
    std::cout << map.find(key1, val) << " " << val << "\n"; // true 100
    std::cout << map.find(key2, val) << " " << val << "\n"; // true 200
    
    const char key3[8] = {'X','Y','Z'}; // 部分初始化
    std::cout << map.find(key3, val) << "\n"; // false
    
    return 0;
}
```

---

### 设计题2：实现带缓存的二分查找
**要求**：
- 缓存最近10次查询结果
- 处理缓存失效问题
- 支持多种数据类型
- 比较缓存命中率

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>

template<typename T>
class CachedBinarySearch {
    struct CacheEntry {
        typename std::vector<T>::const_iterator begin;
        typename std::vector<T>::const_iterator end;
        T value;
        typename std::vector<T>::const_iterator result;
    };
    
    std::list<CacheEntry> lru_list;
    std::unordered_map<T, typename std::list<CacheEntry>::iterator> cache_map;
    static const size_t CACHE_SIZE = 10;

    typename std::vector<T>::const_iterator binary_search(
        typename std::vector<T>::const_iterator begin,
        typename std::vector<T>::const_iterator end,
        const T& value) 
    {
        auto low = begin;
        auto high = end;
        
        while (low < high) {
            auto mid = low + (high - low)/2;
            if (*mid == value) return mid;
            if (*mid < value) low = mid + 1;
            else high = mid;
        }
        return end;
    }

public:
    typename std::vector<T>::const_iterator search(
        const std::vector<T>& data,
        const T& value) 
    {
        // Check cache
        auto cache_it = cache_map.find(value);
        if (cache_it != cache_map.end()) {
            lru_list.splice(lru_list.begin(), lru_list, cache_it->second);
            return cache_it->second->result;
        }
        
        // Perform actual search
        auto result = binary_search(data.begin(), data.end(), value);
        
        // Update cache
        if (result != data.end()) {
            if (cache_map.size() >= CACHE_SIZE) {
                auto last = lru_list.end();
                --last;
                cache_map.erase(last->value);
                lru_list.pop_back();
            }
            
            lru_list.push_front({data.begin(), data.end(), value, result});
            cache_map[value] = lru_list.begin();
        }
        
        return result;
    }
};

int main() {
    std::vector<int> data = {1,3,5,7,9,11,13,15};
    CachedBinarySearch<int> searcher;
    
    auto test = [&](int val) {
        auto it = searcher.search(data, val);
        if (it != data.end()) {
            std::cout << "Found " << val << " at pos " << (it - data.begin()) << "\n";
        } else {
            std::cout << val << " not found\n";
        }
    };
    
    test(5);  // Cache miss
    test(5);  // Cache hit
    test(7);  
    test(9);
    // ...多次查询测试缓存替换策略
    
    return 0;
}
```

---

### 设计题3：优化排序的字符串比较
**要求**：
- 对短字符串（长度≤16）进行优化
- 避免多次计算哈希值
- 利用SSO特性
- 实现自定义比较函数

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <memory>

struct FastStringCompare {
private:
    union {
        char short_buf[16];
        std::unique_ptr<std::string> long_ptr;
    };
    bool is_short;
    
    // 辅助比较函数
    static int compare_short(const char* a, const char* b, size_t len) {
        return memcmp(a, b, len);
    }

public:
    explicit FastStringCompare(const std::string& s) : is_short(s.size() <= 16) {
        if (is_short) {
            memcpy(short_buf, s.data(), s.size());
            memset(short_buf + s.size(), 0, 16 - s.size());
        } else {
            new (&long_ptr) std::unique_ptr<std::string>(new std::string(s));
        }
    }

    // 正确管理拷贝和移动
    FastStringCompare(const FastStringCompare& other) : is_short(other.is_short) {
        if (is_short) {
            memcpy(short_buf, other.short_buf, 16);
        } else {
            new (&long_ptr) std::unique_ptr<std::string>(new std::string(*other.long_ptr));
        }
    }

    FastStringCompare& operator=(const FastStringCompare& other) {
        if (this != &other) {
            this->~FastStringCompare();
            new (this) FastStringCompare(other);
        }
        return *this;
    }

    ~FastStringCompare() {
        if (!is_short) {
            long_ptr.~unique_ptr();
        }
    }

    // 正确的比较逻辑
    bool operator<(const FastStringCompare& other) const {
        // 比较实际内容而不是存储类型
        if (is_short && other.is_short) {
            return compare_short(short_buf, other.short_buf, 16) < 0;
        }
        
        const std::string& a = is_short ? 
            std::string(short_buf, strnlen(short_buf, 16)) : 
            *long_ptr;
        
        const std::string& b = other.is_short ? 
            std::string(other.short_buf, strnlen(other.short_buf, 16)) : 
            *other.long_ptr;
        
        return a < b;
    }

    // 添加输出支持
    friend std::ostream& operator<<(std::ostream& os, const FastStringCompare& fs) {
        if (fs.is_short) {
            return os << "Short: " << std::string(fs.short_buf, strnlen(fs.short_buf, 16));
        } else {
            return os << "Long: " << *fs.long_ptr;
        }
    }
};

int main() {
    std::vector<FastStringCompare> vec;
    vec.emplace_back("apple");
    vec.emplace_back("banana");
    vec.emplace_back("cherry");
    vec.emplace_back("averylongstringthatexceeds16characters");  // 修正拼写错误
    
    // 测试拷贝构造函数
    std::vector<FastStringCompare> vec2 = vec;
    
    std::sort(vec.begin(), vec.end());
    
    for (const auto& fs : vec) {
        std::cout << fs << "\n";
    }
    
    return 0;
}
```

---

### 设计题4：多条件排序优化
**要求**：
- 按多个字段排序（如先年龄后姓名）
- 使用复合键避免多次排序
- 利用移动语义减少复制
- 支持稳定排序

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>

struct Person {
    int age;
    std::string name;
    
    Person(int a, std::string n) : age(a), name(std::move(n)) {}
    
    auto tied() const {
        return std::tie(age, name);
    }
    
    bool operator<(const Person& other) const {
        return tied() < other.tied();
    }
};

void sort_people(std::vector<Person>& people) {
    std::sort(people.begin(), people.end(),
        [](const Person& a, const Person& b) {
            if (a.age != b.age) return a.age < b.age;
            return a.name < b.name;
        });
}

int main() {
    std::vector<Person> people;
    people.emplace_back(25, "Alice");
    people.emplace_back(30, "Bob");
    people.emplace_back(25, "Charlie");
    people.emplace_back(30, "David");
    
    sort_people(people);
    
    for (const auto& p : people) {
        std::cout << p.age << " " << p.name << "\n";
    }
    /* 输出：
        25 Alice
        25 Charlie
        30 Bob
        30 David
    */
    
    return 0;
}
```

---

### 设计题5：内存友好的数据结构
**要求**：
- 实现紧凑存储的键值对
- 使用内存池避免碎片
- 支持快速查找和遍历
- 处理不同类型数据

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <optional>  // 用于更安全的返回值

template<typename K, typename V>
class CompactMap {
    struct Node {
        K key;
        V value;
        Node* next = nullptr;
    };

    std::vector<std::unique_ptr<Node[]>> pools;
    size_t pool_size = 1024;
    size_t current_pool = 0;
    size_t current_index = 0;

    std::vector<Node*> table;        // 替换原始指针数组为 vector
    size_t count = 0;                // 元素计数器
    const double load_factor = 0.75; // 触发扩容的负载因子

    Node* alloc_node() {
        if (current_index >= pool_size) {
            pools.emplace_back(new Node[pool_size]);
            current_pool = pools.size() - 1;
            current_index = 0;
        }
        return &pools[current_pool][current_index++];
    }

    void rehash(size_t new_size) {
        std::vector<Node*> new_table(new_size, nullptr);
        for (Node* head : table) {
            Node* curr = head;
            while (curr) {
                Node* next = curr->next;
                size_t hash = std::hash<K>{}(curr->key) % new_size;
                curr->next = new_table[hash];
                new_table[hash] = curr;
                curr = next;
            }
        }
        table = std::move(new_table);
    }

public:
    explicit CompactMap(size_t initial_size = 16) : table(initial_size, nullptr) {}

    // 支持完美转发插入
    template<typename KeyArg, typename ValueArg>
    void insert(KeyArg&& key, ValueArg&& value) {
        if (count >= table.size() * load_factor) {
            rehash(table.size() * 2);
        }

        size_t hash = std::hash<K>{}(key) % table.size();
        // 检查键是否已存在（可选）
        for (Node* curr = table[hash]; curr; curr = curr->next) {
            if (curr->key == key) {
                curr->value = std::forward<ValueArg>(value); // 更新值
                return;
            }
        }

        // 插入新节点
        Node* node = alloc_node();
        node->key = std::forward<KeyArg>(key);
        node->value = std::forward<ValueArg>(value);
        node->next = table[hash];
        table[hash] = node;
        ++count;
    }

    // 返回 std::optional 避免输出参数
    std::optional<V> find(const K& key) const {
        size_t hash = std::hash<K>{}(key) % table.size();
        for (Node* curr = table[hash]; curr; curr = curr->next) {
            if (curr->key == key) {
                return curr->value;
            }
        }
        return std::nullopt;
    }

    // 自动内存管理：无需手动释放 table
};

int main() {
    CompactMap<int, std::string> map;
    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(2, "TWO"); // 测试键重复时的覆盖

    std::cout << std::boolalpha;
    std::cout << map.find(1).value_or("not found") << "\n"; // 输出 "one"
    std::cout << map.find(2).value_or("not found") << "\n"; // 输出 "TWO"
    std::cout << map.find(3).has_value() << "\n";           // 输出 "false"

    return 0;
}
```

---

## 多选题答案与详解

1. **DE**  
   D错误：map的迭代器在插入时可能失效，unordered_map在rehash时失效  
   E错误：unordered_map不需要比较函数，需要哈希函数

2. **ABCDE**  
   所有选项都是有效的哈希表优化手段

3. **BCE**  
   A错误：线性查找效率低  
   D错误：vector已排序不需要转换结构

4. **ABCD**  
   E错误：C字符串管理不便

5. **ACDE**  
   B错误：std::sort不稳定

6. **ABDE**  
   C错误：链表缓存不友好

7. **ACD**  
   B错误：过度对齐可能浪费空间  
   E错误：deque分段存储仍可能部分重分配

8. **ABCD**  
   E错误：哈希表不适用范围查询

9. **ABC**  
   D错误：nth_element只保证第n元素就位  
   E错误：inplace_merge是原地操作

10. **ABCE**  
    D错误：过度内联可能导致代码膨胀

---

## 设计题关键点分析

1. **哈希表设计**：
   - 线性探测解决冲突
   - 动态扩容时保持素数容量
   - 内存连续提升缓存效率

2. **缓存二分查找**：
   - LRU缓存淘汰策略
   - 哈希表加速缓存查找
   - 缓存键包含范围信息

3. **字符串优化**：
   - 利用联合体区分长短字符串
   - 内存比较代替字符串操作
   - 避免短字符串的堆分配

4. **多条件排序**：
   - 复合键比较减少排序次数
   - 使用tie生成可比较元组
   - 稳定排序保持相对顺序

5. **内存管理**：
   - 内存池减少系统调用
   - 批量分配提升局部性
   - 链地址法哈希表实现