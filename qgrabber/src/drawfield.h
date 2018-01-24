/***************************************************************************
                          drawfield.h  -  description
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

#ifndef DRAWFIELD_H
#define DRAWFIELD_H

#include <qwidget.h>
#include <qlayout.h>

#include "datafileitem.h"

class DrawField : public QWidget
{
	Q_OBJECT
	
private:
	QVBoxLayout *drawBox;
	QWidget     *drawChild;
		
	DatafileItem *item;
	
public slots:
	void displayItem (QListViewItem *item);
	void clear ();
	
public:
	DrawField (QWidget *parent = 0, const char *name = 0);
	~DrawField ();
};

#endif
