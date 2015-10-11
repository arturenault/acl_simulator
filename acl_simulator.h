#ifndef __ACL_SIMULATOR_H__
#define __ACL_SIMULATOR_H__

#include "file.h"

void ProcessUserDeclaration(string declaration);

File *FindFile(string filename);

/* Create new file in user definition section */
File *CreateUserFile(string user, string group, string filename);

/* Create file as a user */
File *Create(string user, string group, string filename);

#endif
