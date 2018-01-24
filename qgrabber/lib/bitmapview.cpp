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

#include "bitmapview.h"

#include <qwidget.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>

#include "plugins.h"

/*****************************************************************************
 *
 * Class BitmapViewPrivate
 *
 *****************************************************************************/

class BitmapViewPrivate : public QWidget
{
private:
	BITMAP *bmp;
	QPixmap *pix;
	static int Zoom;

	void paintEvent (QPaintEvent *p);
	void drawBitmap (QPainter *painter);

public:
	BitmapViewPrivate (QWidget *parent = 0, const char *name = 0);
	~BitmapViewPrivate ();

	void setZoom (int zoom);
	void setBitmap (BITMAP *bmp);
	int zoom () const;
};

int BitmapViewPrivate::Zoom = 1;

BitmapViewPrivate::BitmapViewPrivate (QWidget *parent, const char *name)
			: QWidget (parent, name)
{
	bmp = 0;
	pix = 0;
}

BitmapViewPrivate::~BitmapViewPrivate ()
{
	if ( pix )
		delete pix;
}

void BitmapViewPrivate::setZoom (int z)
{
	if ( Zoom == z )
		return;
		
	Zoom = z;
	paintEvent (0);
}

int BitmapViewPrivate::zoom () const
{
	return Zoom;
}

void BitmapViewPrivate::setBitmap (BITMAP *b)
{
	bmp = b;

	QImage *img = bmp2img (bmp);
	pix = new QPixmap (bmp->w, bmp->h, -1, QPixmap::BestOptim);
	pix->convertFromImage (*img);

	delete img;
}

void BitmapViewPrivate::paintEvent (QPaintEvent *event)
{
	if ( !bmp || !pix )
		return;

	QSize size (bmp->w * Zoom, bmp->h * Zoom);
	setMinimumSize (size);
	setMaximumSize (size);

	QPainter paint (this);
	drawBitmap (&paint);
}

void BitmapViewPrivate::drawBitmap (QPainter *paint)
{
	paint->save();

	paint->setWindow (0, 0, bmp->w, bmp->h);
	paint->save();
	paint->drawPixmap (0, 0, *pix);
	paint->restore();

	paint->restore();
}

/*****************************************************************************
 *
 * Class BitmapView
 *
 *****************************************************************************/

BitmapView::BitmapView (QWidget *parent, const char *name)
			: QScrollView (parent, name)
{
	d = new BitmapViewPrivate (0);
	addChild (d);
}

BitmapView::~BitmapView ()
{
	delete d;
}

void BitmapView::setBitmap (BITMAP *bmp)
{
	emit zoomChanged ( d->zoom() );
	d->setBitmap (bmp);
}

void BitmapView::setZoom (int zoom)
{
	d->setZoom (zoom);
}

