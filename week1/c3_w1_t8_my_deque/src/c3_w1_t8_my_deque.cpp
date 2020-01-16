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

	T& Front() { return front.back(); }

	const T& Front() const { return front.back(); }

	T& Back() { return back.back(); }

	const T& Back() const { return back.back(); }

	void PushFront(const T &elem) { front.push_back(elem); }

	void PushBack(const T &elem) { back.push_back(elem); }


private:
	vector<T> front, back;
};

int main() {
	return 0;
}
