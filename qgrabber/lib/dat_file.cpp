/***************************************************************************
                          dat_file.cpp  -  description
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

#include "alleg.h"

#include <qpixmap.h>
#include <qlistview.h>
#include <qmessagebox.h>

#include "prefs.h"

#include "factory.h"

#include "common.h"
#include "datafileitem.h"
#include "dat_file.h"

QPixmap *folderClosed = 0;
QPixmap *folderOpen = 0;

/*****************************************************************************
 *
 * Class Datafile
 *
 *****************************************************************************/

Datafile::Datafile ( DatafileItem *parent, DATAFILE *data )
		: DatafileItem (parent, data)
{
	setPixmap( folderClosed );
}


Datafile::Datafile ( QListView * parent, DATAFILE *data )
		: DatafileItem (parent,  data)
{
	setPixmap( folderClosed );
}


void Datafile::setOpen ( bool o )
{
	if ( o )
		setPixmap( folderOpen );
	else
		setPixmap( folderClosed );

	if ( o && !childCount() && data() )
	{
		listView()->setUpdatesEnabled (false);

		DATAFILE *dat, *d = !isRoot() ? (DATAFILE *)data()->dat : data();
		for (int i = 0; d[i].type != DAT_END; i++)
		{
			dat = d + i;

			if ( !strcmp ("GrabberInfo", get_datafile_property(dat, DAT_NAME)) &&
			     (dat->type == DAT_INFO) )
			{
				continue; // who needs that GrabberInfo? :-)
			}

			ObjectPlugin *plug = ObjectFactory::create (dat->type);

			if (plug)
				plug->open (this, dat->type == DAT_FILE ? dat : copyDatafile(dat));
		}

		listView()->setUpdatesEnabled (true);

		// Create new datafile
		if ( !isRoot() )
			setData ( copyDatafile ( data() ) );
	}

	QListViewItem::setOpen ( o );
}

bool Datafile::isDatafile ()
{
	return true;
}

void Datafile::setup ()
{
	setExpandable ( TRUE );
	QListViewItem::setup ();
}

/*****************************************************************************
 *
 * Class ObjectFile
 *
 *****************************************************************************/

ObjectFile::ObjectFile ()
{
	folderClosed = new QPixmap (getResourcesDir()+"/icons/object/folder.png");
	folderOpen = new QPixmap (getResourcesDir()+"/icons/object/folder_open.png");
}

ObjectFile::~ObjectFile ()
{
	delete folderClosed;
	delete folderOpen;
}

bool ObjectFile::typeSupported (int type)
{
	return (type == DAT_FILE);
}

int ObjectFile::type ()
{
	return DAT_FILE;
}

QString ObjectFile::menuDesc ()
{
	return tr("Datafile");
}

QPixmap ObjectFile::pixmap ()
{
	return *folderClosed;
}

void ObjectFile::open (DatafileItem *parent, DATAFILE *dat)
{
	Datafile *item = new Datafile (parent, dat);

	item->setOpen (true);
	if ( !Prefs::prefs()->openNested() && item->childCount() )
		item->setOpen (false);
}

void *ObjectFile::makeNew (long *size)
{
	DATAFILE *dat = (DATAFILE *)malloc(sizeof(DATAFILE));

	dat->dat = NULL;
	dat->type = DAT_END;
	dat->size = 0;
	dat->prop = NULL;

	return dat;
}

void *ObjectFile::copyData (DATAFILE *dat, long *size)
{
	// we don't need to copy contents of datafile, because DatafileItems themselves must take care of their data
	return makeNew (size);
}

void ObjectFile::saveObject (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f)
{
	pack_mputl (item->childCount(), f);

	DatafileItem *it = static_cast<DatafileItem *>(item->firstChild ());
	ObjectPlugin *plug;

	while ( it )
	{
		plug = ObjectFactory::create( it->type() );
		plug->save (it, (Prefs::prefs()->pack() >= 2), (Prefs::prefs()->pack() >= 1), Prefs::prefs()->strip(), verbose, (Prefs::prefs()->strip() <= 0), f);

		it = static_cast<DatafileItem *>(it->nextSibling ());
	}
}

void ObjectFile::plot (DATAFILE *dat, QWidget *parent)
{
	//label->setText ("Datafile");
}

/*****************************************************************************
 *
 * Class IOFile
 *
 *****************************************************************************/

IOFile::IOFile ()
{
}

IOFile::~IOFile ()
{
}


bool IOFile::typeSupported (int type)
{
	return (type == DAT_FILE);
}

QString IOFile::grabType ()
{
	return tr("Datafiles") + " (*.dat)";
}

QString IOFile::exportType ()
{
	return tr("Datafiles") + " (*.dat)";
}

void *IOFile::grabFile (const char *file, long *size)
{
	return datedit_load_datafile ((AL_CONST char *)file, TRUE, NULL);
}

bool IOFile::grab (const char *filename, DatafileItem *item)
{
	bool ret = IOPlugin::grab (filename, item);

	if ( ret )
	{
		item->setOpen (FALSE);

		QListViewItem *it, *next;
		it  = item->firstChild ();

		while ( it )
		{
			next = it->nextSibling ();
			delete it;
			it = next;
		}
		//item->clear ();
	}

	return ret;
}

int IOFile::exportFile (const char *file, DatafileItem *item)
{
	PACKFILE *f = pack_fopen ((AL_CONST char *)file, (Prefs::prefs()->pack() >= 2) ? F_WRITE_PACKED : F_WRITE_NOPACK);

	if (f)
	{
		pack_mputl (DAT_MAGIC, f);

		pack_mputl (item->childCount(), f);

		DatafileItem *it = static_cast<DatafileItem *>(item->firstChild ());
		ObjectPlugin *plug;

		while ( it )
		{
			plug = ObjectFactory::create( it->type() );
			plug->save (it, (Prefs::prefs()->pack() >= 2), (Prefs::prefs()->pack() >= 1), Prefs::prefs()->strip(), 0, 0, f);

			it = static_cast<DatafileItem *>(it->nextSibling ());
		}

		pack_fclose (f);
		return TRUE;
	}

	return FALSE;
}

