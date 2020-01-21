#include <cstdint>
#include <iostream>
#include <map>
#include <queue>
#include <string>

using namespace std;

struct Booking {
	int64_t time;
	int client_id;
	int room_count;
};

struct Hotel {
	queue<Booking> bookings;
	int booked_rooms = 0;
	map<int, int64_t> last_clients;
};

class BookingManager {
public:
	void Book(int64_t time, const string &hotel_name, int client_id, int room_count) {
		actual_time = time;
		Hotel &h = hotels[hotel_name];
		h.bookings.push( { time, client_id, room_count });
		h.booked_rooms += room_count;
		h.last_clients[client_id] = time;
	}

	int Clients(const string &hotel_name) {
		auto it_hotel = hotels.find(hotel_name);
		if (it_hotel == hotels.end()) {
			return 0;
		}
		Hotel &h = it_hotel->second;
		actualize(h);

		return h.last_clients.size();
	}

	int Rooms(const string &hotel_name) {
		auto it_hotel = hotels.find(hotel_name);
		if (it_hotel == hotels.end()) {
			return 0;
		}
		Hotel &h = it_hotel->second;
		actualize(h);

		return h.booked_rooms;
	}

private:
	void actualize(Hotel &h) {
		if (h.bookings.empty()) {
			return;
		}
		while (!h.bookings.empty() && h.bookings.front().time <= actual_time - 86400) {
			Booking &b = h.bookings.front();

			h.booked_rooms -= b.room_count;

			auto last_time_it = h.last_clients.find(b.client_id);
			if (last_time_it->second == b.time) {
				h.last_clients.erase(last_time_it);
			}

			h.bookings.pop();
		}
	}

	map<string, Hotel> hotels;
	int64_t actual_time;
};

int main() {
	// Для ускорения чтения данных отключается синхронизация
	// cin и cout с stdio,
	// а также выполняется отвязка cin от cout
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	BookingManager manager;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;

		if (query_type == "BOOK") {
			int64_t time;
			cin >> time;
			string hotel_name;
			cin >> hotel_name;
			int client_id;
			cin >> client_id;
			int room_count;
			cin >> room_count;
			manager.Book(time, hotel_name, client_id, room_count);
		} else if (query_type == "CLIENTS") {
			string hotel_name;
			cin >> hotel_name;
			cout << manager.Clients(hotel_name) << "\n";
		} else if (query_type == "ROOMS") {
			string hotel_name;
			cin >> hotel_name;
			cout << manager.Rooms(hotel_name) << "\n";
		}
	}

	return 0;
}
