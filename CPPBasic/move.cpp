#include <iostream>
#include <algorithm>
 
 class A {
 public:
    // 1. 构造函数
 	A(int n = 0) :data(nullptr), sz(n)  {
		if (sz > 0) {
			data = new int(n);
 			sz = n;
		}
 	}
	// 2. 拷贝构造
 	A(const A& other): data(nullptr), sz(other.sz) {
		if (other.sz > 0) {
			data = new int(other.sz);
			std::copy(other.data, other.data + other.sz, data);
		}
 	}

	// 3. 移动构造函数
 	A(A&& other) noexcept:data(other.data), sz(other.sz) {
 		other.data = nullptr;
 		other.sz = 0;
 	}
	// 4. 拷贝赋值操作符
	A& operator=(const A& other) {
		if (this != &other) {
			delete data;
			sz = other.sz;
			data = new int(sz);
			std::copy(other.data, other.data + sz, data);
		}

		return *this;
	}

	// 5. 移动赋值
	A& operator=(A && other) noexcept {
		if (this != &other) {
			delete data;
			data = other.data;
			sz = other.sz;
			other.data = nullptr;
			other.sz = 0;
		}
		return *this;
	}


 
 	~A() {
 		delete data;
 	}
 
 	int size() const {
 		return sz;
 	}
 private:
 	int *data;
 	int sz{};
 };
 
 int main()
 {
 	A a(8);
 	std::cout << a.size() << std::endl;
 	A b = std::move(a);
 	std::cout << a.size() << std::endl;
 	return 0;
 }