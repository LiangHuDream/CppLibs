#pragma once
#include <memory>
#include <iostream>

template <typename T> class SimpleAllocator {
public:
    using value_type = T;

    // 默认构造函数
    SimpleAllocator() = default;

    // 支持 rebind 的构造函数
    template <typename U> 
    SimpleAllocator(const SimpleAllocator<U> &) noexcept {}

    T *allocate(size_t n) {
      return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    void deallocate(T *p, size_t n) {
      ::operator delete(p);
    }

    // 添加比较运算符
    bool operator==(const SimpleAllocator &) const {
      return true;
    }
    bool operator!=(const SimpleAllocator &) const {
      return false;
    }
    // 构造对象
    template <typename... Args> void construct(T *p, Args &&...args) {
      new (p) T(std::forward<Args>(args)...);
    }

    // 销毁对象
    void destroy(T *p) {
      p->~T();
    }

    // rebind 机制
    template <typename U> struct rebind {
      using other = SimpleAllocator<U>;
    };
};
