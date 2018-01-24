/***************************************************************************
                          image.cpp  -  description
                             -------------------
    begin                : Tue Apr 16 2002
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

#include <qmessagebox.h>

static QPixmap *bitmapData = 0;
static QPixmap *rleData = 0;

#include "imageplot.h"

#include "common.h"
#include "plugins.h"
#include "plugins_p.h"
#include "datafileitem.h"

/*****************************************************************************
 *
 * from datimage.c
 *
 *****************************************************************************/

/* checks whether this bitmap has an alpha channel */
static int bitmap_has_alpha(BITMAP *bmp)
{
   int x, y, c;

   if (bitmap_color_depth(bmp) != 32)
      return FALSE;

   for (y=0; y<bmp->h; y++) {
      for (x=0; x<bmp->w; x++) {
	 c = getpixel(bmp, x, y);
	 if (geta32(c))
	    return TRUE;
      }
   }

   return FALSE;
}



/* checks whether this RLE sprite has an alpha channel */
static int rle_has_alpha(AL_CONST RLE_SPRITE *spr)
{
   signed long *p32;
   int x, y, c;

   if (spr->color_depth != 32)
      return FALSE;

   p32 = (signed long *)spr->dat;

   for (y=0; y<spr->h; y++) {
      while ((unsigned long)*p32 != MASK_COLOR_32) {
	 if (*p32 < 0) {
	    p32++;
	 }
	 else {
	    x = *p32;
	    p32++;

	    while (x-- > 0) {
	       c = (unsigned long)*p32;
	       if (geta32(c))
		  return TRUE;
	       p32++;
	    }
	 }
      }

      p32++;
   }

   return FALSE;
}

/*****************************************************************************
 *
 * Class DatafileItemBitmap
 *
 *****************************************************************************/

class DatafileItemBitmap : public DatafileItem
{
public:
	DatafileItemBitmap (QListViewItem *parent, DATAFILE *data)
				: DatafileItem (parent, data)
	{
		setPixmap ( bitmapData );
	}

	~DatafileItemBitmap () {}
};

/*****************************************************************************
 *
 * Class ObjectBMP
 *
 *****************************************************************************/

class ObjectBMP : public ObjectPlugin
{
private:
public:
	ObjectBMP ();
	~ObjectBMP ();

	bool    typeSupported (int type);
	int     type          ();
	QString menuDesc      ();
	QPixmap pixmap        ();
	void    open          (DatafileItem *parent, DATAFILE *data);
	void    *makeNew      (long *size);
	void    *copyData     (DATAFILE *dat, long *size);
	void    saveObject    (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f);
	void    plot          (DATAFILE *dat, QWidget *parent);
};

ObjectBMP::ObjectBMP ()
{
	bitmapData = new QPixmap (getResourcesDir()+"/icons/object/image.png");
}

ObjectBMP::~ObjectBMP ()
{
	delete bitmapData;
}

bool ObjectBMP::typeSupported (int type)
{
	return (type == DAT_BITMAP);
}

int ObjectBMP::type ()
{
	return DAT_BITMAP;
}

QString ObjectBMP::menuDesc ()
{
	return tr("Bitmap");
}

QPixmap ObjectBMP::pixmap ()
{
	return *bitmapData;
}

void ObjectBMP::open (DatafileItem *parent, DATAFILE *data)
{
	DatafileItemBitmap *bmp;
	bmp = new DatafileItemBitmap (parent, data);
}

void *ObjectBMP::makeNew (long *size)
{
	BITMAP *bmp = create_bitmap_ex (8, 32, 32);

	clear_bitmap (bmp);
	text_mode (-1);
	textout_centre (bmp, font, "Hi!", 16, 12, 15);

	return bmp;
}

void *ObjectBMP::copyData (DATAFILE *dat, long *size)
{
	BITMAP *src = (BITMAP *)dat->dat;
	BITMAP *dest = create_bitmap_ex(src->vtable->color_depth, src->w, src->h);
	blit (src, dest, 0, 0, 0, 0, src->w, src->h);
	return dest;
}

void ObjectBMP::saveObject (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f)
{
	BITMAP *bmp = (BITMAP *)item->data()->dat;
	int x, y, c, r, g, b, a;
	unsigned short *p16;
	unsigned long *p32;
	int depth;

	if ( bitmap_has_alpha ( bmp ) )
		depth = -32;
	else
		depth = bitmap_color_depth (bmp);

	pack_mputw (depth,  f);
	pack_mputw (bmp->w, f);
	pack_mputw (bmp->h, f);

	switch (depth)
	{
		case 8:
			/* 256 colors */
			for (y=0; y<bmp->h; y++)
				for (x=0; x<bmp->w; x++)
					pack_putc(bmp->line[y][x], f);
			break;

		case 15:
		case 16:
			/* hicolor */
			for (y=0; y<bmp->h; y++) {
				p16 = (unsigned short *)bmp->line[y];

				for (x=0; x<bmp->w; x++)
				{

					c = p16[x];
					r = getr_depth(depth, c);
					g = getg_depth(depth, c);
					b = getb_depth(depth, c);
					c = ((r<<8)&0xF800) | ((g<<3)&0x7E0) | ((b>>3)&0x1F);
					pack_iputw(c, f);
				}
			}
			break;

		case 24:
			/* 24 bit truecolor */
			for (y=0; y<bmp->h; y++)
			{
				for (x=0; x<bmp->w; x++)
				{
					r = bmp->line[y][x*3+_rgb_r_shift_24/8];
					g = bmp->line[y][x*3+_rgb_g_shift_24/8];
					b = bmp->line[y][x*3+_rgb_b_shift_24/8];
					pack_putc(r, f);
					pack_putc(g, f);
					pack_putc(b, f);
				}
			}
			break;

		case 32:
			/* 32 bit truecolor */
			for (y=0; y<bmp->h; y++)
			{
				p32 = (unsigned long *)bmp->line[y];

				for (x=0; x<bmp->w; x++)
				{
					c = p32[x];
					r = getr32(c);
					g = getg32(c);
					b = getb32(c);
					pack_putc(r, f);
					pack_putc(g, f);
					pack_putc(b, f);
				}
			}
			break;

		case -32:
			/* 32 bit truecolor with alpha channel */
			for (y=0; y<bmp->h; y++)
			{
				p32 = (unsigned long *)bmp->line[y];

				for (x=0; x<bmp->w; x++)
				{
					c = p32[x];
					r = getr32(c);
					g = getg32(c);
					b = getb32(c);
					a = geta32(c);
					pack_putc(r, f);
					pack_putc(g, f);
					pack_putc(b, f);
					pack_putc(a, f);
				}
			}
			break;
	}
}


void ObjectBMP::plot (DATAFILE *dat, QWidget *parent)
{
	QVBoxLayout *vbox = new QVBoxLayout (parent);

	ImagePlot *ip = new ImagePlot (parent);

	BITMAP *bmp = (BITMAP *)dat->dat;
	ip->setBitmap (bmp, bitmap_has_alpha(bmp));
	ip->setType ( tr("Bitmap") );

	vbox->addWidget (ip);

	parent->show ();
}

/*****************************************************************************
 *
 * Class IoBMP
 *
 *****************************************************************************/

class IoBMP : public IOPlugin
{
public:
	IoBMP ();
	~IoBMP ();

	bool    typeSupported (int type);
	QString grabType      ();
	QString exportType    ();
	void    *grabFile     (const char *file, long *size);
	int     exportFile    (const char *file, DatafileItem *item);
};


IoBMP::IoBMP ()
{
}

IoBMP::~IoBMP ()
{
}

bool IoBMP::typeSupported (int type)
{
	return (type == DAT_BITMAP);
}

QString IoBMP::grabType ()
{
	return tr("Images") + " (*.bmp *.lbm *.pcx *.tga)";
}

QString IoBMP::exportType ()
{
	return tr("Images") + " (*.bmp *.pcx *.tga)";
}

void *IoBMP::grabFile (const char *filename, long *size)
{
	BITMAP *bmp = load_bitmap( (char *)filename, datedit_last_read_pal);

	if (!bmp)
		return NULL;

	return bmp;
}

int IoBMP::exportFile (const char *filename, DatafileItem *item)
{
	return ( save_bitmap( (char *)filename, (BITMAP *)item->data()->dat, datedit_current_palette) == 0 );
}

/*****************************************************************************
 *
 * Class DatafileItemRLE
 *
 *****************************************************************************/

class DatafileItemRLE : public DatafileItem
{
public:
	DatafileItemRLE (QListViewItem *parent, DATAFILE *data)
				: DatafileItem (parent, data)
	{
		setPixmap ( rleData );
	}

	~DatafileItemRLE () {}
};

/*****************************************************************************
 *
 * Class ObjectRLE
 *
 *****************************************************************************/

class ObjectRLE : public ObjectPlugin
{
private:
	BITMAP *bmp;

public:
	ObjectRLE ();
	~ObjectRLE ();

	bool    typeSupported (int type);
	int     type          ();
	QString menuDesc      ();
	QPixmap pixmap        ();
	void    open          (DatafileItem *parent, DATAFILE *data);
	void    *makeNew      (long *size);
	void    *copyData     (DATAFILE *dat, long *size);
	void    saveObject    (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f);
	void    plot          (DATAFILE *dat, QWidget *parent);
};

ObjectRLE::ObjectRLE ()
{
	rleData = new QPixmap (getResourcesDir()+"/icons/object/image.png");

	bmp = NULL;

}

ObjectRLE::~ObjectRLE ()
{
	delete rleData;

	if (bmp)
		free (bmp);
}

bool ObjectRLE::typeSupported (int type)
{
	return (type == DAT_RLE_SPRITE);
}

int ObjectRLE::type ()
{
	return DAT_RLE_SPRITE;
}

QString ObjectRLE::menuDesc ()
{
	return tr("RLE sprite");
}

QPixmap ObjectRLE::pixmap ()
{
	return *rleData;
}

void ObjectRLE::open (DatafileItem *parent, DATAFILE *data)
{
	DatafileItemRLE *rle;
	rle = new DatafileItemRLE (parent, data);
}

void *ObjectRLE::makeNew (long *size)
{
	BITMAP *bmp = create_bitmap_ex (8, 32, 32);

	clear_bitmap (bmp);
	text_mode (-1);
	textout_centre (bmp, font, "Hi!", 16, 12, 15);

	RLE_SPRITE *spr = get_rle_sprite (bmp);

	destroy_bitmap (bmp);

	return spr;
}

void *ObjectRLE::copyData (DATAFILE *dat, long *size)
{
	RLE_SPRITE *src = (RLE_SPRITE *)dat->dat;
	BITMAP *bmp = create_bitmap_ex (src->color_depth, src->w, src->h);
	clear_to_color (bmp, bmp->vtable->mask_color);
	draw_rle_sprite (bmp, src, 0, 0);
	RLE_SPRITE *dest = get_rle_sprite (bmp);
	destroy_bitmap (bmp);
	return dest;
}

void ObjectRLE::saveObject (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f)
{
	RLE_SPRITE *spr = (RLE_SPRITE *)item->data()->dat;
	int x, y, c, r, g, b, a;
	signed short *p16;
	signed long *p32;
	unsigned long eol_marker;
	int depth;

	if ( rle_has_alpha (spr) )
		depth = -32;
	else
		depth = spr->color_depth;

	pack_mputw (depth, f);
	pack_mputw (spr->w, f);
	pack_mputw (spr->h, f);
	pack_mputl (spr->size, f);

	switch (depth)
	{
		case 8:
			/* 256 colors, easy! */
			pack_fwrite(spr->dat, spr->size, f);
			break;

		case 15:
		case 16:
			/* hicolor */
			p16 = (signed short *)spr->dat;
			eol_marker = (depth == 15) ? MASK_COLOR_15 : MASK_COLOR_16;

			for (y = 0; y < spr->h; y++)
			{
				while ((unsigned short)*p16 != (unsigned short)eol_marker)
				{
					if (*p16 < 0)
					{
						/* skip count */
						pack_iputw(*p16, f);
						p16++;
					}
					else
					{
						/* solid run */
						x = *p16;
						p16++;

						pack_iputw(x, f);

						while (x-- > 0)
						{
							c = (unsigned short)*p16;
							r = getr_depth(depth, c);
							g = getg_depth(depth, c);
							b = getb_depth(depth, c);
							c = ((r<<8)&0xF800) | ((g<<3)&0x7E0) | ((b>>3)&0x1F);
							pack_iputw(c, f);
							p16++;
						}
					}
				}

				/* end of line */
				pack_iputw(MASK_COLOR_16, f);
				p16++;
			}
			break;

		case 24:
		case 32:
			/* truecolor */
			p32 = (signed long *)spr->dat;
			eol_marker = (depth == 24) ? MASK_COLOR_24 : MASK_COLOR_32;

			for (y=0; y<spr->h; y++)
			{
				while ((unsigned long)*p32 != eol_marker)
				{
					if (*p32 < 0)
					{
						/* skip count */
						pack_iputl(*p32, f);
						p32++;
					}
					else
					{
						/* solid run */
						x = *p32;
						p32++;

						pack_iputl(x, f);

						while (x-- > 0)
						{
							c = (unsigned long)*p32;
							r = getr_depth(depth, c);
							g = getg_depth(depth, c);
							b = getb_depth(depth, c);
							pack_putc(r, f);
							pack_putc(g, f);
							pack_putc(b, f);

							p32++;
						}
					}
				}

				/* end of line */
				pack_iputl(MASK_COLOR_32, f);
				p32++;
			}
			break;

		case -32:
			/* truecolor with alpha channel */
			p32 = (signed long *)spr->dat;

			for (y=0; y<spr->h; y++)
			{
				while ((unsigned long)*p32 != MASK_COLOR_32)
				{
					if (*p32 < 0)
					{
						/* skip count */
						pack_iputl(*p32, f);
						p32++;
					}
					else
					{
						/* solid run */
						x = *p32;
						p32++;

						pack_iputl(x, f);

						while (x-- > 0)
						{
							c = (unsigned long)*p32;
							r = getr32(c);
							g = getg32(c);
							b = getb32(c);
							a = geta32(c);
							pack_putc(r, f);
							pack_putc(g, f);
							pack_putc(b, f);
							pack_putc(a, f);
							p32++;
						}
					}
				}



				/* end of line */
				pack_iputl(MASK_COLOR_32, f);
				p32++;
			}
			break;
	}
}


void ObjectRLE::plot (DATAFILE *dat, QWidget *parent)
{
	QVBoxLayout *vbox = new QVBoxLayout (parent);

	ImagePlot *ip = new ImagePlot (parent);

	RLE_SPRITE *rle = (RLE_SPRITE *)dat->dat;
	if (bmp)
		free (bmp);
	bmp = create_bitmap_ex (rle->color_depth, rle->w, rle->h);
	clear_to_color (bmp, bmp->vtable->mask_color);
	draw_rle_sprite (bmp, rle, 0, 0);

	ip->setBitmap (bmp, rle_has_alpha(rle));
	ip->setType ( tr("RLE sprite") );

	vbox->addWidget (ip);

	parent->show ();
}

/*****************************************************************************
 *
 * Class IoRLE
 *
 *****************************************************************************/

class IoRLE : public IOPlugin
{
public:
	IoRLE ();
	~IoRLE ();

	bool    typeSupported (int type);
	QString grabType      ();
	QString exportType    ();
	void    *grabFile     (const char *file, long *size);
	int     exportFile    (const char *file, DatafileItem *item);
};

IoRLE::IoRLE ()
{
	// ...
}

IoRLE::~IoRLE ()
{
	// ...
}

bool IoRLE::typeSupported (int type)
{
	return (type == DAT_RLE_SPRITE);
}

QString IoRLE::grabType ()
{
	return tr("Images") + " (*.bmp *.lbm *.pcx *.tga)";
}

QString IoRLE::exportType ()
{
	return tr("Images") + " (*.bmp *.pcx *.tga)";
}

void *IoRLE::grabFile (const char *filename, long *size)
{
	BITMAP *bmp;
	RLE_SPRITE *spr;

	bmp = load_bitmap ( (char *)filename, datedit_last_read_pal);
	if (!bmp)
		return NULL;

	spr = get_rle_sprite (bmp);
	destroy_bitmap (bmp);

	return spr;
}

int IoRLE::exportFile (const char *filename, DatafileItem *item)
{
	BITMAP *bmp;
	RLE_SPRITE *rle = (RLE_SPRITE *)item->data()->dat;
	int ret;

	bmp = create_bitmap_ex (rle->color_depth, rle->w, rle->h);
	clear_to_color (bmp, bmp->vtable->mask_color);
	draw_rle_sprite (bmp, rle, 0, 0);

	ret = ( save_bitmap ( (char *)filename, bmp, datedit_current_palette) == 0 );

	destroy_bitmap (bmp);
	return ret;
}


/*****************************************************************************
 *
 * Class ImagePlugin
 *
 *****************************************************************************/

class ImagePlugin : public QGrabberPlugin
{
public:
	ImagePlugin() {}
	~ImagePlugin() {}

	QStringList keys() const
	{
		QStringList list;

		list << "Image";

		return list;
	}

	QStringList keysObject() const
	{
		QStringList list;

		list << "Bitmap";
		list << "RLE sprite";

		return list;
	}

	ObjectPlugin *createObject(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "bitmap")
			return new ObjectBMP;
		else
		if (plug == "rle sprite")
			return new ObjectRLE;

		return 0;
	}

	QStringList keysIO() const
	{
		QStringList list;

		list << "Bitmap";
		list << "RLE sprite";

		return list;
	}

	IOPlugin *createIO(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "bitmap")
			return new IoBMP;
		else
		if (plug == "rle sprite")
			return new IoRLE;

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

EXPORT_PLUGIN( ImagePlugin )

