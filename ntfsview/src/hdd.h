#ifndef HDD_H
#define HDD_H

#include "common.h"

#include <iostream>
#include <vector>
#include <string>
using namespace std;

namespace HDD
{
	int init (int drive);	// 0 -- все нормально
        
	int num ();		// Возвращает число установленных
				// жестких дисков. -1 -- ошибка

	string error (int drive = 0);	// Строка ошибки

	int read (int drive, ullong start, void *buf);
	int read (int drive, ullong start, ulong len, void *buf);

	typedef struct PARTITION_TABLE
	{
		uchar   flag		PACKED;
		uchar   beg_head	PACKED; // useless
		ushort  beg_sec_cyl	PACKED; // useless
		uchar   type		PACKED;
		uchar   end_head	PACKED; // useless
		ushort  end_sec_cyl	PACKED; // useless
		ulong   start		PACKED;
		ulong   size		PACKED;
	};

	typedef struct MBR
	{
		uchar           boot_prog[0x1BE]	PACKED;
		PARTITION_TABLE part[4]			PACKED;
		ushort          signature		PACKED; // must equal `55AA`
	};

	class Partition
	{
	private:
		uchar  type;
		ullong _start, size;
	public:
		Partition (ullong part_start, PARTITION_TABLE table);

		bool   supported ();
		string type_to_str ();
		void   print ();
		string str ();
		ullong start ();
	};

	class PartitionList : public vector<Partition>
	{
	private:
		int drive;
	public:
		PartitionList (int d);
		void print ();
	};
        
	PartitionList partition_list (int drive);
};

#endif

