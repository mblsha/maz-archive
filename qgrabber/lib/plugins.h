/***************************************************************************
                          plugins.h  -  description
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

#ifndef PLUGINS_H
#define PLUGINS_H

#include "alleg.h"
#include "export.h"

#include <qobject.h>
#include <qstring.h>

#include <stdio.h>

#include "datedit.h"

#include "datafileitem.h"

class QAction;
class QPopupMenu;

class EXPORT ObjectPlugin : public QObject
{
	Q_OBJECT

private:
	bool shouldSaveProp (long prop);

public:
	ObjectPlugin ();
	~ObjectPlugin ();

	virtual int     type          () = 0;
	virtual bool    typeSupported (int type) = 0;

	virtual QString menuDesc      () = 0;
	virtual QPixmap pixmap        ();

	virtual void    open          (DatafileItem *parent, DATAFILE *data) = 0;
	virtual void    *makeNew      (long *size) = 0;
	virtual void    *copyData     (DATAFILE *dat, long *size) = 0;
	virtual void    saveObject    (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f);
	virtual void    plot          (DATAFILE *dat, QWidget *parent) = 0;
	virtual bool    doubleClick   (DATAFILE *dat);

	// no need to reimplement these functions
	void save (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f);
	static long fileDatasize;

signals:
	void dirty (); // must be called, when data is changed via GUI. See Font plugin if you want example.
	void savingItem (QString);
	void savedItem (long, long);
};

class EXPORT IOPlugin : public QObject
{
	Q_OBJECT

private:
	DATAFILE *grabHelper (const char *filename, const char *name, int type);

public:
	IOPlugin ();
	~IOPlugin ();

	virtual bool    typeSupported (int type);
	virtual QString grabType      ();
	virtual QString exportType    ();
	virtual void    *grabFile     (const char *file, long *size);
	virtual int     exportFile    (const char *file, DatafileItem *item);

	bool            grab  (const char *filename, DatafileItem *item);
	static DATAFILE *grab (const char *filename, const char *name, int type, int x, int y, int w, int h, int colorDepth);
};

class EXPORT MenuPlugin : public QObject
{
	Q_OBJECT

public:
	MenuPlugin ();
	~MenuPlugin ();

	typedef enum MenuDest {
		Plugins = 0,
		File,
		Options,
		Help
	};

	// These plugins will be placed into `Plugins` menu on the main menubar
	virtual QAction *menuAction ();
	virtual int     menuAddTo ();

	// These plugins will be placed into context menu
	virtual QString    contextMenuText ();
	virtual bool       contextMenuTypeSupported (int type);
	virtual void       contextMenuActivated (DatafileItem *item);
	virtual QPopupMenu *contextMenuPopup (DatafileItem *item);

signals:
	void insertItem(DATAFILE *dat, DatafileItem *parent);
	void insertItem(DATAFILE *dat);
	void dirty ();
};


/*****************************************************************************
 *
 * Helper functions
 *
 *****************************************************************************/

EXPORT DATAFILE *copyDatafile (DATAFILE *dat);

class QImage;
class QColor;

EXPORT void initPlugins ();
EXPORT void selPalette (RGB *pal);

EXPORT QImage *bmp2img (BITMAP *bmp, QColor *trans = 0);
EXPORT QImage *rle2img (RLE_SPRITE *spr);

#endif
