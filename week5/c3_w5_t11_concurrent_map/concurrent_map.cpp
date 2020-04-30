#include "test_runner.h"
#include "profile.h"

#include <algorithm>
#include <future>
#include <numeric>
#include <mutex>
#include <random>
#include <string>
#include <vector>

using namespace std;

template<typename K, typename V>
class ConcurrentMap {
public:
    static_assert(is_integral_v<K>, "ConcurrentMap supports only integer keys");

    struct Access {
        lock_guard<mutex> guard;
        V &ref_to_value;
    };

    explicit ConcurrentMap(size_t bucket_count);

    Access operator[](const K &key);

    map<K, V> BuildOrdinaryMap();

private:
    size_t bucket_count;
    vector<map<K, V>> sub_maps;
    vector<mutex> mutexes;
};

template<typename K, typename V>
ConcurrentMap<K, V>::ConcurrentMap(size_t bucket_count) : bucket_count(bucket_count),
                                                          sub_maps(vector<map<K, V>>(bucket_count)),
                                                          mutexes(vector<mutex>(bucket_count)) {}

template<typename K, typename V>
typename ConcurrentMap<K, V>::Access ConcurrentMap<K, V>::operator[](const K &key) {
    auto ind = abs(static_cast<long long>(key)) % bucket_count;
//    auto ind = abs(key) % bucket_count;
    return {lock_guard<mutex>(mutexes[ind]), sub_maps[ind][key]};
}

template<typename K, typename V>
map<K, V> ConcurrentMap<K, V>::BuildOrdinaryMap() {
    map<K, V> result;
    for (auto[sub_map, m] = make_tuple(sub_maps.begin(), mutexes.begin()); sub_map != sub_maps.end(); sub_map++, m++) {
        lock_guard<mutex> lg(*m);
        result.insert(sub_map->begin(), sub_map->end());
    }
    return result;
}

void RunConcurrentUpdates(
        ConcurrentMap<int, int> &cm, size_t thread_count, int key_count
) {
    auto kernel = [&cm, key_count](int seed) {
        vector<int> updates(key_count);
        iota(begin(updates), end(updates), -key_count / 2);
        shuffle(begin(updates), end(updates), default_random_engine(seed));

        for (int i = 0; i < 2; ++i) {
            for (auto key : updates) {
                cm[key].ref_to_value++;
            }
        }
    };

    vector<future<void>> futures;
    for (size_t i = 0; i < thread_count; ++i) {
        futures.push_back(async(kernel, i));
    }
}

void TestConcurrentUpdate() {
    const size_t thread_count = 3;
    const size_t key_count = 50000;

    ConcurrentMap<int, int> cm(thread_count);
    RunConcurrentUpdates(cm, thread_count, key_count);

    const auto result = cm.BuildOrdinaryMap();
    ASSERT_EQUAL(result.size(), key_count);
    for (auto&[k, v] : result) {
        AssertEqual(v, 6, "Key = " + to_string(k));
    }
}

void TestReadAndWrite() {
    ConcurrentMap<size_t, string> cm(5);

    auto updater = [&cm] {
        for (size_t i = 0; i < 50000; ++i) {
            cm[i].ref_to_value += 'a';
        }
    };
    auto reader = [&cm] {
        vector<string> result(50000);
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = cm[i].ref_to_value;
        }
        return result;
    };

    auto u1 = async(updater);
    auto r1 = async(reader);
    auto u2 = async(updater);
    auto r2 = async(reader);

    u1.get();
    u2.get();

    for (auto f : {&r1, &r2}) {
        auto result = f->get();
        ASSERT(all_of(result.begin(), result.end(), [](const string &s) {
            return s.empty() || s == "a" || s == "aa";
        }));
    }
}

void TestSpeedup() {
    {
        ConcurrentMap<int, int> single_lock(1);

        LOG_DURATION("Single lock");
        RunConcurrentUpdates(single_lock, 4, 50000);
    }
    {
        ConcurrentMap<int, int> many_locks(100);

        LOG_DURATION("100 locks");
        RunConcurrentUpdates(many_locks, 4, 50000);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConcurrentUpdate);
    RUN_TEST(tr, TestReadAndWrite);
    RUN_TEST(tr, TestSpeedup);
}
