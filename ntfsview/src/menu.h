#ifndef MENU_H
#define MENU_H

#include "common.h"

class NTFS;
class INode;

namespace Menu
{
	void main ();
	void part_select (int drive);
	bool view_part (int drive, ullong start, ullong num);
	void view_inode(ulong inum, NTFS *ntfs);
	void view_data(INode *inode, ulong attr);
};

#endif

