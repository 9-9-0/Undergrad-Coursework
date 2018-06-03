#include "parser.hpp"
#include <iostream>
using namespace std;

Parser::Parser(Scanner* scanner_):
  scanner(scanner_),
  root(NULL),
  err_flag(0)
  {}

//Topmost parser call.
parse_tree_node* Parser::parse() {
  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    return NULL;
  }

  root = S();
  if (error_detected()) return root;

  if (cur_token.id == EOF_tk)
    cout << "PARSER: Parse successful.\n";
  else
    err_flag = 2;
    //display expected follow? Double check this

  return root;
}

int Parser::error_detected() {
  //If error has resulted, delete the tree before returning.
  return err_flag;
}

void Parser::print_tree(parse_tree_node* node, int level) {
  if (node == NULL) return;

  printf("%*cL%d [<%s>]: Terminals:", level * 2, ' ', level, node->non_term.c_str());
  for (auto &token: node->terminals) {
    printf(" %s", token.instance.c_str());
  }
  printf("\n");

  for (auto &child : node->children) {
    print_tree(child, level + 1);
  }
}

//Deletion
void Parser::del_tree() {
  if (root != NULL) {
    del_node(root);
  }
}

void Parser::del_node(parse_tree_node* node) {
  for (auto child : node->children) {
    del_node(child);
  }

  delete node;
}

/**************************************************************************
Grammar Rules
- The error mechanism, triggered during grammar violations, is as follows:
1. Parser's err_flag is set accordingly (see parser.hpp for values)
2. If the error is a parser error, the corresponding message is printed. Nothing
   is printed in the case of a scanner error.
3. NULL is returned (if a node was created, delete it before doing so).
**************************************************************************/

/* <S> -> program <vars> <blocks> */
parse_tree_node* Parser::S() {
  if (cur_token.id != 3 && cur_token.instance != "program") {
    err_flag = 2;
    printf(PARSE_ERRORS[0].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    return NULL;
  }

  parse_tree_node* node = new parse_tree_node("S");
  parse_tree_node* child_ptr = NULL;

  child_ptr = vars();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  //Might want to refactor this into a 3 line function...
  if (child_ptr != NULL) {
    node->children.push_back(child_ptr);
    child_ptr = NULL;
  }

  child_ptr = block();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr); //block will always return a node if no errors occur

  return node;
}

/* <vars> -> empty | var {Identifier} <vars> */
parse_tree_node* Parser::vars() {
  //Empty set logic
  if (cur_token.instance != "var")
    return NULL;

  //Fetch the next token and check that it's an identifier
  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    return NULL;
  }

  if (cur_token.id != 1) {
    err_flag = 2;
    printf(PARSE_ERRORS[1].c_str(), cur_token.instance.c_str(), TOKEN_NAMES.at(cur_token.id).c_str(), cur_token.line_num);
    return NULL;
  }

  parse_tree_node* node = new parse_tree_node("vars");
  parse_tree_node* child_ptr = NULL;

  //Store the identifier token, set next token, continue expansion.
  node->terminals.push_back(cur_token);
  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  child_ptr = vars();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  if (child_ptr != NULL)
    node->children.push_back(child_ptr);

  return node;
}

/* <block> -> start <vars> <stats> end */
parse_tree_node* Parser::block() {
  if (cur_token.id != 3 && cur_token.instance != "start") {
    err_flag = 2;
    printf(PARSE_ERRORS[2].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    return NULL;
  }

  parse_tree_node* node = new parse_tree_node("blocks");
  parse_tree_node* child_ptr = NULL;

  child_ptr = vars();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  if (child_ptr != NULL) {
    node->children.push_back(child_ptr);
    child_ptr = NULL;
  }

  child_ptr = stats();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);

  if (cur_token.id != KW_tk && cur_token.instance != "end") {
    err_flag = 2;
    printf(PARSE_ERRORS[3].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  return node;
}

/* <stats> -> <stat> <mStat> */
parse_tree_node* Parser::stats() {
  parse_tree_node* node = new parse_tree_node("stats");
  parse_tree_node* child_ptr = NULL;

  child_ptr = stat();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);
  child_ptr = NULL;

  child_ptr = mStat();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  if (child_ptr != NULL) {
    node->children.push_back(child_ptr);
  }

  return node;
}

/* <stat> -> <in> , | <out> , | <block> , | <ifstat> , | <loop> , | <assign> , */
parse_tree_node* Parser::stat() {
  parse_tree_node* node = new parse_tree_node("stat");
  parse_tree_node* child_ptr = NULL;

  if (cur_token.instance == "read") {
    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    child_ptr = in();
    if (error_detected()) {
      delete node;
      return NULL;
    }
    node->children.push_back(child_ptr);
  }
  else if (cur_token.instance == "print") {
    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    child_ptr = out();
    if (error_detected()) {
      delete node;
      return NULL;
    }
    node->children.push_back(child_ptr);
  }
  else if (cur_token.instance == "start") {
    child_ptr = block();
    if (error_detected()) {
      delete node;
      return NULL;
    }
    node->children.push_back(child_ptr);
  }
  else if (cur_token.instance == "if") {
    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    child_ptr = ifstat();
    if (error_detected()) {
      delete node;
      return NULL;
    }
    node->children.push_back(child_ptr);
  }
  else if (cur_token.instance == "iter") {
    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    child_ptr = loop();
    if (error_detected()) {
      delete node;
      return NULL;
    }
    node->children.push_back(child_ptr);
  }
  else if (cur_token.instance == "let") {
    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    child_ptr = assign();
    if (error_detected()) {
      delete node;
      return NULL;
    }
    node->children.push_back(child_ptr);
  }
  else {
    err_flag = 2;
    printf(PARSE_ERRORS[4].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    return NULL;
  }

  //Process comma here
  if (cur_token.instance != ",") {
    err_flag = 2;
    printf(PARSE_ERRORS[6].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  return node;
}

/* <mStat> -> empty | <stats> */
parse_tree_node* Parser::mStat() {
  //Empty set logic
  if (cur_token.instance != "read" && cur_token.instance != "print" &&
      cur_token.instance != "start" && cur_token.instance != "if" &&
      cur_token.instance != "iter" && cur_token.instance != "let") {
    return NULL;
  }

  parse_tree_node* node = new parse_tree_node("mStat");
  parse_tree_node* child_ptr = NULL;

  child_ptr = stats();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);

  return node;
}

/* <in> -> read {Identifier}
1. Create a node
2. Check that the current token is an identifier
3. Store the identifier in the node
4. Fetch next token
**************************************************************************/
parse_tree_node* Parser::in() {
  parse_tree_node* node = new parse_tree_node("in");

  if (cur_token.id != IDENT_tk) {
    err_flag = 2;
    printf(PARSE_ERRORS[5].c_str(), cur_token.instance.c_str(), TOKEN_NAMES.at(cur_token.id).c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }

  node->terminals.push_back(cur_token);

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  return node;
}

/* <out> -> print <expr>
1. Create node
2. Call expr() and attach returned node
**************************************************************************/
parse_tree_node* Parser::out() {
  parse_tree_node* node = new parse_tree_node("out");
  parse_tree_node* child_ptr = NULL;

  child_ptr = expr();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);

  return node;
}

/* <ifstat> -> if ( <expr> <O> <expr> ) <stat>
1. Create a node
2. Check for '(' token and consume it
3. Expand into <expr>, followed by <O>, and then <expr> again.
4. Consume ')' token.
5. Expand into <stat>
**************************************************************************/
parse_tree_node* Parser::ifstat() {
  parse_tree_node* node = new parse_tree_node("ifstat");
  parse_tree_node* child_ptr = NULL;

  if (cur_token.instance != "(") {
    err_flag = 2;
    printf(PARSE_ERRORS[9].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  child_ptr = expr();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);
  child_ptr = NULL;

  child_ptr = O();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);
  child_ptr = NULL;

  child_ptr = expr();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);
  child_ptr = NULL;

  if (cur_token.instance != ")") {
    err_flag = 2;
    printf(PARSE_ERRORS[10].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  child_ptr = stat();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);

  return node;
}

/* <loop> -> iter ( <expr> <O> <expr> ) <stat>
1. Create a node
2. Check for '(' token and consume it
3. Expand into <expr>, followed by <O>, and then <expr> again.
4. Consume ')' token
5. Expand into <stat>
**************************************************************************/
parse_tree_node* Parser::loop() {
  parse_tree_node* node = new parse_tree_node("loop");
  parse_tree_node* child_ptr = NULL;

  if (cur_token.instance != "(") {
    err_flag = 2;
    printf(PARSE_ERRORS[12].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  child_ptr = expr();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);
  child_ptr = NULL;

  child_ptr = O();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);
  child_ptr = NULL;

  child_ptr = expr();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);
  child_ptr = NULL;

  if (cur_token.instance != ")") {
    err_flag = 2;
    printf(PARSE_ERRORS[13].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  child_ptr = stat();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);

  return node;
}

/* <assign> -> let {Identifier} = <expr> */
parse_tree_node* Parser::assign() {
  parse_tree_node* node = new parse_tree_node("assign");
  parse_tree_node* child_ptr = NULL;

  if (cur_token.id != IDENT_tk) {
    err_flag = 2;
    printf(PARSE_ERRORS[14].c_str(), cur_token.instance.c_str(), TOKEN_NAMES.at(cur_token.id).c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }
  node->terminals.push_back(cur_token);

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  if (cur_token.instance != "=") {
    err_flag = 2;
    printf(PARSE_ERRORS[15].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }

  cur_token = scanner->fetch_token();
  if (scanner->error_detected()) {
    err_flag = 1;
    delete node;
    return NULL;
  }

  child_ptr = expr();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);

  return node;
}

/* <O> -> < | > | : */
parse_tree_node* Parser::O() {
  parse_tree_node* node = new parse_tree_node("O");

  if (cur_token.instance == "<" || cur_token.instance == ">" || cur_token.instance == ":") {
    node->terminals.push_back(cur_token);

    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    return node;
  }
  else {
    err_flag = 2;
    printf(PARSE_ERRORS[11].c_str(), cur_token.instance.c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }
}

/* <expr> -> <H> + <expr> | <H> - <expr> | <H> / <expr> | <H> * <expr> | <H> */
parse_tree_node* Parser::expr() {
  parse_tree_node* node = new parse_tree_node("expr");
  parse_tree_node* child_ptr = NULL;

  child_ptr = H();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);
  child_ptr = NULL;

  if (cur_token.instance == "+" || cur_token.instance == "-" || cur_token.instance == "/" || cur_token.instance == "*") {
    node->terminals.push_back(cur_token);

    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    child_ptr = expr();
    if (error_detected()) {
      delete node;
      return NULL;
    }
    node->children.push_back(child_ptr);

    return node;
  }
  else
    return node;
}

/* <H> -> # <R> | <R> */
parse_tree_node* Parser::H() {
  parse_tree_node* node = new parse_tree_node("H");
  parse_tree_node* child_ptr = NULL;

  if (cur_token.instance == "#") {
    node->terminals.push_back(cur_token);

    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }
  }

  child_ptr = R();
  if (error_detected()) {
    delete node;
    return NULL;
  }
  node->children.push_back(child_ptr);

  return node;
}

/* <R> -> ( <expr> ) | {Identifier} | {Integer} */
parse_tree_node* Parser::R() {
  parse_tree_node* node = new parse_tree_node("R");
  parse_tree_node* child_ptr = NULL;

  if (cur_token.instance == "(") {
    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    child_ptr = expr();
    if (error_detected()) {
      delete node;
      return NULL;
    }
    node->children.push_back(child_ptr);

    if (cur_token.instance != ")") {
      err_flag = 2;
      printf(PARSE_ERRORS[8].c_str(), cur_token.instance.c_str(), cur_token.line_num);
      delete node;
      return NULL;
    }
    
    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }

    return node;
  }
  else if (cur_token.id == IDENT_tk || cur_token.id == NUM_tk) {
    node->terminals.push_back(cur_token);

    cur_token = scanner->fetch_token();
    if (scanner->error_detected()) {
      err_flag = 1;
      delete node;
      return NULL;
    }
    return node;
  }
  else {
    err_flag = 2;
    printf(PARSE_ERRORS[7].c_str(), cur_token.instance.c_str(), TOKEN_NAMES.at(cur_token.id).c_str(), cur_token.line_num);
    delete node;
    return NULL;
  }
}
