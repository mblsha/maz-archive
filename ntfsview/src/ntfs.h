#ifndef NTFS_H
#define NTFS_H

#include "common.h"

#include <vector>

class NTFSPrivate;

class MFT_REF {
public:
	union {
		ullong mft_ref;
		struct {
			ulong	lcn1	PACKED;
			ushort	lcn2	PACKED;
			ushort	desc	PACKED;
		} data;
	};	
public:

	ullong ref() { return mft_ref;   }
	ullong num() { return data.lcn1; }
	ushort seq() { return data.desc; }
};

class DirList;
class NTFSItem // общий класс одной записи в браузере
{
private:
	MFT_REF mft_ref;
	char *name_data;
	
public:
	NTFSItem (MFT_REF _mft_ref, char *_name);
	virtual ~NTFSItem ();
	
	char	*name ();
	MFT_REF	ref ();

	virtual bool is_file (); // true - если это файл, false - если директория
	friend class DirList;
};

class NTFSFile : public NTFSItem
{
public:
	NTFSFile (MFT_REF mft_ref, char *_name);
	virtual ~NTFSFile ();
};

class NTFSDir : public NTFSItem
{
public:
	NTFSDir (MFT_REF mft_ref, char *_name);
	virtual ~NTFSDir ();

	bool is_file ();
};

typedef NTFSItem *NTFSItemP;
class DirList : public std::vector<NTFSItemP>
{
public:
	void add_item (NTFSItemP item, int ns);
};

class NTFS
{
//private:
public:
	NTFSPrivate *d;

public:
	NTFS (ullong start, int drive);
	~NTFS ();
	
	DirList read_directory (ullong mft_num);

	friend class INode;
};

#endif

