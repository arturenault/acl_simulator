#include "file.h"
#include <iostream>
File::File(string name) : name_(name) {}

File& File::AddChild(string name) {
  File new_file(name);
  this->children_.push_back(new_file);
  return this->children_.back();
}

File* File::GetChildByName(string name) {
  for (auto iter = this->children_.begin(); iter != this->children_.end();
       iter++) {
    if (iter->name_ == name) {
      return &*iter;
    }
  }
  return nullptr;
}

string File::name() { return name_; }

void File::AddPermission(string user, string group, bool can_read,
                         bool can_write) {
  AclEntry new_entry(user, group, can_read, can_write);
  this->permissions_.push_back(new_entry);
}

bool File::HasPermission(string user, string group, bool write) {
  for (auto iter = this->permissions_.begin(); iter != this->permissions_.end();
       iter++) {
    if ((iter->user() == user || iter->user() == "*") &&
        (iter->group() == group || iter->group() == "*")) {
      return write ? iter->can_write() : iter->can_read();
    }
  }
  return false;
}
