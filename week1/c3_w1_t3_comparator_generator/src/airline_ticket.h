#pragma once

#include <string>
using namespace std;

struct Date {
  int year, month, day;
};

struct Time {
  int hours, minutes;
};

struct AirlineTicket {
  string from;
  string to;
  string airline;
  Date departure_date;
  Time departure_time;
  Date arrival_date;
  Time arrival_time;
  int price;
};

bool operator< (const Date& l, const Date& r);
bool operator< (const Time& l, const Time& r);

bool operator== (const Date& l, const Date& r);
bool operator== (const Time& l, const Time& r);

ostream& operator<< (ostream& os, const Date& d);
ostream& operator<< (ostream& os, const Time& t);
