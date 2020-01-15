#include "airline_ticket.h"
#include "test_runner.h"

using namespace std;

#define UPDATE_FIELD(ticket, field, values) { 	\
		auto it = values.find(#field);       	\
		if (it != values.end()) {				\
			istringstream is(it->second);		\
			is >> ticket.field;					\
	}											\
}


bool operator< (const Date& l, const Date& r) {
	return tie(l.year, l.month, l.day) < tie(r.year, r.month, r.day);
}

bool operator< (const Time& l, const Time& r) {
	return tie(l.hours, l.minutes) < tie(r.hours, r.minutes);
}

bool operator== (const Date& l, const Date& r) {
	return tie(l.year, l.month, l.day) == tie(r.year, r.month, r.day);
}

bool operator== (const Time& l, const Time& r) {
	return tie(l.hours, l.minutes) == tie(r.hours, r.minutes);
}

istream& operator>> (istream& is, Date& d) {
	char temp_char;
	return is >> d.year >> temp_char >> d.month >> temp_char >> d.day;
}

istream& operator>> (istream& is, Time& t) {
	char temp_char;
	return is >> t.hours >> temp_char >> t.minutes;
}

ostream& operator<< (ostream& os, const Date& d) {
	return os << d.year << '-' << d.month << '-' << d.day;
}

ostream& operator<< (ostream& os, const Time& t) {
	return os << t.hours << ':' << t.minutes;
}

void TestUpdate() {
    AirlineTicket t;
    t.price = 0;

    const map<string, string> updates1 = {
        {"departure_date", "2018-2-28"},
        {"departure_time", "17:40"},
    };
    UPDATE_FIELD(t, departure_date, updates1);
    UPDATE_FIELD(t, departure_time, updates1);
    UPDATE_FIELD(t, price, updates1);

    ASSERT_EQUAL(t.departure_date, (Date{2018, 2, 28}));
    ASSERT_EQUAL(t.departure_time, (Time{17, 40}));
    ASSERT_EQUAL(t.price, 0);

    const map<string, string> updates2 = {
        {"price", "12550"},
        {"arrival_time", "20:33"},
    };
	UPDATE_FIELD(t, departure_date, updates2);
	UPDATE_FIELD(t, departure_time, updates2);
	UPDATE_FIELD(t, arrival_time, updates2);
	UPDATE_FIELD(t, price, updates2);

	// updates2 не содержит ключей "departure_date" и "departure_time", поэтому
	// значения этих полей не должны измениться
	ASSERT_EQUAL(t.departure_date, (Date { 2018, 2, 28 }));
	ASSERT_EQUAL(t.departure_time, (Time { 17, 40 }));
	ASSERT_EQUAL(t.price, 12550);
	ASSERT_EQUAL(t.arrival_time, (Time { 20, 33 }));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestUpdate);
}
