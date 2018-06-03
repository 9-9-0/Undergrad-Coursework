#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <unordered_set>
#include <unordered_map>
using namespace std;

//Corresponding type names for the numerations
const unordered_map<int, string> TOKEN_NAMES =
  {
    {0, "ERROR"},
    {1, "Identifier"},
    {2, "Number"},
    {3, "Keyword"},
    {4, "Operator/Delimiter"},
    {5, "EOF Token"}
  };

//Alphabet Sets
const unordered_set<char> ALPHA_LOW =
  {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
  };
const unordered_set<char> ALPHA_UPPER =
  {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
  };
const unordered_set<char> DIGIT =
  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
const unordered_set<char> VALID_OPS =
  {'=', '<', '>', ':', '+', '-', '*', '/', '#', '.', '(', ')', ',', '{', '}', ';', '[', ']'};
const unordered_set<char> WHITE_SPACES =
  {' '};
const unordered_set<string> KEYWORDS = {"start", "end", "iter", "void", "var", "return", "read", "print", "program", "if", "then", "let"};

//Enumerates identifiers, numbers, keywords, operators/delimiters, and EOF token IDs.
enum T_ID {
  ERR_tk = 0,
  IDENT_tk = 1,
  NUM_tk,
  KW_tk,
  OP_tk,
  EOF_tk
};

typedef struct {
  T_ID id;
  string instance;
  int line_num;
} token;

#endif
