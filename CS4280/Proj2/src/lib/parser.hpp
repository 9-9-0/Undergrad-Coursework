#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include "scanner.hpp"
using namespace std;

//Re-structure these later, not very manageable if your grammar changes...which it probably won't for this.
const string PARSE_ERRORS[16] =
  {
    "PARSER: <S> expansion expected 'program', got '%s' on line %d.\n",
    "PARSER: <vars> expansion expected an Identifier type, got '%s' of type %s on line %d.\n",
    "PARSER: <block> expansion expected 'start', got '%s' on line %d.\n",
    "PARSER: <block> expension expected 'end', got '%s' on line %d.\n",
    "PARSER: <stat> expansion expected 'read', 'print', 'start', 'if', 'iter', or 'let'. Got '%s' on line %d.\n",
    "PARSER: <in> expansion expected an Identifier type, got '%s' of type %s on line %d.\n",
    "PARSER: <stat> expansion expected ',', got %s on line %d.\n",
    "PARSER: <R> expansion expected '(', an Identifier type, or an Integer type. Got '%s' of type %s on line %d.\n",
    "PARSER: <R> expansion expected ')', got '%s' of type %s on line %d.\n",
    "PARSER: <ifstat> expansion expected '(', got %s on line %d.\n",
    "PARSER: <ifstat> expansion expected ')', got %s on line %d.\n",
    "PARSER: <O> expansion expected '>', '<', or ':'. Got %s on line %d.\n",
    "PARSER: <ifstat> expansion expected '(', got %s on line %d.\n",
    "PARSER: <ifstat> expansion expected ')', got %s on line %d.\n",
    "PARSER: <assign> expansion expected an Identifier type, got '%s' of type %s on line %d.\n",
    "PARSER: <assign> expansion expected '=', got %s on line %d.\n"
  };

struct parse_tree_node {
  string non_term;  //The LHS of the grammar rule being expanded
  vector<token> terminals;  //Non-syntactical non-terminals that need storing
  vector<parse_tree_node*> children;

  parse_tree_node(string non_term_): non_term(non_term_)
  {}
};

class Parser {
public:
  Parser(Scanner* scanner_);

  parse_tree_node* parse();
  int error_detected();
  void print_tree(parse_tree_node* node, int level);
  void del_tree();

private:
  Scanner* scanner;
  parse_tree_node* root;
  int err_flag;           //0 if parsing process resulted in no errors, 1 if scanner error, 2 if parsing error
  token cur_token;

  void del_node(parse_tree_node* node);

  //Grammar Rules
  parse_tree_node* S();
  parse_tree_node* block();
  parse_tree_node* vars();

  parse_tree_node* stats();
  parse_tree_node* stat();
  parse_tree_node* mStat();

  parse_tree_node* in();
  parse_tree_node* out();
  parse_tree_node* ifstat();
  parse_tree_node* loop();
  parse_tree_node* assign();

  parse_tree_node* expr();
  parse_tree_node* H();
  parse_tree_node* R();
  parse_tree_node* O();
};

#endif
