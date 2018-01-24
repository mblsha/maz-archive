/***************************************************************************
                          plugins_intern.h  -  description
                             -------------------
    begin                : óÒÄ á×Ç 21 2002
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

#ifndef PLUGINS_P_H
#define PLUGINS_P_H

#include "export.h"

#include <qgplugin.h>
#include <qstringlist.h>

class ObjectPlugin;
class IOPlugin;
class MenuPlugin;

class QGrabberPluginPrivate;

class EXPORT QGrabberPlugin : public QGPlugin
{
	Q_OBJECT

public:
	QGrabberPlugin();
	~QGrabberPlugin();

	virtual QStringList keys() const = 0;

	virtual ObjectPlugin *createObject ( const QString& plug ) = 0;
	virtual IOPlugin     *createIO     ( const QString& plug ) = 0;
	virtual MenuPlugin   *createMenu   ( const QString& plug ) = 0;

	virtual QStringList keysObject () const = 0;
	virtual QStringList keysIO     () const = 0;
	virtual QStringList keysMenu   () const = 0;

private:
	QGrabberPluginPrivate *d;
};

#ifdef Q_WS_WIN
#	define EXPORT_PLUGIN(x)	Q_EXPORT_PLUGIN(x)					\
	void datedit_msg(AL_CONST char *fmt, ...)      {}				\
	void datedit_startmsg(AL_CONST char *fmt, ...) {}				\
	void datedit_endmsg(AL_CONST char *fmt, ...)   {}				\
	void datedit_error(AL_CONST char *fmt, ...)    {}				\
	int datedit_ask(AL_CONST char *fmt, ...)       { return 'y'; }
#else
#	define EXPORT_PLUGIN(x)	Q_EXPORT_PLUGIN(x)
#endif

#endif
