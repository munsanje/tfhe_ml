#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "io.hpp"

using namespace std;

vector<vector<int>> readFile(char* filepath) {
  string line;
  ifstream file(filepath);
  vector<vector<int>> read_to;
  if(file.is_open()) {
    while(getline(file, line)) {
      read_to.push_back(parseLine(line));
    }
    file.close();
  }
  else {
    cout << "Error: failed to open file.";
  }
  read_to;
}

void writeFile(std::vector<vector<int>> write_from, char* filepath) {


}

vector<int> parseLine(string line, char delimiter) {
  size_t next = 0;
  string elem;
  vector<int> parsed;
  do {
    next = line.find(delimiter);
    elem = line.substr(0, next);
    cout << elem << endl;
    parsed.push_back(stoi(elem));
    line.erase(0, next + 1);
  } while(next != string::npos);
  return parsed;
}

int main() {
  string line = "32,43,3,24,3";
  // cout << line.length() << endl;
  vector<int> parsed = parseLine(line, ',');
  for(vector<int>::iterator it = parsed.begin(); it != parsed.end(); it++)
    cout << " " << *it;
  cout << endl;
}
