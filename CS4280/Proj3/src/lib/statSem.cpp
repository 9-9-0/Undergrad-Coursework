#include "statSem.hpp"
using namespace std;

Static_Sem_Checker::Static_Sem_Checker(parse_tree_node* tree_root):
  root(tree_root),
  err_flag(0)
  {}

/* Runs through the list of implemented checks */
int Static_Sem_Checker::run_checks() {
  var_check(root);
  return err_flag;
}

/**************************************************************************/

bool Static_Sem_Checker::insert_var(string var, token _token) {
  if (declared_vars.count(var) == 1) {
    return false;
  }
  else {
    declared_vars[var] = _token;
    return true;
  }
}

bool Static_Sem_Checker::verify_var(string var) {
  return (declared_vars.count(var) == 1);
}

/* Globally scoped variable checking:
    - Each variable is only defined ONCE.
    - Variables are defined prior to use.
*/
void Static_Sem_Checker::var_check(parse_tree_node* node) {
  if (err_flag != 0) return;

  if (node->non_term == "vars") {
    //Loop should only execute once
    for (auto &t: node->terminals) {
      if (!insert_var(t.instance, t)) {
        err_flag = 1;
        printf(SEM_ERRORS[0].c_str(), t.instance.c_str(), declared_vars[t.instance].line_num);
        return;
      }
    }
  }
  else {
    for (auto &t: node->terminals) {
      if (t.id == IDENT_tk) {
        if (!verify_var(t.instance)) {
          err_flag = 1;
          printf(SEM_ERRORS[1].c_str(), t.instance.c_str(), t.line_num);
          return;
        }
      }
    }
  }

  for (auto &c: node->children) {
    var_check(c);
  }
  return;
}

/**************************************************************************/
