#include "debug.h"

#include "attr.h"
#include "ntfs_p.h"
#include "ntfs.h"
#include "inode.h"

class INode::Private
{
public:
	ullong inum;
	NTFS *ntfs;
	NTFSPrivate *p;

	typedef Attribute* AttributeP;

/*	class AttributeP {
	private:
		Attribute *real;
		
	public:
		AttributeP(uchar *data, NTFSPrivate *p) {
			real = new Attribute(data, p);
		}
		~AttributeP() {
			delete real;
		}
		Attribute *operator->() {
			return real;
		}
		Attribute *attr() {
			return real;
		}
		Attribute(Attribute &from) {
			real = new A
		}
	};*/
	
	vector<AttributeP> attrs;
};


///////////

INode::INode(ulong inum, NTFS *ntfs)
{
	d = new Private;

	d->inum = inum;
	d->ntfs = ntfs;
	d->p = ntfs->d;

	ullong file_size = d->p->cluster_size * d->p->cpmft;
	uchar *buf = new uchar[ file_size ];

	if ( inum ) // нужно искать запись в 0x80 аттрибуте нулевой ноды
		d->p->mft->attr(0x80)->read_data(file_size * inum, file_size, buf);
	else	// иначе можно просто прочитать с начала диска
		d->p->read(inum * d->p->cpmft + d->p->mft_start, d->p->cpmft, buf);

	NTFS_FILE file;
	memcpy(&file, buf, sizeof(NTFS_FILE));

	// пишем обратно слова из update sequence
	int upd_size = file.update_size - 1;
	for (int i = 0; i < upd_size; i++) {
		ushort bytes;
		memcpy(&bytes, buf + file.update_offset + 2 + i*2, sizeof(bytes));

		memcpy(buf + 0x200*(i+1) - 2, &bytes, sizeof(bytes));
	}

	ulong offset = file.updseq_offset;

	if (file.magic != 0x454C4946 ) {
		debug("INode::INode: magic number is (%x)\n", file.magic);
		goto exit;
	}

	while (1) {
		ulong type;
		memcpy(&type, buf + offset, sizeof(type));
		if ( type == 0xFFFFFFFF )
			break;
	
		NTFS_ATTR attr;
		memcpy(&attr, buf + offset, sizeof(NTFS_ATTR));

		d->attrs.push_back( new /*Private::*/Attribute(buf + offset, d->p) );
		offset += attr.full_length;
	}

exit:
	delete[] buf;
}

INode::~INode()
{
	delete d;
}

Attribute *INode::attr(ulong num)
{
	for (uint i = 0; i < d->attrs.size(); i++)
		if ( d->attrs[i]->type() == num )
			return d->attrs[i]/*.attr()*/;

	return 0;
}

vector<ulong> INode::attrs() {
	vector<ulong> list;

	for (uint i = 0; i < d->attrs.size(); i++)
		list.push_back( d->attrs[i]->type() );

	return list;
}
