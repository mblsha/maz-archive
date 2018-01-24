/***************************************************************************
                          common.cpp  -  description
                             -------------------
    begin                : þÔ× á×Ç 22 2002
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
#include "common.h"

#include <qstring.h>
#include <qdir.h>

#ifdef ALLEGRO_UNIX
#	include <sys/types.h>
#	include <sys/stat.h>
#endif

#ifdef ALLEGRO_WINDOWS
#	include <windows.h>
#endif

#include "prefs.h"

QString getResourcesDir()
{
	if ( !Prefs::prefs()->resourcesDir().isEmpty() )
		return Prefs::prefs()->resourcesDir ();

#ifdef ALLEGRO_UNIX
	return "/usr/local/qgrabber";
#endif
#ifdef ALLEGRO_WINDOWS
	return ".";
#endif
}

QString getHomeDir()
{
#ifdef ALLEGRO_UNIX
	QDir proghome(QDir::homeDirPath() + "/.qgrabber");
	if (!proghome.exists()) 
	{
		QDir home = QDir::home();
		home.mkdir(".qgrabber");
		chmod(proghome.path().latin1(), 0700);
	}

	return proghome.path();
#endif
#ifdef ALLEGRO_WINDOWS
	QString base;

	if(QDir::homeDirPath() == QDir::rootDirPath())
		base = ".";			// Windows 9x
	else
		base = QDir::homeDirPath();	// Windows NT/2K/XP variant

	// no trailing slash
	if(base.at(base.length()-1) == '/')
		base.truncate(base.length()-1);

	QDir proghome(base + "/QGrabberData");
	if(!proghome.exists()) {
		QDir home(base);
		home.mkdir("QGrabberData");
	}

	return proghome.path();
#endif
}

