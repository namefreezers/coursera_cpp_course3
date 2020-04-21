#include "test_runner.h"
#include <algorithm>
#include <functional>
#include <iterator>
//#include <memory>
#include <vector>

using namespace std;

template<typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    auto len = range_end - range_begin;
    if (len < 2) {
        return;
    }
    vector<typename RandomIt::value_type> v1, v2, v3, temp;
    move(range_begin, range_begin + len / 3, back_inserter(v1));
    move(range_begin + len / 3, range_begin + len * 2 / 3, back_inserter(v2));
    move(range_begin + len * 2 / 3, range_end, back_inserter(v3));
    for (vector<typename RandomIt::value_type> &v : {ref(v1), ref(v2), ref(v3)}) {
        MergeSort(begin(v), end(v));
    }
    merge(
            make_move_iterator(begin(v1)), make_move_iterator(end(v1)),
            make_move_iterator(begin(v2)), make_move_iterator(end(v2)),
            back_inserter(temp)
    );
    merge(
            make_move_iterator(begin(temp)), make_move_iterator(end(temp)),
            make_move_iterator(begin(v3)), make_move_iterator(end(v3)),
            range_begin
    );
}

void TestIntVector() {
    vector<int> numbers = {6, 1, 3, 9, 1, 9, 8, 12, 1};
    MergeSort(begin(numbers), end(numbers));
    ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestIntVector);
    return 0;
}
