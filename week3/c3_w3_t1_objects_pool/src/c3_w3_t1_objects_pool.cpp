#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template<class T>
class ObjectPool {
public:
	T* Allocate() {
		T *res;
		if (pending.empty()) {
			res = new T();
		} else {
			res = pending.front();
			pending.pop();
		}
		allocated.insert(res);
		return res;
	}
	T* TryAllocate() {
		if (pending.empty()) {
			return nullptr;
		}
		// else
		T *res = pending.front();
		pending.pop();
		allocated.insert(res);
		return res;
	}

	void Deallocate(T *object) {
		auto res_it = allocated.find(object);
		if (res_it == allocated.end()) {
			throw invalid_argument("");
		}
		pending.push(*res_it);
		allocated.erase(res_it);
	}

	~ObjectPool() {
		for (T* ob : allocated) {
			delete ob;
		}
		while (!pending.empty()) {
			delete pending.front();
			pending.pop();
		}
	}

private:
	set<T*> allocated;
	queue<T*> pending;
};

void TestObjectPool() {
	ObjectPool<string> pool;

	auto p1 = pool.Allocate();
	auto p2 = pool.Allocate();
	auto p3 = pool.Allocate();

	*p1 = "first";
	*p2 = "second";
	*p3 = "third";

	pool.Deallocate(p2);
	ASSERT_EQUAL(*pool.Allocate(), "second");

	pool.Deallocate(p3);
	pool.Deallocate(p1);
	ASSERT_EQUAL(*pool.Allocate(), "third");
	ASSERT_EQUAL(*pool.Allocate(), "first");

	pool.Deallocate(p1);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestObjectPool);
	return 0;
}
