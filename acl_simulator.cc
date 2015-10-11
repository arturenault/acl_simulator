#include <cstdio>
#include <iostream>
#include <sstream>
#include "acl_entry.h"
#include "acl_simulator.h"

using namespace std;

File root("");
string message;
bool valid;

int main() {
  int line_no = 1;
  File tmp = root.AddChild("tmp");
  tmp.AddPermission("*", "*", true, true);

  string line;
  getline(cin, line);

  while (line != ".") {
    string user_group, user, group, filename;
    stringstream reader(line);

    message = "";
    valid = true;

    // Parse user info
    getline(reader, user,   '.');
    getline(reader, group,  ' ');
    getline(reader, filename   );

    if (!filename.empty()) {
      CreateUserFile(filename, user, group);
    } else {
      /*File *userptr = root.GetChildByName(user);
      if (!userptr) {
        valid = false;
        message = "First reference to user is missing a filename.";
      } else {
        userptr->AddPermission(user, group, true, true);
      }*/
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

/* Create new file in user definition section */
File *CreateUserFile(string filename, string user, string group) {
  string component;
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;
  File *next_file;

  getline(stream, component, '/');

  if (!component.empty()) {
    valid   = false;
    message = "All filenames must begin with /";
    return nullptr;
  }

  while (stream.peek() != EOF) {
    getline(stream, component, '/');
    path.push_back(component);
  }

  int i;
  for (i = 0; i < path.size()-1; i++) {
    next_file = file->GetChildByName(path[i]);
    /* File doesn't exist yet; create it. */
    if (!next_file) {
      next_file = &file->AddChild(path[i]);
      next_file->AddPermission("*", "*", true, false);
    }
  }

  next_file = file->GetChildByName(path[i]);

  if (next_file) {
    valid = false;
    message = "File already belongs to another user";
    return nullptr;
  } else {
    next_file = &file->AddChild(path[i]);
    next_file->AddPermission(user, group, true, true);
  }

  return next_file;
}

/* Create file as a user */
File *Create(string user, string group, string filename) {
  string component;
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;

  getline(stream, component, '/');

  if (!component.empty()) {
    valid   = false;
    message = "All filenames must begin with /";
    return nullptr;
  }

  while (stream.peek() != EOF) {
    getline(stream, component, '/');
    path.push_back(component);
  }

  /* Path.size - 1 because the last file doesn't have
   * to exist yet. */
  int i;
  for (i = 0; i < path.size() - 1; i++) {
    file = file->GetChildByName(path[i]);

    if (!file) {
      valid   = false;
      message = "All components in the path must exist before creating a new one";
      return nullptr;
    }

    if (i < path.size() - 2) { 
      if (!file->HasPermission(user, group, false)) {
        valid = false;
        message = "Read permissions on all components in the path are needed to reach a file";
        return nullptr;
      }
    } else {
      if (!file->HasPermission(user, group, true)) {
        valid = false;
        message = "Write permission on the parent component is needed to create a file";
        return nullptr;
      }
    }
  }

  return &file->AddChild(path[i]);
}
