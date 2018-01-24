/***************************************************************************
                          grid.cpp  -  description
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

#include <qlineedit.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qmessagebox.h>

#include "grid.h"

#include "griddialog.h"

#include "prefs.h"
#include "plugins.h"
#include "plugins_p.h"
#include "datafileitem.h"

/* helper for cropping bitmaps */
static BITMAP *crop_bitmap(BITMAP *bmp, int *tx, int *ty)
{
   int tw, th, i, j, c;
   int changed = FALSE;

   *tx = 0;
   *ty = 0;
   tw = bmp->w;
   th = bmp->h;

   if ((tw > 0) && (th > 0)) {
      c = getpixel(bmp, 0, 0);

      for (j=*ty; j<(*ty)+th; j++) {       /* top of image */
	 for (i=*tx; i<(*tx)+tw; i++) {
	    if (getpixel(bmp, i, j) != c)
	       goto finishedtop;
	 }
	 (*ty)++;
	 th--;
	 changed = TRUE;
      }

      finishedtop:

      for (j=(*ty)+th-1; j>*ty; j--) {     /* bottom of image */
	 for (i=*tx; i<(*tx)+tw; i++) {
	    if (getpixel(bmp, i, j) != c)
	       goto finishedbottom;
	 }
	 th--;
	 changed = TRUE;
      }

      finishedbottom:

      for (j=*tx; j<*(tx)+tw; j++) {       /* left of image */
	 for (i=*ty; i<(*ty)+th; i++) {
	    if (getpixel(bmp, j, i) != c)
	       goto finishedleft;
	 }
	 (*tx)++;
	 tw--;
	 changed = TRUE;
      }

      finishedleft:

      for (j=*(tx)+tw-1; j>*tx; j--) {     /* right of image */
	 for (i=*ty; i<(*ty)+th; i++) {
	    if (getpixel(bmp, j, i) != c)
	       goto finishedright;
	 }
	 tw--;
	 changed = TRUE;
      }

      finishedright:
	 ;
   }

   if ((tw != 0) && (th != 0) && (changed)) {
      BITMAP *b2 = create_bitmap_ex(bitmap_color_depth(bmp), tw, th);
      clear_to_color(b2, b2->vtable->mask_color);
      blit(bmp, b2, *tx, *ty, 0, 0, tw, th);
      destroy_bitmap(bmp);
      return b2;
   }
   else
      return bmp;
}

/*****************************************************************************
 *
 * Class MenuGrid
 *
 *****************************************************************************/

MenuGrid::MenuGrid ()
{
	action = new QAction (this, "MenuGrid::action");
	action->setMenuText ( tr("Grab from grid") );

	connect (action, SIGNAL(activated()), this, SLOT(activated()));
}

MenuGrid::~MenuGrid ()
{
}

QAction *MenuGrid::menuAction ()
{
	return action;
}

void MenuGrid::activated ()
{
	if ( !Prefs::graphic() )
	{
		QMessageBox::warning (0, tr("Error"), tr("You must read in a bitmap file\nbefore you can grab data from it"), QMessageBox::Ok, QMessageBox::NoButton);

		return;
	}

	GridDialog *gd = new GridDialog (0, 0, true);
	if ( gd->exec() == QDialog::Accepted )
	{
		int type;
		switch ( gd->type->currentItem() )
		{
			case 0: type = DAT_BITMAP; break;
			case 1: type = DAT_RLE_SPRITE; break;
		}

		int depth;
		switch ( gd->colorDepth->currentItem() )
		{
			case 0: depth = 8; break;
			case 1: depth = 15; break;
			case 2: depth = 16; break;
			case 3: depth = 24; break;
			case 4: depth = 32; break;
		}

		QString name   = gd->nameEdit->text();
		bool skipEmpty = gd->skipEmpties->isChecked();
		bool autocrop  = gd->autocrop->isChecked();

		if ( gd->useColor->isChecked() )
			boxGriddle ( name, type, skipEmpty, autocrop, depth );
		else
			gridGriddle ( name, type, skipEmpty, autocrop, depth, gd->xSize->value(), gd->ySize->value() );
	}
	delete gd;
}

void MenuGrid::boxGriddle (const QString &name, int type, bool skipEmpty, bool autocrop, int depth)
{
	int c = 0;
	int x = 0, y = 0, w = 0, h = 0;

	datedit_find_character (Prefs::graphic(), &x, &y, &w, &h);

	while ( (w > 0) && (h > 0) )
	{
		void *item = griddlit (name, c, type, skipEmpty, autocrop, depth, x+1, y+1, w, h);
		if ( item )
			c++;

		x += w;
		datedit_find_character (Prefs::graphic(), &x, &y, &w, &h);
	}
}

void MenuGrid::gridGriddle (const QString &name, int type, bool skipEmpty, bool autocrop, int depth, int xGrid, int yGrid)
{
	int x, y;
	int c = 0;

	for (y = 0; y+yGrid <= Prefs::graphic()->h; y += yGrid)
	{
		for (x = 0; x+xGrid <= Prefs::graphic()->w; x += xGrid)
		{
			void *item = griddlit (name, c, type, skipEmpty, autocrop, depth, x, y, xGrid, yGrid);
			if (item)
				c++;
		}
	}
}

bool MenuGrid::bitmapIsEmpty (BITMAP *bmp)
{
	int x, y;
	int c = getpixel (bmp, 0, 0);

	for (y = 0; y < bmp->h; y++)
		for (x = 0; x < bmp->w; x++)
			if ( getpixel(bmp, x, y) != c )
				return false;

	return true;
}

void *MenuGrid::griddlit (const QString &name, int c, int type, int skipEmpty, int autocrop, int depth, int x, int y, int w, int h)
{
	RGB tmprgb = datedit_current_palette[0];
	if ( (type == DAT_RLE_SPRITE) || (type == DAT_C_SPRITE) || (type == DAT_XC_SPRITE) )
	{
		datedit_current_palette[0].r = 63;
		datedit_current_palette[0].g = 0;
		datedit_current_palette[0].b = 63;
	}
	select_palette (datedit_current_palette);

	BITMAP *bmp = create_bitmap_ex (depth, w, h);
	clear_to_color (bmp, bmp->vtable->mask_color);
	blit (Prefs::graphic(), bmp, x, y, 0, 0, w, h);

	unselect_palette ();
	datedit_current_palette[0] = tmprgb;

	if ( (skipEmpty) && (bitmapIsEmpty (bmp)) )
	{
		destroy_bitmap (bmp);
		return 0;
	}

	int tx = 0, ty = 0;
	if ( autocrop )
		bmp = crop_bitmap (bmp, &tx, &ty);

	void *v;
	if (type == DAT_RLE_SPRITE)
	{
		v = get_rle_sprite (bmp);
		destroy_bitmap (bmp);
	}
	else
		v = bmp;

	DATAFILE *dat = (DATAFILE *)malloc(sizeof(DATAFILE));
	dat->type = type;
	dat->size = 0;
	dat->prop = 0;
	dat->dat  = v;

	char buf[1024];
	sprintf(buf, "%s%04d", (const char *)name.utf8(), c);
	datedit_set_property(dat, DAT_NAME, buf);

	sprintf(buf, "%d", x);
	datedit_set_property(dat, DAT_XPOS, buf);

	sprintf(buf, "%d", y);
	datedit_set_property(dat, DAT_YPOS, buf);

	sprintf(buf, "%d", w);
	datedit_set_property(dat, DAT_XSIZ, buf);

	sprintf(buf, "%d", h);
	datedit_set_property(dat, DAT_YSIZ, buf);

	if (tx || ty)
	{
		sprintf(buf, "%d", tx);
		datedit_set_property(dat, DAT_XCRP, buf);

		sprintf(buf, "%d", ty);
		datedit_set_property(dat, DAT_YCRP, buf);
	}

	datedit_set_property (dat, DAT_ORIG, Prefs::graphicOrigin());
	datedit_set_property (dat, DAT_DATE, Prefs::graphicDate());

	datedit_sort_properties (dat->prop);

	emit insertItem (dat);
	return v;
}

/*****************************************************************************
 *
 * Class MenuAutocrop
 *
 *****************************************************************************/

MenuAutocrop::MenuAutocrop ()
{
	action = new QAction (this, "MenuAutocrop::action");
	action->setMenuText ( tr("Autocrop") );

	connect (action, SIGNAL(activated()), this, SLOT(activated()));
}

MenuAutocrop::~MenuAutocrop ()
{
}

QAction *MenuAutocrop::menuAction ()
{
	return action;
}

void MenuAutocrop::activated ()
{
	if ( !Prefs::graphic() )
	{
		QMessageBox::warning (0, tr("Error"), tr("You must read in a bitmap file\nbefore you can autocrop it"), QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}

	int tx = 0, ty = 0;
	Prefs::setGraphic ( crop_bitmap (Prefs::graphic(), &tx, &ty) );
}

QString MenuAutocrop::contextMenuText ()
{
	return tr("Autocrop");
}

bool MenuAutocrop::contextMenuTypeSupported (int type)
{
	return type == DAT_BITMAP || type == DAT_RLE_SPRITE;
}

void MenuAutocrop::contextMenuActivated (DatafileItem *item)
{
	DATAFILE *dat = item->data();
	int tx = 0, ty = 0;

	if ( dat->type == DAT_BITMAP )
	{
		dat->dat = crop_bitmap ((BITMAP*)dat->dat, &tx, &ty);
	}
	else if ( dat->type == DAT_RLE_SPRITE )
	{
		RLE_SPRITE *spr = (RLE_SPRITE *)dat->dat;
		BITMAP *bmp = create_bitmap_ex (spr->color_depth, spr->w, spr->h);
		clear_to_color (bmp, bmp->vtable->mask_color);
		draw_rle_sprite (bmp, spr, 0, 0);
		destroy_rle_sprite (spr);
		bmp = crop_bitmap (bmp, &tx, &ty);
		dat->dat = get_rle_sprite (bmp);
		destroy_bitmap (bmp);
	}

	emit dirty();
}

/*****************************************************************************
 *
 * Class GridPlugin
 *
 *****************************************************************************/

class GridPlugin : public QGrabberPlugin
{
public:
	GridPlugin() {}
	~GridPlugin() {}

	QStringList keys() const
	{
		QStringList list;

		list << "Grid";

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

		list << "Grab from grid";
		list << "Autocrop";

		return list;
	}

	MenuPlugin *createMenu(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "grab from grid")
			return new MenuGrid;
		else
		if (plug == "autocrop")
			return new MenuAutocrop;

		return 0;
	}
};

EXPORT_PLUGIN( GridPlugin )
