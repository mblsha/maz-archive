/***************************************************************************
                          dat_file.h  -  description
                             -------------------
    begin                : Thu Apr 11 2002
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

#ifndef DAT_FILE_H
#define DAT_FILE_H

#include "plugins.h"

class ObjectFile : public ObjectPlugin
{
public:
	ObjectFile ();
	~ObjectFile ();

	bool    typeSupported (int type);
	int     type          ();
	QString menuDesc      ();
	QPixmap pixmap        ();
	void    open          (DatafileItem *parent, DATAFILE *data);
	void    *makeNew      (long *size);
	void    *copyData     (DATAFILE *dat, long *size);
	void    saveObject    (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f);
	void    plot          (DATAFILE *dat, QWidget *parent);
};

class IOFile : public IOPlugin
{
private:
public:
	IOFile ();
	~IOFile ();

	bool    typeSupported (int type);
	QString grabType      ();
	QString exportType    ();
	void    *grabFile     (const char *file, long *size);
	int     exportFile    (const char *file, DatafileItem *item);

	bool grab (const char *filename, DatafileItem *item);
};

#endif
