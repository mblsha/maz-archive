/***************************************************************************
                          factory.h  -  description
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

#ifndef FACTORY_H
#define FACTORY_H

#include "export.h"

class QString;
class QStringList;

#include "plugins.h"

class EXPORT ObjectFactory
{
public:
	static QStringList  keys();
	static ObjectPlugin *create( const QString &key );
	static ObjectPlugin *create( const int type );
};

class EXPORT IOFactory
{
public:
	static QStringList keys();
	static IOPlugin *create( const QString &key );
	static IOPlugin *create( const int type );
};

class EXPORT MenuFactory
{
public:
	static QStringList keys();
	static QStringList menuKeys();
	static QStringList menuKeys( int dest );
	static QStringList contextKeys();
	static QStringList contextKeys( const int type );
	static MenuPlugin *create( const QString &key );
};

#endif

