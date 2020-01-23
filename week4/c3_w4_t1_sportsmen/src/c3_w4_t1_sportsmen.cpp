#include <iostream>
#include <list>
#include <vector>
using namespace std;

int main() {
	int q;
	cin >> q;
	list<int> res;
	vector<list<int>::iterator> positions(100001, res.end());  // if before_who absent - then by default, to the res.end(). But is res.end() invalidated??
	for (int i = 0; i < q; ++i) {
		int value, before_who;
		cin >> value >> before_who;

		positions[value] = res.insert(positions[before_who], value);
	}
	for (int v : res) {
		cout << v << " ";
	}
	return 0;
}
