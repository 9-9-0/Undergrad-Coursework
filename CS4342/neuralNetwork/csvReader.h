#ifndef CSV_READ_H
#define CSV_READ_H

#include <istream>
#include <string>
#include <vector>

std::vector<std::string> readCSVRow(const std::string &row);
std::vector<std::vector<std::string>> readCSV(std::istream &in);
#endif
