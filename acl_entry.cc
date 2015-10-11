#include "acl_entry.h"

using namespace std;

AclEntry::AclEntry(string user, string group, bool can_read, bool can_write)
    : user_(user), group_(group), can_read_(can_read), can_write_(can_write) {}

const string& AclEntry::user() { return this->user_; }

const string& AclEntry::group() { return this->group_; }

bool AclEntry::can_read() { return this->can_read_; }

bool AclEntry::can_write() { return this->can_write_; }
