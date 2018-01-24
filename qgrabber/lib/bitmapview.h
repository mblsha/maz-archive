/***************************************************************************
                          bitmapview.h  -  description
                             -------------------
    begin                : Tue Sep 3 2002
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

#ifndef BITMAPVIEW_H
#define BITMAPVIEW_H

#include "alleg.h"
#include "export.h"

#include <qscrollview.h>

class BitmapViewPrivate;

/*****************************************************************************
 *
 * Class BitmapView
 *
 *****************************************************************************/

class EXPORT BitmapView : public QScrollView
{
	Q_OBJECT

private:
	BitmapViewPrivate *d;

public:
	BitmapView (QWidget *parent, const char *name = 0);
	~BitmapView ();

public slots:
	void setBitmap (BITMAP *bmp);
	void setZoom (int zoom);
	
signals:
	void zoomChanged (int);
};

#endif
