#include <iostream>
#include <iterator>

template <class T, class Distance = ptrdiff_t>
class istreamIterator {
protected:
    std::istream* stream;  // 输入流指针
    T value;               // 当前缓存值
    bool end_marker;        // 流结束标志

    // 核心数据读取方法
    void read() {
        end_marker = (*stream) ? true : false;
        if (end_marker) {
            *stream >> value;          // 尝试提取数据
            end_marker = (*stream) ? true : false;  // 验证提取结果
        }
        if (!end_marker) {
            stream = nullptr;         // 标记流失效
        }
    }

public:
    // 类型定义
    typedef std::input_iterator_tag iterator_category;
    typedef T value_type;
    typedef Distance difference_type;
    typedef const T* pointer;
    typedef const T& reference;

    // 构造函数
    istreamIterator() : stream(nullptr), end_marker(true) {}
    explicit istreamIterator(std::istream& s) : stream(&s), end_marker(false) { read(); }

    // 禁用拷贝
    istreamIterator(const istreamIterator&) = delete;
    istreamIterator& operator=(const istreamIterator&) = delete;

    // 解引用
    reference operator*() const { 
        if (!stream) throw std::logic_error("Dereference on end iterator");
        return value; 
    }

    pointer operator->() const { return &(operator*()); }

    // 递增操作符
    istreamIterator& operator++() {
        if (!stream) throw std::logic_error("Increment on end iterator");
        read();
        return *this;
    }

    istreamIterator operator++(int) {
        istreamIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    // 比较运算符
    friend bool operator==(const istreamIterator& x, const istreamIterator& y) { /* 同上述修正方案 */ }
    friend bool operator!=(const istreamIterator& x, const istreamIterator& y) { return !(x == y); }
};
