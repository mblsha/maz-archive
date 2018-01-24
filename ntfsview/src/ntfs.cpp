#include "debug.h"

#include "ntfs.h"
#include "hdd.h"
#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "attr.h"
#include "ntfs_p.h"

/***************************************************************************
 * Class NTFSPrivate
 ***************************************************************************/

NTFSPrivate::NTFSPrivate (ullong _start, int _drive)
{
	start = _start;
	drive = _drive;
	
	load_boot ();
	//read_mft ();
}

NTFSPrivate::~NTFSPrivate ()
{
	delete[] cluster;
	delete mft;
}

void NTFSPrivate::load_boot ()
{
	NTFS_BOOT boot;
	HDD::read (drive, start, &boot);

	// проверка магических чисел
	if ( boot.magic != 0x5346544E || boot.dummy1 != 0x20202020 )
	{
		printf ("SystemId != \"NTFS    \"\n");
		exit (1);
	}

	if ( boot.media_desc != 0xF8 )
	{
		printf ("Ошибка в медиа дескрипторе раздела!\n");
		exit (1);
	}

	if ( boot.magic2 != 0x00800080 )
	{
		printf ("Ошибка второго магического числа!\n");
		exit (1);
	}

	if ( boot.signature != 0xAA55 )
	{
		printf ("Не сходится подпись раздела!\n");
		exit (1);
	}

	if ( boot.bpsector != 0x200 )
	{
		printf ("ОШИБКА: Размер сектора не равен 512 байт!\n");
		exit (1);
	}

	// сохранение значений
	spcluster = boot.spcluster; // ВНИМАНИЕ! тут возможно надо возводить 2 в степень
	sptrack   = boot.sptrack;
	heads     = boot.heads;
	mft_start     = boot.mft;
	mftmirr_start = boot.mftmirr;
	cpmft   = boot.cpmft;
	cpindex = boot.cpindex;

	cluster_size = 0x200 * spcluster;

	cluster = new uchar[ cluster_size ];
}

void NTFSPrivate::read_cluster (ullong vcn)
{
	ullong sector = start + vcn*spcluster;
	HDD::read (drive, sector, spcluster, cluster);
}

void NTFSPrivate::read_cluster (ullong vcn, uchar *buf)
{
	ullong sector = start + vcn*spcluster;
	
	// отладка
	debug ("reading cluster @ sector %lld; ", sector);
	debug ("start = %lld; ", start);
	debug ("vcn = %lld; ", vcn);
	debug ("\n");
	//getch ();
	
	HDD::read (drive, sector, spcluster, buf);
}

void NTFSPrivate::read (ullong vcn, ulong len, uchar *buf)
{
	/*if ( !vcn ) // надо читать нули
	{
		for (uint i = 0; i < len; i++)
			for (uint j = 0; j < cluster_size; j++)
				buf[i*cluster_size + j] = 0;
		return;
	}*/

	for (uint i = 0; i < len; i++, vcn++)
	{
		debug ("...reading cluster #%d\n"/*", at offset %d\n"*/, vcn/*, i*cluster_size*/);
		
		read_cluster (vcn, &buf[ i * cluster_size ]);
	}
}

void read_attr_index_root (uchar *data)
{
	struct INDEX_ROOT
	{
		ulong type			PACKED;
		ulong collation_rule		PACKED;
		ulong size_of_index_alloc	PACKED;
		uchar cpindex_record		PACKED; // clusters per index record
		uchar padding[3]		PACKED;
	};
	struct INDEX_HEADER
	{
		ulong index_entry_offset	PACKED;
		ulong size_index_entries	PACKED;
		ulong alloc_size_index_entries	PACKED;
		uchar flags			PACKED;
		uchar padding[3]		PACKED;
	};
	struct INDEX_ENTRY
	{
		ullong file_ref			PACKED;
		ushort length			PACKED;
		ushort stream_length		PACKED;
		uchar  flags			PACKED;
	};
}

void NTFSPrivate::read_resident_attribute (uchar *buf)
{
	NTFS_ATTR_RESIDENT attr;
	memcpy (&attr, buf, sizeof(attr));
	debug ("attrib: %X; name: \"%s\"\n", attr.type, decode_ustring(attr.name_length, buf + attr.name_offset));

	uchar *data = new uchar[ attr.length ];
	memcpy (data, buf + attr.attr_offset, attr.length);

	// обработать аттрибут
	switch ( attr.type )
	{
		case 0x90: // INDEX_ROOT. Всегда резидентный
			read_attr_index_root ( data );
			break;
	}
	
	delete[] data;
}

void NTFSPrivate::read_datarun (uchar *dr, uchar *data)
{
	ullong lcn = 0; // текущий LCN в datarun'e
	ullong data_offset = 0; // текущее смещение в data

	while ( 1 ) // Чтение DataRun'a
	{
		uchar lenoffs; // упакованные в одном байте размер длины и оффсета
		memcpy (&lenoffs, dr, sizeof(lenoffs));
		if ( !lenoffs )
			break; // dataruns кончились

		uchar len_size  =  lenoffs & 0x0F;
		uchar offs_size = (lenoffs & 0xF0) >> 4;

		ullong len = 0; // длина всегда unsigned
		memcpy (&len, dr + 1, len_size);

		ullong offset = 0; // смещение всегда signed
		memcpy (&offset, dr + 1 + len_size, offs_size);
/*
		bool zero_offs = false; // надо ли читать нули

#define READ_OFFS	memcpy(&offset, dr + 1 + len_size, offs_size); \
			lcn += (offset != 0) ? offset : 0; \
			zero_offs = offset != 0; \
			break;

		debug("here...\n");

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
*/
		//debug ("%lld @ %lld\n", offset, len);
		dr += 1 + len_size + offs_size;

		if ( !lcn )
			lcn = offset;
		else
			lcn += offset;

		//debug ("reading %lld clusters @ %lld (data_offset = %lld)...\n", len, lcn, data_offset);
		//debug("here\n");
		//read (zero_offs ? 0 : lcn, len, data + data_offset);
		read (lcn, len, data + data_offset);
		data_offset += len * cluster_size;
	}
}

void NTFSPrivate::read_attr_index_alloc (NTFS_ATTR_NONRESIDENT attr, uchar *data)
{
	NTFS_INDX	indx;
	NTFS_INDX_ENTRY	entry;

	memcpy (&indx, data, sizeof(indx));	
	ulong offset = 0x18 + indx.ie_offset;
	
	while ( 1 )
	{
		memcpy (&entry, data + offset, sizeof(entry));
		if ( !entry.mft_ref.ref() )
			break; // TODO: надо найти более хорошее решение

		debug ("\"%s\" - %d - %s\n",
			decode_ustring(entry.filename_length, data + offset + 0x52),
			entry.filename_namespace,
			(entry.dirflag == 0x10) ? "Directory" : "File");
		
		offset += entry.size;
	}
}

void NTFSPrivate::read_nonresident_attribute (uchar *buf)
{
	NTFS_ATTR_NONRESIDENT attr;
	memcpy (&attr, buf, sizeof(attr));
	debug ("attrib: %X; name: \"%s\"\n", attr.type, decode_ustring(attr.name_length, buf + attr.name_offset));
	//debug ("non-resident attrib: %X;\n", attr.type);

	//debug ("alloc_size = %d; cl_size = %d\n", attr.alloc_size, cluster_size);
	uchar *data = new uchar[ attr.alloc_size ]; // данные аттрибута
	read_datarun (buf + attr.datarun_offset, data);

	switch ( attr.type )
	{
		case 0xA0: // INDEX_ALLOCATION. Всегда нерезидентный
			read_attr_index_alloc ( attr, data );
			break;
	}

	delete[] data;	
}

ulong NTFSPrivate::read_attribute (uchar *buf)
{
	NTFS_ATTR attr;
	memcpy (&attr, buf, sizeof(attr));

	if ( attr.type == 0xFFFFFFFF )
		return 0;

	if ( !attr.nonresflag )
		read_resident_attribute ( buf );
	else
		read_nonresident_attribute ( buf );

	return attr.full_length;
}

void NTFSPrivate::read_mft_item (ullong num)
{
	//debug ("cpmft = %d; cluster_size = %d;\n", cpmft, cluster_size);
	uchar *buf = new uchar [ cpmft * cluster_size ];
	read (mft_start + num*cpmft, cpmft*spcluster, buf);

	NTFS_FILE file;
	memcpy (&file, buf, sizeof(file));

	if ( file.magic != 0x454C4946 )
	{
		printf ("$MFT item's magic isn't \"FILE\" (%X)!\n", file.magic);
		exit (1);
	}

	// аттрибуты начинают идти после: update_offset + update_size*2
	ulong start = file.update_offset + file.update_size*2;
	ulong len;
	for ( len = 1; len; start += len )
		len = read_attribute(buf + start);

	delete[] buf;
}

void NTFSPrivate::read_mft ()
{
	/*for (ullong i = 0; i < 16; i++)
	{
		debug ("Reading MFT item #%d...\n", i);
		read_mft_item ( i );
	}*/
	
	debug ("Reading MFT item #5 -- \".\"...\n");
	read_mft_item ( 5 );
}

uchar *NTFSPrivate::read_attribute (ullong num, ulong id)
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
		NTFS_ATTR attr;
		memcpy (&attr, buf + start, sizeof(attr));

		if ( attr.type == 0xFFFFFFFF )
			break;

		if ( attr.type != id )
			goto next;
			
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

next:
		start += attr.full_length;
	}

	delete[] buf;
	return data;
}

DirList NTFSPrivate::read_directory (ullong mft_num)
{
	DirList list;
	uchar *data = read_attribute (mft_num, 0xA0);

	if ( !data )
		return list;

	NTFS_INDX	indx;
	NTFS_INDX_ENTRY	entry;

	memcpy (&indx, data, sizeof(indx));	
	ulong offset = 0x18 + indx.ie_offset;
	
	while ( 1 )
	{
		memcpy (&entry, data + offset, sizeof(entry));
		if ( !entry.mft_ref.ref() )
			break; // TODO: надо найти более хорошее решение для выхода из цикла

		char *name = decode_ustring(entry.filename_length, data + offset + 0x52);

		//debug ("Adding member (MFT_REF = %d)\n", entry.mft_ref.num());
		//debug (", name = '%s')\n", name);

		if ( entry.dirflag == 0x10 )
			list.add_item ( new NTFSDir (entry.mft_ref, name), entry.filename_namespace );
		else
			list.add_item ( new NTFSFile (entry.mft_ref, name), entry.filename_namespace );
		
		offset += entry.size;
	}

	delete[] data;
	return list;
}

/***************************************************************************
 * Class NTFS
 ***************************************************************************/

NTFS::NTFS (ullong start, int drive)
{
	d = new NTFSPrivate (start, drive);
	d->mft = new INode(0, this);
}

NTFS::~NTFS ()
{
	delete d;
}

DirList NTFS::read_directory (ullong mft_num)
{
	return d->read_directory (mft_num);
}

/***************************************************************************
 * Class NTFSItem
 ***************************************************************************/

NTFSItem::NTFSItem (MFT_REF _mft_ref, char *_name)
{
	mft_ref = _mft_ref;
	name_data = new char[ strlen(_name) + 1 ];
	strcpy (name_data, _name);
}

NTFSItem::~NTFSItem ()
{
	delete[] name_data;
}

char *NTFSItem::name ()
{
	return name_data;
}

MFT_REF NTFSItem::ref ()
{
	return mft_ref;
}

bool NTFSItem::is_file ()
{
	return true;
}

/***************************************************************************
 * Class NTFSFile
 ***************************************************************************/

NTFSFile::NTFSFile (MFT_REF _mft_ref, char *_name)
	: NTFSItem (_mft_ref, _name)
{
}

NTFSFile::~NTFSFile ()
{
}

/***************************************************************************
 * Class NTFSDir
 ***************************************************************************/

NTFSDir::NTFSDir (MFT_REF _mft_ref, char *_name)
	: NTFSItem (_mft_ref, _name)
{
}

NTFSDir::~NTFSDir ()
{
}

bool NTFSDir::is_file ()
{
	return false;
}

/***************************************************************************
 * Class DirList
 ***************************************************************************/

void DirList::add_item (NTFSItem *item, int ns)
{
	if ( ns == 2 )
		return; // пропускаем DOS namespace

	//if ( item->mft_ref.num() < 25 )
	//	return; // пропускаем служебные файлы и каталоги
		
	push_back (item);
}


