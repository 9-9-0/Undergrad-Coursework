#include "incrDiv.hpp"
using namespace std;

void incr_div(vector<int> &nums, ofstream &out) {
  string temp;

  for (auto i : nums) {
    temp = "";
    incr_div(temp, i, 2, 0);
    out << temp;
  }
}

void incr_div(string &out, int a, int b, int num_calls) {
  if (a == 0) {
    out += "numCalls = " + to_string(num_calls) + "\n";
    return;
  }

  a = a / b;
  out += to_string(a) + " ";
  num_calls++;
  incr_div(out, a, b + 1, num_calls);
}
