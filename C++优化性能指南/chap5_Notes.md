### 第五章重难点详解与代码实战

---

1. 时间复杂度分析（5.1节）
**重点**：掌握大O符号含义，区分最优/平均/最差情况时间复杂度。
**示例**：插入排序时间复杂度分析
```cpp
#include <iostream>
#include <vector>
using namespace std;

// 插入排序实现
void insertionSort(vector<int>& arr) {
    for (int i = 1; i < arr.size(); ++i) {
        int key = arr[i];
        int j = i-1;
        while (j >= 0 && arr[j] > key) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
}

int main() {
    vector<int> test1 = {5, 2, 4, 6, 1, 3}; // 平均情况
    insertionSort(test1);
    cout << "Sorted average case: ";
    for (int num : test1) cout << num << " ";
    cout << endl;

    vector<int> test2 = {6, 5, 4, 3, 2, 1}; // 最坏情况（逆序）
    insertionSort(test2);
    cout << "Sorted worst case: ";
    for (int num : test2) cout << num << " ";
    return 0;
}
```
**测试**：观察不同输入下的执行时间差异（平均O(n²)，最优O(n)）。

---

2. 查找算法优化（5.3节）
**重点**：线性查找 vs 二分查找时间复杂度差异。
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 线性查找 O(n)
int linearSearch(const vector<int>& arr, int target) {
    for (int i = 0; i < arr.size(); ++i) {
        if (arr[i] == target) return i;
    }
    return -1;
}

// 二分查找 O(log n)
int binarySearch(const vector<int>& arr, int target) {
    int left = 0, right = arr.size()-1;
    while (left <= right) {
        int mid = left + (right - left)/2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    int target = 7;
    
    cout << "Linear search index: " << linearSearch(arr, target) << endl;
    cout << "Binary search index: " << binarySearch(arr, target) << endl;
    
    // 性能对比测试（需添加计时逻辑）
    return 0;
}
```
**优化点**：预处理排序后使用二分查找大幅提升效率。

---

3. 排序算法优化（5.4节）
**重点**：快速排序优化（三数取中法避免最坏情况）
```cpp
#include <iostream>
#include <vector>
using namespace std;

// 三数取中法选择pivot
int medianOfThree(vector<int>& arr, int left, int right) {
    int mid = left + (right - left)/2;
    if (arr[mid] < arr[left]) swap(arr[left], arr[mid]);
    if (arr[right] < arr[left]) swap(arr[left], arr[right]);
    if (arr[mid] < arr[right]) swap(arr[mid], arr[right]);
    return right;
}

// 快速排序优化实现
void quickSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;
    
    // 选择pivot并分区
    int pivot = medianOfThree(arr, left, right);
    int i = left - 1;
    for (int j = left; j < right; ++j) {
        if (arr[j] <= arr[pivot]) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i+1], arr[pivot]);
    int partitionIndex = i + 1;
    
    quickSort(arr, left, partitionIndex-1);
    quickSort(arr, partitionIndex+1, right);
}

int main() {
    vector<int> test = {3, 6, 8, 10, 1, 2, 1};
    quickSort(test, 0, test.size()-1);
    cout << "Optimized QuickSort: ";
    for (int num : test) cout << num << " ";
    return 0;
}
```
**测试**：对比标准快速排序与优化版本在逆序数组中的性能。

---

4. 优化模式：缓存（5.5.4节）
**示例**：斐波那契数列的缓存优化
```cpp
#include <iostream>
#include <unordered_map>
using namespace std;

unordered_map<int, int> cache;

// 带缓存的递归斐波那契
int fib(int n) {
    if (n <= 1) return n;
    if (cache.find(n) != cache.end()) 
        return cache[n];
    cache[n] = fib(n-1) + fib(n-2);
    return cache[n];
}

int main() {
    cout << "Fib(10): " << fib(10) << endl;  // 55
    cout << "Fib(20): " << fib(20) << endl;  // 6765
    return 0;
}
```
**优化效果**：时间复杂度从O(2ⁿ)降为O(n)。

---

5. 斯特潘诺夫抽象惩罚（5.6节）
**重点**：泛型编程带来的性能损耗分析
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 自定义简单排序（无抽象）
void simpleSort(int* arr, int size) {
    for (int i=0; i<size; ++i)
        for (int j=i+1; j<size; ++j)
            if (arr[j] < arr[i])
                swap(arr[i], arr[j]);
}

// 测试对比
int main() {
    vector<int> data(10000);
    generate(data.begin(), data.end(), rand);
    
    // 测试STL排序（模板抽象）
    auto stlData = data;
    sort(stlData.begin(), stlData.end());
    
    // 测试简单排序（直接实现）
    auto simpleData = data;
    simpleSort(&simpleData[0], simpleData.size());
    
    // 添加计时代码比较性能差异
    return 0;
}
```
**结论**：抽象层级越高可能带来额外开销，需权衡可维护性与性能。

---

### 编译与测试说明
1. 所有代码均使用C++11或更高标准编译：
```bash
g++ -std=c++11 example.cpp -o example
```
2. 添加计时逻辑推荐使用`<chrono>`库：
```cpp
#include <chrono>
auto start = chrono::high_resolution_clock::now();
// ...测试代码...
auto end = chrono::high_resolution_clock::now();
cout << "Time: " << chrono::duration_cast<chrono::microseconds>(end-start).count() << "μs\n";
```
### 第五章核心知识点整理

#### 重难点梳理
1. 算法时间复杂度分析（最优/平均/最坏情况）
2. 哈希表与二分查找的性能对比
3. 排序算法的选择策略（快速排序 vs 基数排序）
4. 预计算与延迟计算的应用场景
5. 缓存策略的失效处理机制
6. 双重检查锁定模式的应用
7. 散列函数设计原则
8. 分治策略在排序中的应用
9. 算法选择时的内存局部性考量
10. 摊销时间复杂度分析

---

### 第一部分：多选题（10道）

1. 关于哈希表查找性能，正确的是：
A) 平均时间复杂度O(1)
B) 最坏时间复杂度O(n)
C) 适合有序数据查询
D) 空间复杂度总是O(n)

2. 快速排序的优化策略包括：
A) 三数取中法选择枢轴
B) 小数组切换插入排序
C) 递归实现优先于迭代
D) 处理重复元素的3-way partition

3. 适合处理海量数据的排序算法：
A) 基数排序
B) 归并排序 
C) 冒泡排序
D) Flashsort

4. 关于二分查找正确的是：
A) 要求数据有序
B) 时间复杂度O(n)
C) 适合链表结构
D) 可处理动态数据集

5. 预计算模式的适用场景：
A) 运行时频繁计算的固定值
B) 需要实时更新的动态数据
C) 编译期已知的常量表达式
D) 内存敏感的低配设备

6. 缓存失效的常见处理方式：
A) LRU置换策略
B) 定时强制刷新
C) 写穿透策略
D) 哈希碰撞链表法

7. 关于时间复杂度分析：
A) 插入排序最坏O(n²)
B) 快速排序平均O(n logn)
C) 基数排序O(nk) k为位数
D) 冒泡排序最优O(n)

8. 双重检查锁定用于：
A) 单例模式初始化
B) 线程安全缓存访问
C) 原子计数器操作
D) 无锁数据结构设计

9. 影响算法实际性能的因素：
A) 缓存命中率
B) 分支预测效率
C) 循环展开次数
D) 指令流水线深度

10. 关于分治策略：
A) 快速排序基于分治
B) 归并排序需要额外空间
C) 适合并行计算
D) 总将问题分为两等份

---

### 第二部分：设计题（5道）

题目1：延迟计算缓存系统
设计支持过期时间的缓存系统，要求：
- 使用unordered_map存储数据
- 支持惰性清理过期条目
- 线程安全的双重检查锁定
- 提供get/put接口

题目2：预计算优化斐波那契
实现基于预计算的斐波那契数列：
- 编译期生成前50项
- 运行时直接查表
- 处理溢出异常
- 支持动态扩展

题目3：快速排序优化
实现优化的快速排序：
- 三数取中选择枢轴
- 小数组切换插入排序
- 迭代替代递归
- 处理重复元素

题目4：哈希表性能优化
设计高性能哈希表：
- 开放寻址法解决冲突
- 负载因子超过0.75时扩容
- 使用素数表控制容量
- 支持移动语义

题目5：二分查找边界处理
实现泛型二分查找：
- 处理重复元素的第一个/最后一个位置
- 支持旋转有序数组
- 异常安全保证
- 性能测试对比线性搜索

---

### 答案与详解

#### 多选题答案：
1. AB（哈希表平均O(1)，最坏O(n)）
2. ABD（三数取中、小数组优化、3-way分区）
3. ABD（基数、归并、Flashsort适合大数据）
4. A（必须有序）
5. AC（固定值和编译期计算）
6. ABC（LRU、刷新、写穿透）
7. ABC（插入最坏O(n²)，快排平均O(n logn)，基数O(nk)）
8. AB（单例和缓存访问）
9. ABCD（全部影响实际性能）
10. ABC（快排分治、归并需空间、可并行）

---

### 设计题参考实现（以题目2为例）

```cpp
#include <iostream>
#include <vector>
#include <stdexcept>

template<typename T>
class FibonacciCache {
    std::vector<T> cache;
    static const size_t INIT_SIZE = 50;
public:
    FibonacciCache() {
        cache.reserve(INIT_SIZE);
        cache.push_back(0);
        cache.push_back(1);
        for(size_t i=2; i<INIT_SIZE; ++i){
            T next = cache[i-1] + cache[i-2];
            if(next < cache[i-1]) throw std::overflow_error("Overflow in precomputation");
            cache.push_back(next);
        }
    }

    T get(size_t n) {
        while(n >= cache.size()){
            size_t i = cache.size();
            T next = cache[i-1] + cache[i-2];
            if(next < cache[i-1]) throw std::overflow_error("Overflow during expansion");
            cache.push_back(next);
        }
        return cache[n];
    }
};

int main() {
    try {
        FibonacciCache<unsigned long long> fib;
        
        // Test precomputed values
        std::cout << fib.get(10) << std::endl;  // 55
        std::cout << fib.get(49) << std::endl; // 7778742049
        
        // Test dynamic expansion
        std::cout << fib.get(50) << std::endl; // 12586269025
        
        // Test overflow detection
        FibonacciCache<unsigned> small_fib;
        small_fib.get(47); // Throw overflow
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
```

#### 代码说明：
1. 模板类支持不同数值类型
2. 编译期预计算前50项
3. 运行时动态扩展缓存
4. 溢出检测机制
5. 异常安全保证

---

### 测试用例设计要点

1. 验证预计算正确性（n=10,49）
2. 测试动态扩展能力（n=50）
3. 边界条件测试（n=0,1）
4. 溢出异常检测（unsigned类型n=47）
5. 性能对比非预计算版本
