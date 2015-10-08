#ifndef __ACL_ENTRY_H__
#define __ACL_ENTRY_H__

#include <string>
using namespace std;

class AclEntry {
  public:
    AclEntry(string u, string g, bool r, bool w);
    bool can_read();
    bool can_write();

  private:
    string user;
    string group;
    bool can_read_;
    bool can_write_;
};

#endif
