#include <string>
#include <vector>
using namespace std;

#define g(line_no) UNIQ_ID_ ## line_no
#define f(line_no) g(line_no)
#define UNIQ_ID f(__LINE__)

int main() {
  int UNIQ_ID = 0;
  string UNIQ_ID = "hello";
  vector<string> UNIQ_ID = {"hello", "world"};
  vector<int> UNIQ_ID = {1, 2, 3, 4};
}
