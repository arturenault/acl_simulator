#include <iostream>
#include "acl_entry.h"
#include "file.h"

using namespace std;

int main() {
  string line;
  getline(cin, line);
  File root("/home");

  while (line != ".") {
    cout << line << endl;
    getline(cin, line);
    // Do user stuff
  }

  while (!getline(cin, line).eof()) {
    cout << line << endl;
    // Do command stuff
  }
}
