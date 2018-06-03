#include "generator.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <regex>
using namespace std;

Generator::Generator(string _file_name, parse_tree_node* _tree):
  out_fname(process_fname(_file_name)),
  tree(_tree),
  err_flag(0),
  temp_counter(-1),
  br_counter(-1)
  {}

int Generator::err_check() {
  return err_flag;
}

//Given the file argument passed in, returns the filename with .asm appended.
string Generator::process_fname(string in_fname) {
  //Remove all path prefixes
  int slash_pos = -1;
  for (auto i = 0; i < in_fname.length(); i++) {
    if (in_fname[i] == '/') slash_pos = i;
  }
  if (slash_pos != -1) {
    in_fname = in_fname.substr(slash_pos + 1, in_fname.length() - slash_pos + 1);
  }

  //Find the last period
  int dot_pos = -1;
  for (int i = in_fname.length() - 1; i >= 0; i--) {
    if (in_fname[i] == '.') {
      dot_pos = i;
      break;
    }
  }

  if (dot_pos == -1)
    return in_fname + ".asm";
  else
    return in_fname.substr(0, dot_pos) + ".asm";
}

void Generator::run() {
  out_file.open(out_fname);
  if (out_file.is_open()) {
    code_gen(tree);
    optimize();
    print_code();
    print_vars();
  }
  else {
    cout << "COMPILATION: Could not open .asm file for code generation.\n";
    err_flag = 1;
  }
}

void Generator::code_gen(parse_tree_node* node) {
  if (node->non_term == "S") {
    //Should only execute twice
    for (auto &c : node->children) {
      code_gen(c);
    }
    out_code += "STOP\n";
  }

  else if (node->non_term == "block") {
    //Left then right traversal
    for (auto &c : node->children) {
      code_gen(c);
    }
  }

  else if (node->non_term == "vars") {
    //Should only execute once
    for (auto &token : node->terminals) {
      variables.push_back(token.instance);
    }
    //Should only execute # of times = number of variables declared
    for (auto &c : node->children) {
      code_gen(c);
    }
  }

  else if (node->non_term == "expr") {
    if (node->children.size() == 2) {
      //Recurse through right child, then left, then evaluate the operation
      code_gen(node->children[1]);

      temp_counter++;
      string temp = "T" + to_string(temp_counter);
      out_code += "STORE " + temp + "\n";

      code_gen(node->children[0]);

      if (node->terminals[0].instance == "+") {
          out_code += "ADD " + temp + "\n";
      }
      else if (node->terminals[0].instance == "-") {
          out_code += "SUB " + temp + "\n";
      }
      else if (node->terminals[0].instance == "/") {
          out_code += "DIV " + temp + "\n";
      }
      else if (node->terminals[0].instance == "*") {
          out_code += "MULT " + temp + "\n";
      }
    }
    else {
      //Case of single <H> expansion
      code_gen(node->children[0]);
    }
  }

  else if (node->non_term == "H") {
    //Recurse into <R> child
    code_gen(node->children[0]);

    //If # (negation) token is detected, print the corresponding assembly code
    if (node->terminals.size() == 1 && node->terminals[0].instance == "#") {
      out_code += "MULT -1\n";
    }
  }

  else if (node->non_term == "R") {
    if (node->terminals.size() == 0) {
      //Recurse into <expr>
      code_gen(node->children[0]);
    }
    else {
      //If a terminal (identifier or integer) is detected, output code
      out_code += "LOAD " + node->terminals[0].instance + "\n";
    }
  }

  else if (node->non_term == "stats") {
    //Should only execute twice
    for (auto &c : node->children) {
      code_gen(c);
    }
  }
  else if (node->non_term == "mStat") {
    //Should only execute once
    for (auto &c : node->children) {
      code_gen(c);
    }
  }
  else if (node->non_term == "stat") {
    //Should only execute once
    for (auto &c : node->children) {
      code_gen(c);
    }
  }

  else if (node->non_term == "in") {
    out_code += "READ " + node->terminals[0].instance + "\n";
  }

  else if (node->non_term == "out") {
    //Should only execute once
    for (auto &c : node->children) {
      code_gen(c);
    }

    //Store accumulator contents to temp variable, then write out temp variable
    temp_counter++;
    string temp = "T" + to_string(temp_counter);
    out_code += "STORE " + temp + "\n";
    out_code += "WRITE " + temp + "\n";
  }

  else if (node->non_term == "ifstat") {
    //Recurse into right hand <expr>
    code_gen(node->children[2]);

    //Store accumulator contents to temp variable
    temp_counter++;
    string temp = "T" + to_string(temp_counter);
    out_code += "STORE " + temp + "\n";

    //Recurse into left hand <expr>
    code_gen(node->children[0]);

    //Subtract temp variable from accumulator
    out_code += "SUB " + temp + "\n";

    //Create branch based on <O> token
    br_counter++;
    string branch = "BR" + to_string(br_counter);

    if (node->children[1]->terminals[0].instance == ">") {
      out_code += "BRZNEG " + branch + "\n";
    }
    else if (node->children[1]->terminals[0].instance == "<") {
      out_code += "BRZPOS " + branch + "\n";
    }
    else if (node->children[1]->terminals[0].instance == ":") {
      out_code += "BRPOS " + branch + "\n";
      out_code += "BRNEG " + branch + "\n";
    }

    //Recurse into <stat>
    code_gen(node->children[3]);

    out_code += branch + ": NOOP\n";
  }

  else if (node->non_term == "loop") {
    //Create a branch for the loop
    br_counter++;
    string loop_start = "BR" + to_string(br_counter);
    out_code += loop_start + ": ";

    //Recurse into right hand <expr>
    code_gen(node->children[2]);

    //Store accumulator contents to temp variable
    temp_counter++;
    string temp = "T" + to_string(temp_counter);
    out_code += "STORE " + temp + "\n";

    //Recurse into left hand <expr>
    code_gen(node->children[0]);

    //Subtract temp variable from accumulator
    out_code += "SUB " + temp + "\n";

    //Create branch based on <O> token
    br_counter++;
    string out_branch = "BR" + to_string(br_counter);

    if (node->children[1]->terminals[0].instance == ">") {
      out_code += "BRZNEG " + out_branch + "\n";
    }
    else if (node->children[1]->terminals[0].instance == "<") {
      out_code += "BRZPOS " + out_branch + "\n";
    }
    else if (node->children[1]->terminals[0].instance == ":") {
      out_code += "BRPOS " + out_branch + "\n";
      out_code += "BRNEG " + out_branch + "\n";
    }

    //Recurse into <stat>
    code_gen(node->children[3]);

    //Returning to the start of the loop
    out_code += "BR " + loop_start + "\n";

    //The exit branch
    out_code += out_branch + ": ";
  }

  else if (node->non_term == "assign") {
    //Expand into <expr> child
    code_gen(node->children[0]);

    //Set Identifier to accumulator contents
    out_code += "STORE " + node->terminals[0].instance + "\n";
  }

  else if (node->non_term == "O") {
    //Explicit reminder that this will never run.
  }
}

void Generator::print_code() {
  for (auto &line : out_code_lines) {
    out_file << line << "\n";
  }
}

void Generator::print_vars() {
  //Output declared variables
  for (auto &var : variables) {
    out_file << var + " 0 \n";
  }
  //Output temp variables
  for (auto i = 0; i <= temp_counter; i++) {
    out_file << "T" + to_string(i) + " 0\n";
  }
}

void Generator::optimize() {
  post_process_str();
  load_store_opt();
}

/* Deletes the lines that follow the format:
   STORE X
   LOAD X
*/
void Generator::load_store_opt() {
  int i = 0;
  vector<string> words;
  string var = "";

  while (i < out_code_lines.size() && out_code_lines[i] != "STOP") {
    words = split_line(out_code_lines[i], ' ');
    if (words[0] == "STORE") {
      var = words[1];
      words = split_line(out_code_lines[i + 1], ' ');
      if (words[0] == "LOAD") {
        if (var == words[1]) {
          //Remove only the LOAD command following STORE
          out_code_lines.erase(out_code_lines.begin() + i+1, out_code_lines.begin() + i+2);
          continue;
        }
      }
    }

    i++;
  }
}

//out_code -> vector<string> for post-processing optimization
void Generator::post_process_str() {
  regex newline_re("\\n");
  out_code_lines = {
    sregex_token_iterator(out_code.begin(), out_code.end(), newline_re, -1),
    sregex_token_iterator()
  };
}

vector<string> Generator::split_line(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}
