#pragma once

#include <vector>

// COULD use a template but the use case is quite specific
std::vector<std::vector<double>> readFile(char* filepath, char delimiter=',');
int writeFile(std::vector<std::vector<double>> write_from, char* filepath, char delimiter=',');
std::vector<double> parseLine(std::string line, char delimiter=',');
std::string rowToString(std::vector<double> row, char delimiter=',');
