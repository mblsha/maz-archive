/***************************************************************************
                          readbitmap.h  -  description
                             -------------------
    begin                : Sat Sep 7 2002
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

#ifndef READBITMAP_H
#define READBITMAP_H

#include <qaction.h>

#include "plugins.h"

/*****************************************************************************
 *
 * Class MenuReadBitmap
 *
 *****************************************************************************/

class MenuReadBitmap : public MenuPlugin
{
	Q_OBJECT

private:
	QAction *action;

private slots:
	void activated();

public:
	MenuReadBitmap ();
	~MenuReadBitmap ();

	QAction *menuAction ();
	int     menuAddTo ();
};

/*****************************************************************************
 *
 * Class MenuViewBitmap
 *
 *****************************************************************************/

class MenuViewBitmap : public MenuPlugin
{
	Q_OBJECT

private:
	QAction *action;

private slots:
	void activated();

public:
	MenuViewBitmap ();
	~MenuViewBitmap ();

	QAction *menuAction ();
	int     menuAddTo ();
	
	friend class MenuReadBitmap;
};

#endif
