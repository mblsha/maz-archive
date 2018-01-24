/***************************************************************************
                          datafileitem.h  -  description
                             -------------------
    begin                : Mon Apr 8 2002
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

#ifndef DATAFILEITEM_H
#define DATAFILEITEM_H

#include "alleg.h"
#include "export.h"

#include <qlistview.h>
#include <qdragobject.h>

// Uncomment it to test new drag and drop code.
// #define DRAGNDROP_TEST

class EXPORT DatafileItem : public QListViewItem
{
private:
	QPixmap *pix;
	DATAFILE *dat;
	bool root;

public:
	DatafileItem ( QListViewItem *parent, DATAFILE *data );
	DatafileItem ( QListView *parent, DATAFILE *data );
	~DatafileItem ();

	DATAFILE *data ();
	void     setData (DATAFILE *dat);

	void destroyData ();

	bool isRoot ();
	virtual bool isDatafile ();

	QString strType () const; // QString equivalent of type(). Ex: type() == DAT_FILE --> strType() == "FILE"
	int     type () const;
	QString name () const;
	virtual void setName (QString name);

	QString text ( int column ) const;
	const QPixmap *pixmap ( int i ) const;
	const QPixmap pixmap () const;
	void setPixmap ( QPixmap *p );

#ifdef DRAGNDROP_TEST
	bool acceptDrop ( const QMimeSource * );
	void dropped ( QDropEvent *e );
#endif
};


class EXPORT Datafile : public DatafileItem
{
private:

public:
	Datafile ( QListView *parent, DATAFILE *data );
	Datafile ( DatafileItem *parent, DATAFILE *data );

	bool isDatafile ();

	void setOpen ( bool );
	void setup ();
};

class EXPORT DatafileItemDrag : public QStoredDrag
{
private:
	static DatafileItem *item;

public:
	DatafileItemDrag (DatafileItem *item, QWidget *dragSource, const char *name = 0);
	DatafileItemDrag (QWidget *dragSource, const char *name = 0);
	~DatafileItemDrag ();

	void setItem (DatafileItem *item);

	static bool canDecode (const QMimeSource *e);
	static DatafileItem *decode (const QMimeSource *e, bool &takeItem);
};

class EXPORT DatafileViewDrag
{
public:
	static bool canDecode (const QMimeSource *e);
	static DatafileItem *decode (const QMimeSource *e, bool &takeItem);
};

#endif
