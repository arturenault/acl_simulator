#ifndef __FILE_H__
#define __FILE_H__

#include <vector>
#include "acl_entry.h"

class File {
  string name_;
  vector<AclEntry> permissions_;
  vector<File> children_;

 public:
  File(string name);
  File& AddChild(string name);
  File* GetChildByName(string name);

  string name();

  void AddPermission(string user, string group, bool can_read, bool can_write);

  bool HasPermission(string user, string group, bool write);
};

#endif
