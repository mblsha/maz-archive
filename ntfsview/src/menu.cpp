#include "debug.h"

#include "menu.h"
#include "hdd.h"
#include "screen.h"
#include "ntfs.h"

#include "attr.h"
#include "inode.h"
#include "util.h"

#include <stdio.h>
#include <conio.h>

namespace Menu {

const uchar status_col = Black | (LightGray << 4);
const uchar status_key_col = Red | (LightGray << 4);

char address[1024];

struct AttrInfo {
	ulong type;
	char *name;
} attr_info[] = {
	{ 0x10,  "$STANDARD_INFORMATION"	},
	{ 0x20,  "$ATTRIBUTE_LIST"		},
	{ 0x30,  "$FILE_NAME"			},
	{ 0x40,  "$OBJECT_ID"			},
	{ 0x50,  "$SECURITY_DESCRIPTOR"		},
	{ 0x60,  "$VOLUME_NAME"			},
	{ 0x80,  "$DATA"			},
	{ 0x90,  "$INDEX_ROOT"			},
	{ 0xA0,  "$INDEX_ALLOCATION"		},
	{ 0xB0,  "$BITMAP"			},
	{ 0xC0,  "$REPARSE_POINT"		},
	{ 0xD0,  "$EA_INFORMATION"		},
	{ 0xE0,  "$EA"				},
	{ 0xF0,  "$PROPERTY_SET"		},
	{ 0x100, "$LOGGED_UTILITY_STREAM"	},
	{ 0x000, "" }
};


void main ()
{
	Screen::clear ();

	bool cursor = false;
	int num = 0; // номер выбранного жесткого диска
	while (1)
	{
		// Рисуем экран
		Screen::print (1, 1, "Выберите Диск:", LightGray);
		
		for (int i = 0; i < HDD::num(); i++)
		{
			char buf[64];
			sprintf (buf, " Диск %X ", i | 0x80);
		
			uchar col = (i == num) ? Black | (LightGray << 4) :
						 LightGray;
						 
			Screen::print (1, i+3, buf, col);
		}

		// Строка состояния
		Screen::gotoxy (1, 25);
		Screen::print (" ",		status_col);
		Screen::print ("//ENTER ",	status_key_col);
		Screen::print ("Выбор диска  ",	status_col);
		Screen::print ("ESC ",		status_key_col);
		Screen::print ("Выход",		status_col);

		while ( Screen::pos_x > 1 )
			Screen::print (" ", status_col);

		// Выбор пользователя
		switch ( int key = getch() )
		{
			case 13:	// ENTER
				part_select ( num );
				Screen::clear ();
				break;
				
			case 27:	// ESC
				return;

			case 72:	// стрелка вверх
				if ( cursor )
				{
					num--;
					if ( num < 0 )
						num = 0;
				}
				break;

			case 80:	// стрелка вниз
				if ( cursor )
				{
					num++;
					if ( num >= HDD::num() )
						num = HDD::num() - 1;
				}
				break;

			case 0:		// если 0, то нажата клавиша курсора
				cursor = true;
				break;

			default:
				cursor = false;
		}
	}

	//cout << "В системе установлено " << HDD::num() << " НМД\n";
	//for (int i = 0; i < HDD::num(); i++)
	//	HDD::partition_list (i).print();
}

void part_select (int drive)
{
	Screen::clear ();
	
	if ( HDD::init (drive) )
	{
		cout << "Расширения INT 13 для выбранного диска не доступны. Обновите BIOS.\n";
		getch ();
		return;
	}

	bool cursor = false;
	int num = 0; // номер выбранного жесткого диска
	while (1)
	{
		// Рисуем экран
		Screen::print (1, 1, "Выберите Раздел:", LightGray);
		
		Screen::print (1, 2, "┌─────────────────┬─────────────────────────┬──────────┐", LightGray);
		Screen::print (1, 3, "│      Тип        │   Начало - Кол-во сект. │  Размер  │", LightGray);
		Screen::print (1, 4, "├─────────────────┼─────────────────────────┼──────────┤", LightGray);

		int count = 0;
		HDD::PartitionList l = HDD::partition_list (drive);
		HDD::PartitionList::iterator it;
		for (it = l.begin(); it != l.end(); it++, count++)
		{
			uchar col = (count == num) ? Black | (LightGray << 4) :
						     LightGray;
						 
			Screen::print (1, count+5, (*it).str().c_str(), col);
		}
		Screen::print (1, count+5, "└─────────────────┴─────────────────────────┴──────────┘", LightGray);

		// Строка состояния
		Screen::gotoxy (1, 25);
		Screen::print (" ",		status_col);
		Screen::print ("//ENTER ",	status_key_col);
		Screen::print ("Выбор раздела  ",	status_col);
		Screen::print ("ESC ",		status_key_col);
		Screen::print ("Выбор диска",	status_col);

		while ( Screen::pos_x > 1 )
			Screen::print (" ", status_col);

		// Выбор пользователя
		switch ( int key = getch() )
		{
			case 13:	// ENTER
				if ( l[num].supported() )
				{
					strcpy(address, "/");
					view_part ( drive, l[num].start(), 5 );
					Screen::clear ();
				}
				break;
				
			case 27:	// ESC
				return;

			case 72:	// стрелка вверх
				if ( cursor )
				{
					num--;
					if ( num < 0 )
						num = 0;
				}
				break;

			case 80:	// стрелка вниз
				if ( cursor )
				{
					num++;
					if ( num >= count )
						num = count - 1;
				}
				break;

			case 0:		// если 0, то нажата клавиша курсора
				cursor = true;
				break;

			default:
				cursor = false;
		}
	}
}

bool view_part (int drive, ullong start, ullong mft_num)
{
	Screen::clear ();
	
	NTFS ntfs (start, drive);
	Dir dir(mft_num, &ntfs);
	DirList list = dir.list();

	Screen::clear ();

	bool cursor = false;
	//int num = 0;
	int offset = 0;
	int pos = 0;
	while (1)
	{
		int num = offset + pos;
	
		Screen::gotoxy (1, 1);
		Screen::print ("Адрес: ", status_col);
		Screen::print (address, status_col);
		while ( Screen::pos_x > 1 )
			Screen::print (" ", status_col);
		DirList::iterator it;
		int count = 0;
		for (it = list.begin(); it != list.end(); it++) {
		
			if ( ((/*num*/ - offset + count) >= 0) && ((/*num*/ - offset + count) < 24) ) {
			
				char buf[256];
				sprintf (buf, "%3s %-60s (%6lld)", !(*it)->is_file() ? "DIR" : " ", (*it)->name(), (*it)->ref().num());
			
				uchar col = (count == num) ? Black | (LightGray << 4) : LightGray;
				Screen::print (1, /*num*/ - offset + count + 2, buf, col);
			}
			
			count++;
		}

		// Строка состояния
		Screen::gotoxy (1, 25);
		Screen::print (" ",		status_col);
		Screen::print ("//ENTER/BACKSPACE ",	status_key_col);
		Screen::print ("Переход ",	status_col);
		//Screen::print ("F3 ",		status_key_col);
		Screen::print ("SPACE ",		status_key_col);
		Screen::print ("Просмотр аттрибутов ",	status_col);
		Screen::print (" ESC ",		status_key_col);
		Screen::print ("Выбор раздела",	status_col);

		while ( Screen::pos_x > 1 )
			Screen::print (" ", status_col);

		// Выбор пользователя
		switch ( int key = getch() )
		{
			case 13:	// ENTER
				if ( !list[num]->is_file() )
				{
					char bak[1024];
					strcpy(bak, address);
					strcat(address, list[num]->name());
					strcat(address, "/");
					
					if ( view_part ( drive, start, list[num]->ref().num() ) )
						return true;

					strcpy(address, bak);
					Screen::clear ();
				}
				break;

			case 8:		// BACKSPACE
				return false; // на уровень выше
				
			case 27:	// ESC
				return true; // действительно выходим

			case ' '://61:	// F3
				//if ( cursor ) {
					view_inode(list[num]->ref().num(), &ntfs);
					Screen::clear();
				//}
				break;

			case 72:	// стрелка вверх
				if ( cursor ) {
					if ( pos > 0 )
						pos--;
					else
						if ( offset > 0 )
							offset--;
				}
				break;

			case 80:	// стрелка вниз
				if ( cursor ) {
					if ( (offset + pos < (int)list.size() - 1) && pos < 22 )
						pos++;
					else
						if ( offset + pos < (int)list.size() - 1 )
							offset++;
				}				
				break;

			case 0:		// если 0, то нажата клавиша курсора
				cursor = true;
				break;

			default:
				cursor = false;
		}
	}
}

char *attr_name(Attribute *attr)
{
	char *name = attr->name();
	if ( strlen(name) )
		return name;

	int i;
	for (i = 0; attr_info[i].type; i++)
		if ( attr_info[i].type == attr->type() )
			break;

	return attr_info[i].name;
}

void view_inode(ulong inum, NTFS *ntfs)
{
	INode inode(inum, ntfs);

	bool cursor = false;
	int num = 0;
	while (1)
	{
		// Рисуем экран
#define COL (LightGray | Blue << 4)
		Screen::print (13, 2, "┌─────────────────────────────────────────────────────┐", COL);
		Screen::print (13, 3, "│ Выберите аттрибут:                                  │", COL);
		Screen::print (13, 4, "├─────────────────────────────────────────────────────┤", COL);

		int count = 0;
		vector<ulong> l = inode.attrs();
		vector<ulong>::iterator it;
		for (it = l.begin(); it != l.end(); it++, count++)
		{
			uchar col = (count == num) ? Black | (LightGray << 4) : COL;

			char buf[80];		 // 44 - 35
			//sprintf(buf, "│ (0x%X) %-44s │", *it, attr_info[i].name);
			sprintf(buf, "│ (0x%X) %-30s %7d bytes │", *it, attr_name(inode.attr(*it)), inode.attr(*it)->size());
						 
			Screen::print (13, count+5, buf, col);
		}
		Screen::print (13, count+5, "└─────────────────────────────────────────────────────┘", COL);

/*		// Строка состояния
		Screen::gotoxy (1, 25);
		Screen::print (" ",		status_col);
		Screen::print ("//ENTER ",	status_key_col);
		Screen::print ("Выбор раздела  ",	status_col);
		Screen::print ("ESC ",		status_key_col);
		Screen::print ("Выбор диска",	status_col);

		while ( Screen::pos_x > 1 )
			Screen::print (" ", status_col);
*/
		// Выбор пользователя
		switch ( int key = getch() )
		{
			case 13:	// ENTER
				view_data ( &inode, l[num] );
				Screen::clear ();
				break;

			case 27:	// ESC
				return;

			case 72:	// стрелка вверх
				if ( cursor )
				{
					num--;
					if ( num < 0 )
						num = 0;
				}
				break;

			case 80:	// стрелка вниз
				if ( cursor )
				{
					num++;
					if ( num >= count )
						num = count - 1;
				}
				break;

			case 0:		// если 0, то нажата клавиша курсора
				cursor = true;
				break;

			default:
				cursor = false;
		}
	}
}

void view_data(INode *inode, ulong attr_type)
{
	Screen::clear ();

	Attribute *attr = inode->attr(attr_type);
	if ( !attr->size() )
		return;
	
	uchar *data = attr->data();
	if ( !data ) {
		debug("Menu::view_data: NO MEMORY!\n");
		return;
	}

	typedef enum ViewMode {
		ViewHEX,
		ViewASCII
	};

	ViewMode mode = ViewHEX;

	// Хекс - режим
	ullong hex_max_offs = attr->size() / 0x10;
	ullong hex_offs = 0;

	// ASCII - режим
	// TODO

/*	int attr_indx;
	for (attr_indx = 0; attr_info[attr_indx].type; attr_indx++)
		if ( attr_info[attr_indx].type == attr_type )
			break;*/
	
	bool cursor = false;
	
	char top_bar[80];
	
	while (1)
	{
		// Рисуем экран
		sprintf(top_bar, " View: %-30s %30lld bytes %3d%% ", /*attr_info[attr_indx].name*/attr_name(attr), attr->size(), (int)((float)hex_offs * 0x10 / attr->size() * 100));
		Screen::print (1, 1, top_bar, status_col);

		if ( mode == ViewHEX ) {
			int y = 2;
			ullong offset = hex_offs * 0x10;
			char buf[80];
			char buf2[16];

			while ( y < 25 ) {
				if ( offset < attr->size() ) {
					sprintf (buf, " %08llX  ", offset);
					//sprintf(buf, " %08lX  ", (ulong)offset);

					// заполняем хекс-коды
					int x;
					int cx = 0;
					for(x = 0; x < 16; x++) {
						if ( (offset + x) >= attr->size() )
							break;

						sprintf(buf2, "%02X ", data[offset + x]);
						strcat(buf, buf2);

						if ( !((x+1) % 4) ) {
							strcat(buf, " ");
							cx++;
						}
					}

					if ( cx < 4 )
						for (int i = 4; i > cx; i--)
							strcat(buf, " ");

					if ( x < 16 )
						for (int i = 16; i > x; i--)
							strcat(buf, "   ");

					// заполняем аски-символами
					int xx;
					for(xx = 0; xx < x; xx++) {
						uchar c = data[offset + xx];
						if ( c < ' ' )
							c = '.';
						sprintf(buf2, "%c", c);
						strcat(buf, buf2);
					}

					if ( x < 16 )
						for (int i = 16; i > x; i--)
							strcat(buf, " ");

			//		strcat(buf, " ");
				}
				else {
					for ( ; y < 25; y++) {
						strcpy (buf, "");
						int len = 80;
						for ( ; len > 0; len--)
							strcat(buf, " ");
						Screen::print(1, y, buf, LightGray);
					}
					break;
				}
				
				Screen::print(1, y, buf, LightGray);
				y++;
				offset = offset + 0x10;
			}
		}
		else if ( mode == ViewASCII ) {
			// TODO
		}
/*#define COL (LightGray | Blue << 4)
		Screen::print (13, 2, "┌─────────────────────────────────────────────────────┐", COL);
		Screen::print (13, 3, "│ Выберите аттрибут:                                  │", COL);
		Screen::print (13, 4, "├─────────────────────────────────────────────────────┤", COL);

		int count = 0;
		vector<ulong> l = inode.attrs();
		vector<ulong>::iterator it;
		for (it = l.begin(); it != l.end(); it++, count++)
		{
			uchar col = (count == num) ? Black | (LightGray << 4) : COL;

			int i;
			for (i = 0; attr_info[i].type; i++)
				if ( attr_info[i].type == *it )
					break;



			char buf[80];
			sprintf(buf, "│ (0x%X) %-44s │", *it, attr_info[i].name);
						 
			Screen::print (13, count+5, buf, col);
		}
		Screen::print (13, count+5, "└─────────────────────────────────────────────────────┘", COL);
*/
		Screen::gotoxy (1, 25);
		char buf[80];
		//sprintf(buf, "hex_offs = %d %25s", hex_offs, " ");
		//Screen::print (1, 25, buf, status_col);
/*		// Строка состояния
		Screen::gotoxy (1, 25);
		Screen::print (" ",		status_col);
		Screen::print ("//ENTER ",	status_key_col);
		Screen::print ("Выбор раздела  ",	status_col);
		Screen::print ("ESC ",		status_key_col);
		Screen::print ("Выбор диска",	status_col);

		while ( Screen::pos_x > 1 )
			Screen::print (" ", status_col);
*/
		// Выбор пользователя
		switch ( int key = getch() )
		{
/*			case 13:	// ENTER
				if ( l[num].supported() )
				{
					view_part ( drive, l[num].start(), 5 );
					Screen::clear ();
				}
				break;
*/
			case 27:	// ESC
				goto exit;

			case 72:	// стрелка вверх
				if ( cursor )
				{
					if ( mode == ViewHEX ) {
						if ( hex_offs > 0 )
							hex_offs--;
/*						hex_offs--;
						if ( hex_offs < 0 )
							hex_offs = 0;*/
					}
					else if ( mode == ViewASCII ) {}
				}
				break;

#define NUM	23
			case 80:	// стрелка вниз
				if ( cursor )
				{
					if ( mode == ViewHEX ) {
						//hex_offs++;
						if ( hex_offs < (hex_max_offs - NUM) )
							hex_offs++; // = hex_max_offs - 23 - 1;
					}
					else if ( mode == ViewASCII ) {}
				}
				break;

			case 73:	// PgUP
				if ( cursor ) {
					if ( mode == ViewHEX ) {
						if ( hex_offs >= NUM )
							hex_offs -= NUM;
						else
							hex_offs = 0;
					}
					else if ( mode == ViewASCII ) {}
				}
				break;
				
			case 81:	// PgDN
				if ( cursor ) {
					if ( mode == ViewHEX && hex_offs < (hex_max_offs - NUM)) {
						if ( hex_offs < (hex_max_offs - NUM) )
							hex_offs += NUM;
						else
							hex_offs = hex_max_offs - NUM;
					}
					else if ( mode == ViewASCII ) {}
				}
				break;
#undef NUM
			case 0:		// если 0, то нажата клавиша курсора
				cursor = true;
				break;

			default:
				cursor = false;
		}
	}

exit:
	delete[] data;
}

}; // namespace Menu
