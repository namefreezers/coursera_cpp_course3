#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <utility>

using namespace std;

class ReadingManager {
public:
	void Read(int user_id, int new_page_count) {
		if (user_pages[user_id] == -1) {  // new user
			for (int p = new_page_count + 1; p <= 1000; p++) {
				pages_users_before[p]++;
			}
			user_pages[user_id] = new_page_count;
			users++;
		} else { // known user
			for (int p = user_pages[user_id] + 1; p <= new_page_count; p++) {
				pages_users_before[p]--;
			}
			user_pages[user_id] = new_page_count;
		}
	}

	double Cheer(int user_id) const {
		if (user_pages[user_id] == -1) {
			return 0;
		}
		if (users == 1) {
			return 1;
		}
		return static_cast<double>(pages_users_before[user_pages[user_id]]) / (users - 1);
	}

private:
	vector<int> user_pages = vector<int>(100001, -1);
	vector<int> pages_users_before = vector<int>(1001);
	int users = 0;
};

int main() {
	// Для ускорения чтения данных отключается синхронизация
	// cin и cout с stdio,
	// а также выполняется отвязка cin от cout
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	ReadingManager manager;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;
		int user_id;
		cin >> user_id;

		if (query_type == "READ") {
			int page_count;
			cin >> page_count;
			manager.Read(user_id, page_count);
		} else if (query_type == "CHEER") {
			cout << setprecision(6) << manager.Cheer(user_id) << "\n";
		}
	}

	return 0;
}
