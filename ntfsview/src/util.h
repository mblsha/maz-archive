#ifndef UTIL_H
#define UTIL_H

#include "ntfs.h"

class Dir
{
public:
	Dir(ulong inum, NTFS *ntfs);
	~Dir();

	DirList list();

private:
	class Private;
	Private *d;
};

#endif
