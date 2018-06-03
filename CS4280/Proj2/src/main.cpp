#include "lib/scanner.hpp"
#include "lib/parser.hpp"
#include <iostream>
using namespace std;

const string ERR_INV_INPUT = "PROGRAM ERROR: Invalid input. Supply the name of a single file.\n";

void print_tokens(vector<token> &tokens_);

int main(int argc, char** argv) {
  if (argc != 2) {
    cout << ERR_INV_INPUT;
    return 1;
  }

  //Scanner Initialization
  Scanner my_scanner(argv[1]);
  if (my_scanner.error_detected() != 0) return 1;

  my_scanner.parse_file();
  if (my_scanner.error_detected() != 0) return 1;


  //Parsing
  Parser my_parser(&my_scanner);

  parse_tree_node* root = my_parser.parse();

  if (my_parser.error_detected()) {
    cout << "PARSER: Parse unsuccessful.\n";
  }
  else {
    my_parser.print_tree(root, 0);
  }

  //Free up parse tree memory
  my_parser.del_tree();
  return 0;
}

void print_tokens(vector<token> &tokens_) {
  for (auto &t: tokens_) {
    cout << t.instance << " | TYPE: " << TOKEN_NAMES.at(t.id) << " | LINE: " << t.line_num << "\n";
  }
}
