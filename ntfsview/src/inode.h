#ifndef INODE_H
#define INODE_H

#include <vector>
using namespace std;

class Attribute;

class INode
{
public:
	INode(ulong inum, NTFS *ntfs);
	~INode();

	Attribute *attr(ulong num);
	vector<ulong> attrs();

private:
	class Private;
	Private *d;
};

#endif
