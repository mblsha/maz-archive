/***************************************************************************
                          propview.h  -  description
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

#ifndef PROPVIEW_H
#define PROPVIEW_H

#include <qlistview.h>

class QPopupMenu;
class PropViewItem;
class DatafileItem;

class PropView : public QListView
{
	Q_OBJECT

private:
	DatafileItem *item;
	PropViewItem *propItem;

	QPopupMenu *menu;

signals:
	void updated (QListViewItem *item);
	void dirty ();

private slots:
	void handleMousePress ( int button, QListViewItem *_item, const QPoint &pos, int c );
	void updateSelection (QListViewItem *item);

	void doubleClick (QListViewItem *item);

	void popupAdd ();
	void popupDelete ();
	void popupModify ();

public slots:
	void displayItem (QListViewItem *item);

public:
	PropView (QWidget *parent = 0, const char *name = 0);
	~PropView ();
	
	QSize sizeHint () const;
};

#endif
