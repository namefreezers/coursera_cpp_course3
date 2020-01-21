#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

class RouteManager {
public:
	void AddRoute(int start, int finish) {
		reachable_lists_[start].insert(finish);
		reachable_lists_[finish].insert(start);
	}
	int FindNearestFinish(int start, int finish) const {
		int result = abs(start - finish);
		auto it_start = reachable_lists_.find(start);
		if (it_start == reachable_lists_.end()) {
			return result;
		}

		const set<int> &routes_to = it_start->second;
		auto l_b_it = routes_to.lower_bound(finish);
		if (l_b_it != routes_to.end() && abs(*l_b_it - finish) < result) {
			result = abs(*l_b_it - finish);
		}
		if (l_b_it != routes_to.begin() && abs(*(--l_b_it) - finish) < result) {
			result = abs(*l_b_it - finish);
		}

		return result;
	}
private:
	map<int, set<int>> reachable_lists_;
};

int main() {
	RouteManager routes;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;
		int start, finish;
		cin >> start >> finish;
		if (query_type == "ADD") {
			routes.AddRoute(start, finish);
		} else if (query_type == "GO") {
			cout << routes.FindNearestFinish(start, finish) << "\n";
		}
	}

	return 0;
}
