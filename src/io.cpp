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
    cout << "Error: failed to open file." << endl;
  }
  read_to;
}

int writeFile(std::vector<vector<int>> write_from, char* filepath) {
  ofstream file(filepath);
  if(file.is_open()) {
    for(vector<vector<int>>::iterator it = write_from.begin(); it != write_from.end(); it++) {
      file << rowToString(*it) << "\n";
    }
    file.close();
  }
  else {
    cout << "Error: failed to open file." << endl;
    return -1;
  }

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

string rowToString(vector<int> row, char delimiter) {
  string line = "";
  for(int i: row) {
    line += to_string(i) + delimiter;
  }
  line.erase(line.length()-1);
  return line;

}

int main() {
  string line = "32,43,3,24,3";
  vector<int> parsed = parseLine(line);
  for(int i: parsed)
    cout << " " << i;
  cout << endl;
  cout << rowToString(parsed) << endl;
  vector<vector<int>> vec;
  vec.push_back(parsed);
  vec.push_back(parsed);
  cout << writeFile(vec, "testcsv.csv") << endl;
}
