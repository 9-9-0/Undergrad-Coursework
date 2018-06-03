#include "scanner.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

/*
 *  Initializes the scanner with:
 *  1) Input file name
 *  2) Error message hashmap
 *  3) sets initial error flags
 */
Scanner::Scanner(const string &file_name_):
  FILE_NAME(file_name_),
  err_flag(file_exists(file_name_)),
  line_pos(0),
  lookahead_pos(-1),
  err_line(0),
  err_char('\0'),
  newline_ack(false),
  pause_lookahead(false)
  {}

/******************************************************************************/

/*
 *  Given that our input file is valid, perform the following:
 *  1) Process the file into a vector<string>, elements of which are each line's contents
 *  2) Run the comment filter over each line's contents, erasing the characters encapsulated by '!'.
       Report errors if detected.
 *  3) Run the invalid character filter over each line's content.
 */
void Scanner::parse_file() {
  file_to_lines(lines);

  filter_comments(lines);
  if (err_flag) return;

  filter_invalids(lines);
  if (err_flag) return;
}

/*
 *  An error checking function to be called after scanner state changes, invoked by such functions:
 *  parse_file()
 *  fetch_token()
 */
int Scanner::error_detected() {
  if (err_flag != 0) {
    cout << ERROR_MAP.at(err_flag);

    //Additional Error Messages
    switch (err_flag) {
      case 3:
        cout << "line " << err_line << endl;
        break;
      case 4:
      case 5:
        cout << err_char << " at line " << err_line << endl;
        break;
    }
  }
  return err_flag;
}

/*
 *  "Testing function" as dictated by the specs
 */
vector<token> Scanner::testScanner() {
  vector<token> tokens;
  token next_token;

  while ( (next_token = fetch_token()).id != 5) {
    if (err_flag) return tokens;

    tokens.push_back(next_token);
  }

  //Push back the EOF token
  tokens.push_back(next_token);
  return tokens;
}

/******************************************************************************/

const unordered_map<int, string> Scanner::ERROR_MAP =
{
  {1, "PROGRAM ERROR: File not found.\n" },
  {2, "PROGRAM ERROR: Invalid input. Supply the name of a single file.\n"},
  {3, "PREPROCESSOR ERROR: Unmatched comment at "},
  {4, "PREPROCESSOR ERROR: Invalid character "},
  {5, "SCANNER ERROR: No token starting with uppercase char "}
};

const vector<vector<int>> Scanner::FSA_GRAPH =
{
  {1, -1, 3, 2, 0, 0, 1000},
  {1, 1, 1, 1001, 1001, 1001, 1001},
  {1002, 1002, 1002, 1002, 1002, 1002, 1002},
  {1003, 1003, 3, 1003, 1003, 1003, 1003}
};

//Mappings from final states to token IDs as defined in token.hpp
const unordered_map<int, int> Scanner::FINAL_STATES =
{
  {1000, 5},
  {1001, 1},
  {1002, 4},
  {1003, 2}
};

//Error States and their corresponding err_flag value.
const unordered_map<int, int> Scanner::ERROR_STATES =
{
  {-1, 5}
};
/******************************************************************************/

/*
 *  Called in constructor's initialization list to determine if the file is valid or not.
 */
int Scanner::file_exists(const string &file_name_) {
  ifstream ifs(file_name_);
  bool temp = (bool)ifs;

  return (temp != 1);
}

/*
 *  Parses the file into a vector<string> of lines.
 */
void Scanner::file_to_lines(vector<string> &lines_) {
  ifstream in_file;
  in_file.open(FILE_NAME);

  string line;
  stringstream lineStream;
  lineStream << in_file.rdbuf();
  while (getline(lineStream, line, '\n')) {
    lines_.push_back(line);
  }
}

void Scanner::filter_comments(vector<string> &lines_) {
  bool comment_switch = false; //True when a ! is detected, false when a ! gets matched.

  for (uint i = 0; i < lines_.size(); i++) {
    int com_pos = -1, start_pos = 0, end_pos = lines_[i].length() - 1;

    while ( (com_pos = lines_[i].find('!', com_pos + 1)) != -1) {
      if (comment_switch) {
        comment_switch = false;
        end_pos = com_pos;
        lines_[i].erase(start_pos, end_pos + 1 - start_pos);
        //Reset Indices
        com_pos = -1;
        start_pos = 0;
        end_pos = lines_[i].length() - 1;
      }
      else {
        comment_switch = true;
        err_line = i + 1; //Save the line of the starting '!'
        start_pos = com_pos;
      }
    }

    if (comment_switch) {
      lines_[i].erase(start_pos, end_pos + 1 - start_pos);
    }

    //cout << "Line " << i + 1 << " " << lines_[i] << endl;
  }

  if (comment_switch) err_flag = 3;
}

/*
 *  After filtering out comments, the contents of each line are checked for validity.
 *  The valid characters are defined in token.hpp
 */
void Scanner::filter_invalids(vector<string> &lines_) {
  for (uint i = 0; i < lines_.size(); i++) {
    err_line = i + 1;
    for (auto& c : lines_[i]) {
      if (!ALPHA_LOW.count(c) && !ALPHA_UPPER.count(c) && !DIGIT.count(c) && !VALID_OPS.count(c) && !WHITE_SPACES.count(c)) {
        err_flag = 4;
        err_char = c;
        return;
      }
    }
  }
}

/******************************************************************************/

/*
 *  This function is to be called until EOF is hit, at which point subsequent calls will keep returning the EOF token.
 *  str.append() will throw an exception if overflown. Catching not implemented yet.
 */
token Scanner::fetch_token() {
  token out;

  int curr_state = INITIAL_STATE;
  int next_state;
  string token_instance;

  //EOF is simply represented as all character of all lines read
  while (FINAL_STATES.find(curr_state) == FINAL_STATES.end()) {
    char next_char;
    if (!pause_lookahead) {
      next_char = (char)fetch_next_char();
    }
    else {
      pause_lookahead = false;
      next_char = last_char;
    }

    /*
    cout << "next_char: " << next_char << endl;
    cout << "curr_state: " << curr_state << endl;
    cout << "next col: " << char_to_col(next_char) << endl;
    */

    next_state = FSA_GRAPH[curr_state][char_to_col(next_char)];

    //cout << "next state: " << next_state << endl;

    if (ERROR_STATES.find(next_state) != ERROR_STATES.end()) {
      err_flag = ERROR_STATES.at(next_state);
      err_char = next_char;
      err_line = line_pos + 1;
      return token{ERR_tk, "Error.", line_pos + 1}; //This token isn't explicity used for anything as of now
    }

    //Final State Detected
    if (FINAL_STATES.find(next_state) != FINAL_STATES.end()) {
      pause_lookahead = true;
      last_char = next_char;
      int fin_tk_id = FINAL_STATES.find(next_state)->second;

      //Check if it's a keyword
      if (KEYWORDS.count(token_instance)) {
        return
        token {
          KW_tk,
          token_instance,
          line_pos + 1
        };
      }
      else {
        return
        token {
          static_cast<T_ID>(FINAL_STATES.at(next_state)),
          token_instance,
          line_pos + 1
        };
      }
    }
    else {
      curr_state = next_state;
      if (next_char != ' ' && next_char != '\n') token_instance.append(string(1, next_char));
    }
  }
}

//Needs testing
//Currently set up so that newline terminates
int Scanner::fetch_next_char() {
  //If on last call, a new line was detected
  if (newline_ack) {
    newline_ack = false;
    line_pos++;
  }

  //EOF char return case
  if (line_pos >= lines.size()) return -1;

  lookahead_pos++;
  if (lookahead_pos >= lines[line_pos].size()) {
    lookahead_pos = -1;
    newline_ack = true;
    return '\n';
  }

  return (int)lines[line_pos][lookahead_pos];
}

/*
 *  Returns the column associated with class of character
 */
int Scanner::char_to_col(char c) {
  if (ALPHA_LOW.count(c)) return 0;
  if (ALPHA_UPPER.count(c)) return 1;
  if (DIGIT.count(c)) return 2;
  if (VALID_OPS.count(c)) return 3;
  if (WHITE_SPACES.count(c)) return 4;
  if (c == '\n') return 5;
  if ((int)c == -1) return 6;
}
