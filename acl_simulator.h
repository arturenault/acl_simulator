#ifndef __ACL_SIMULATOR_H__
#define __ACL_SIMULATOR_H__

#include "file.h"

void ProcessUserDeclaration(string declaration);

File *FindFile(string filename);

/* Create new file in user definition section */
File *CreateUserFile(string user, string group, string filename);

void Read(string user, string group, string filename);

void Write(string user, string group, string filename);

File *Create(string user, string group, string filename);

void Delete(string user, string group, string filename);

void Acl(string user, string group, string filename);

void ProcessCommand(string command);

void ProcessAcl(File &new_file);

bool IsValidName(string name);

bool IsValidComponent(string name);

bool IsValidPermission(string name);
#endif
