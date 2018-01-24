/***************************************************************************
                          palette.cpp  -  description
                             -------------------
    begin                : Wed Apr 17 2002
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

#include <qimage.h>
#include <qpixmap.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qpainter.h>

static QPixmap *paletteData = 0;

#include "common.h"
#include "plugins.h"
#include "plugins_p.h"
#include "datafileitem.h"

#include "palette.h"
#include "paletteplot.h"

#include <allegro/internal/aintern.h>

/*****************************************************************************
 *
 * Class DatafileItemPalette
 *
 *****************************************************************************/

class DatafileItemPalette : public DatafileItem
{
public:
	DatafileItemPalette (QListViewItem *parent, DATAFILE *data)
				: DatafileItem (parent, data)
	{
		setPixmap ( paletteData );
	}

	~DatafileItemPalette () {}
};


/*****************************************************************************
 *
 * Class PaletteView
 *
 *****************************************************************************/

PaletteView::PaletteView (QWidget *parent, const char *name)
			: QWidget (parent, name)
{
	pix = 0;
}

PaletteView::~PaletteView ()
{
	if (pix)
		delete pix;
}

void PaletteView::setPalette (RGB *pal)
{
	BITMAP *bmp = create_bitmap_ex (32, 16, 16);
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			int col = makecol32 ( pal[y*16 + x].r * 4,
					      pal[y*16 + x].g * 4,
					      pal[y*16 + x].b * 4 );

			putpixel (bmp, x, y, col);
		}
	}
	QImage *img = bmp2img (bmp);
	destroy_bitmap (bmp);

	pix = new QPixmap (*img);
	delete img;
}

void PaletteView::paintEvent (QPaintEvent *p)
{
	if ( !pix )
		return;

	QPainter paint (this);

	paint.save();

	paint.setWindow (0, 0, 16, 16);
	paint.save();
	paint.drawPixmap (0, 0, *pix);
	paint.restore();

	paint.restore();
}

/*****************************************************************************
 *
 * Class ObjectPAL
 *
 *****************************************************************************/

class ObjectPAL : public ObjectPlugin
{
private:
public:
	ObjectPAL ();
	~ObjectPAL ();

	bool    typeSupported (int type);
	int     type          ();
	QString menuDesc      ();
	QPixmap pixmap        ();
	void    open          (DatafileItem *parent, DATAFILE *data);
	void    *makeNew      (long *size);
	void    *copyData     (DATAFILE *dat, long *size);
	void    plot          (DATAFILE *dat, QWidget *parent);
	bool    doubleClick   (DATAFILE *dat);
};

ObjectPAL::ObjectPAL ()
{
	paletteData = new QPixmap (getResourcesDir()+"/icons/object/palette.png");
}

ObjectPAL::~ObjectPAL ()
{
	delete paletteData;
}

bool ObjectPAL::typeSupported (int type)
{
	return (type == DAT_PALETTE);
}

int ObjectPAL::type ()
{
	return DAT_PALETTE;
}

QString ObjectPAL::menuDesc ()
{
	return tr("Palette");
}

QPixmap ObjectPAL::pixmap ()
{
	return *paletteData;
}

void ObjectPAL::open (DatafileItem *parent, DATAFILE *data)
{
	DatafileItemPalette *item;
	item = new DatafileItemPalette (parent, data);
}

void *ObjectPAL::makeNew (long *size)
{
	RGB *pal = (RGB *) malloc(sizeof(PALETTE));

	memcpy (pal, default_palette, sizeof(PALETTE));
	*size = sizeof(PALETTE);

	return pal;
}

void *ObjectPAL::copyData (DATAFILE *dat, long *size)
{
	RGB *pal = (RGB *)malloc(sizeof(PALETTE));
	memcpy (pal, dat->dat, sizeof(PALETTE));

	*size = sizeof(PALETTE);
	return pal;
}

void ObjectPAL::plot (DATAFILE *dat, QWidget *parent)
{
	QVBoxLayout *vbox = new QVBoxLayout (parent);

	PalettePlot *pp = new PalettePlot (parent, "PalettePlot");
	pp->setPalette ((RGB *)dat->dat);

	vbox->addWidget (pp);

	parent->show ();
}

bool ObjectPAL::doubleClick (DATAFILE *dat)
{
	selPalette ( (RGB *)dat->dat );
	return true;
}

/*****************************************************************************
 *
 * Class IoPAL
 *
 *****************************************************************************/

class IoPAL : public IOPlugin
{
private:
public:
	IoPAL ();
	~IoPAL ();

	bool    typeSupported (int type);
	QString grabType      ();
	QString exportType    ();
	void    *grabFile     (const char *file, long *size);
	int     exportFile    (const char *file, DatafileItem *item);
};

IoPAL::IoPAL ()
{
}

IoPAL::~IoPAL ()
{
}

bool IoPAL::typeSupported (int type)
{
	return (type == DAT_PALETTE);
}

QString IoPAL::grabType ()
{
	return tr("Images") + " (*.bmp *.lbm *.pcx *.tga)";
}

QString IoPAL::exportType ()
{
	return tr("Images") + " (*.bmp *.pcx *.tga)";
}

void *IoPAL::grabFile (const char *filename, long *size)
{
	int oldcolordepth = _color_depth;
	RGB *pal;
	BITMAP *bmp;

	_color_depth = 8;
	set_color_conversion (COLORCONV_TOTAL);

	pal = (RGB *)malloc (sizeof(PALETTE));
	bmp = load_bitmap ( (char *)filename, pal);

	_color_depth = oldcolordepth;
	set_color_conversion (COLORCONV_NONE);

	if (!bmp)
	{
		free (pal);
		return NULL;
	}

	destroy_bitmap (bmp);

	*size = sizeof (PALETTE);
	return pal;
}

int IoPAL::exportFile (const char *filename, DatafileItem *item)
{
	BITMAP *bmp;
	int ret;

	bmp = create_bitmap_ex (8, 32, 8);
	clear_bitmap (bmp);
	text_mode (0);
	textout (bmp, font, "PAL.", 0, 0, 255);

	ret = ( save_bitmap ( (char *)filename, bmp, (RGB *)item->data()->dat) == 0 );

	destroy_bitmap (bmp);
	return ret;
}

/*****************************************************************************
 *
 * Class PalettePlugin
 *
 *****************************************************************************/

class PalettePlugin : public QGrabberPlugin
{
public:
	PalettePlugin() {}
	~PalettePlugin() {}

	QStringList keys() const
	{
		QStringList list;

		list << "Palette";

		return list;
	}

	QStringList keysObject() const
	{
		QStringList list;

		list << "Palette";

		return list;
	}

	ObjectPlugin *createObject(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "palette")
			return new ObjectPAL;

		return 0;
	}

	QStringList keysIO() const
	{
		QStringList list;

		list << "Palette";

		return list;
	}

	IOPlugin *createIO(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "palette")
			return new IoPAL;

		return 0;
	}

	QStringList keysMenu() const
	{
		QStringList list;

		return list;
	}

	MenuPlugin *createMenu(const QString &key)
	{
		QString plug = key.lower();

		return 0;
	}
};

EXPORT_PLUGIN( PalettePlugin )

