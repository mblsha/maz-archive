#include "hdd.h"

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/farptr.h>

namespace HDD {

int init (int drive)
{
	union REGS inregs, outregs;

	inregs.h.ah = 0x41;
	inregs.w.bx = 0x55AA;
	inregs.h.dl = drive | 0x80;
	int86 (0x13, &inregs, &outregs);

	if ( outregs.h.cflag )
		return -1;
           
	if ( outregs.w.bx != 0xAA55 )
	{
		//printf ("BX != AA55h (%Xh)\n", outregs.w.bx);
		return -1;
	}
        
	if ( outregs.h.ah < 0x30 )
	{
		//printf ("AH = %Xh\n", outregs.h.ah);
		//cout << "EDD-3.0 Extensions required.\n";
		return -1;
	}

	return 0;
}

int num ()
{
	return _farpeekb (_dos_ds, 0x475);
}

string error (int d)
{
	union REGS inregs, outregs;

	inregs.h.ah = 0x01;
	inregs.h.dl = d | 0x80;
	int86 (0x13, &inregs, &outregs);

	if ( !outregs.h.cflag )
		return "No error";

	int err = outregs.h.ah;
	switch (err)
	{
                case 0x00: return "Successful completion";
                case 0x01: return "Invalid request or a bad command";
                case 0x02: return "Address mark not found";
                case 0x03: return "Disk write protected";
                case 0x04: return "Sector not found";
                case 0x05: return "Reset failed";
                case 0x06: return "Floppy disk removed";
                case 0x07: return "Drive parameter activity failed";
                case 0x08: return "Direct Memory Access (DMA) overrun";
                case 0x09: return "DMA crossed 64K boundary";
                case 0x0A: return "Bad sector flag detected";
                case 0x0B: return "Bad track flag detected";
                case 0x0C: return "Media type not found";
                case 0x0D: return "Invalid number of sectors on format";
                case 0x0E: return "Control data access mark detected";
                case 0x0F: return "DMA arbitration level out of range";
                case 0x10: return "Data read (CRC or ECC) error";
                case 0x11: return "Corrected data read (ECC) error";
                case 0x20: return "Controller failure";
                case 0x40: return "Seek error";
                case 0x80: return "Disk timed out or failed to respond";
                case 0xAA: return "Drive not ready";
                case 0xBB: return "Undefined error";
                case 0xCC: return "Write fault on drive";
                case 0xE0: return "Status error";
                case 0xFF: return "Sense operation failed";

                default:   return string("Unknown error: ") + itoa(err, 0, 10);
	}
}

int read (int drive, ullong start, void *buf)
{
	struct {
		uchar  size	PACKED;
		uchar  dummy	PACKED;
		uchar  num	PACKED;
		uchar  dummy2	PACKED;
		ushort roffs	PACKED;
		ushort rseg	PACKED;
		ullong start	PACKED;
	} packet;

	__dpmi_regs regs;

	regs.h.ah = 0x42;
	regs.h.dl = drive | 0x80;
	regs.x.ds = __tb >> 4;   // загоняем адрес disk address packet
	regs.x.si = __tb & 0x0F; // в DS:SI

	// заполняем disk address packet
	packet.size   = 0x10;
	packet.dummy  = 0;
	packet.num    = 1;
	packet.dummy2 = 0;

	packet.rseg  = __tb >> 4; // загоняем адрес буфера для чтения
	packet.roffs = (__tb & 0x0F) + 0x20;
	packet.start = start;

	dosmemput (&packet, sizeof(packet), __tb);

	__dpmi_int (0x13, &regs);

	if ( (regs.x.flags & 1) || (regs.h.ah) ) // carry flag
		return -1;	// произошла ошибка

	dosmemget (__tb + 0x20, 512, buf);
	return 0;
}

int read (int drive, ullong start, ulong len, void *buf)
{
	uchar *b = (uchar *)buf;

	for (uint i = 0; i < len; i++)
		if ( read(drive, start+i, &b[i*512]) )
			return -1;
               
	return 0;
}

static int read_partition_table (int drive, ullong sector, PartitionList &list)
{
	MBR mbr;
	if ( read (drive, sector, &mbr) )
	{
		cout << "Error occured: " + HDD::error(0) + string("\n");
		return -1;
	}

	if ( mbr.signature != 0xAA55 )
	{
		//cout << "drive " << drive << "; sector " << sector << "; \n";
		//cout << "Corrupted partition table!!!\n";
		return -1;
	}

	ullong all_len = 63; // начальное смещение первого раздела
	for (int i = 0; i < 4; i++)
	{
		switch ( mbr.part[i].type )
		{
			case 0x05: // Extended
			case 0x0F: // Extended LBA
				if ( read_partition_table (drive, sector + mbr.part[i].start, list) )
					if ( read_partition_table (drive, sector + all_len, list) )
						return -1;
				break;

			case 0x00:
				// просто игнорируем
				break;

			default:
				list.push_back ( Partition (sector, mbr.part[i]) );
				all_len += mbr.part[i].size;
		}
	}
	
	return 0;
}

PartitionList partition_list (int drive)
{
	PartitionList list (drive);
	read_partition_table (drive, 0, list);
	return list;
}

/***************************************************************************
 *
 * Class Partition
 *
 ***************************************************************************/

Partition::Partition (ullong part_start, PARTITION_TABLE table)
{
	type   = table.type;
	_start = part_start + table.start;
	size   = table.size;
}

bool Partition::supported ()
{
	if ( type == 0x07 )
		return true;

	return false;
}

string Partition::type_to_str ()
{
	switch ( type )
	{
		case 0x04: return "FAT16 (<32M)";
		case 0x05: return "Extended";
		case 0x06: return "FAT16";
		case 0x07: return "NTFS";
		case 0x0B: return "FAT32";
		case 0x0E: return "FAT16 (LBA)";
		case 0x0F: return "Extended (LBA)";
		case 0x0C: return "FAT32 (LBA)";
		case 0x82: return "Linux Swap";
		case 0x83: return "Linux Native";
	}

	return "(unknown)";
}

string Partition::str ()
{
	char buf[64];
	sprintf (buf, "│ %-15s │ %010d - %010d │ %5d MB │", type_to_str().c_str(), (int)start(), (int)size, size*512/(1024*1024));

	return string (buf);
}

void Partition::print ()
{
	cout << str() << "\n";
}

ullong Partition::start ()
{
	return _start;
}

/***************************************************************************
 *
 * Class PartitionList
 *
 ***************************************************************************/

PartitionList::PartitionList (int d)
{
	drive = d;
}

void PartitionList::print ()
{
	printf ("--- Partitions for drive %d ---\n", drive);

	iterator it;
	int count = 0;
	for (it = begin(); it != end(); it++, count++)
	{
		printf ("%d: ", count);
		(*it).print ();
	}
}

} // namespace HDD

