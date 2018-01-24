/***************************************************************************
                          vecteditview.cpp  -  description
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

#include <qlayout.h>
#include <qpixmap.h>

#include "vecteditview.h"
#include "editor.h"

#include <qlabel.h>
#include "colorbutton.h"

VectEditView::VectEditView(QWidget *parent, const char *name)
			: QWidget(parent, name)
{
	QVBoxLayout *vbox = new QVBoxLayout (this);

	canvas = new QCanvas (640, 480);
	canvas->setAdvancePeriod (30);

	edit = new Editor (canvas, this, "Editor");
	vbox->addWidget (edit);

	QHBoxLayout *hbox = new QHBoxLayout (0, 5, 5);
	vbox->addLayout (hbox);

	hbox->addStretch (0);

	QLabel *colorLabel = new QLabel (this);
	colorLabel->setText ("Color:");
	hbox->addWidget (colorLabel);

	ColorButton *selColor = new ColorButton (this);
	hbox->addWidget (selColor);

	connect (selColor, SIGNAL(colorChanged(const QColor &)), edit, SLOT(setColor(const QColor &)));
	connect (edit, SIGNAL(colorChanged(const QColor &)), selColor, SLOT(setColor(const QColor &)));
	connect (edit, SIGNAL(colorChanged(const QColor &)), edit, SLOT(setColor(const QColor &)));
}

VectEditView::~VectEditView()
{
	delete canvas;
}

void VectEditView::slotLoad (QString filename)
{
	fileName = filename;
	edit->slotLoad (filename);
}

void VectEditView::slotSave (QString filename)
{
	if ( !filename.isEmpty() )
	{
		fileName = filename;
		edit->slotSave (filename);
	}
	else
		edit->slotSave (fileName);
}

void VectEditView::slotClear ()
{
	edit->slotClear ();
}

void VectEditView::slotLoadBackground (QString filename)
{
	QPixmap *back = new QPixmap;

	canvas->setBackgroundColor (Qt::white);

	if ( back->load(filename) )
		canvas->setBackgroundPixmap ( *back );
}


