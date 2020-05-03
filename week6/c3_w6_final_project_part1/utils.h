#pragma once
#include <cstdint>
#include <string>
#include <vector>

using namespace std;

using Count = int;
using WordId = uint16_t;
using DocId = uint16_t;

vector<string> SplitIntoWords(const string &line);