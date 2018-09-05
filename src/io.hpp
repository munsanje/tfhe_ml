#pragma once

#include <vector>

// COULD use a template but the use case is quite specific
std::vector<std::vector<int>> readFile(char* filepath);
int writeFile(std::vector<std::vector<int>> write_from, char* filepath);
std::vector<int> parseLine(std::string line, char delimiter=',');
std::string rowToString(std::vector<int> row, char delimiter=',');
