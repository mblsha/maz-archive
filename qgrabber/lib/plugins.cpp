/***************************************************************************
                          plugins.cpp  -  description
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

#include "alleg.h"
#include <allegro/internal/aintern.h>

#include <qapp.h>

#include <qimage.h>
#include <qcolor.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>

#include "factory.h"
#include "plugins.h"
#include "datafileitem.h"

#include "prefs.h"

/*****************************************************************************
 *
 * Class ObjectPlugin
 *
 *****************************************************************************/

long ObjectPlugin::fileDatasize = 0;

ObjectPlugin::ObjectPlugin ()
{
}

ObjectPlugin::~ObjectPlugin ()
{
}

QPixmap ObjectPlugin::pixmap ()
{
	return QPixmap();
}

void ObjectPlugin::saveObject (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f)
{
	// save binary data
	pack_fwrite ( item->data()->dat, item->data()->size, f );
}

bool ObjectPlugin::shouldSaveProp (long type)
{
	bool ret;
	if ( Prefs::prefs()->strip() == 0 )
	{
		ret = true;
	}
	else if ( Prefs::prefs()->strip() >= 2 )
	{
		ret = false;
	}
	else
	{
		ret = ((type != DAT_ORIG) &&
		       (type != DAT_DATE) &&
		       (type != DAT_XPOS) &&
		       (type != DAT_YPOS) &&
		       (type != DAT_XSIZ) &&
		       (type != DAT_YSIZ) &&
		       (type != DAT_XCRP) &&
		       (type != DAT_YCRP));
	}

	if ( !ret )
	{
		//
		// Check the exception list
		//
		char *tok;
		char tmp[256];
		strcpy (tmp, Prefs::prefs()->stripExceptions());
		tok = strtok(tmp, ";");
		while (tok)
		{
			if ( datedit_clean_typename (tok) == type )
			{
				ret = true;
				break;
			}
			tok = strtok(NULL, ";");
		}
	}

	return ret;
}

void ObjectPlugin::save (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f)
{
	DATAFILE *dat = item->data();
	DATAFILE_PROPERTY *prop;

	prop = dat->prop;
	datedit_sort_properties (prop);

	while ((prop) && (prop->type != DAT_END))
	{
		if ( shouldSaveProp(prop->type) )
		{
			pack_mputl (DAT_PROPERTY, f);
			pack_mputl (prop->type, f);
			pack_mputl (strlen(prop->dat), f);
			pack_fwrite (prop->dat, strlen(prop->dat), f);
			fileDatasize += 12 + strlen (prop->dat);
		}

		prop++;
		if (errno)
		{
			qWarning ("ObjectPlugin::save(): error saving properties");
			return;
		}
	}

	emit savingItem (item->name());
	qApp->processEvents();

	pack_mputl (dat->type, f);
	f = pack_fopen_chunk (f, ((!packed) && (packkids) && (dat->type != DAT_FILE)));
	fileDatasize += 12;

	ObjectPlugin *p = ObjectFactory::create (dat->type);
	if (dat->type == DAT_FILE)
		p->saveObject (item, packed, packkids, strip, verbose, FALSE, f);
	else
		p->saveObject (item, (packed || packkids), FALSE, strip, verbose, FALSE, f);

	pack_fclose_chunk (f);

	if (dat->type == DAT_FILE)
		fileDatasize += 4;
	else
		fileDatasize += _packfile_datasize;

	emit savedItem (_packfile_datasize, _packfile_filesize);
	qApp->processEvents();
}

bool ObjectPlugin::doubleClick (DATAFILE *dat)
{
	return false; // no update is needed, because data wasn't changed
}

/*****************************************************************************
 *
 * Class IOPlugin
 *
 *****************************************************************************/

IOPlugin::IOPlugin ()
{
}

IOPlugin::~IOPlugin ()
{
}

bool IOPlugin::typeSupported (int type)
{
	return false;
}

QString IOPlugin::grabType ()
{
	return "";
}

QString IOPlugin::exportType ()
{
	return "";
}

// grab binary data
void *IOPlugin::grabFile (const char *filename, long *size)
{
	return NULL;
}

// export binary data
int IOPlugin::exportFile (const char *filename, DatafileItem *item)
{
	PACKFILE *f = pack_fopen( (char *)filename, F_WRITE);

	if (f)
	{
		pack_fwrite (item->data()->dat, item->data()->size, f);
		pack_fclose (f);
	}

	return (errno == 0);
}

DATAFILE *IOPlugin::grabHelper (const char *filename, const char *name, int type)
{
	DATAFILE *dat = (DATAFILE *)malloc(sizeof(DATAFILE));
	dat->dat  = 0;
	dat->size = 0;
	dat->prop = 0;
	dat->type = type;

	dat->dat = grabFile (filename, &dat->size);

	if (dat->dat)
	{
		datedit_set_property (dat, DAT_NAME, (AL_CONST char *)name );
		datedit_set_property (dat, DAT_ORIG, (AL_CONST char *)filename);
		datedit_set_property (dat, DAT_DATE, datedit_ftime2asc(file_time( (AL_CONST char *)filename)));

		errno = 0;
	}
	else
	{
		char buf[16];
		sprintf (buf, "%c%c%c%c", type >> 24, type >> 16, type >> 8, type);

		QMessageBox::warning ( 0, tr("Error!"), tr("Error reading '%1' as type '%2'").arg(filename)
		                                                                             .arg(buf) );
	}

	return dat;
}

bool IOPlugin::grab (const char *filename, DatafileItem *item)
{
	DATAFILE *tmp = grabHelper (filename, get_datafile_property (item->data(), DAT_NAME), item->type());

	if ( tmp->dat )
	{
		item->destroyData ();
		item->setData ( tmp );

		return true;
	}
	else
	{
		_unload_datafile_object ( tmp );
		free ( tmp );

		return false;
	}
}

DATAFILE *IOPlugin::grab (const char *filename, const char *name, int type, int x, int y, int w, int h, int colorDepth)
{
	DATAFILE *dat = (DATAFILE *) malloc ( sizeof (DATAFILE) );
	dat->type = type;
	dat->size = 0;
	dat->prop = 0;

	IOPlugin *plug = IOFactory::create (type);
	dat->dat = plug->grabHelper (filename, name, type);

	if ( dat->dat && x != -1 && y != -1 && w != -1 && h != -1 )
	{	// TODO
		qWarning ("Whoops! Tile grab isn't supported yet. Sorry.");
	}

	if ( dat->dat )
	{
		datedit_set_property ( dat, DAT_NAME, (AL_CONST char *)name );
		datedit_set_property ( dat, DAT_ORIG, (AL_CONST char *)filename );
		datedit_set_property ( dat, DAT_DATE, datedit_ftime2asc( file_time((AL_CONST char *)filename) ) );
	}

	return dat;
}

/*****************************************************************************
 *
 * Class MenuPlugin
 *
 *****************************************************************************/

MenuPlugin::MenuPlugin ()
{
}

MenuPlugin::~MenuPlugin ()
{
}

QAction *MenuPlugin::menuAction ()
{
	return 0;
}

int MenuPlugin::menuAddTo ()
{
	return Plugins;
}

bool MenuPlugin::contextMenuTypeSupported (int type)
{
	return false;
}

QString MenuPlugin::contextMenuText ()
{
	return QString();
}

void MenuPlugin::contextMenuActivated (DatafileItem *item)
{
	return;
}

QPopupMenu *MenuPlugin::contextMenuPopup (DatafileItem *item)
{
	return 0;
}

/*****************************************************************************
 *
 * Helper functions
 *
 *****************************************************************************/

DATAFILE *copyDatafile (DATAFILE *data)
{
	DATAFILE *dat = (DATAFILE *) malloc ( sizeof (DATAFILE) );
	dat->type = data->type;
	dat->size = data->size;
	dat->prop = NULL;

	ObjectPlugin *plug = ObjectFactory::create ( dat->type );
	dat->dat = plug->copyData ( data, &dat->size );

	if ( !dat->dat )
		qWarning ("copyDatafile: NULL data detected!");

	DATAFILE_PROPERTY *prop = data->prop;
	if (prop)
	{
		while (prop->dat)
		{
			datedit_set_property ( dat, prop->type, prop->dat );
			prop++;
		}
	}

	return dat;
}

void initPlugins ()
{
	selPalette ( default_palette );
}

void selPalette (RGB *pal)
{
	memcpy (datedit_current_palette, pal, sizeof(PALETTE));
	set_palette (datedit_current_palette);
	Prefs::setPalette ( datedit_current_palette );
}

QImage *bmp2img (BITMAP *bmp, QColor *trans)
{
	int depth = bmp->vtable->color_depth;
	QImage *img = new QImage (bmp->w, bmp->h, (depth > 8) ? 32 : 8, 256);

	if (depth == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			img->setColor ( i, qRgb(datedit_current_palette[i].r * 4,
			                        datedit_current_palette[i].g * 4,
			                        datedit_current_palette[i].b * 4) );
		}
	}

	for (int y = 0; y < bmp->h; y++)
	{
		for (int x = 0; x < bmp->w; x++)
		{
			int col = getpixel (bmp, x, y);

			if (trans && col == bmp->vtable->mask_color)
			{
				if (depth == 8)
				{
#ifdef DEBUG
					qWarning ("bmp2img: Currently trans color in 8bpp mode is not supported");
#endif
				}
				else
				{
					uint *p = (uint *)img->scanLine(y) + x;
					*p = trans->rgb();
				}
				continue;
			}

			switch (depth)
			{
				case 8:
					*(img->scanLine(y) + x) = col;
					break;

				case 15:
				case 16:
				case 24:
				case 32:
					int r, g, b;

					switch (depth)
					{
						case 15:
							r = getr15 (col);
							g = getg15 (col);
							b = getb15 (col);
							break;
						case 16:
							r = getr16 (col);
							g = getg16 (col);
							b = getb16 (col);
							break;
						case 24:
							r = getr24 (col);
							g = getg24 (col);
							b = getb24 (col);
							break;
						case 32:
							r = getr32 (col);
							g = getg32 (col);
							b = getb32 (col);
							break;
					}

					uint *p = (uint *)img->scanLine(y) + x;
					*p = qRgb (r, g, b);
					break;
			}
		}
	}

	return img;
}

QImage *rle2img (RLE_SPRITE *spr)
{
	BITMAP *bmp = create_bitmap_ex (spr->w, spr->h, spr->color_depth);
	draw_rle_sprite (bmp, spr, 0, 0);

	QImage *img = bmp2img (bmp);
	free (bmp);

	return img;
}
/*
#ifdef ALLEGRO_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpCmdLine, int nCmdShow)
{
	return 0;
}
#endif
*/

/*****************************************************************************
 *
 * Dummy functions for Allegro DatEdit
 *
 *****************************************************************************/

void datedit_msg(AL_CONST char *fmt, ...)      {}
void datedit_startmsg(AL_CONST char *fmt, ...) {}
void datedit_endmsg(AL_CONST char *fmt, ...)   {}
void datedit_error(AL_CONST char *fmt, ...)    {}
int datedit_ask(AL_CONST char *fmt, ...)       { return 'y'; }
