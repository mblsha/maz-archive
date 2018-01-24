/***************************************************************************
                          midi.cpp  -  description
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

#include <qpixmap.h>
#include <qlayout.h>

#include "midiplot.h"

static QPixmap *midiData = 0;

#include "common.h"
#include "plugins.h"
#include "plugins_p.h"
#include "datafileitem.h"

/*****************************************************************************
 *
 * Class DatafileItemMidi
 *
 *****************************************************************************/

class DatafileItemMidi : public DatafileItem
{
public:
	DatafileItemMidi (QListViewItem *parent, DATAFILE *data)
				: DatafileItem (parent, data)
	{
		setPixmap ( midiData );
	}

	~DatafileItemMidi () {}
};

/*****************************************************************************
 *
 * Class ObjectMidi
 *
 *****************************************************************************/

class ObjectMidi : public ObjectPlugin
{
public:
	ObjectMidi ();
	~ObjectMidi ();

	bool    typeSupported (int type);
	int     type          ();
	QString menuDesc      ();
	QPixmap pixmap        ();
	void    open          (DatafileItem *parent, DATAFILE *data);
	void    *makeNew      (long *size);
	void    *copyData     (DATAFILE *dat, long *size);
	void    saveObject    (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f);
	void    plot          (DATAFILE *dat, QWidget *parent);
	bool    doubleClick   (DATAFILE *dat);
};

ObjectMidi::ObjectMidi ()
{
	install_sound (DIGI_AUTODETECT, MIDI_AUTODETECT, 0);
	midiData = new QPixmap (getResourcesDir()+"/icons/object/midi.png");
}

ObjectMidi::~ObjectMidi ()
{
	if ( midiData )
		delete midiData;
}

bool ObjectMidi::typeSupported (int type)
{
	return type == DAT_MIDI;
}

int ObjectMidi::type ()
{
	return DAT_MIDI;
}

QString ObjectMidi::menuDesc ()
{
	return tr("MIDI");
}

QPixmap ObjectMidi::pixmap ()
{
	return *midiData;
}

void ObjectMidi::open (DatafileItem *parent, DATAFILE *dat)
{
	DatafileItemMidi *item;
	item = new DatafileItemMidi (parent, dat);
}

void *ObjectMidi::makeNew (long *size)
{
	MIDI *mid;
	int c;

	mid = (MIDI*)malloc(sizeof(MIDI));
	mid->divisions = 120;

	for (c = 0; c < MIDI_TRACKS; c++)
	{
		mid->track[c].data = NULL;
		mid->track[c].len = 0;
	}

	return mid;
}

void *ObjectMidi::copyData (DATAFILE *dat, long *size)
{
	MIDI *src = (MIDI*)dat->dat;
	MIDI *mid;
	int c;

	mid = (MIDI*)malloc(sizeof(MIDI));
	mid->divisions = src->divisions;

	for (c = 0; c < MIDI_TRACKS; c++)
	{
		mid->track[c].len = src->track[c].len;
		if ( src->track[c].len > 0 )
		{
			mid->track[c].data = (unsigned char*)malloc( src->track[c].len );
			mid->track[c].data = src->track[c].data;
		}
		else
			mid->track[c].data = 0;
	}

	return mid;
}

void ObjectMidi::saveObject (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f)
{
	MIDI *midi = (MIDI *)item->data()->dat;
	int c;

	pack_mputw (midi->divisions, f);

	for (c = 0; c < MIDI_TRACKS; c++)
	{
		pack_mputl (midi->track[c].len, f);
		if (midi->track[c].len > 0)
			pack_fwrite (midi->track[c].data, midi->track[c].len, f);
	}
}

void ObjectMidi::plot (DATAFILE *dat, QWidget *parent)
{
	QVBoxLayout *vbox = new QVBoxLayout (parent);

	MidiPlot *sp = new MidiPlot (parent);
	sp->setMidi ((MIDI*)dat->dat);
	vbox->addWidget (sp);

	parent->show ();
}

bool ObjectMidi::doubleClick (DATAFILE *dat)
{
	play_midi ((MIDI*)dat->dat, FALSE);
	return false;
}

/*****************************************************************************
 *
 * Class IOMidi
 *
 *****************************************************************************/

class IOMidi : public IOPlugin
{
public:
	IOMidi ();
	~IOMidi ();

	bool    typeSupported (int type);
	QString grabType      ();
	QString exportType    ();
	void    *grabFile     (const char *file, long *size);
	int     exportFile    (const char *file, DatafileItem *item);
};

IOMidi::IOMidi ()
{
}

IOMidi::~IOMidi ()
{
}

bool IOMidi::typeSupported (int type)
{
	return type == DAT_MIDI;
}

QString IOMidi::grabType ()
{
	return tr("MIDI") + " (*.mid)";
}

QString IOMidi::exportType ()
{
	return tr("MIDI") + " (*.mid)";
}

void *IOMidi::grabFile (const char *file, long *size)
{
	return load_midi (file);
}

int IOMidi::exportFile (const char *file, DatafileItem *item)
{
	MIDI *midi = (MIDI *)item->data()->dat;
	PACKFILE *f;
	int num_tracks;
	int c;

	num_tracks = 0;
	for (c = 0; c < MIDI_TRACKS; c++)
	if (midi->track[c].len > 0)
		num_tracks++;

	f = pack_fopen (file, F_WRITE);

	if (f)
	{
		pack_fputs("MThd", f);                 /* MIDI header */
		pack_mputl(6, f);                      /* size of header chunk */
		pack_mputw(1, f);                      /* type 1 */
		pack_mputw(num_tracks, f);             /* number of tracks */
		pack_mputw(midi->divisions, f);        /* beat divisions */

		for (c=0; c<MIDI_TRACKS; c++)          /* for each track */
		{
			if (midi->track[c].len > 0)
			{
				pack_fputs("MTrk", f);           /* write track data */
				pack_mputl(midi->track[c].len, f);
				pack_fwrite(midi->track[c].data, midi->track[c].len, f);
			}
		}

		pack_fclose(f);
	}

	return (errno == 0);
}

/*****************************************************************************
 *
 * Class MidiPlugin
 *
 *****************************************************************************/

class MidiPlugin : public QGrabberPlugin
{
public:
	MidiPlugin() {}
	~MidiPlugin() {}

	QStringList keys() const
	{
		QStringList list;

		list << "MIDI";

		return list;
	}

	QStringList keysObject() const
	{
		QStringList list;

		list << "MIDI";

		return list;
	}

	ObjectPlugin *createObject(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "midi")
			return new ObjectMidi;

		return 0;
	}

	QStringList keysIO() const
	{
		QStringList list;

		list << "MIDI";

		return list;
	}

	IOPlugin *createIO(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "midi")
			return new IOMidi;

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

EXPORT_PLUGIN( MidiPlugin )


