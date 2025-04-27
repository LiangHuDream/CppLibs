#include <iostream>

// 草稿版本 仅用于验证移动构造

using namespace std;

class A {
public:
	A() = default;

	A(int n) {
		data = new int(n);
		sz = n;
	}
	A(const A& other) {
		data = new int(other.sz);
		sz = other.sz;

		copy(other.data, other.data + other.sz, data);
	}

	A(A&& other) noexcept {
		data = other.data;
		sz = other.sz;

		other.data = nullptr;
		other.sz = 0;
	}

	~A() {
		delete data;
	}

	int size() const {
		return sz;
	}
private:
	int *data;
	int sz;
};

int main()
{
	A a(8);
	cout << a.size() << endl;
	A b = move(a);

	cout << a.size() << endl;

	return 0;
}
