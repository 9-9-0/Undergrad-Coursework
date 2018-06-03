#include "lib/scanner.hpp"
#include <iostream>
using namespace std;

const string ERR_INV_INPUT = "PROGRAM ERROR: Invalid input. Supply the name of a single file.\n";

void print_tokens(vector<token> &tokens_);

int main(int argc, char** argv) {
  if (argc != 2) {
    cout << ERR_INV_INPUT;
    return 1;
  }

  Scanner my_scanner(argv[1]);
  if (my_scanner.error_detected() != 0) return 1;

  my_scanner.parse_file();
  if (my_scanner.error_detected() != 0) return 1;

  vector<token> tokens = my_scanner.testScanner();
  if (my_scanner.error_detected() != 0) return 1;

  print_tokens(tokens);

  return 0;
}

void print_tokens(vector<token> &tokens_) {
  for (auto &t: tokens_) {
    cout << t.instance << " | TYPE: " << TOKEN_NAMES.at(t.id) << " | LINE: " << t.line_num << "\n";
  }
}
