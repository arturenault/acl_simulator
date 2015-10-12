#include <regex>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "acl_entry.h"
#include "acl_simulator.h"

using namespace std;

File root("");
bool valid, permitted;
string message;

const int kMaxComponentLength = 16;
const int kMaxFilenameLength = 256;
const string kNamePattern = "[a-z]+";
const string kComponentPattern =
    "[a-z\\.]{1," + to_string(kMaxComponentLength) + "}";
const string kPermissionPattern = "^(r|w|rw|-)$";

unordered_map<string, unordered_set<string>> users;
unordered_map<string, unordered_set<string>> groups;
map<string, string> user_files;

int main() {
  string line;
  int line_no = 1;

  /* Create initial folders */
  root.AddChild("home").AddPermission("*", "*", true, false);
  root.AddChild("tmp").AddPermission("*", "*", true, true);

  getline(cin, line);
  while (line != ".") {
    message = "";
    valid = true;

    ProcessUserDeclaration(line);

    /* Log output */
    cout << line_no++ << "\t" << (valid ? "Y\t" : "X\t") << message << endl;

    getline(cin, line);
  }

  /* Add *-* r to every user-file ACL */
  for (auto iter = user_files.begin(); iter != user_files.end(); ++iter) {
    FindFile(iter->second)->AddPermission("*", "*", true, false);
  }

  line_no = 1;
  while (!getline(cin, line).eof()) {
    ProcessCommand(line);
    cout << line_no++ << (valid ? permitted ? "\tY\t" : "\tN\t" : "\tX\t")
         << line << endl;
  }
}

void ProcessUserDeclaration(string declaration) {
  string user, group, filename;
  stringstream reader(declaration);
  regex name_regex(kNamePattern);

  // Parse command info
  getline(reader, user, '.');
  getline(reader, group, ' ');
  getline(reader, filename);

  if (!regex_match(user, name_regex)) {
    message = "Invalid username";
    valid = false;
    return;
  }

  if (!regex_match(group, name_regex)) {
    message = "Invalid group name";
    valid = false;
    return;
  }

  if (filename.length() > kMaxFilenameLength) {
    message = "File name too long";
    valid = false;
    return;
  }

  if (!users[user].empty() && !groups[group].empty()) {
    message = "Duplicate user declaration";
    valid = false;
    return;
  }

  if (!filename.empty()) {
    if (users[user].empty()) {
      if (CreateUserFile(filename, user, group)) {
        users[user].insert(group);
        groups[group].insert(user);
        user_files[user] = filename;
      }
    } else {
      message = "Each user may only have one file";
      valid = false;
    }
  } else {
    if (users[user].empty()) {
      message = "First reference to a username must include a filename";
      valid = false;
    } else {
      FindFile(filename)->AddPermission(user, group, true, true);
    }
  }
}

void ProcessCommand(string line) {
  string operation, user, group, filename;
  stringstream stream(line);

  getline(stream, operation, ' ');
  getline(stream, user, '.');
  getline(stream, group, ' ');
  getline(stream, filename);

  if (operation == "READ") {
  } else if (operation == "WRITE") {
  } else if (operation == "CREATE") {
  } else if (operation == "DELETE") {
  } else if (operation == "ACL") {
  } else {
    valid = false;
    message = "Invalid operation";
    return;
  }
}

File *FindFile(string filename) {
  string component;
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;

  getline(stream, component, '/');

  while (stream.peek() != EOF) {
    getline(stream, component, '/');
    path.push_back(component);
  }

  for (auto iter = path.begin(); iter != path.end(); ++iter) {
    file = file->GetChildByName(*iter);
    if (!file) {
      return nullptr;
    }
  }
  return file;
}

/* Create new file in user definition section */
File *CreateUserFile(string filename, string user, string group) {
  string component;
  regex component_regex(kComponentPattern);
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;
  File *next_file;

  getline(stream, component, '/');

  if (!component.empty()) {
    valid = false;
    message = "All filenames must begin with /";
    return nullptr;
  }

  while (stream.peek() != EOF) {
    getline(stream, component, '/');
    if (!regex_match(component, component_regex)) {
      valid = false;
      message = "Invalid component name";
      return nullptr;
    }
    path.push_back(component);
  }

  int i;
  for (i = 0; i < path.size() - 1; ++i) {
    next_file = file->GetChildByName(path[i]);
    /* File doesn't exist yet; create it. */
    if (!next_file) {
      next_file = &file->AddChild(path[i]);
      next_file->AddPermission("*", "*", true, false);
    }
    file = next_file;
  }

  next_file = file->GetChildByName(path[i]);

  if (next_file) {
    valid = false;
    message = "File already exists";
    return nullptr;
  } else {
    next_file = &file->AddChild(path[i]);
    file->AddPermission(user, group, true, true);
  }

  return next_file;
}

/* User create command */
File *Create(string user, string group, string filename) {
  string component;
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;

  getline(stream, component, '/');

  if (!component.empty()) {
    valid = false;
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
  for (i = 0; i < path.size() - 1; ++i) {
    file = file->GetChildByName(path[i]);

    if (!file) {
      valid = false;
      message =
          "All components in the path must exist before creating a new one";
      return nullptr;
    }

    if (i < path.size() - 2) {
      if (!file->HasPermission(user, group, false)) {
        valid = false;
        message =
            "Read permissions on all components in the path are needed to "
            "reach a file";
        return nullptr;
      }
    } else {
      if (!file->HasPermission(user, group, true)) {
        valid = false;
        message =
            "Write permission on the parent component is needed to create a "
            "file";
        return nullptr;
      }
    }
  }

  File new_file(path[i]);

  ProcessAcl(new_file);

  if (valid) return &file->AddChild(new_file);
  else return nullptr;
}

void ProcessAcl(File &new_file) {
  string line, user, group, permissions;
  regex name_regex(kNamePattern), permission_regex(kPermissionPattern);
  bool can_read, can_write;

  getline(cin, line);

  while (line != ".") {
    stringstream stream(line);

    getline(stream, user, '.');
    getline(stream, group, ' ');
    getline(stream, permissions);

    /* User doesn't exist yet */
    if (users[user].empty()) {
      if (!regex_match(user, name_regex)) {
        valid = false;
        message = "Invalid username";
        return;
      }

      users[user].insert(group);
      groups[group].insert(user);
    }

    /* Group doesn't exist yet */
    if (groups[group].empty()) {
      if (!regex_match(group, name_regex)) {
        valid = false;
        message = "Invalid group name";
        return;
      }

      users[user].insert(group);
      groups[group].insert(user);
    }

    if (!regex_match(permissions, permission_regex)) {
      valid = false;
      message = "Invalid permissions";
      return;
    }

    can_read = permissions.find('r') != string::npos;
    can_write = permissions.find('w') != string::npos;

    new_file.AddPermission(user, group, can_read, can_write);

    getline(cin, line);
  }
}
