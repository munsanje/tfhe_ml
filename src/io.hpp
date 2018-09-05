#pragma once

#include <vector>


// COULD use a template but the use case is quite specific
std::vector<std::vector<int>> readFile(char* filepath);
void writeFile(std::vector<vector<int>> write_from, char* filepath);
std::vector<int> parseLine(std::string line, char delimiter);
