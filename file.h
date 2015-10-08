#ifndef __FILE_H__
#define __FILE_H__

#include <vector>
#include "acl_entry.h"

class File {
  string              name_;
  vector<AclEntry>    permissions_;
  vector<File>        children_;

  public:
  File(string n);
};

#endif
