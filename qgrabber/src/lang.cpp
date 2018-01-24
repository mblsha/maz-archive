/***************************************************************************
                          lang.cpp  -  description
                             -------------------
    begin                : Пон Сен 30 16:45:16 MSD 2002
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

#include "lang.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qdir.h>
#include <qapplication.h>

#include "common.h"

QTranslator *trans;
QString curLang = "en";
QString curLangName = QT_TR_NOOP("language_name");

Lang::Lang ()
{
	trans = new QTranslator(0);
	qApp->installTranslator (trans);

	langs.set ("en", "English");

	QStringList dirs = langSearchPaths ();
	for (QStringList::Iterator it = dirs.begin(); it != dirs.end(); it++)
	{
		if ( !QFile::exists(*it) )
			continue;

		QDir d(*it);
		QStringList entries = d.entryList();
		for (QStringList::Iterator it2 = entries.begin(); it2 != entries.end(); it2++)
		{
			if( *it2 == "." || *it2 == ".." )
				continue;

			QString str = *it2;

			// verify that it is a language file
			const int prefixLen = 9;
			if ( str.left(prefixLen) != "qgrabber_" )
				continue;
			int n = str.find ('.', prefixLen);
			if ( n == -1 )
				continue;
			if ( str.mid(n) != ".qm" )
				continue;
			QString lang = str.mid (prefixLen, n - prefixLen);

			qDebug ("found [%s], lang=[%s]\n", str.latin1(), lang.latin1());

			// get the language_name
			QString name = QString("[") + str + "]";
			QTranslator t(0);
			if( !t.load(str, *it) )
				continue;

			if(t.contains("@default", "language_name", 0))
			{
				QString s = t.findMessage("@default", "language_name", 0).translation();
				if(!s.isEmpty())
					name = s;
			}

			langs.set (lang, name);
		}
	}
}

Lang::~Lang ()
{
}

QStringList Lang::searchPaths ()
{
	QStringList dirs;
	QString subdir = "";
	dirs += "." + subdir;
	dirs += getHomeDir()      + subdir;
	dirs += getResourcesDir() + subdir;

	return dirs;
}

void Lang::set (const QString &lang)
{
	trans->clear ();
	if ( lang == "en" )
	{
		curLang = lang;
		curLangName = "English";
		return;
	}

	QStringList dirs = langSearchPaths ();
	for (QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		if ( !QFile::exists(*it) )
			continue;
		if ( trans->load("qgrabber_" + lang, *it) )
		{
			curLang = lang;
			return;
		}
	}
}

