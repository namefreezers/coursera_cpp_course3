#include "stats.h"
#include "http_request.h"

#include <iostream>
#include <sstream>
#include <string_view>
using namespace std;

Stats::Stats() {
	methods = { { "GET", 0 }, { "PUT", 0 }, { "POST", 0 }, { "DELETE", 0 }, { "UNKNOWN", 0 }, };
	uris = { { "/", 0 }, { "/order", 0 }, { "/product", 0 }, { "/basket", 0 }, { "/help", 0 }, { "unknown", 0 }, };
}

void Stats::AddMethod(string_view method) {
	if (auto existing_method_it = methods.find(method); existing_method_it != methods.end()) {  // if it is among known
		existing_method_it->second++;
	} else {																					// if it is NOT among known
		methods["UNKNOWN"]++;
	}
}

void Stats::AddUri(string_view uri) {
	if (auto existing_uri_it = uris.find(uri); existing_uri_it != uris.end()) {		// if it is among known
		existing_uri_it->second++;
	} else {																		// if it is NOT among known
		uris["unknown"]++;
	}
}

const map<string_view, int>& Stats::GetMethodStats() const {
	return methods;
}

const map<string_view, int>& Stats::GetUriStats() const {
	return uris;
}

HttpRequest ParseRequest(string_view line) {
	HttpRequest res;

	size_t pos_start_method = line.find_first_not_of(' ');
	line.remove_prefix(pos_start_method);
	size_t pos_end_method = line.find(' ');
	res.method = line.substr(0, pos_end_method);
	line.remove_prefix(pos_end_method);

	size_t pos_start_uri = line.find_first_not_of(' ');
	line.remove_prefix(pos_start_uri);
	size_t pos_end_uri = line.find(" ");
	res.uri = line.substr(0, pos_end_uri);
	line.remove_prefix(pos_end_uri);

	size_t pos_start_protocol = line.find_first_not_of(' ');
	line.remove_prefix(pos_start_protocol);
	size_t pos_end_protocol = line.find(" ");
	res.protocol = line.substr(0, pos_end_protocol);
	line.remove_prefix(pos_end_protocol);

	return res;
}
