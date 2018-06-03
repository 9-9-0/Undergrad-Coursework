#ifndef STATSEM_HPP
#define STATSEM_HPP

#include "parser.hpp"
#include <utility>
using namespace std;

const string SEM_ERRORS[2] =
  {
    "STATIC SEMANTICS: Variable %s already defined on line %d.\n",
    "STATIC SEMANTICS: Attempted to reference undeclared variable %s on line %d.\n"
  };

class Static_Sem_Checker {
public:
  Static_Sem_Checker(parse_tree_node* tree_root);
  int run_checks();
private:
  parse_tree_node* root;
  int err_flag; //Values are as follows: 1 - variable check error, 2 - etc, 3 - etc.

  //Globally-scoped variable checking
  unordered_map<string, token> declared_vars;
  bool insert_var(string var, token _token);
  bool verify_var(string var);

  void var_check(parse_tree_node* node);
};

#endif
