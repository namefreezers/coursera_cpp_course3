#include "test_runner.h"
#include "profile.h"

#include <algorithm>
#include <future>
#include <map>
#include <string>
#include <string_view>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats &other) {
        for (const auto &k_v : other.word_frequences) {
            word_frequences[k_v.first] += k_v.second;
        }
    }
};

Stats ExploreLine(const set<string> &key_words, const string &line) {
    string_view line_sv(line);
    size_t end_of_cur;
    while ((end_of_cur = line_sv.find(' ')) != string_view::npos) {
        key_words.count(line_sv.substr(0, end_of_cur));

    }
    return {};
}

Stats ExploreKeyWordsSingleThread(
        const set<string> &key_words, istream &input
) {
    Stats result;
    for (string line; getline(input, line);) {
        result += ExploreLine(key_words, line);
    }
    return result;
}

Stats ExploreKeyWords(const set<string> &key_words, istream &input) {
    vector<future<Stats>> futures;
    string s;
    while (getline(input, s)) {

    }
    return {};
}

void TestBasic() {
    const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    const auto stats = ExploreKeyWords(key_words, ss);
    const map<string, int> expected = {
            {"yangle", 6},
            {"rocks",  2},
            {"sucks",  1}
    };
    ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestBasic);
}

//#include <functional>
//#include <iostream>
//#include <vector>
//
//using namespace std;
//
//void f(vector<int>& v) {
//    v.push_back(5);
//    cout << v.size();
//}
//
//int main() {
//    const vector<int> v = {1,2,3};
//    f(ref(v));
//    cout << v.size();
//}