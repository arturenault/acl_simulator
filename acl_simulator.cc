#include <iostream>

using namespace std;

int main() {
    string line;
    getline(cin, line);
    
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
