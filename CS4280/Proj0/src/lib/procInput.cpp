#include "procInput.hpp"
#include <fstream>
#include <sstream>
using namespace std;

vector<string> split(const string& s, char delimiter)
{
   vector<std::string> tokens;
   string token;
   istringstream tokenStream(s);
   while (getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

int is_num(const string &str) {
  char * pEnd;
  int res;
  res = strtol(str.c_str(), &pEnd, 10);

  if (*pEnd != '\0') return -1;

  return res;
}

bool file_exists(const string &filename) {
  ifstream ifs(filename);
  return (bool)ifs;
}

bool neg_check(vector<int> &nums) {
  for (auto i : nums) {
    if (i < 0) return false;
  }
  return true;
}

bool parse_nums(int argc, char** args, vector<int> &container) {
  int res;
  for (int i = 1; i < argc; i++) {
    if ( (res = is_num(args[i])) == -1) return false;
    container.push_back(res);
  }

  return true;
}

bool parse_nums_file(ifstream &infile, vector<int> &container) {
  std::string str;
  vector<string> temp;
  int res;

  while (std::getline(infile, str)) {
    temp = split(str, ' ');
    for (int i = 0; i < (int)temp.size(); i++) {
      if ( (res = is_num(temp[i])) == -1) return false;
      container.push_back(res);
    }
  }

  if (container.size() > MAX_NUM_INTS || container.size() < MIN_NUM_INTS) return false;

  return true;
}

bool treat_as_file(string fname) {
  char in;
  cout << "File detected. Treat " << fname << " as a file? Y or N: ";
  cin >> in;
  while (1) {
    switch((char)tolower(in)) {
      case 'y' :
        return true;
      case 'n' :
        return false;
      default :
        cout << "Invalid input. Enter Y or N: ";
        cin >> in;
        break;
    }
  }
}
