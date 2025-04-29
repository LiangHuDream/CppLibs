#pragma once

// ============ 通用模板类 ============
template <typename T>
class MyClass {
private:
    T data;
public:
    MyClass(T value);
    void print() const;
};

// ============ 特化类声明（仅声明，不定义成员）============
template <>
class MyClass<int>;       // int全特化声明

template <>
class MyClass<double>;    // double全特化声明

template <>
class MyClass<long long>; // long long全特化声明
