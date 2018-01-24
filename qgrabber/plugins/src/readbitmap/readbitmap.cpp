/***************************************************************************
                          readbitmap.cpp  -  description
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

#include "readbitmap.h"

#include <qfiledialog.h>
#include <qmessagebox.h>

#include "viewbitmap.h"

#include "prefs.h"
#include "plugins_p.h"

static void viewGraphic ()
{
	if ( !Prefs::graphic() )
	{
		QMessageBox::warning (0, MenuViewBitmap::tr("Error"), MenuViewBitmap::tr("You must read in a bitmap file\nbefore you can view it"), QMessageBox::Ok, QMessageBox::NoButton);

		return;
	}

	ViewBitmap *vb = new ViewBitmap (0);
	vb->exec ();
}

/*****************************************************************************
 *
 * Class MenuReadBitmap
 *
 *****************************************************************************/

MenuReadBitmap::MenuReadBitmap ()
{
	action = new QAction (this, "MenuReadBitmap::action");
	action->setMenuText ( tr("Read Bitmap") );

	connect (action, SIGNAL(activated()), this, SLOT(activated()));
}

MenuReadBitmap::~MenuReadBitmap ()
{
}

QAction *MenuReadBitmap::menuAction ()
{
	return action;
}

int MenuReadBitmap::menuAddTo ()
{
	return File;
}


void MenuReadBitmap::activated ()
{
	QString filter = tr("Images") + " (*.bmp *.pcx *.lbm *.tga)";

	QString fileName = QFileDialog::getOpenFileName (QFile::decodeName(Prefs::graphicOrigin()), filter, 0, tr("Read Bitmap"));
	if ( !fileName.isEmpty() )
	{
		if ( Prefs::graphic() )
			destroy_bitmap ( Prefs::graphic() );

		Prefs::setGraphic ( load_bitmap ( QFile::encodeName( fileName ), 0 ) );

		if ( !Prefs::graphic() )
		{
			QMessageBox::warning (0, tr("Error"), tr("Could not read file '%1' as bitmap").arg(fileName), QMessageBox::Ok, QMessageBox::NoButton);

			return;
		}

		Prefs::setGraphicOrigin ( QFile::encodeName( fileName ) );
		Prefs::setGraphicDate ( datedit_ftime2asc( file_time( QFile::encodeName( fileName ) ) ) );

		viewGraphic ();
	}
}

/*****************************************************************************
 *
 * Class MenuViewBitmap
 *
 *****************************************************************************/

MenuViewBitmap::MenuViewBitmap ()
{
	action = new QAction (this, "MenuViewBitmap::action");
	action->setMenuText ( tr("View Bitmap") );

	connect (action, SIGNAL(activated()), this, SLOT(activated()));
}

MenuViewBitmap::~MenuViewBitmap ()
{
}

QAction *MenuViewBitmap::menuAction ()
{
	return action;
}

int MenuViewBitmap::menuAddTo ()
{
	return File;
}

void MenuViewBitmap::activated ()
{
	viewGraphic ();
}

/*****************************************************************************
 *
 * Class ReadBitmapPlugin
 *
 *****************************************************************************/

class ReadBitmapPlugin : public QGrabberPlugin
{
public:
	ReadBitmapPlugin() {}
	~ReadBitmapPlugin() {}

	QStringList keys() const
	{
		QStringList list;

		list << "ReadBitmap";

		return list;
	}

	QStringList keysObject() const
	{
		QStringList list;

		return list;
	}

	ObjectPlugin *createObject(const QString &key)
	{
		QString plug = key.lower();

		return 0;
	}

	QStringList keysIO() const
	{
		QStringList list;

		return list;
	}

	IOPlugin *createIO(const QString &key)
	{
		QString plug = key.lower();

		return 0;
	}

	QStringList keysMenu() const
	{
		QStringList list;

		list << "Read Bitmap";
		list << "View Bitmap";

		return list;
	}

	MenuPlugin *createMenu(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "read bitmap")
			return new MenuReadBitmap;
		else
		if (plug == "view bitmap")
			return new MenuViewBitmap;

		return 0;
	}
};

EXPORT_PLUGIN( ReadBitmapPlugin )
