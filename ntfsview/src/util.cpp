#include "debug.h"

#include "util.h"
#include "ntfs.h"
#include "attr.h"
#include "inode.h"
#include "ntfs_p.h"

#include <stdio.h>

class Dir::Private
{
public:
	Private() {}
	~Private() {
		delete inode;
	}

	ulong inum;
	NTFS *ntfs;
	NTFSPrivate *p;

	ulong clust_per_record;
	ulong record_size;

	INode *inode;
	
	void iterate(uchar *data, DirList &list)
	{
		struct INDEX_ENTRY {
			ulong mft_num	PACKED;
			ushort blah	PACKED;
			ushort seq	PACKED;

			ushort length	PACKED;
			ushort stream_size	PACKED;

			uchar flags	PACKED;
		} entry;

		while (1) {
			memcpy(&entry, data, sizeof(INDEX_ENTRY));
			if ( entry.flags & 1 ) { // указывает на суб-ноду
				ullong nextblock;
				memcpy(&nextblock, data + entry.length - 0x08, sizeof(nextblock));

				uchar *record = new uchar[ record_size ];
				Attribute *attr = inode->attr( 0xA0 );
				attr->read_data(nextblock * p->cluster_size, record_size, record);

				NTFS_INDX indx;
				memcpy(&indx, record, sizeof(NTFS_INDX));

				// update sequence
				int upd_size = indx.updseq_size - 1;
				for (int i = 0; i < upd_size; i++) {
					ushort bytes;
					memcpy(&bytes, record + indx.updseq_offset + 2 + i*2, sizeof(bytes));

					memcpy(record + 0x200*(i+1) - 2, &bytes, sizeof(bytes));
				}

				
				iterate(record + 0x18 + indx.ie_offset, list);

				delete[] record;
			}
			
			if ( entry.flags & 2 ) { // последний элемент
				//debug("*** flags & 2\n");
				break;
			}
			else if ( entry.stream_size > 0x25 ) { // достаточно ли места для записи
				NTFS_INDX_ENTRY index;
				memcpy(&index, data, sizeof(index));
				
				char *name = decode_ustring(index.filename_length, data + 0x52);

				//debug ("*** (%s)\n", name);

				if ( index.dirflag == 0x10 )
					list.add_item ( new NTFSDir (index.mft_ref, name), index.filename_namespace );
				else
					list.add_item ( new NTFSFile (index.mft_ref, name), index.filename_namespace );
			}

			data += entry.length;
		}
	}
};

Dir::Dir(ulong inum, NTFS *ntfs)
{
	d = new Private;

	d->inode = new INode(inum, ntfs);
	
	d->inum = inum;
	d->ntfs = ntfs;
	d->p = ntfs->d;
}

Dir::~Dir()
{
	delete d;
}

DirList Dir::list()
{
	DirList dirlist;

	struct INDEX_ROOT {
		ulong type		PACKED;
		ulong collation		PACKED;
		ulong index_alloc_size	PACKED;
		uchar cpindex_record	PACKED;
		char dummy[3]		PACKED;
	} index_root;
	
	uchar *root = 0;

	Attribute *attr = d->inode->attr( 0x90 );
	if ( !attr ) {
		debug("Dir::list: inum = %d, 0x90 attr = 0\n", d->inum);
		goto exit;
	}

	root = attr->data();
	if ( !root ) {
		debug("Dir::list: root for inum %d is NULL\n", d->inum);
		goto exit;
	}

	memcpy(&index_root, root, sizeof(INDEX_ROOT));
	d->clust_per_record = index_root.cpindex_record;
	d->record_size = index_root.index_alloc_size;

	debug("Dir: c_p_r = %d, r_s = %d\n", d->clust_per_record, d->record_size);

	d->iterate(root + 0x20, dirlist);

/*	for (int i = 20; i < 35; i++) {
		//INode inode(i, d->ntfs);
	
		//uchar *record = new uchar[ record_size ];
		//Attribute *attr = inode->attr( 0xA0 );
		//attr->read_data(nextblock * p->cluster_size, record_size, record);
		//
		//NTFS_INDX_ENTRY index;
		//memcpy(&index, data, sizeof(index));


		char name[128];
		sprintf(name, "FILE #%d", i);

		MFT_REF ref;
		ref.mft_ref = i;
		dirlist.add_item ( new NTFSDir (ref, name), 1 );

		//char *name = decode_ustring(index.filename_length, data + 0x52);

		//debug ("*** (%s)\n", name);

		//if ( index.dirflag == 0x10 )
		//	list.add_item ( new NTFSDir (index.mft_ref, name), index.filename_namespace );
		//else
		//	list.add_item ( new NTFSFile (index.mft_ref, name), index.filename_namespace );

		//delete[] record;

	}*/

exit:
	if ( root )
		delete[] root;
	return dirlist;
}
