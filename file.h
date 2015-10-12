#ifndef __FILE_H__
#define __FILE_H__

#include <vector>
#include "acl_entry.h"

class File {
  File* parent_;
  string name_;
  vector<AclEntry> permissions_;
  vector<File> children_;

 public:
  File(string name);

  File& AddChild(string name);
  File& AddChild(File& file);

  File* GetChildByName(string name);

  string name();
  File* parent();

  void AddPermission(string user, string group, bool can_read, bool can_write);

  bool HasPermission(string user, string group, bool write);

  string ToString();
};

#endif
