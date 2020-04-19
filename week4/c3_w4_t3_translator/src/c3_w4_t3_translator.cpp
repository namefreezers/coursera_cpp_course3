#include "test_runner.h"
#include <deque>
#include <map>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

class Translator {
public:
	void Add(string_view source, string_view target) {
		map<string_view, string_view>::iterator it_fw_map = get_existing_sv_or_insert_into_map_as_key(source, fw_map);
		map<string_view, string_view>::iterator it_bw_map = get_existing_sv_or_insert_into_map_as_key(target, bw_map);

		it_fw_map->second = it_bw_map->first;
		it_bw_map->second = it_fw_map->first;
	}
	string_view TranslateForward(string_view source) const {
		return get_res(source, fw_map);
	}
	string_view TranslateBackward(string_view target) const {
		return get_res(target, bw_map);
	}
private:
	map<string_view, string_view>::iterator get_existing_sv_or_insert_into_map_as_key(string_view sv_key, map<string_view, string_view> &to) {
		map<string_view, string_view>::iterator it_to_insert = to.lower_bound(sv_key);
		bool s_exists = (it_to_insert != to.end()) && (it_to_insert->first == sv_key);
		if (!s_exists) {
			it_to_insert = to.insert(it_to_insert, { words_data.emplace_back(sv_key), { } });
		}
		return it_to_insert;
	}

	string_view get_res(string_view source, const map<string_view, string_view> &dict) const {
		auto it = dict.find(source);
		if (it == dict.end()) {
			return {};
		}
		return it->second;
	}

	deque<string> words_data;
	map<string_view, string_view> fw_map, bw_map;
};

void TestSimple() {
	Translator translator;
	translator.Add(string("okno"), string("window"));
	translator.Add(string("stol"), string("table"));

	ASSERT_EQUAL(translator.TranslateForward("okno"), "window");
	ASSERT_EQUAL(translator.TranslateBackward("table"), "stol");
	ASSERT_EQUAL(translator.TranslateBackward("stol"), "");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSimple);
	return 0;
}
