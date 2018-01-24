#include "hdd.h"
#include "screen.h"
#include "common.h"
#include "menu.h"

#include <iostream>
using namespace std;

#include <unistd.h>

int main (int argc, char *argv[])
{
	init (argc, argv);
	Screen::about ();

	unlink("ntfsview.log");

	Menu::main ();
	return 0;
}

