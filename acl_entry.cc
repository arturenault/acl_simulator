#include "acl_entry.h"

using namespace std;

AclEntry::AclEntry(string u, string g, bool r, bool w) :
  user(u),
  group(g),
  can_read_(r),
  can_write_(w)
{
}

bool AclEntry::can_read() {
  return this->can_read_;
}

bool AclEntry::can_write() {
  return this->can_write_;
}
