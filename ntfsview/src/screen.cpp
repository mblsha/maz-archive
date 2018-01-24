#include "debug.h"

#include "screen.h"
#include "common.h"

#include <conio.h>

#include <iostream>
using namespace std;

/***************************************************************************
 *
 * namespace Screen
 *
 ***************************************************************************/

namespace Screen {

void clear ()
{
	clrscr ();
}

void about ()
{
	if ( findarg( "--noabout" ) )
		return;

	char logo[] =	"NTFSview (c) 2002 by Michail Pishchagin\n\n"
			"Браузер NTFS дисков\n\n"
			"Нажмите любую клавишу для продолжения";
			
	cout << logo;
	getch ();
}

static char buf[256];
static void fill_buf (const char *str, uchar attr, bool till_end = false)
{
	char *s = (char *)str;
	char *b = buf;
	while ( *s )
	{
		*b++ = *s++;
		*b++ = attr;
	}
	
	if ( till_end )
		while ( (b - buf) < 80*120 )
		{
			*b++ = ' ';
			*b++ = attr;
		}
}

void print (int x, int y, const char *str, uchar attr)
{
	//debug("print: strlen(str) = %d\n", strlen(str));

	fill_buf (str, attr);
	puttext (x, y, x + strlen(str) - 1, y, buf);
}

void printline (int y, const char *str, uchar attr)
{
	fill_buf (str, attr, true);
	puttext (1, y, 80, y, buf);
}

void printline_centre (int y, const char *str, uchar attr)
{
	fill_buf (str, attr, true);
	puttext (1, y, 80, y, buf);
}

int pos_x = 1;
int pos_y = 1;

void gotoxy (int x, int y)
{
	pos_x = x;
	pos_y = y;
}

void print (const char *str, uchar attr)
{
	fill_buf (str, attr);
	puttext (pos_x, pos_y, pos_x + strlen(str), pos_y, buf);
	
	pos_x += strlen (str);
	if (pos_x > 80)
	{
		pos_x -= 80;	
		pos_y = pos_y == 25 ? 25 : pos_y++;
	}
}

}; // namespace Screen

