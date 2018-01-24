/***************************************************************************
                          itype.h  -  description
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

#ifndef ITYPE_H
#define ITYPE_H

#include <qpopupmenu.h>

#include "plugins.h"
#include "datafileitem.h"

/*****************************************************************************
 *
 * Class MenuChangeImageType
 *
 *****************************************************************************/

class MenuChangeImageType : public MenuPlugin
{
	Q_OBJECT

private:
	DatafileItem *item;

private slots:
	void toBitmap ();
	void toRLESprite ();

public:
	MenuChangeImageType();
	~MenuChangeImageType ();

	QString contextMenuText ();
	bool    contextMenuTypeSupported (int type);
	QPopupMenu *contextMenuPopup (DatafileItem *item);
};

/*****************************************************************************
 *
 * Class MenuChangeImageDepth
 *
 *****************************************************************************/

class MenuChangeImageDepth : public MenuPlugin
{
	Q_OBJECT

private:
	DatafileItem *item;
	void convert (int depth);

private slots:
	void to8  ();
	void to15 ();
	void to16 ();
	void to24 ();
	void to32 ();

public:
	MenuChangeImageDepth();
	~MenuChangeImageDepth ();

	QString contextMenuText ();
	bool    contextMenuTypeSupported (int type);
	QPopupMenu *contextMenuPopup (DatafileItem *it);
};

#endif