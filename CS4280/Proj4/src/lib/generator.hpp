#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "parser.hpp"
#include <fstream>
using namespace std;

class Generator {
public:
  Generator(string _file_name, parse_tree_node* _tree);
  void run();
  int err_check();

private:
  parse_tree_node* tree;
  string out_fname;
  int err_flag;
  int temp_counter;
  int br_counter;

  ofstream out_file;
  string out_code;
  vector<string> variables;
  vector<string> out_code_lines;

  //Compilation process functions
  string process_fname(string in_fname);
  void code_gen(parse_tree_node* node);
  void print_code();
  void print_vars();
  void optimize();

  //Optimization functions
  void load_store_opt();
  void post_process_str();
  vector<string> split_line(const string &s, char delim);
};

#endif
