#include "test_runner.h"
#include "profile.h"

#include <algorithm>
#include <deque>
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

// GroupLines - набор строк, обрабатываемых одним потоком
using GroupLines = vector<string>;

Stats ExploreGroupLines(const set<string> &key_words, const GroupLines &lines) {
    Stats stats;

    for (const string &line : lines) {
        istringstream is(line);
        string word;
        while (is >> word) {
            if (key_words.count(word) != 0) {
                stats.word_frequences[move(word)]++;
            }
        }
    }

    return stats;
}

Stats ExploreKeyWords(const set<string> &key_words, istream &input) {
    const size_t GROUP_SIZE = 5000;

    vector<future<Stats>> futures;

    deque<GroupLines> deque_groups;
    while (true) {
        GroupLines current_group;
        current_group.reserve(GROUP_SIZE);

        string input_line;
        for (size_t count = 0; count < GROUP_SIZE && getline(input, input_line); count++) {
            current_group.push_back(move(input_line));
        }
        deque_groups.push_back(move(current_group));

        if (deque_groups.back().empty()) { break; }

        futures.push_back(async(
                ExploreGroupLines, ref(key_words), ref(deque_groups.back())
        ));
    }

    Stats stats;
    for (auto &f : futures) {
        stats += f.get();
    }

    return stats;
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
