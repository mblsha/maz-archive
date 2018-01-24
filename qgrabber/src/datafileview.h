/***************************************************************************
                          datafileview.h  -  description
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

#ifndef DATAFILEVIEW_H
#define DATAFILEVIEW_H

#include "alleg.h"

#include <qlistview.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qaction.h>

#include <datafileitem.h>

class QWidget;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;

class DatafileItem;
class DatafileRoot;

class DatafileView : public QListView
{
	Q_OBJECT

private:
	DatafileRoot *root; // parent folder

	QPopupMenu *contextMenuNew, *contextMenu;
	QAction *grabAction, *exportAction, *deleteAction, *renameAction, *shellEditAction;

	QPoint mousePressPos;

	void displayContextMenu (DatafileItem *, const QPoint &);

	// handles loading and saving of datafile
	void loadDatafile (const char *name);
	void saveDatafile (const char *name);

	// reimplemented
	void keyPressEvent (QKeyEvent *);
#ifndef DRAGNDROP_TEST
	void viewportMousePressEvent (QMouseEvent *);
	void viewportMouseMoveEvent (QMouseEvent *);
#endif

	bool updateDataHelper (DATAFILE *dat, const char *filename, int x, int y, int w, int h);
	bool updateData (DATAFILE *dat);
	void updateHelper (DatafileItem *r, bool force);

protected:
	// reimplemented
#ifdef DRAGNDROP_TEST
	QDragObject *dragObject ();
#else
	void dragEnterEvent (QDragEnterEvent *);
	void dropEvent (QDropEvent *);
#endif

signals:
	void dirty (bool); // true if data is changed, false if data is saved
	void changeCaption (const QString &); // fileName to display into window caption
	void addRecentlyOpenedFile (const QString &);

public slots:
	void newFile ();
	void openFile (); // display File Open dialog and open file
	void openFile (const QString &); // open passed filename
	void saveFile ();
	void saveFileAs ();

	void update();
	void updateSelection();
	void forceUpdate();

private slots:
	void menuPluginDirty();
	void updateSelection (QListViewItem *);
	void singleClick (int, QListViewItem*, const QPoint &, int);
	void doubleClick (QListViewItem *item);
	void renamedItem (QListViewItem*, int, const QString &);

	void contextMenuPlugin (int id);
	void insertItem (DATAFILE *dat, DatafileItem *parent);
	void insertItem (DATAFILE *dat);

	// slots for context popup menu
	void contextGrab ();
	void contextExport ();
	void contextDelete ();
	void contextRename ();
	void contextShellEdit ();
	void contextNew (int type);

public:
	DatafileView ( QWidget *parent = 0, const char *name = 0, bool sdo = FALSE);
	~DatafileView ();

	QSize sizeHint () const;
};

#endif
