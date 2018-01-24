/***************************************************************************
                          itype.cpp  -  description
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

#include "itype.h"

#include <qlistbox.h>
#include <qpopupmenu.h>

#include "plugins.h"
#include "plugins_p.h"
#include "datafileitem.h"

/*****************************************************************************
 *
 * Class MenuChangeImageType
 *
 *****************************************************************************/

MenuChangeImageType::MenuChangeImageType ()
{
}

MenuChangeImageType::~MenuChangeImageType ()
{
}

QString MenuChangeImageType::contextMenuText ()
{
	return tr("Change Type");
}

bool MenuChangeImageType::contextMenuTypeSupported (int type)
{
	return type == DAT_BITMAP || type == DAT_RLE_SPRITE;
}

void MenuChangeImageType::toBitmap ()
{
	DATAFILE *dat = item->data();

	if ( dat->type != DAT_RLE_SPRITE )
	{
		qWarning ("Oops. Conversion from unknown image type.");
		return;
	}

	RLE_SPRITE *spr = (RLE_SPRITE *)dat->dat;
	BITMAP *bmp = create_bitmap_ex(spr->color_depth, spr->w, spr->h);
	clear_to_color(bmp, bmp->vtable->mask_color);
	draw_rle_sprite(bmp, spr, 0, 0);
	dat->dat = bmp;
	destroy_rle_sprite(spr);

	dat->type = DAT_BITMAP;
	emit dirty();
}

void MenuChangeImageType::toRLESprite ()
{
	DATAFILE *dat = item->data();

	if ( dat->type != DAT_BITMAP )
	{
		qWarning ("Oops. Conversion from unknown image type.");
		return;
	}

	BITMAP *bmp = (BITMAP *)dat->dat;
	RLE_SPRITE *spr = get_rle_sprite(bmp);
	dat->dat = spr;
	destroy_bitmap(bmp);

	dat->type = DAT_RLE_SPRITE;
	emit dirty();
}

QPopupMenu *MenuChangeImageType::contextMenuPopup (DatafileItem *it)
{
	item = it;
	QPopupMenu *pop = new QPopupMenu;

	DATAFILE *dat = item->data();

	pop->insertItem (tr("To Bitmap"), this, SLOT(toBitmap()), 0, 1);
	if ( dat->type == DAT_BITMAP )
		pop->setItemEnabled (1, false);
	pop->insertItem (tr("To RLE sprite"), this, SLOT(toRLESprite()), 0, 2);
	if ( dat->type == DAT_RLE_SPRITE )
		pop->setItemEnabled (2, false);

	return pop;
}

/*****************************************************************************
 *
 * Class MenuChangeImageDepth
 *
 *****************************************************************************/

MenuChangeImageDepth::MenuChangeImageDepth ()
{
}

MenuChangeImageDepth::~MenuChangeImageDepth ()
{
}

QString MenuChangeImageDepth::contextMenuText ()
{
	return tr("Change Depth");
}

bool MenuChangeImageDepth::contextMenuTypeSupported (int type)
{
	return type == DAT_BITMAP || type == DAT_RLE_SPRITE;
}

QPopupMenu *MenuChangeImageDepth::contextMenuPopup (DatafileItem *it)
{
	item = it;
	QPopupMenu *pop = new QPopupMenu ();

	DATAFILE *dat = item->data();
	int depth = dat->type == DAT_RLE_SPRITE ?	((RLE_SPRITE *)dat->dat)->color_depth :
							bitmap_color_depth ((BITMAP *)dat->dat);

	pop->insertItem (tr("256 color palette"), this, SLOT(to8()), 0, 1);
	if ( depth == 8 )
		pop->setItemEnabled (1, false);
	pop->insertItem (tr("15 bit hicolor"), this, SLOT(to15()), 0, 2);
	if ( depth == 15 )
		pop->setItemEnabled (2, false);
	pop->insertItem (tr("16 bit hicolor"), this, SLOT(to16()), 0, 3);
	if ( depth == 16 )
		pop->setItemEnabled (3, false);
	pop->insertItem (tr("24 bit truecolor"), this, SLOT(to24()), 0, 4);
	if ( depth == 24 )
		pop->setItemEnabled (4, false);
	pop->insertItem (tr("32 bit hicolor"), this, SLOT(to32()), 0, 5);
	if ( depth == 32 )
		pop->setItemEnabled (5, false);

	return pop;
}

void MenuChangeImageDepth::convert (int depth)
{
	DATAFILE   *dat = item->data();
	BITMAP     *bmp, *bmp2;
	RLE_SPRITE *spr;
	RGB tmprgb = datedit_current_palette[0];

	if (dat->type == DAT_RLE_SPRITE)
	{
		spr = (RLE_SPRITE *)dat->dat;
		if (spr->color_depth == depth)
			return;

		bmp = create_bitmap_ex(spr->color_depth, spr->w, spr->h);
		clear_to_color(bmp, bmp->vtable->mask_color);
		draw_rle_sprite(bmp, spr, 0, 0);
		bmp2 = create_bitmap_ex(depth, bmp->w, bmp->h);

		datedit_current_palette[0].r = 63;
		datedit_current_palette[0].g = 0;
		datedit_current_palette[0].b = 63;
		select_palette(datedit_current_palette);

		blit(bmp, bmp2, 0, 0, 0, 0, bmp->w, bmp->h);

		unselect_palette();
		datedit_current_palette[0] = tmprgb;

		dat->dat = get_rle_sprite(bmp2);
		destroy_bitmap(bmp);
		destroy_bitmap(bmp2);
		destroy_rle_sprite(spr);
	}
	else
	{
		bmp = (BITMAP *)dat->dat;
		if (bitmap_color_depth(bmp) == depth)
			return;

		bmp2 = create_bitmap_ex(depth, bmp->w, bmp->h);

		if ((dat->type == DAT_C_SPRITE) || (dat->type == DAT_XC_SPRITE))
		{
			datedit_current_palette[0].r = 63;
			datedit_current_palette[0].g = 0;
			datedit_current_palette[0].b = 63;
		}
		select_palette(datedit_current_palette);

		blit(bmp, bmp2, 0, 0, 0, 0, bmp->w, bmp->h);

		unselect_palette();
		datedit_current_palette[0] = tmprgb;

		dat->dat = bmp2;
		destroy_bitmap(bmp);
	}

	emit dirty();
}

void MenuChangeImageDepth::to8 ()
{
	convert (8);
}

void MenuChangeImageDepth::to15 ()
{
	convert (15);
}

void MenuChangeImageDepth::to16 ()
{
	convert (16);
}

void MenuChangeImageDepth::to24 ()
{
	convert (24);
}

void MenuChangeImageDepth::to32 ()
{
	convert (32);
}

/*****************************************************************************
 *
 * Class ITypePlugin
 *
 *****************************************************************************/

class ITypePlugin : public QGrabberPlugin
{
public:
	ITypePlugin() {}
	~ITypePlugin() {}

	QStringList keys() const
	{
		QStringList list;

		list << "IType";

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

		list << "Change Image Type";
		list << "Change Image Depth";

		return list;
	}

	MenuPlugin *createMenu(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "change image type")
			return new MenuChangeImageType;
		else
		if (plug == "change image depth")
			return new MenuChangeImageDepth;


		return 0;
	}
};

EXPORT_PLUGIN( ITypePlugin )
