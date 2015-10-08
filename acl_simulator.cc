#include <iostream>
#include <sstream>
#include "acl_entry.h"
#include "file.h"

using namespace std;

File root("");

int main() {
  int line_no = 1;
  File home = root.AddChild("home");
  home.AddPermission("*", "*", true, true);

  string line;
  getline(cin, line);

  while (line != ".") {
    string user_group, user, group, filename, message;
    stringstream reader(line);
    bool valid = true;

    // Parse user info
    getline(reader, user,   '.');
    getline(reader, group,  ' ');
    getline(reader, filename   );

    if (!filename.empty()) {
      File userfile = home.AddChild(user);
      userfile.AddPermission(user, group, true, true);
    } else {
      File *userptr = home.GetChildByName(user);
      if (!userptr) {
        valid = false;
        message = "First reference to user is missing a filename.";
      } else {
        userptr->AddPermission(user, group, true, true);
      }
    }

    // Log output
    cout << line_no++ << "\t" << (valid? "Y\t" : "X\t") << message << endl;

    // Get next line
    getline(cin, line);
  }

  line_no = 1;
  while (!getline(cin, line).eof()) {
    cout << line_no++ << "\tY\t" <<  line << endl;
    // Do command stuff
  }
}
