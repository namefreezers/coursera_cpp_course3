#include "test_runner.h"
#include <algorithm>
#include <future>
#include <numeric>
#include <vector>

using namespace std;

template<typename T>
T calculate_matrix_range_sum(const vector<vector<int>> &matrix, const size_t cur_start, const size_t page_size) {
    const size_t last_row = min(cur_start + page_size, matrix.size());

    T res = 0;
    for (size_t row_idx = cur_start; row_idx < last_row; row_idx++) {
        const vector<int> &row = matrix[row_idx];
        res += accumulate(row.begin(), row.end(), static_cast<T>(0));
    }
    return res;
}

int64_t CalculateMatrixSum(const vector<vector<int>> &matrix) {
    using T = int64_t;

    const size_t thread_amount = 4;
    const size_t sz = matrix.size();
    const size_t page_size = (sz - 1) / thread_amount + 1;

    vector<future<T>> futures;
    for (size_t cur_start = 0; cur_start < sz; cur_start += page_size) {
        futures.push_back(async([&matrix, cur_start, page_size] {
            return calculate_matrix_range_sum<T>(matrix, cur_start, page_size);
        }));
    }

    T res = 0;
    for (auto &f : futures) {
        res += f.get();
    }
    return res;
}

void TestCalculateMatrixSum() {
    const vector<vector<int>> matrix = {
            {1,  2,  3,  4},
            {5,  6,  7,  8},
            {9,  10, 11, 12},
            {13, 14, 15, 16}
    };
    ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestCalculateMatrixSum);
}
