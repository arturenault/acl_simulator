#include "file.h"
#include <iostream>
File::File(string name) : name_(name) {}

File& File::AddChild(string name) {
  File new_file(name);
  new_file.parent_ = this;
  this->children_.push_back(new_file);
  return this->children_.back();
}

File& File::AddChild(File& file) {
  file.parent_ = this;
  if (file.permissions_.empty()) {
    file.permissions_ = this->permissions_;
  }
  this->children_.push_back(file);
  return this->children_.back();
}

File* File::GetChildByName(string name) {
  for (auto iter = this->children_.begin(); iter != this->children_.end();
       ++iter) {
    if (iter->name_ == name) {
      return &*iter;
    }
  }
  return nullptr;
}

string File::name() { return name_; }

File* File::parent() { return parent_; }

void File::AddPermission(string user, string group, bool can_read,
                         bool can_write) {
  AclEntry new_entry(user, group, can_read, can_write);
  this->permissions_.push_back(new_entry);
}

bool File::HasPermission(string user, string group, bool write) {
  for (auto iter = this->permissions_.begin(); iter != this->permissions_.end();
       ++iter) {
    if ((iter->user() == user || iter->user() == "*") &&
        (iter->group() == group || iter->group() == "*")) {
      return write ? iter->can_write() : iter->can_read();
    }
  }
  return false;
}

string File::ToString() {
  string output;
  output += name_ += "\n";
  for (auto iter = this->permissions_.begin(); iter != this->permissions_.end();
       ++iter) {
    output += iter->user() + "." + iter->group() + " " +
              (iter->can_read() ? "r" : "") +
              (iter->can_write() ? "w\n" : "\n");
  }
  return output;
}
