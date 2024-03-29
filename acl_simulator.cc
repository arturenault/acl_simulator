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
File dummy("");
bool valid, permitted;
string message;

const int kMaxComponentLength = 16;
const int kMaxFilenameLength = 256;
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
  while (line != "." && !cin.eof()) {
    message = "";
    valid = true;

    ProcessUserDeclaration(line);

    /* Log output */
    cout << line_no++ << "\t" << (valid ? "Y\t" : "X\t") << message << endl;

    getline(cin, line);
  }

  /* Add *-* r to every user-file ACL */
  for (auto iter = user_files.begin(); iter != user_files.end(); ++iter) {
    File *file = FindFile(iter->second);
    file->AddPermission("*", "*", true, false);
  }

  line_no = 1;
  while (!getline(cin, line).eof()) {
    message = "";
    valid = true;
    permitted = true;

    ProcessCommand(line);
    cout << line_no++ << (valid ? (permitted ? "\tY\t" : "\tN\t") : "\tX\t")
         << line << "\t" << message << endl;
  }
}

void ProcessUserDeclaration(string declaration) {
  string user, group, filename;
  stringstream reader(declaration);

  // Parse command info
  getline(reader, user, '.');
  getline(reader, group, ' ');
  getline(reader, filename);

  if (!IsValidName(user)) {
    message = "Invalid username";
    valid = false;
    return;
  }

  if (!IsValidName(group)) {
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
      users[user].insert(group);
      groups[group].insert(user);
      FindFile(user_files[user])->AddPermission(user, group, true, true);
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

  if (users[user].empty()) {
    valid = false;
    message = "User does not exist";
    return;
  } else if (users[user].find(group) == users[user].end()) {
    permitted = false;
    message = "User is not part of given group";
    return;
  }

  if (groups[group].empty()) {
    valid = false;
    message = "Group does not exist";
    return;
  }

  if (operation == "READ") {
    Read(user, group, filename);
  } else if (operation == "WRITE") {
    Write(user, group, filename);
  } else if (operation == "CREATE") {
    Create(user, group, filename);
  } else if (operation == "DELETE") {
    Delete(user, group, filename);
  } else if (operation == "ACL") {
    Acl(user, group, filename);
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
    if (!IsValidComponent(component)) {
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
    next_file->AddPermission(user, group, true, true);
  }

  return next_file;
}

/* User read command */
void Read(string user, string group, string filename) {
  string component;
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;

  getline(stream, component, '/');

  if (!component.empty()) {
    valid = false;
    message = "All filenames must begin with /";
  } else {
    while (stream.peek() != EOF) {
      getline(stream, component, '/');
      path.push_back(component);
    }

    int i;
    for (i = 0; i < path.size(); ++i) {
      file = file->GetChildByName(path[i]);

      if (!file) {
        valid = false;
        message = "File not found: all files in path must exist";
        break;
      }

      if (!file->HasPermission(user, group, false)) {
        permitted = false;
        message = "Permission denied: cannot read file";
        break;
      }
    }
  }
}

/* User write command */
void Write(string user, string group, string filename) {
  string component;
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;

  getline(stream, component, '/');

  if (!component.empty()) {
    valid = false;
    message = "File not found: all filenames begin with /";
  } else {
    while (stream.peek() != EOF) {
      getline(stream, component, '/');
      path.push_back(component);
    }

    int i;
    for (i = 0; i < path.size() - 1; ++i) {
      file = file->GetChildByName(path[i]);

      if (!file) {
        valid = false;
        message = "File not found: all files in path must exist";
        return;
      }

      if (!file->HasPermission(user, group, false)) {
        permitted = false;
        message = "Permission denied: cannot access file";
        return;
      }
    }

    file = file->GetChildByName(path[i]);
    if (!file) {
      valid = false;
      message = "File not found: file must be created before being written.";
      return;
    }
    if (!file->HasPermission(user, group, true)) {
      permitted = false;
      message = "Permission denied: cannot write to file";
      return;
    }
  }
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
    message = "File not found: all filenames begin with /";
  } else {
    while (stream.peek() != EOF) {
      getline(stream, component, '/');
      path.push_back(component);
    }

    if (path.size() == 1) {
      permitted = false;
      message = "Permission denied: users cannot write to the root directory";
    }

    /* Path.size - 1 because the last file doesn't have
     * to exist yet. */
    int i;
    for (i = 0; i < path.size() - 1; ++i) {
      file = file->GetChildByName(path[i]);

      if (!file) {
        valid = false;
        message =
            "File not found: all components in the path must exist before "
            "creating a new one";
        break;
      }

      if (i < path.size() - 2) {
        if (!file->HasPermission(user, group, false)) {
          permitted = false;
          message =
              "Permission denied: permissions on all components in the path "
              "are needed to "
              "reach a file";
          break;
        }
      } else {
        if (!file->HasPermission(user, group, true)) {
          // cout << file->ToString() << endl;
          permitted = false;
          message =
              "Permission denied: Write permission on the parent component is "
              "needed to create a "
              "file";
          break;
        } else if (file->GetChildByName(path.back()) != nullptr) {
          valid = false;
          message = "File already exists";
          break;
        }
      }
    }
  }

  if (!IsValidComponent(path.back())) {
    valid = false;
    message = "Invalid component name";
  }

  File *new_file = &file->AddChild(path.back());

  ProcessAcl(*new_file);

  if (valid && permitted) {
    if (!new_file->HasPermissions()) {
      new_file->CopyPermissionsFromParent();
    }
    return new_file;
  } else {
    file->DeleteChild(path.back());
    return nullptr;
  }
}

/* User Delete command */
void Delete(string user, string group, string filename) {
  string component;
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;

  getline(stream, component, '/');

  if (!component.empty()) {
    valid = false;
    message = "File not found: all filenames begin with /";
  } else {
    while (stream.peek() != EOF) {
      getline(stream, component, '/');
      path.push_back(component);
    }

    /* Path.size - 1 because we don't care about it just yet */
    int i;
    for (i = 0; i < path.size() - 1; ++i) {
      file = file->GetChildByName(path[i]);

      if (!file) {
        valid = false;
        message =
            "File not found: all components in the path must exist before "
            "creating a new one";
        return;
      }

      if (i < path.size() - 2) {
        if (!file->HasPermission(user, group, false)) {
          permitted = false;
          message =
              "Permission denied: permissions on all components in the path "
              "are needed to "
              "reach a file";
          return;
        }
      } else {
        if (!file->HasPermission(user, group, true)) {
          permitted = false;
          message =
              "Permission denied: Write permission on the parent component is "
              "needed to delete a "
              "file";
          return;
        }
      }
    }
  }

  File *delete_file = file->GetChildByName(path.back());
  if (!delete_file) {
    valid = false;
    message = "File not found";
    return;
  } else {
    if (delete_file->HasChildren()) {
      permitted = false;
      message = "Cannot delete a file that has children";
      return;
    } else {
      file->DeleteChild(path.back());
    }
  }
}

void Acl(string user, string group, string filename) {
  string component;
  vector<string> path;
  stringstream stream(filename);
  File *file = &root;

  getline(stream, component, '/');

  if (!component.empty()) {
    valid = false;
    message = "File not found: all filenames begin with /";
  } else {
    while (stream.peek() != EOF) {
      getline(stream, component, '/');
      path.push_back(component);
    }

    /* Path.size - 1 because the last file doesn't need read permissions. */
    int i;
    for (i = 0; i < path.size() - 1; ++i) {
      file = file->GetChildByName(path[i]);

      if (!file) {
        valid = false;
        message = "File not found: cannot reach file.";
        break;
      }

      if (!file->HasPermission(user, group, false)) {
        permitted = false;
        message =
            "Permission denied: permissions on all components in the path "
            "are needed to "
            "reach a file";
        break;
      }
    }
  }

  file = file->GetChildByName(path.back());

  if (!file) {
    file = &dummy;
    valid = false;
    message = "File not found";
  } else if (!file->HasPermission(user, group, true)) {
    permitted = false;
    message =
        "Permission denied: Write permission is needed for the ACL command";
  }

  File file_backup = *file;
  file->ClearPermissions();

  ProcessAcl(*file);

  if (!valid || !permitted)
    *file = file_backup;
  else if (!file->HasPermissions()) {
    file->CopyPermissionsFromParent();
  }
}

void ProcessAcl(File &new_file) {
  string line;
  bool can_read, can_write;

  getline(cin, line);

  while (line != "." && !cin.eof()) {
    string user, group, permissions;
    if (valid && permitted) {
      stringstream stream(line);

      getline(stream, user, '.');
      getline(stream, group, ' ');
      getline(stream, permissions);

      /* User doesn't exist yet */
      if (user != "*" && users[user].empty()) {
        if (!IsValidName(user)) {
          valid = false;
          message = "Invalid username in ACL";
          continue;
        }

        users[user].insert(group);
        groups[group].insert(user);
      }

      /* Group doesn't exist yet */
      if (group != "*" && groups[group].empty()) {
        if (!IsValidName(group)) {
          valid = false;
          message = "Invalid group name in ACL";
          continue;
        }

        users[user].insert(group);
        groups[group].insert(user);
      }

      if (!IsValidPermission(permissions)) {
        valid = false;
        message = "Invalid permissions";
        continue;
      }

      can_read = permissions.find('r') != string::npos;
      can_write = permissions.find('w') != string::npos;

      new_file.AddPermission(user, group, can_read, can_write);
    }
    getline(cin, line);
  }
}

bool IsValidName(string name) {
  if (name.length() == 0) return false;

  for (auto iter = name.cbegin(); iter != name.cend(); ++iter) {
    if (!islower(*iter)) {
      return false;
    }
  }

  return true;
}

bool IsValidComponent(string name) {
  if (name.length() == 0 || name.length() > kMaxComponentLength) return false;

  for (auto iter = name.cbegin(); iter != name.cend(); ++iter) {
    if (!islower(*iter) && *iter != '.') {
      return false;
    }
  }

  return true;
}

bool IsValidPermission(string name) {
  return name == "r" || name == "w" || name == "rw" || name == "-";
}
