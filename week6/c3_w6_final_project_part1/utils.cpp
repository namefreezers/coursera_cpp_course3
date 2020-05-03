#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<string> SplitIntoWords(const string &line) {
    istringstream words_input(line);
    return {istream_iterator<string>(words_input), istream_iterator<string>()};
}
