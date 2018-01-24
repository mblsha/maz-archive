#ifndef SCREEN_H
#define SCREEN_H

#include "common.h"

enum Colors {
    //  dark colors
    Black,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Brown,
    LightGray,
    //  light colors
    DarkGray, // "light black"
    LightBlue,
    LightGreen,
    LightCyan,
    LightRed,
    LightMagenta,
    Yellow,
    White
};

const uchar Blink = 0x80;	//  blink bit
/*
class Symbol
{
public:
	Symbol ();
	Symbol (uchar s, uchar a);
	
	uchar symb;
	uchar attr;
};

#include <string>
using namespace std;

class String : basic_string<Symbol>
{
public:
	String (const char *str, uchar attr);
	const char *str ();
};
*/
namespace Screen
{
	void clear ();
	void about ();

	void print		(int x, int y, const char *str, uchar attr);
	void printline		(int y, const char *str, uchar attr);
	void printline_centre	(int y, const char *str, uchar attr);

	extern int pos_x;
	extern int pos_y;
	void gotoxy (int x, int y);
	void print (const char *str, uchar attr);
};

#endif

