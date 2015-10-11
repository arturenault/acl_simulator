#ifndef __ACL_ENTRY_H__
#define __ACL_ENTRY_H__

#include <string>
using namespace std;

class AclEntry {
 public:
  AclEntry(string user, string group, bool can_read, bool can_write);
  const string& user();
  const string& group();
  bool can_read();
  bool can_write();

 private:
  string user_;
  string group_;
  bool can_read_;
  bool can_write_;
};

#endif
