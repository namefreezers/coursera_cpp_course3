#pragma once

#include <cstdlib>

// Реализуйте шаблон SimpleVector
template<typename T>
class SimpleVector {
public:
	SimpleVector() {};
	explicit SimpleVector(size_t size_) {
		size = size_;
		capacity = size_;
		data = new T[size_];
	}
	~SimpleVector() { delete[] data; }

	T& operator[](size_t index) { return data[index]; }

	T* begin() { return data; }
	T* end() { return data + size; }

	size_t Size() const { return size; }
	size_t Capacity() const { return capacity; }
	void PushBack(const T &value) {
		// reallocate
		if (size == capacity) {
			capacity = capacity == 0 ? 1 : capacity * 2;
			T* new_data = new T[capacity];
			for (size_t i = 0; i < size; i++) {
				new_data[i] = data[i];
			}
			delete[] data;
			data = new_data;
		}

		// assign
		data[size++] = value;
	}

private:
	T *data = nullptr;
	size_t size = 0;
	size_t capacity = 0;
};
