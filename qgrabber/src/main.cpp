/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : ðÎÄ áÐÒ  8 01:10:08 MSD 2002
    copyright            : (C) 2002 by Michail Pishchagin
    email                : mblsha@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "alleg.h"

#include <qapplication.h>
#include <qfont.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include "qgrabber.h"

#ifdef ALLEGRO_WINDOWS

#undef RaiseException

#define MSVC_CONV
#ifdef MSVC_CONV
#	define CONV (const unsigned short *)
#else
#	define CONV
#endif

static void al_win_preinit(void)
{
    static char appname[] = "QGrabber Auxilary";
    HINSTANCE inst;
    WNDCLASS klass;
    HWND wnd;

    inst = GetModuleHandle(NULL);

    klass.style         = 0;
    klass.lpfnWndProc   = DefWindowProc;
    klass.cbClsExtra    = 0;
    klass.cbWndExtra    = 0;
    klass.hInstance     = inst;
    klass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    klass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    klass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    klass.lpszMenuName  = 0;
    klass.lpszClassName = CONV appname;
    RegisterClass(&klass);

    wnd = CreateWindow (CONV appname, CONV appname, 0,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
		NULL, NULL, inst, NULL);

    // Register our window BEFORE installing Allegro.
    win_set_window (wnd);
}
#endif


int main(int argc, char *argv[])
{
	QApplication app (argc, argv);

	QTranslator translator ( 0 );
	translator.load ( QString("qgrabber.") + QTextCodec::locale(), "." );
	app.installTranslator ( &translator );

#ifdef ALLEGRO_WINDOWS
	al_win_preinit ();
#endif

	QGrabberApp *qgrabber = new QGrabberApp (argc, argv);
	app.setMainWidget (qgrabber);

	qgrabber->show();

	return app.exec();
}
END_OF_MAIN()

