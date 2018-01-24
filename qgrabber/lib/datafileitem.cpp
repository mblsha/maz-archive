/***************************************************************************
                          datafileitem.cpp  -  description
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

#include "alleg.h"

#include <qlistview.h>
#include <qpixmap.h>

#include <stdio.h>

#include <allegro/internal/aintern.h>
#include "datedit.h"

#include "datafileitem.h"
#include "plugins.h"
#include "factory.h"

#include "prefs.h"

/*****************************************************************************
 *
 * Class DatafileItem
 *
 *****************************************************************************/

DatafileItem::DatafileItem ( QListViewItem *parent, DATAFILE *data )
			: QListViewItem( parent ), pix( 0 )
{
	dat = data;
	root = false;

#ifdef DRAGNDROP_TEST
	setDragEnabled (true);
	setDropEnabled (true);
#endif
}

DatafileItem::DatafileItem ( QListView *parent, DATAFILE *data )
			: QListViewItem( parent ), pix( 0 )
{
	dat = data;
	root = true;

#ifdef DRAGNDROP_TEST
	setDragEnabled (true);
	setDropEnabled (true);
#endif
}

DatafileItem::~DatafileItem ()
{
	if ( !isRoot() )
		destroyData ();
}

void DatafileItem::destroyData ()
{
	if ( !dat )
		return;

#ifdef DEBUG
	qWarning ("Destroying DatafileItem: '%s' of type '%s'", name().latin1(), strType().latin1());
#endif
	_unload_datafile_object (dat);
	free (dat);
#ifdef DEBUG
	qWarning ("	OK");
#endif
}

void DatafileItem::setPixmap( QPixmap *p )
{
	pix = p;
	setup ();
	widthChanged ( 0 );
	invalidateHeight ();
	repaint ();
}

bool DatafileItem::isRoot ()
{
	return root;
}

bool DatafileItem::isDatafile ()
{
	return false;
}

DATAFILE *DatafileItem::data ()
{
	return dat;
}

void DatafileItem::setData (DATAFILE *tmp)
{
	dat = tmp;
}

const QPixmap *DatafileItem::pixmap( int i ) const
{
	if ( i )
		return 0;
	return pix;
}

const QPixmap DatafileItem::pixmap () const
{
	if ( pix )
		return *pix;
	else
		return QPixmap();
}

QString DatafileItem::text ( int column ) const
{
	if ( column == 0 )
		return name ();
	else
		return strType ();
}

int DatafileItem::type () const
{
	return dat->type;
}

QString DatafileItem::strType () const
{
	if ( dat )
	{
		char buf[16];
		sprintf (buf, "%c%c%c%c", dat->type >> 24, dat->type >> 16, dat->type >> 8, dat->type);
		
		QString str = buf;
		return str;
	}
	else
		return "";
}

QString DatafileItem::name () const
{
	if ( dat )
		return QString::fromUtf8( get_datafile_property (dat, DAT_NAME) );
	else
		return "";
}

void DatafileItem::setName (QString name)
{
	// nasty MSVC trick
	QCString utf8 = name.utf8();
	char *c = new char[ utf8.size() ];
	strcpy ( c, (const char *)utf8 );
	datedit_set_property (dat, DAT_NAME, (AL_CONST char *)c );
	delete[] c;
}

#ifdef DRAGNDROP_TEST
bool DatafileItem::acceptDrop (const QMimeSource *mime)
{
	return DatafileViewDrag::canDecode(mime);
}

void DatafileItem::dropped (QDropEvent *e)
{
	// searching for root
	DatafileItem *root = this;
	while ( !root->isRoot() )
		root = static_cast<DatafileItem *>( root->parent() );

	qDebug ("dropped...");

	bool takeItem = false;
	DatafileItem *item;
	if ( item = DatafileViewDrag::decode(e, takeItem) )
	{
		DatafileItem *dest;
		if ( isDatafile() )
		{
			qDebug ("dest = this");
			dest = this;
		}
		else
		{
			qDebug ("dest = parent");
			dest = static_cast<DatafileItem *>( parent() );
		}

		if ( takeItem )
		{
			DatafileItem *parent = static_cast<DatafileItem *>( item->parent() );

			if ( item->isDatafile() )
			{	// we need to checkup that the place we are moving item to
				// is not child of item
				DatafileItem *it = dest;
				while ( it != root )
				{
					if ( it == item )
						return;

					it = static_cast<DatafileItem *>( it->parent() );
				}
			}

			parent->takeItem (item);
		}

		dest->insertItem (item);

		//emit selectionChanged (0);
		//emit dirty (true);
	}
}
#endif

/*****************************************************************************
 *
 * Class DatafileItemDrag
 *
 *****************************************************************************/

DatafileItem *DatafileItemDrag::item = 0;

DatafileItemDrag::DatafileItemDrag (DatafileItem *it, QWidget *dragSrc, const char *name)
			: QStoredDrag ("data/datafile-item", dragSrc, name)
{
	setItem (it);
}

DatafileItemDrag::DatafileItemDrag (QWidget *dragSrc, const char *name)
			: QStoredDrag ("data/datafile-item", dragSrc, name)
{
}

DatafileItemDrag::~DatafileItemDrag ()
{
	item = 0;
}

void DatafileItemDrag::setItem (DatafileItem *it)
{
	setPixmap ( it->pixmap() );
	//QByteArray a;

	item = it;

	//setEncodedData (a);
}

bool DatafileItemDrag::canDecode (const QMimeSource *e)
{
	// Disables dragging from other windows
	return e->provides ("data/datafile-item") && item;
}

DatafileItem *DatafileItemDrag::decode (const QMimeSource *e,  bool &takeItem)
{
	//QByteArray a = e->encodedData ("data/datafile-item");
	//if ( a.size() )
	if ( e->provides("data/datafile-item") && item )
	{
		takeItem = true; // we're moving item between one QGrabber window
		return item;
	}

	return 0;
}

/*****************************************************************************
 *
 * Class DatafileViewDrag
 *
 *****************************************************************************/

bool DatafileViewDrag::canDecode (const QMimeSource *e)
{
	return DatafileItemDrag::canDecode (e);
}

DatafileItem *DatafileViewDrag::decode (const QMimeSource *e, bool &takeItem)
{
	return DatafileItemDrag::decode (e, takeItem);
}


