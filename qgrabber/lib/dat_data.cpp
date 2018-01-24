/***************************************************************************
                          dat_data.cpp  -  description
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

#include <qpixmap.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qlabel.h>

#include <qmessagebox.h>

#include "dat_data.h"
#include <allegro/internal/aintern.h>

QPixmap *unknownData = 0;

#include "common.h"

/*****************************************************************************
 *
 * Class DatafileItemData
 *
 *****************************************************************************/

class DatafileItemData : public DatafileItem
{
public:
	DatafileItemData (QListViewItem *parent, DATAFILE *data)
				: DatafileItem (parent, data)
	{
		setPixmap ( unknownData );
	}

	~DatafileItemData () {}
};

/*****************************************************************************
 *
 * Class ObjectData
 *
 *****************************************************************************/

ObjectData::ObjectData ()
{
	unknownData = new QPixmap (getResourcesDir()+"/icons/object/unknown.png");
}

ObjectData::~ObjectData ()
{
	delete unknownData;
}

bool ObjectData::typeSupported (int type)
{
	return true;
}

int ObjectData::type ()
{
	return 0;
}

QString ObjectData::menuDesc ()
{
	return tr("Custom Data");
}

QPixmap ObjectData::pixmap ()
{
	return *unknownData;
}

void ObjectData::open (DatafileItem *parent, DATAFILE *data)
{
	DatafileItemData *item;
	item = new DatafileItemData (parent, data);
}

void *ObjectData::makeNew (long *size)
{
	char str[] = "Binary Data";

	char *data;
	data = (char *)malloc (strlen(str));
	strcpy (data, str);
	*size = 12;
	return data;
}

void *ObjectData::copyData (DATAFILE *dat, long *size)
{
	void *data;
	data = malloc (dat->size);
	memcpy (data, dat->dat, dat->size);
	*size = dat->size;
	return data;
}

void ObjectData::plot (DATAFILE *dat, QWidget *parent)
{
	QVBoxLayout *box = new QVBoxLayout (parent);
	box->setSpacing (5);

	QLabel *label = new QLabel (parent);
	box->addWidget (label);

	QString l = tr("Unknown Data (%1 bytes)").arg(dat->size);
	label->setText ( l );

	QMultiLineEdit *lineEdit = new QMultiLineEdit (parent);
	box->addWidget (lineEdit);

	QString text = QString((const char *)dat->dat).left(dat->size);
	lineEdit->setText (text);
	lineEdit->setReadOnly (true);

	parent->show ();
}

/*****************************************************************************
 *
 * Class IOData
 *
 *****************************************************************************/

IOData::IOData ()
{
}

IOData::~IOData ()
{
}

bool IOData::typeSupported (int type)
{
	return true;
}

QString IOData::grabType ()
{
	return tr("All files") + " (*)";
}

QString IOData::exportType ()
{
	return tr("All files") + " (*)";
}

void *IOData::grabFile (const char *filename, long *size)
{
	void *mem;
	long sz = file_size ( (char *)filename);
	
	if (sz <= 0)
		return NULL;
	
	PACKFILE *f;
	mem = malloc (sz);

	f = pack_fopen( (char *)filename, F_READ);
	if (!f)
	{
		free (mem);
		return NULL;
	}

	pack_fread (mem, sz, f);
	pack_fclose (f);

	if (errno)
	{
		free (mem);
		return NULL;
	}

	*size = sz;
	return mem;	
}

