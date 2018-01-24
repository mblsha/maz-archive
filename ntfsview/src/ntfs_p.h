#ifndef NTFSP_H
#define NTFSP_H

#include "ntfs.h"

#include "attr.h"
#include "inode.h"

struct NTFS_BOOT	// первый сектор NTFS-раздела
{
	char	bootjmp[3]	PACKED;
	ulong	magic		PACKED; // "NTFS"
	ulong	dummy1		PACKED; // "    "
	ushort	bpsector	PACKED; // bytes per sector
	uchar	spcluster	PACKED; // sectors per cluster
	char	dummy2[7]	PACKED;
	uchar	media_desc	PACKED; // 0xF8 -- Hard disk
	char	dummy3[2]	PACKED;
	ushort	sptrack		PACKED; // sectors per track
	ushort	heads		PACKED; // number of heads
	char	dummy4[8]	PACKED;
	ulong	magic2		PACKED; // always 0x00800080
	ullong	sectors		PACKED; // number of sectors
	ullong	mft, mftmirr	PACKED; // LCNs of $MFT & $MFTMirr
	ulong	cpmft		PACKED; // clusters per MFT record
	ulong	cpindex		PACKED; // clusters per Index record
	ullong	serial		PACKED; // volume serial number

	char	loader[0x1AE]	PACKED;

	ushort	signature	PACKED; // must be 0xAA55
};

struct NTFS_FILE	// запись в MFT
{
	ulong	magic		PACKED; // must be "FILE"
	ushort	update_offset	PACKED;
	ushort	update_size	PACKED;
	ullong	lsn		PACKED; // $LogFile Sequence Number
	ushort	seq_number	PACKED;
	ushort	hardlink_count	PACKED; // в скольких директориях находится
	ushort	updseq_offset	PACKED;
	ushort	flags		PACKED;
	ulong	real_size	PACKED;
	ulong	alloc_size	PACKED;
	ullong	file_ref	PACKED;
	ushort	next_attr	PACKED;
};

struct NTFS_ATTR	// аттрибут записи в MFT
{
	ulong	type		PACKED;
	ulong	full_length	PACKED;
	uchar	nonresflag	PACKED; // 0x00
	uchar	name_length	PACKED;
	ushort	name_offset	PACKED;
	ushort	flags		PACKED;
	ushort	id		PACKED;
	// дальше зависит от резидентности аттрибута
};

struct NTFS_ATTR_RESIDENT	// резидентный аттрибут записи в MFT
{
	ulong	type		PACKED;
	ulong	full_length	PACKED;
	uchar	nonresflag	PACKED; // 0x00
	uchar	name_length	PACKED;
	ushort	name_offset	PACKED;
	ushort	flags		PACKED;
	ushort	id		PACKED;

	ulong	length		PACKED; // длина данных в атрибуте
	ushort	attr_offset	PACKED;
	uchar	indexed_tag	PACKED;
	uchar	padding		PACKED;
	// ushort name [ name_length ]
	// uchar  attribute [ length ]
};

struct NTFS_ATTR_NONRESIDENT	// нерезидентный аттрибут записи в MFT
{
	ulong	type		PACKED;
	ulong	full_length	PACKED;
	uchar	nonresflag	PACKED; // 0x01
	uchar	name_length	PACKED;
	ushort	name_offset	PACKED;
	ushort	flags		PACKED;
	ushort	id		PACKED;

	ullong	start_vcn	PACKED;
	ullong	end_vcn		PACKED;
	ushort	datarun_offset	PACKED;
	ushort	comp_size	PACKED; // compression unit size
	uchar	padding[4]	PACKED;
	ullong	alloc_size	PACKED;
	ullong	real_size	PACKED;
	ullong	stream_size	PACKED;
	// data runs...
};

struct NTFS_INDX
{
	ulong type		PACKED; // "INDX"
	ushort updseq_offset	PACKED;
	ushort updseq_size	PACKED;
	ullong lsn		PACKED;
	ullong vcn		PACKED; // VCN of this INDX
	ulong ie_offset		PACKED; // Смещение к index entries
	ulong ie_size		PACKED;
	ulong ie_alloc_size	PACKED;
	uchar is_not_leaf_node	PACKED;
	uchar padding[2]	PACKED;
};

struct NTFS_INDX_ENTRY
{
	MFT_REF mft_ref		PACKED;
	ushort size		PACKED;
	ushort filename_offset	PACKED;
	ushort index_flags	PACKED;
	uchar  padding[2]	PACKED;
	ullong parent_mft_ref	PACKED;
	ullong creation_time	PACKED;
	ullong last_mod_time	PACKED; // last modification time
	ullong last_mod_of_file	PACKED; // --||-- of FILE record
	ullong last_access_time	PACKED;
	ullong alloc_size	PACKED;
	ullong real_size	PACKED;

	//ullong flags		PACKED;
	uchar flag1		PACKED;
	uchar dummy[2]		PACKED;
	uchar dirflag		PACKED; // если = 0x10 то это директория
	uchar dummy2[4]		PACKED;
		
	uchar filename_length	PACKED;
	uchar filename_namespace PACKED;
};

///

class NTFSPrivate
{
//private:
public:
	ullong	start;	// начальный сектор раздела
	int	drive;	// номер жесткого диска с разделом

	// инициализируются из загрузочного сектора
	uchar	spcluster;
	ushort	sptrack;
	ushort	heads;
	ullong	mft_start, mftmirr_start;
	ulong	cpmft;	// размер одной записи MFT в кластерах
 	ulong	cpindex;
	ushort	cluster_size;

	INode *mft;

	uchar	*cluster; // буфер на один кластер
	void	read_cluster (ullong vcn); // читает VCN в буфер кластера
	void	read_cluster (ullong vcn, uchar *buf);
	void	read (ullong vcn, ulong len, uchar *buf);

	void	read_attr_index_alloc (NTFS_ATTR_NONRESIDENT attr, uchar *data); // во втором параметре, то что было в datarun'ах

	void	read_datarun (uchar *dr, uchar *data);
	void	read_resident_attribute (uchar *buf);
	void	read_nonresident_attribute (uchar *buf);
	
	uchar	*read_attribute (ullong num, ulong id); // возвращает данные, или 0
	ulong	read_attribute (uchar *buf); // возвращает длину аттрибута
	
	void load_boot ();	// загрузка загрузочного сектора
	
	void read_mft_item (ullong num);
	void read_mft ();

public:
	NTFSPrivate (ullong _start, int _drive);
	~NTFSPrivate ();
	
	DirList read_directory (ullong mft_num);
	
/*	friend class NTFSFile;
	friend class NTFSDir;

	friend class Attribute;
	friend class Attribute::Private;
	friend class INode;*/
};

#endif
