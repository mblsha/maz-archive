/***************************************************************************
                          grid.h  -  description
                             -------------------
    begin                : Mon Sep 2 2002
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

#ifndef GRID_H
#define GRID_H

#include <qaction.h>

#include "plugins.h"

/*****************************************************************************
 *
 * Class MenuGrid
 *
 *****************************************************************************/

class MenuGrid : public MenuPlugin
{
	Q_OBJECT

private:
	QAction *action;

	bool bitmapIsEmpty (BITMAP *bmp);
	void *griddlit (const QString &name, int c, int type, int skipEmpty, int autocrop, int depth, int x, int y, int w, int h);
	void boxGriddle (const QString &name, int type, bool skipEmpty, bool autocrop, int depth);
	void gridGriddle (const QString &name, int type, bool skipEmpty, bool autocrop, int depth, int xGrid, int yGrid);

private slots:
	void activated();

public:
	MenuGrid ();
	~MenuGrid ();

	QAction *menuAction ();
};

/*****************************************************************************
 *
 * Class MenuAutocrop
 *
 *****************************************************************************/

class MenuAutocrop : public MenuPlugin
{
	Q_OBJECT

private:
	QAction *action;

private slots:
	void activated();

public:
	MenuAutocrop ();
	~MenuAutocrop ();

	QAction *menuAction ();
	QString contextMenuText ();
	bool    contextMenuTypeSupported (int type);
	void    contextMenuActivated (DatafileItem *item);
};

#endif
