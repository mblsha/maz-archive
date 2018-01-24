/***************************************************************************
                          dat_data.h  -  description
                             -------------------
    begin                : Wed Apr 10 2002
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

#ifndef DAT_DATA_H
#define DAT_DATA_H

#include "datafileitem.h"
#include "plugins.h"

extern QPixmap *unknownData;

class ObjectData : public ObjectPlugin
{
private:
public:
	ObjectData ();
	~ObjectData ();

	bool    typeSupported (int type);
	int     type          ();
	QString menuDesc      ();
	QPixmap pixmap        ();
	void    open          (DatafileItem *parent, DATAFILE *data);
	void    *makeNew      (long *size);
	void    *copyData     (DATAFILE *dat, long *size);
	void    plot          (DATAFILE *dat, QWidget *parent);
};

class IOData : public IOPlugin
{
private:
public:
	IOData ();
	~IOData ();

	bool    typeSupported (int type);
	QString grabType      ();
	QString exportType    ();
	void    *grabFile     (const char *filename, long *size);
};

#endif
