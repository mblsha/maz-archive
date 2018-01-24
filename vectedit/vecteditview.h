/***************************************************************************
                          vecteditview.h  -  description
                             -------------------
    begin                : Û¬‘ Ì¡   4 11:04:57 MSD 2002
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

#ifndef VECTEDITVIEW_H
#define VECTEDITVIEW_H

// include files for QT
#include <qwidget.h>

// application specific includes
#include "editor.h"

class VectEditView : public QWidget
{
	Q_OBJECT

private:
	QCanvas *canvas;
	Editor *edit;

	QString fileName;

public slots:
	void slotLoadBackground (QString file);
	void slotLoad (QString file);
	void slotSave (QString file);
	void slotClear ();

protected slots:

public:
	VectEditView (QWidget *parent=0, const char *name = 0);
	~VectEditView();
};

#endif
