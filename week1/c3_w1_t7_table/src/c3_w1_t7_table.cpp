#include "test_runner.h"

#include <vector>
using namespace std;

template<typename T>
class Table {
public:
	Table(size_t new_h, size_t new_w) {
		Resize(new_h, new_w)
	}

	vector<T>& operator[](int row_ind) {
		return data[row_ind];
	}

	const vector<T>& operator[](int row_ind) const {
		return data[row_ind];
	}

	void Resize(size_t new_h, size_t new_w) {
		h = new_h;
		w = new_w;
		data.resize(new_h);
		for (vector<T>& row : data) {
			row.resize(new_w);
		}
	}

	pair<size_t, size_t> Size() const {
		return pair<size_t, size_t>(h, w);
	}

private:
	size_t h, w;
	vector<vector<T>> data;
};

void TestTable() {
	Table<int> t(1, 1);
	ASSERT_EQUAL(t.Size().first, 1u);
	ASSERT_EQUAL(t.Size().second, 1u);
	t[0][0] = 42;
	ASSERT_EQUAL(t[0][0], 42);
	t.Resize(3, 4);
	ASSERT_EQUAL(t.Size().first, 3u);
	ASSERT_EQUAL(t.Size().second, 4u);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestTable);
	return 0;
}
