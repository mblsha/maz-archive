#include "debug.h"

#include "attr.h"

#include "ntfs_p.h"

#include <vector>
using namespace std;

class Attribute::Private
{
public:
	Private() {
		name = 0;
		data = 0;
		compressed = false;
	}
	~Private() {
		if (name)
			delete[] name;
		if (data)
			delete[] data;
	}

	ulong type;
	char *name;
	bool resident;
	ullong size, allocated, initialized;
	bool compressed;

	uchar *data; // если атрибут резидентный то тут находятся данные
	struct DataRun {
		ullong lcn;
		ullong len;

		DataRun(ullong o, ullong l) { lcn = o; len = l; }
	};
	vector<DataRun> runlist;

	NTFSPrivate *p;

	void read_data(ulong cluster, ulong size, uchar *data) {
		uchar *buf = new uchar[ p->cluster_size ];
		ulong data_offs = 0;

		for (ulong i = 0; size; i++) {
			p->read_cluster(cluster + i, buf);

			ulong l = size >= p->cluster_size ? p->cluster_size : size;

			memcpy(data + data_offs, buf, l);

			data_offs += l;
			size -= l;
		}

		delete[] buf;
	}
};

//////////////////////////////////

bool mft_debug = true;

Attribute::Attribute(uchar *buf, NTFSPrivate *p)
{
	debug("*** Attr::Attr()\n");

	d = new Private;
	d->p = p;

	NTFS_ATTR attr;
	memcpy (&attr, buf, sizeof(attr));

	d->type = attr.type;

	if ( d->type == 0xFFFFFFFF ) {
		d->size = 0;
		d->data = 0;
		return;
	}
	
	char *tmp_name = decode_ustring (attr.name_length, buf + attr.name_offset);
	d->name = new char[ strlen(tmp_name) + 1 ];
	strcpy (d->name, tmp_name);

	d->resident = !attr.nonresflag;

	if ( !attr.nonresflag ) {
		NTFS_ATTR_RESIDENT attr;
		memcpy (&attr, buf, sizeof(attr));

		d->size = attr.length;
		
		d->data = new uchar[ attr.length ];
		memcpy (d->data, buf + attr.attr_offset, attr.length);
	}
	else {
		NTFS_ATTR_NONRESIDENT attr;
		memcpy (&attr, buf, sizeof(attr));

		d->size = attr.real_size;
		d->allocated = attr.alloc_size;
		d->initialized = attr.stream_size;

		//d->compressed = attr.comp_size; // хмм...

		if ( mft_debug )
			debug("$MFT::Attr (%X): ", attr.type);

		// читаем datarun-ы
		{
			ullong lcn = 0;
			uchar *dr = buf + attr.datarun_offset;
			while(1) {
				uchar lenoffs;
				memcpy (&lenoffs, dr, sizeof(lenoffs));
				if ( !lenoffs )
					break;

				uchar len_size  =  lenoffs & 0x0F;
				uchar offs_size = (lenoffs & 0xF0) >> 4;

				ullong len = 0; // длина всегда unsigned
				memcpy (&len, dr + 1, len_size);

				if ( mft_debug )
					debug("len = %lld; ", len);

				bool zero_offs;

#define READ_OFFS		memcpy(&offset, dr + 1 + len_size, offs_size);	\
				if ( !lcn )					\
					lcn = offset;				\
				else						\
					lcn += offset;				\
				zero_offs = (offset == 0);			\
				debug("lcn = %lld", lcn);			\
				break;

				switch (offs_size) {
					case 1: {
						char offset;
						READ_OFFS
					}
					case 2: {
						short offset;
						READ_OFFS
					}
					case 3: {
						cout << "AAA!!! 3-byte offset in datarun!!!";
						int offset;
						READ_OFFS
					}
					case 4: {
						int offset;
						READ_OFFS
					}
					case 5: {
						cout << "AAA!!! 5-byte offset in datarun!!!";
						llong offset;
						READ_OFFS
					}
					case 6: {
						cout << "AAA!!! 6-byte offset in datarun!!!";
						llong offset;
						READ_OFFS
					}
					case 7: {
						cout << "AAA!!! 7-byte offset in datarun!!!";
						llong offset;
						READ_OFFS
						}
					case 8: {
						llong offset;
						READ_OFFS
					}
				}
#undef READ_OFFS
				dr += 1 + len_size + offs_size;
				d->runlist.push_back( Private::DataRun(zero_offs ? 0 : lcn, len) );
			}
		}
		if ( mft_debug )
			debug("\n");
		//mft_debug = false;
	}
}

Attribute::~Attribute()
{
	debug("*** Attr::~Attr()\n");
	delete d;
}

ulong Attribute::type() const
{
	return d->type;
}

char *Attribute::name() const
{
	return d->name;
}

bool Attribute::resident() const
{
	return d->resident;
}

ullong Attribute::size() const
{
	return d->size;
}

ullong Attribute::allocated() const
{
	return d->allocated;
}

ullong Attribute::initialized() const
{
	return d->initialized;
}

bool Attribute::compressed() const
{
	return d->compressed;
}

uchar *Attribute::data()
{
	uchar *buf = new uchar[ size() ];
	if ( !buf )
		return 0;
	
	if ( resident() )
		memcpy( buf, d->data, size() );
	else
		read_data(0, size(), buf);

	return buf;
}

void Attribute::read_data(ulong offset, ulong len, uchar *buf)
{
	if (offset >= size()) {
		return;
	}

	ullong l = len;
	if (offset + len >= size()) {
		l = size() - offset;
	}

	if ( resident() ) {
		memcpy(buf, d->data + offset, l);
		return;
	}

	if ( offset >= initialized() ) {
		memset(buf, 0, l);
		return;
	}

	if ( offset + l > initialized() ) { // was >=
		ullong chunk = offset + l - initialized();
		read_data (offset, l - chunk, buf);
		memset (buf + l - chunk, 0, chunk);
		return;
	}

	if ( compressed() ) {
		debug("Attr::read_data: WARNING! Tried to read compressed attr!\n");
		memset (buf, 0, l); // не поддерживается
		return;
	}

	ulong cluster_size = d->p->cluster_size;

	// читаем данные нерезидентного аттрибута
	ulong s_vcn = offset / cluster_size;
	ulong vcn = 0;
	ulong i;
	for (i = 0; i < d->runlist.size() && ((vcn + d->runlist[i].len) <= s_vcn); i++)
		vcn += d->runlist[i].len;

	if ( i == d->runlist.size() )
		return;

	ullong buf_offs = 0;
	while ( l > 0 ) { // тут написано эль-маленькое!
		s_vcn = offset / cluster_size;
		ulong cluster = d->runlist[i].lcn;
		ulong len = d->runlist[i].len;

		ulong s_cluster = cluster + s_vcn - vcn;

#undef MIN
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))

		ullong chunk = MIN(((vcn + len) * cluster_size - offset), l); // сколько байт читаем

		d->read_data(s_cluster, chunk, buf + buf_offs);
		buf_offs += chunk;

		l -= chunk;
		offset += chunk;

		if (l && offset >= (vcn + len) * cluster_size) {
			vcn += len;
			i++;
		}
	}
}

/***************************************************************************
 * Class Attribute
 ***************************************************************************/
/*
struct Runlist
{
	ullong offset;
	ullong len;
};

class Attribute
{
private:
	int type;
	char *name; // имя в локальной кодировке
	int attrno;
	int size, alloc, init, compsize;
	int compressed, resident, indexed;
	int cengine;

	uchar *data; // если аттрибут резидентный
	list<Runlist> runlist;
	
public:
	Attribute (uchar *buf);
	~Attribute ();
};

Attribute::Attribute (uchar *buf)
{
	NTFS_ATTR attr;
	memcpy (&attr, buf, sizeof(attr));

	type = attr.type;
	
	char *tmp_name = decode_ustring (attr.name_length, buf + attr.name_offset);
	name = new char[ strlen(tmp_name) + 1 ];
	strcpy (name, tmp_name);

	// читаем данные
	if ( !attr.nonresflag )
	{
		NTFS_ATTR_RESIDENT attr;
		memcpy (&attr, buf + start, sizeof(attr));
		
		data = new uchar[ attr.length ];
		memcpy (data, buf + attr.attr_offset, attr.length);
		break;
	}
	else
	{
		NTFS_ATTR_NONRESIDENT attr;
		memcpy (&attr, buf + start, sizeof(attr));
		
		data = new uchar[ attr.alloc_size ]; // данные аттрибута
		read_datarun (buf + start + attr.datarun_offset, data);
		break;
	}
}

class INode
{
private:
	NTFS *ntfs;
	ullong i_num;
	ushort seq_num;

	list<Attribute> attrs;
	typedef int *intP;
	list<intP> records;
};

INode::INode ()
{
	uchar *data = 0; // будем это возвращать
	uchar *buf = new uchar [ cpmft * cluster_size ];
	read (mft_start + num*cpmft, cpmft*spcluster, buf);

	NTFS_FILE file;
	memcpy (&file, buf, sizeof(file));

	if ( file.magic != 0x454C4946 )
	{
		printf ("$MFT item's magic isn't \"FILE\" (%04lX)!\n", (long)file.magic);
		exit (1);
	}

	// аттрибуты начинают идти после: update_offset + update_size*2
	ulong start = file.update_offset + file.update_size*2;
	while ( 1 )
	{
		if ( attr.type == 0xFFFFFFFF )
			break;


next:
		start += attr.full_length;
	}

	delete[] buf;
	return data;
}*/

