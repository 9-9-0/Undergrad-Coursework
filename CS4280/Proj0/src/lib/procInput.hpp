#ifndef PROC_INPUT_HPP
#define PROC_INPUT_HPP
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
using namespace std;

const int MAX_NUM_INTS = 10;
const int MIN_NUM_INTS = 1;

//Error Messages
const string ERR_NUM_INPUTS = "Invalid number of arguments entered. Range is [1,10].\n";
const string ERR_INPUT_VAL = "Invalid argument detected. Only integers are parseable.\n";
const string ERR_INPUT_FILE = "Invalid file contents. Only 1 to 10 positive integers delimited by spaces are allowed.\n";
const string ERR_FILE = "File not found.\n";
const string ERR_OFILE = "Output file could not be opened.\n";
const string ERR_NEG = "Negative integer detected in input. Only positive integers accepted.\n";

vector<string> split(const string& s, char delimiter);
int is_num(const string &str);
bool file_exists(const string &filename);
bool neg_check(vector<int> &nums);
bool parse_nums(int argc, char** args, vector<int> &container);
bool parse_nums_file(ifstream &infile, vector<int> &container);
bool treat_as_file(string fname);

#endif
