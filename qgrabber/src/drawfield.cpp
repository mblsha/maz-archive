/***************************************************************************
                          drawfield.cpp  -  description
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

#include "drawfield.h"
#include "plugins.h"
#include "factory.h"

#include <qlayout.h>
#include <qlabel.h>

#include <qmultilineedit.h>

DrawField::DrawField (QWidget *parent, const char *name)
			: QWidget (parent, name)
{
	drawBox = new QVBoxLayout (this, 0, -1, "DrawField::drawBox");
	drawBox->setMargin ( 5 );	
	
	drawChild = NULL;
}

DrawField::~DrawField ()
{
	clear ();
}

void DrawField::displayItem (QListViewItem *it)
{
	clear ();

	if ( !it )
		return;

	item = static_cast<DatafileItem *>(it);

	if ( item->isRoot () )
		return;

	ObjectPlugin *plug = ObjectFactory::create (item->type());

	if (plug)
	{
		drawChild = new QWidget (this);
		drawBox->addWidget (drawChild);

		plug->plot (item->data(), drawChild);
	}
}

void DrawField::clear ()
{
	if (drawChild)
	{
		delete drawChild;
		drawChild = NULL;
	}
}
