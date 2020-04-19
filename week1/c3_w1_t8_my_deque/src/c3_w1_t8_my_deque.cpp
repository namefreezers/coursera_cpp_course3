#include <iostream>
#include <vector>
using namespace std;

template <typename T>
class Deque {
public:
	Deque() { }

	bool Empty() const { return front.empty() && back.empty(); }

	size_t Size() const { return front.size() + back.size(); }

	T& operator[](size_t index) { return front.size() > index ? front[front.size() - index - 1] : back[-front.size() + index]; }

	const T& operator[](size_t index) const { return front.size() > index ? front[front.size() - index - 1] : back[-front.size() + index]; }

	T& At(size_t index) {
		if (index < 0 || index >= Size()) {
			throw out_of_range("");
		}
		return operator[](index);
	}

	const T& At(size_t index) const {
		if (index < 0 || index >= Size()) {
			throw out_of_range("");
		}
		return operator[](index);
	}

	T& Front() {
		if (!front.empty()) {
			return front.back();
		}
		else {
			return back.front();
		}
	}

	const T& Front() const {
		if (!front.empty()) {
			return front.back();
		}
		else {
			return back.front();
		}
	}

	T& Back() {
		if (!back.empty()) {
			return back.back();
		}
		else {
			return front.front();
		}
	}

	const T& Back() const {
		if (!back.empty()) {
			return back.back();
		}
		else {
			return front.front();
		}
	}

	void PushFront(const T &elem) { front.push_back(elem); }

	void PushBack(const T &elem) { back.push_back(elem); }


private:
	vector<T> front, back;
};

int main() {

	vector<int> v;
	try {
		cout << v[0];
	} catch(out_of_range e) {

	}
	
	return 0;
}
