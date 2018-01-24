/***************************************************************************
                          font.cpp  -  description
                             -------------------
    begin                : Sat Aug 24 2002
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

#include <qpixmap.h>
#include <qlayout.h>
#include <qimage.h>
#include <qpainter.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qmap.h>
#include <qfiledialog.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <qapplication.h>

#include "font.h"
#include "fontplot.h"
#include "chardialog.h"

#include <allegro/internal/aintern.h>

static QPixmap *fontData = 0;

#include "common.h"
#include "plugins_p.h"
#include "datafileitem.h"


static QString int2hex (int num)
{
	QString s;
	s.sprintf ("%04X", num);
	return s;
}

/*****************************************************************************
 *
 * from datfont.c
 *
 *****************************************************************************/

/* creates a new font object */
static void* makenew_font(long* size)
{
    FONT *f;
    FONT_MONO_DATA *mf = 0, *mfread = (FONT_MONO_DATA *)font->data;

    f = (FONT *)malloc(sizeof(FONT));

    f->height = font->height;
    f->vtable = font->vtable;

    while(mfread) {
        int i;

        if(mf) {
            mf->next = (FONT_MONO_DATA *)malloc(sizeof(FONT_MONO_DATA));
            mf = mf->next;
        } else f->data = mf = (FONT_MONO_DATA *)malloc(sizeof(FONT_MONO_DATA));

        mf->begin = mfread->begin;
        mf->end = mfread->end;
        mf->next = 0;
        mf->glyphs = (FONT_GLYPH **)malloc(sizeof(FONT_GLYPH*) * (mf->end - mf->begin));

        for(i = mf->begin; i < mf->end; i++) {
            FONT_GLYPH *gsrc = mfread->glyphs[i - mf->begin], *gdest;
            int sz = ((gsrc->w + 7) / 8) * gsrc->h;

            gdest = (FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + sz);
            gdest->w = gsrc->w;
            gdest->h = gsrc->h;
            memcpy(gdest->dat, gsrc->dat, sz);

            mf->glyphs[i - mf->begin] = gdest;
        }

        mfread = mfread->next;
    }

    return f;
}

/* magic number for GRX format font files */
#define FONTMAGIC    0x19590214L

/* import routine for the 8x8 and 8x16 BIOS font formats */
static FONT* import_bios_font(AL_CONST char *filename)
{
    PACKFILE *pack;
    FONT *f = 0;
    FONT_MONO_DATA *mf = 0;
    FONT_GLYPH **gl = 0;
    int i, h;

    f = (FONT *)malloc(sizeof(FONT));
    mf = (FONT_MONO_DATA *)malloc(sizeof(FONT_MONO_DATA));
    gl = (FONT_GLYPH **)malloc(sizeof(FONT_GLYPH*) * 256);

    pack = pack_fopen(filename, F_READ);
    if(!pack) return 0;

    h = (pack->todo == 2048) ? 8 : 16;

    for(i = 0; i < 256; i++) {
        gl[i] = (FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + 8);
        gl[i]->w = gl[i]->h = 8;
        pack_fread(gl[i]->dat, 8, pack);
    }

    f->vtable = font_vtable_mono;
    f->data = mf;
    f->height = h;

    mf->begin = 0;
    mf->end = 256;
    mf->glyphs = gl;
    mf->next = 0;

    pack_fclose(pack);

    return f;
}

/* state information for the bitmap font importer */
static BITMAP *import_bmp = NULL;

static int import_x = 0;
static int import_y = 0;

/* import_bitmap_font_mono:
 *  Helper for import_bitmap_font, below.
 */
static int import_bitmap_font_mono(FONT_GLYPH** gl, int num)
{
    int w = 1, h = 1, i;

    for(i = 0; i < num; i++) {
        if(w > 0 && h > 0) datedit_find_character(import_bmp, &import_x, &import_y, &w, &h);
        if(w <= 0 || h <= 0) {
            int j;

            gl[i] = (FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + 8);
            gl[i]->w = 8;
            gl[i]->h = 8;

            for(j = 0; j < 8; j++) gl[i]->dat[j] = 0;
        } else {
            int sx = ((w + 7) / 8), j, k;

            gl[i] = (FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + sx * h);
            gl[i]->w = w;
            gl[i]->h = h;

            for(j = 0; j < sx * h; j++) gl[i]->dat[j] = 0;
            for(j = 0; j < h; j++) {
                for(k = 0; k < w; k++) {
                    if(getpixel(import_bmp, import_x + k + 1, import_y + j + 1))
                        gl[i]->dat[(j * sx) + (k / 8)] |= 0x80 >> (k & 7);
                }
            }

            import_x += w;
        }
    }

    return 0;
}

/* import_bitmap_font_color:
 *  Helper for import_bitmap_font, below.
 */
static int import_bitmap_font_color(BITMAP** bits, int num)
{
    int w = 1, h = 1, i;

    for(i = 0; i < num; i++) {
        if(w > 0 && h > 0) datedit_find_character(import_bmp, &import_x, &import_y, &w, &h);
        if(w <= 0 || h <= 0) {
            bits[i] = create_bitmap_ex(8, 8, 8);
            if(!bits[i]) return -1;
            clear_to_color(bits[i], 255);
        } else {
            bits[i] = create_bitmap_ex(8, w, h);
            if(!bits[i]) return -1;
            blit(import_bmp, bits[i], import_x + 1, import_y + 1, 0, 0, w, h);
            import_x += w;
        }
    }

    return 0;
}

/* bitmap_font_ismono:
 *  Helper for import_bitmap_font, below.
 */
static int bitmap_font_ismono(BITMAP *bmp)
{
    int x, y, col = -1, pixel;

    for(y = 0; y < bmp->h; y++) {
        for(x = 0; x < bmp->w; x++) {
            pixel = getpixel(bmp, x, y);
            if(pixel == 0 || pixel == 255) continue;
            if(col > 0 && pixel != col) return 0;
            col = pixel;
        }
    }

    return 1;
}

/* upgrade_to_color, upgrade_to_color_data:
 *  Helper functions. Upgrades a monochrome font to a color font.
 */
static FONT_COLOR_DATA* upgrade_to_color_data(FONT_MONO_DATA* mf)
{
    FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)malloc(sizeof(FONT_COLOR_DATA));
    BITMAP** bits = (BITMAP **)malloc(sizeof(BITMAP*) * (mf->end - mf->begin));
    int i;

    cf->begin = mf->begin;
    cf->end = mf->end;
    cf->bitmaps = bits;
    cf->next = 0;

    text_mode(-1);

    for(i = mf->begin; i < mf->end; i++) {
        FONT_GLYPH* g = mf->glyphs[i - mf->begin];
        BITMAP* b = create_bitmap_ex(8, g->w, g->h);
        clear_to_color(b, 0);
        b->vtable->draw_glyph(b, g, 0, 0, 1);

        bits[i - mf->begin] = b;
        free(g);
    }

    free(mf->glyphs);
    free(mf);

    return cf;
}

static void upgrade_to_color(FONT* f)
{
    FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;
    FONT_COLOR_DATA * cf, *cf_write = 0;

    if(f->vtable == font_vtable_color) return;
    f->vtable = font_vtable_color;

    while(mf) {
        FONT_MONO_DATA* mf_next = mf->next;

        cf = upgrade_to_color_data(mf);
        if(!cf_write) f->data = cf;
        else cf_write->next = cf;

        cf_write = cf;
        mf = mf_next;
    }
}

/* bitmap_font_count:
 *  Helper for `import_bitmap_font', below.
 */
static int bitmap_font_count(BITMAP* bmp)
{
    int x = 0, y = 0, w = 0, h = 0;
    int num = 0;

    while(1) {
        datedit_find_character(bmp, &x, &y, &w, &h);
        if (w <= 0 || h <= 0) break;
        num++;
        x += w;
    }

    return num;
}

/* import routine for the Allegro .pcx font format */
static FONT* import_bitmap_font(AL_CONST char* fname, int begin, int end, int cleanup)
{
    /* NB: `end' is -1 if we want every glyph */
    FONT *f;

    if(fname) {
        PALETTE junk;

        if(import_bmp) destroy_bitmap(import_bmp);
        import_bmp = load_bitmap(fname, junk);

        import_x = 0;
        import_y = 0;
    }

    if(!import_bmp) return 0;

    if(bitmap_color_depth(import_bmp) != 8) {
        destroy_bitmap(import_bmp);
        import_bmp = 0;
        return 0;
    }

    f = (FONT *)malloc(sizeof(FONT));
    if(end == -1) end = bitmap_font_count(import_bmp) + begin;

    if (bitmap_font_ismono(import_bmp)) {

        FONT_MONO_DATA* mf = (FONT_MONO_DATA *)malloc(sizeof(FONT_MONO_DATA));

        mf->glyphs = (FONT_GLYPH **)malloc(sizeof(FONT_GLYPH*) * (end - begin));

        if( import_bitmap_font_mono(mf->glyphs, end - begin) ) {

            free(mf->glyphs);
            free(mf);
            free(f);
            f = 0;

        } else {

            f->data = mf;
            f->vtable = font_vtable_mono;
            f->height = mf->glyphs[0]->h;

            mf->begin = begin;
            mf->end = end;
            mf->next = 0;
        }

    } else {

        FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)malloc(sizeof(FONT_COLOR_DATA));
        cf->bitmaps = (BITMAP **)malloc(sizeof(BITMAP*) * (end - begin));

        if( import_bitmap_font_color(cf->bitmaps, end - begin) ) {

            free(cf->bitmaps);
            free(cf);
            free(f);
            f = 0;

        } else {

            f->data = cf;
            f->vtable = font_vtable_color;
            f->height = cf->bitmaps[0]->h;

            cf->begin = begin;
            cf->end = end;
            cf->next = 0;

        }

    }

    if(cleanup) {
        destroy_bitmap(import_bmp);
        import_bmp = 0;
    }

    return f;
}

/* helper for save_font, below */
static void save_mono_font(FONT* f, PACKFILE* pack)
{
    FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;
    int i = 0;

    /* count number of ranges */
    while(mf) {
        i++;
        mf = mf->next;
    }
    pack_mputw(i, pack);

    mf = (FONT_MONO_DATA *)f->data;

    while(mf) {

        /* mono, begin, end-1 */
        pack_putc(1, pack);
        pack_mputl(mf->begin, pack);
        pack_mputl(mf->end - 1, pack);

        for(i = mf->begin; i < mf->end; i++) {
            FONT_GLYPH* g = mf->glyphs[i - mf->begin];

            pack_mputw(g->w, pack);
            pack_mputw(g->h, pack);

            pack_fwrite(g->dat, ((g->w + 7) / 8) * g->h, pack);
        }

        mf = mf->next;

    }
}

/* helper for save_font, below */
static void save_color_font(FONT* f, PACKFILE* pack)
{
    FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)f->data;
    int i = 0;

    /* count number of ranges */
    while(cf) {
        i++;
        cf = cf->next;
    }
    pack_mputw(i, pack);

    cf = (FONT_COLOR_DATA *)f->data;

    while(cf) {

        /* mono, begin, end-1 */
        pack_putc(0, pack);
        pack_mputl(cf->begin, pack);
        pack_mputl(cf->end - 1, pack);

        for(i = cf->begin; i < cf->end; i++) {
            BITMAP* g = cf->bitmaps[i - cf->begin];
            int y;

            pack_mputw(g->w, pack);
            pack_mputw(g->h, pack);

            for(y = 0; y < g->h; y++) {
                pack_fwrite(g->line[y], g->w, pack);
            }

        }

        cf = cf->next;

    }
}

/* saves a font into a datafile */
static void save_font(DATAFILE* dat, int packed, int packkids, int strip, int verbose, int extra, PACKFILE* pack)
{
    FONT* f = (FONT *)dat->dat;

    pack_mputw(0, pack);

    if(f->vtable == font_vtable_mono) save_mono_font(f, pack);
    else save_color_font(f, pack);
}

/*****************************************************************************
 *
 * from datfont.c, but slightly modified
 *
 *****************************************************************************/

#define tr(x)	qApp->translate("FontPlugin", x)

// import routine for the GRX font format
static FONT *importGrxFont(const char *filename)
{
	PACKFILE *pack;
	FONT *f;
	FONT_MONO_DATA *mf;
	FONT_GLYPH **gl;
	int w, h, num, i;
	int* wtab = 0;

	pack = pack_fopen((AL_CONST char *)filename, F_READ);
	if(!pack) return 0;

	if(pack_igetl(pack) != FONTMAGIC) {
		pack_fclose(pack);
		return 0;
	}
	pack_igetl(pack);

	f = (FONT *)malloc(sizeof(FONT));
	mf = (FONT_MONO_DATA *)malloc(sizeof(FONT_MONO_DATA));

	f->data = mf;
	f->vtable = font_vtable_mono;
	mf->next = 0;

	w = pack_igetw(pack);
	h = pack_igetw(pack);

	f->height = h;

	mf->begin = pack_igetw(pack);
	mf->end = pack_igetw(pack) + 1;
	num = mf->end - mf->begin;

	gl = mf->glyphs = (FONT_GLYPH **)malloc(sizeof(FONT_GLYPH*) * num);

	if(pack_igetw(pack) == 0) {
		for(i = 0; i < 38; i++) pack_getc(pack);
		wtab = (int *)malloc(sizeof(int) * num);
		for(i = 0; i < num; i++) wtab[i] = pack_igetw(pack);
	} else {
		for(i = 0; i < 38; i++) pack_getc(pack);
	}

	for(i = 0; i < num; i++) {
		int sz;

		if(wtab) w = wtab[i];

		sz = ((w + 7) / 8) * h;
		gl[i] = (FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + sz);
		gl[i]->w = w;
		gl[i]->h = h;

		pack_fread(gl[i]->dat, sz, pack);
	}

	if(!pack_feof(pack)) {
		char cw_char[1024];

		strcpy(cw_char, filename);
		strcpy(get_extension(cw_char), "txt");
		i = QMessageBox::information (0, tr("Information"), tr("Save font copyright message into '%1'?").arg(QFile::decodeName(cw_char)), QMessageBox::Yes, QMessageBox::No);
		if(i != QMessageBox::No) {
			PACKFILE* pout = pack_fopen(cw_char, F_WRITE);
			if(pout) {
				while(!pack_feof(pack)) {
					i = pack_fread(cw_char, 1024, pack);
					pack_fwrite(cw_char, i, pout);
				}
			}
			pack_fclose(pout);
		}
	}

	pack_fclose(pack);
	if(wtab) free(wtab);

	return f;
}


// import routine for the multiple range .txt font format
static FONT *importScriptedFont(const char *filename)
{
	char buf[1024], *bmp_str, *start_str = 0, *end_str = 0;
	FONT *f, *f2;
	PACKFILE *pack;
	int begin, end;

	pack = pack_fopen((AL_CONST char *)filename, F_READ);
	if(!pack) return 0;

	f = (FONT *)malloc(sizeof(FONT));
	f->data = NULL;
	f->height = 0;
	f->vtable = NULL;

	while(pack_fgets(buf, sizeof(buf)-1, pack)) {
		bmp_str = strtok(buf, " \t");
		if(bmp_str) start_str = strtok(0, " \t");
		if(start_str) end_str = strtok(0, " \t");

		if(!bmp_str || !start_str || !end_str) {
			QMessageBox::warning (0, tr("Error"), tr("Bad font description (expecting 'file.pcx start end')"), QMessageBox::Ok, QMessageBox::NoButton);

			_al_free(f);
			pack_fclose(pack);

			return 0;
		}

		if(bmp_str[0] == '-') bmp_str = 0;
		begin = strtol(start_str, 0, 0);
		if(end_str) end = strtol(end_str, 0, 0) + 1;
		else end = -1;

		if(begin <= 0 || (end > 0 && end < begin)) {
			QMessageBox::warning (0, tr("Error"), tr("Bad font description (expecting 'file.pcx start end');\nstart > 0, end > start"), QMessageBox::Ok, QMessageBox::NoButton);

			_al_free(f);
			pack_fclose(pack);

			return 0;
		}

		f2 = import_bitmap_font(bmp_str, begin, end, FALSE);
		if(!f2) {
			if(bmp_str)
				QMessageBox::warning (0, tr("Error"), tr("Unable to read font images from %1").arg(QFile::decodeName(bmp_str)), QMessageBox::Ok, QMessageBox::NoButton);
			else
				QMessageBox::warning (0, tr("Error"), tr("Unable to read continuation font images"), QMessageBox::Ok, QMessageBox::NoButton);

			_al_free(f);
			pack_fclose(pack);

			return 0;
		}

		if(!f->vtable) f->vtable = f2->vtable;
		if(!f->height) f->height = f2->height;

		if(f2->vtable != f->vtable) {
			upgrade_to_color(f);
			upgrade_to_color(f2);
		}

		/* add to end of linked list */

		if(f->vtable == font_vtable_mono) {
			FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;
			if(!mf) f->data = f2->data;
			else {
				while(mf->next) mf = mf->next;
				mf->next = (FONT_MONO_DATA *)f2->data;
			}
			free(f2);
		} else {
			FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)f->data;
			if(!cf) f->data = f2->data;
			else {
				while(cf->next) cf = cf->next;
				cf->next = (FONT_COLOR_DATA *)f2->data;
			}
			free(f2);
		}
	}

	destroy_bitmap(import_bmp);
	import_bmp = 0;

	pack_fclose(pack);
	return f;
}

// imports a font from an external file (handles various formats)
static void *grabFont(const char *filename, long *size, int x, int y, int w, int h, int depth)
{
	PACKFILE *f;
	int id;

	if (stricmp(get_extension((AL_CONST char *)filename), "fnt") == 0) {
		f = pack_fopen((AL_CONST char *)filename, F_READ);
		if (!f)
			return NULL;

		id = pack_igetl(f);
		pack_fclose(f);

		if (id == FONTMAGIC)
			return importGrxFont(filename);
		else
			return import_bios_font((AL_CONST char *)filename);
	}
	else if (stricmp(get_extension((AL_CONST char *)filename), "txt") == 0) {
		return importScriptedFont(filename);
	}
	else {
		return import_bitmap_font((AL_CONST char *)filename, ' ', -1, TRUE);
	}
}

#undef tr

/*****************************************************************************
 *
 * Class DatafileItemFont
 *
 *****************************************************************************/

class DatafileItemFont : public DatafileItem
{
public:
	DatafileItemFont (QListViewItem *parent, DATAFILE *data)
				: DatafileItem (parent, data)
	{
		setPixmap ( fontData );
	}

	~DatafileItemFont () {}
};

/*****************************************************************************
 *
 * Class FontView
 *
 *****************************************************************************/

FontView::FontView (QWidget *parent, const char *name)
			: QWidget (parent, name)
{
	d = 0;
}

void FontView::setData (DATAFILE *dat)
{
	d = dat;
}

void FontView::paintRange (QPainter *painter, int range, int *x, int *y, int *maxh)
{
	FONT *fnt = (FONT *)d->dat;
	int begin, end;
	BITMAP** bits = 0;
	FONT_GLYPH** gl = 0;
	int i = range;

	if (fnt->vtable == font_vtable_mono)
	{
		FONT_MONO_DATA *mf = (FONT_MONO_DATA *)fnt->data;
		while(i--)
			mf = mf->next;

		gl = mf->glyphs;
		begin = mf->begin;
		end = mf->end;
	}
	else
	{
		FONT_COLOR_DATA *cf = (FONT_COLOR_DATA *)fnt->data;
		while(i--)
			cf = cf->next;

		bits = cf->bitmaps;
		begin = cf->begin;
		end = cf->end;
	}

	QColor trans = paletteBackgroundColor();

	for (i = begin; i < end; i++) {
		int w, h;

		if (gl) {
			w = gl[i - begin]->w;
			h = gl[i - begin]->h;
		}
		else {
			w = bits[i - begin]->w;
			h = bits[i - begin]->h;
		}

		if (h > *maxh)
			*maxh = h;

		if (w + *x + 4 > QWidget::width()) {
			*x = 0;
			*y += *maxh + 4;
		}

		if (*y > QWidget::height())
		{
			*maxh = 0;
			break;
		}

		// create QImage with glyph
		BITMAP *bmp = create_bitmap_ex (16, w, h);
		clear_to_color (bmp, bmp->vtable->mask_color);
		textprintf (bmp, fnt, 0, 0, makecol_depth(16, 0,0,0), "%c", i);
		QImage *img = bmp2img (bmp, &trans);
		destroy_bitmap (bmp);

		painter->drawImage (*x, *y, *img);
		delete img;

		*x += w + 4;
	}
}

void FontView::paintEvent( QPaintEvent *e )
{
	if ( !d )
	{
		qWarning ("FontView::paintEvent(): no data!");
		return;
	}

	QPainter p(this);
	p.setClipRegion(e->region());

	FONT *fnt = (FONT *)d->dat;
	int ranges = 0;
	if(fnt->vtable == font_vtable_mono) {
		FONT_MONO_DATA *mf = (FONT_MONO_DATA *)fnt->data;

		while(mf) {
			ranges++;
			mf = mf->next;
		}
	} else {
		FONT_COLOR_DATA *cf = (FONT_COLOR_DATA *)fnt->data;

		while(cf) {
			ranges++;
			cf = cf->next;
		}
	}

	text_mode (-1);

	int x = 0, y = 0, maxh = 0;
	for (int i = 0; i < ranges; i++)
		paintRange (&p, i, &x, &y, &maxh);
}

/*****************************************************************************
 *
 * Class FontRangeSelect
 *
 *****************************************************************************/

FontRangeSelect::FontRangeSelect (QWidget *parent, const char *name)
			: QListBox (parent, name)
{
	d = 0;
	range = 0;

	connect (this, SIGNAL(selectionChanged(QListBoxItem*)), this, SLOT(changedSelection(QListBoxItem*)));
	connect (this, SIGNAL(update(int)), this, SLOT(updateList(int)));
}

FontRangeSelect::~FontRangeSelect ()
{
}

void FontRangeSelect::setData (DATAFILE *dat)
{
	d = dat;
	updateList (0);
}

void FontRangeSelect::changedSelection (QListBoxItem *item)
{
	range = items[item];
	emit selectionChanged (range);
}

void FontRangeSelect::updateList (int rangeToSelect)
{
	if ( !d )
	{
		qWarning ("FontRangeSelect::updateList(): no data!");
		return;
	}

	range = rangeToSelect;
	FONT *fnt = (FONT *)d->dat;
	int r = 0;

	clear ();

	if(fnt->vtable == font_vtable_mono) {
		FONT_MONO_DATA* mf = (FONT_MONO_DATA *)fnt->data;

		while(mf) {
			insertItem (tr("%1 - %2F, mono").arg(int2hex(mf->begin)).arg(int2hex(mf->end)), r);
			items[item(r)] = r;

			setSelected (r, r == rangeToSelect);

			mf = mf->next;
			r++;
		}
	} else {
		FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)fnt->data;

		while(cf) {
			insertItem (tr("%1 - %2F, color").arg(int2hex(cf->begin)).arg(int2hex(cf->end)), r);
			items[item(r)] = r;

			setSelected (r, r == rangeToSelect);

			cf = cf->next;
			r++;
		}
	}
}

void FontRangeSelect::importRange ()
{
	if ( !d )
	{
		qWarning ("FontRangeSelect::importRange(): no data!");
		return;
	}

	QString fileName = QFileDialog::getOpenFileName ("", tr("Font") + " (*.bmp *.fnt *.lbm *.pcx *.tga)", this, tr("Import range"));

	if ( !fileName.isEmpty() )
	{
		long size;

		QCursor (WaitCursor);
		FONT *fnt = (FONT *)grabFont(QFile::encodeName(fileName), &size, -1, -1, -1, -1, -1);
		QCursor (ArrowCursor);

		if ( !fnt )
		{
			QMessageBox::warning (0, tr("Error"), tr("Error importing '%1'").arg(fileName), QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}

		// get the base char
		CharDialog *chardlg = new CharDialog (this, "CharDialog", true);
		chardlg->exec ();

		int base = strtol(chardlg->base->text().latin1(), NULL, 0);
		int import_begin, import_end;

		if(fnt->vtable == font_vtable_mono) {
			FONT_MONO_DATA* mf = (FONT_MONO_DATA *)fnt->data;
			import_end = (mf->end += (base - mf->begin));
			import_begin = mf->begin = base;
		} else {
			FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)fnt->data;
			import_end = (cf->end += (base - cf->begin));
			import_begin = cf->begin = base;
		}

		FONT *f = (FONT *)d->dat;

		if (f->vtable == font_vtable_mono) {
			FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;
			while(mf) {
				if(mf->end > import_begin && mf->begin < import_end) {
					QMessageBox::warning (0, tr("Warning"), tr("Data overlaps with an\nexisting range. This almost\ncertainly isn't what you want"), QMessageBox::Ok, QMessageBox::NoButton);
					break;
				}

				mf = mf->next;
			}
		} else {
			FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)f->data;
			while(cf) {
				if(cf->end > import_begin && cf->begin < import_end) {
					QMessageBox::warning (0, tr("Warning"), tr("Data overlaps with an\nexisting range. This almost\ncertainly isn't what you want"), QMessageBox::Ok, QMessageBox::NoButton);
					break;
				}

				cf = cf->next;
			}
		}

		if(f->vtable != fnt->vtable) {
			upgrade_to_color(f);
			upgrade_to_color(fnt);
		}

		f = (FONT *)d->dat;
		int i = 0;

		if(f->vtable == font_vtable_mono) {
			FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data, * mf2 = (FONT_MONO_DATA *)fnt->data;

			if(mf->begin > import_begin) {
				mf2->next = mf;
				f->data = mf2;
			} else {
				i++;
				while(mf->next && mf->next->begin < import_begin) {
					mf = mf->next;
					i++;
				}
				mf2->next = mf->next;
				mf->next = mf2;
			}
		} else {
			FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)f->data, * cf2 = (FONT_COLOR_DATA *)fnt->data;

			if(cf->begin > import_begin) {
				cf2->next = cf;
				f->data = cf2;
			} else {
				i++;
				while(cf->next && cf->next->begin < import_begin) {
					cf = cf->next;
					i++;
				}
				cf2->next = cf->next;
				cf->next = cf2;
			}
		}

		emit update(i);
		emit selectionChanged(i);
		emit updated();
	}
}

void FontRangeSelect::deleteRange ()
{
	if ( !d )
	{
		qWarning ("FontRangeSelect::deleteRange(): no data!");
		return;
	}

	FONT *fnt = (FONT *)d->dat;
	int i;

	if(fnt->vtable == font_vtable_mono)
	{
		FONT_MONO_DATA* mf = (FONT_MONO_DATA *)fnt->data, * mf_prev = 0;

		if(!mf->next) {
			QMessageBox::warning (0, tr("Warning"), tr("Deletion not possible:\nfonts must always have at\nleast one character range"), QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}

		i = range;
		while(i--) {
			mf_prev = mf;
			mf = mf->next;
		}

		if(mf_prev) mf_prev->next = mf->next;
		else fnt->data = mf->next;

		for(i = mf->begin; i < mf->end; i++)
			free(mf->glyphs[i - mf->begin]);
		free(mf->glyphs);
		free(mf);
	}
	else
	{
		FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)fnt->data, * cf_prev = 0;

		if(!cf->next) {
			QMessageBox::warning (0, tr("Warning"), tr("Deletion not possible:\nfonts must always have at\nleast one character range"), QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}

		i = range;
		while(i--) {
			cf_prev = cf;
			cf = cf->next;
		}

		if(cf_prev) cf_prev->next = cf->next;
		else fnt->data = cf->next;

		for(i = cf->begin; i < cf->end; i++)
			destroy_bitmap(cf->bitmaps[i - cf->begin]);
		free(cf->bitmaps);
		free(cf);
	}

	emit update(0);
	emit selectionChanged(0);
	emit updated();
}

/*****************************************************************************
 *
 * Class FontRangeView
 *
 *****************************************************************************/

FontRangeView::FontRangeView (QWidget *parent, const char *name)
			: QListView (parent, name)
{
	d = 0;
	range = 0;

	setSorting (-1); // don't sort glyphs
	addColumn ( tr("Unicode") );
	addColumn ( tr("Size") );
	addColumn ( tr("Char") );
	addColumn ( tr("Glyph") );
}

FontRangeView::~FontRangeView ()
{
}

void FontRangeView::setData (DATAFILE *dat)
{
	d = dat;
	rangeChanged (range);
}

void FontRangeView::rangeChanged (int rangeToSelect)
{
	range = rangeToSelect;

	if ( !d )
	{
		//qWarning ("FontRangeView::rangeChanged(): no data!");
		return;
	}

	clear ();

	text_mode (-1);

	FONT *fnt = (FONT *)d->dat;
	QListViewItem *item, *prev = 0;
	int begin, end;
	BITMAP** bits = 0;
	FONT_GLYPH** gl = 0;
	int i = range;

	if (fnt->vtable == font_vtable_mono)
	{
		FONT_MONO_DATA *mf = (FONT_MONO_DATA *)fnt->data;
		while(i--)
			mf = mf->next;

		gl = mf->glyphs;
		begin = mf->begin;
		end = mf->end;
	}
	else
	{
		FONT_COLOR_DATA *cf = (FONT_COLOR_DATA *)fnt->data;
		while(i--)
			cf = cf->next;

		bits = cf->bitmaps;
		begin = cf->begin;
		end = cf->end;
	}

	QColor trans = paletteBackgroundColor();

	for (i = begin; i < end; i++) {
		int w, h;

		if (gl) {
			w = gl[i - begin]->w;
			h = gl[i - begin]->h;
		}
		else {
			w = bits[i - begin]->w;
			h = bits[i - begin]->h;
		}

		// create QImage with glyph
		BITMAP *bmp = create_bitmap_ex (16, w, h);
		clear_to_color (bmp, bmp->vtable->mask_color);
		textprintf (bmp, fnt, 0, 0, makecol_depth(16, 0,0,0), "%c", i);
		QImage *img = bmp2img (bmp, &trans);
		destroy_bitmap (bmp);

		item = new QListViewItem (this, prev, QString("U+%1X").arg(int2hex(i)), QString("%1x%2").arg(w).arg(h), QString("%1").arg(QChar(i)));
		item->setPixmap (3, QPixmap(*img));

		delete img;

		prev = item;
	}
}

/*****************************************************************************
 *
 * Class ObjectFont
 *
 *****************************************************************************/

ObjectFont::ObjectFont ()
{
	fontData = new QPixmap (getResourcesDir()+"/icons/object/font_bitmap.png");
}

ObjectFont::~ObjectFont ()
{
	delete fontData;
}

bool ObjectFont::typeSupported (int type)
{
	return type == DAT_FONT;
}

int ObjectFont::type ()
{
	return DAT_FONT;
}

QString ObjectFont::menuDesc ()
{
	return tr("Font");
}

QPixmap ObjectFont::pixmap ()
{
	return *fontData;
}

void ObjectFont::open (DatafileItem *parent, DATAFILE *data)
{
	DatafileItemFont *item;
	item = new DatafileItemFont (parent, data);
}

void *ObjectFont::makeNew (long *size)
{
	return makenew_font(size);
}

void *ObjectFont::copyData (DATAFILE *dat, long *size)
{
	FONT *src = (FONT *)dat->dat;
	FONT *f = (FONT *)malloc(sizeof(FONT));

	if (src->vtable == font_vtable_mono)
	{
		FONT_MONO_DATA *mf = 0, *mfread = (FONT_MONO_DATA *)src->data;

		f->height = font->height;
		f->vtable = font->vtable;

		while (mfread)
		{
			int i;

			if (mf)
			{
				mf->next = (FONT_MONO_DATA *)malloc(sizeof(FONT_MONO_DATA));
				mf = mf->next;
			} else
				f->data = mf = (FONT_MONO_DATA *)malloc(sizeof(FONT_MONO_DATA));

			mf->begin = mfread->begin;
			mf->end = mfread->end;
			mf->next = 0;
			mf->glyphs = (FONT_GLYPH **)malloc(sizeof(FONT_GLYPH*) * (mf->end - mf->begin));

			for (i = mf->begin; i < mf->end; i++)
			{
				FONT_GLYPH *gsrc = mfread->glyphs[i - mf->begin], *gdest;
				int sz = ((gsrc->w + 7) / 8) * gsrc->h;

				gdest = (FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + sz);
				gdest->w = gsrc->w;
				gdest->h = gsrc->h;
				memcpy(gdest->dat, gsrc->dat, sz);

				mf->glyphs[i - mf->begin] = gdest;
			}

			mfread = mfread->next;
		}
	}
	else
	{
		FONT_COLOR_DATA *cf = 0, *cfread = (FONT_COLOR_DATA *)src->data;

		f->height = font->height;
		f->vtable = font->vtable;

		while (cfread)
		{
			if (cf)
			{
				cf->next = (FONT_COLOR_DATA *)malloc(sizeof(FONT_COLOR_DATA));
				cf = cf->next;
			} else
				f->data = cf = (FONT_COLOR_DATA *)malloc(sizeof(FONT_COLOR_DATA));

			cf->begin = cfread->begin;
			cf->end = cfread->end;
			cf->next = 0;
			cf->bitmaps = (BITMAP **)malloc(sizeof(BITMAP*) * (cf->end - cf->begin));

			for (int i = cf->begin; i < cf->end; i++)
			{
				BITMAP *gsrc  = cfread->bitmaps[i - cf->begin];
				BITMAP *gdest = create_bitmap_ex(gsrc->vtable->color_depth, gsrc->w, gsrc->h);
				
				blit (gsrc, gdest, 0, 0, 0, 0, gsrc->w, gsrc->h);

				cf->bitmaps[i - cf->begin] = gdest;
			/*
				FONT_GLYPH *gsrc = mfread->glyphs[i - mf->begin], *gdest;
				int sz = ((gsrc->w + 7) / 8) * gsrc->h;

				gdest = (FONT_GLYPH *)malloc(sizeof(FONT_GLYPH) + sz);
				gdest->w = gsrc->w;
				gdest->h = gsrc->h;
				memcpy(gdest->dat, gsrc->dat, sz);

				mf->glyphs[i - mf->begin] = gdest;
				*/
			}

			cfread = cfread->next;
		}
	}

	return f;
}

void ObjectFont::plot (DATAFILE *dat, QWidget *parent)
{
	QVBoxLayout *vbox = new QVBoxLayout (parent);

	FontPlot *fp = new FontPlot (parent, "FontPlot");
	fp->setData (dat, this);

	vbox->addWidget (fp);

	parent->show ();
}

void ObjectFont::saveObject (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f)
{
	save_font (item->data(), packed, packkids, strip, verbose, extra, f);
}

/*****************************************************************************
 *
 * Class IOFont
 *
 *****************************************************************************/

class IOFont : public IOPlugin
{
private:
	int   exportFont (const DATAFILE* dat, const char* filename);

public:
	IOFont ();
	~IOFont ();

	bool    typeSupported (int type);
	QString grabType      ();
	QString exportType    ();
	void    *grabFile     (const char *file, long *size);
	int     exportFile    (const char *file, DatafileItem *item);
};

IOFont::IOFont ()
{
}

IOFont::~IOFont ()
{
}

bool IOFont::typeSupported (int type)
{
	return type == DAT_FONT;
}

QString IOFont::grabType ()
{
	QString g =	tr("Images") + " (*.bmp *.lbm *.pcx *.tga);;";
	g +=		tr("Grx/BIOS font") + " (*.fnt);;";
	g +=		tr("Font script") + " (*.txt)";
	
	return g;
}

QString IOFont::exportType ()
{
	QString g =	tr("Images") + " (*.bmp *.pcx *.tga);;";
	g +=		tr("Font script") + " (*.txt)";
	
	return g;
}

void *IOFont::grabFile (const char *file, long *size)
{
	return grabFont(file, size, -1, -1, -1, -1, -1);
}

int IOFont::exportFile (const char *file, DatafileItem *item)
{
	return exportFont (item->data(), file);
}

// exports a font into an external file
int IOFont::exportFont(const DATAFILE* dat, const char *filename)
{
	char buf[1024], tmp[1024];
	PACKFILE *pack;
	FONT* f = (FONT *)dat->dat;

	BITMAP *bmp;
	PALETTE pal;

	int w = 0, h = 0, max = 0, i;

	if (stricmp(get_extension((AL_CONST char *)filename), "txt") == 0) {
		replace_extension (buf, (AL_CONST char *)filename, "pcx", sizeof(buf));

		if(exists(buf)) {
			i = QMessageBox::information (0, tr("Information"), tr("%1 already exists, overwrite?").arg(QFile::decodeName(buf)), QMessageBox::Yes, QMessageBox::No);
			if(i == QMessageBox::No)
				return TRUE;
		}

		pack = pack_fopen ((AL_CONST char *)filename, F_WRITE);
		if(!pack) return FALSE;

		if(f->vtable == font_vtable_mono) {
			FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;
			while(mf) {
				sprintf(tmp, "%s 0x%04X 0x%04X\n", (mf == f->data) ? get_filename(buf) : "-", mf->begin, mf->end - 1);
				pack_fputs(tmp, pack);
				mf = mf->next;
			}
		} else {
			FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)f->data;
			while(cf) {
				sprintf(tmp, "%s 0x%04X 0x%04X\n", (cf == f->data) ? get_filename(buf) : "-", cf->begin, cf->end - 1);
				pack_fputs(tmp, pack);
				cf = cf->next;
			}
		}

		pack_fclose(pack);

		filename = buf;
	} else {
		int multi = 0;

		if(f->vtable == font_vtable_mono) {
			if( ((FONT_MONO_DATA*)f->data) ->next) multi = 1;
			} else {
			if( ((FONT_COLOR_DATA*)f->data)->next) multi = 1;
		}

		if(multi) {
			i = QMessageBox::information (0, tr("Information"), tr("Really export multi-range font as bitmap rather than .txt?"), QMessageBox::Yes, QMessageBox::No);
			if(i == QMessageBox::No)
				return TRUE;
		}
	}

	if(f->vtable == font_vtable_mono) {
		FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;

		while(mf) {
			for(i = mf->begin; i < mf->end; i++) {
				FONT_GLYPH* g = mf->glyphs[i - mf->begin];

				max++;
				if(g->w > w) w = g->w;
				if(g->h > h) h = g->h;
			}

			mf = mf->next;
		}
	} else {
		FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)f->data;
		int i;

		while(cf) {
			for(i = cf->begin; i < cf->end; i++) {
				BITMAP* g = cf->bitmaps[i - cf->begin];

				max++;
				if(g->w > w) w = g->w;
				if(g->h > h) h = g->h;
			}

			cf = cf->next;
		}
	}

	w = (w + 16) & 0xFFF0;
	h = (h + 16) & 0xFFF0;

	bmp = create_bitmap_ex(8, 1 + w * 16, 1 + h * ((max + 15) / 16) );
	clear_to_color(bmp, 255);
	text_mode(0);

	max = 0;

	if(f->vtable == font_vtable_mono) {
		FONT_MONO_DATA* mf = (FONT_MONO_DATA *)f->data;

		while(mf) {
			for(i = mf->begin; i < mf->end; i++) {
				textprintf(bmp, f, 1 + w * (max & 15), 1 + h * (max / 16), 1, "%c", i);
				max++;
			}

			mf = mf->next;
		}
	} else {
		FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)f->data;

		while(cf) {
			for(i = cf->begin; i < cf->end; i++) {
				textprintf(bmp, f, 1 + w * (max & 15), 1 + h * (max / 16), -1, "%c", i);
				max++;
			}

			cf = cf->next;
		}
	}

	get_palette (pal);

	save_bitmap ((AL_CONST char *)filename, bmp, pal);
	destroy_bitmap(bmp);

	return (errno == 0);
}

/*****************************************************************************
 *
 * Class FontPlugin
 *
 *****************************************************************************/

class FontPlugin : public QGrabberPlugin
{
public:
	FontPlugin() {}
	~FontPlugin() {}

	QStringList keys() const
	{
		QStringList list;

		list << "Font";

		return list;
	}

	QStringList keysObject() const
	{
		QStringList list;

		list << "Font";

		return list;
	}

	ObjectPlugin *createObject(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "font")
			return new ObjectFont;

		return 0;
	}

	QStringList keysIO() const
	{
		QStringList list;

		list << "Font";

		return list;
	}

	IOPlugin *createIO(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "font")
			return new IOFont;

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

EXPORT_PLUGIN( FontPlugin )

