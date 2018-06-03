#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "token.hpp"
#include <vector>
using namespace std;

const int INITIAL_STATE = 0;

class Scanner {
public:
  Scanner(const string &file_name_);

  void parse_file();
  int error_detected();
  vector<token> testScanner();

private:
  const string FILE_NAME;

  //File Content Related
  vector<string> lines;
  vector<token> tokens;

  //For place keeping in the vector<string> of lines.
  //Changes on each successful fetch_token() call.
  //Refactor this later
  int line_pos;
  int lookahead_pos;
  char last_char;
  bool newline_ack;
  bool pause_lookahead;
  string curr_str_instance;

  //FSA Table Related
  static const vector<vector<int>> FSA_GRAPH;
  static const unordered_map<int, int> FINAL_STATES;
  static const unordered_map<int, int> ERROR_STATES;

  //Error Related
  static const unordered_map<int, string> ERROR_MAP;
  int err_flag;
  int err_line;
  char err_char;

  //Preprocessor Functions
  int file_exists(const string &file_name);
  void file_to_lines(vector<string> &lines_);
  void filter_comments(vector<string> &lines_);
  void filter_invalids(vector<string> &lines_);
  void lines_to_tokens(vector<string> &lines_, vector<string> &tokens_); //This might not be needed...scrap later

  //Scanning functions
  token fetch_token();
  int fetch_next_char();
  int char_to_col(char c);
};

#endif
