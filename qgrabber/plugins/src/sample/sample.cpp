/***************************************************************************
                          sample.cpp  -  description
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

#include "sampleplot.h"

static QPixmap *sampleData = 0;

#include "common.h"
#include "plugins.h"
#include "plugins_p.h"
#include "datafileitem.h"

/*****************************************************************************
 *
 * Class DatafileItemSample
 *
 *****************************************************************************/

class DatafileItemSample : public DatafileItem
{
public:
	DatafileItemSample (QListViewItem *parent, DATAFILE *data)
				: DatafileItem (parent, data)
	{
		setPixmap ( sampleData );
	}

	~DatafileItemSample () {}
};

/*****************************************************************************
 *
 * Class ObjectSample
 *
 *****************************************************************************/

class ObjectSample : public ObjectPlugin
{
public:
	ObjectSample ();
	~ObjectSample ();

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

ObjectSample::ObjectSample ()
{
	install_sound (DIGI_AUTODETECT, MIDI_AUTODETECT, 0);
	sampleData = new QPixmap (getResourcesDir()+"/icons/object/sound.png");
}

ObjectSample::~ObjectSample ()
{
	if ( sampleData )
		delete sampleData;
}

bool ObjectSample::typeSupported (int type)
{
	return type == DAT_SAMPLE;
}

int ObjectSample::type ()
{
	return DAT_SAMPLE;
}

QString ObjectSample::menuDesc ()
{
	return tr("Sample");
}

QPixmap ObjectSample::pixmap ()
{
	return *sampleData;
}

void ObjectSample::open (DatafileItem *parent, DATAFILE *dat)
{
	DatafileItemSample *item;
	item = new DatafileItemSample (parent, dat);
}

void *ObjectSample::makeNew (long *size)
{
	return create_sample(8, FALSE, 11025, 1024);
}

void *ObjectSample::copyData (DATAFILE *dat, long *size)
{
	void *data;
	data = malloc (dat->size);
	memcpy (data, dat->dat, dat->size);
	*size = dat->size;
	return data;
}

void ObjectSample::saveObject (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f)
{
	SAMPLE *spl = (SAMPLE *)item->data()->dat;

	pack_mputw((spl->stereo) ? -spl->bits : spl->bits, f);
	pack_mputw(spl->freq, f);
	pack_mputl(spl->len, f);
	if (spl->bits == 8)
	{
		pack_fwrite(spl->data, spl->len * ((spl->stereo) ? 2 : 1), f);
	}
	else
	{
		int i;

		for (i=0; i < (int)spl->len * ((spl->stereo) ? 2 : 1); i++)
		{
			pack_iputw(((unsigned short *)spl->data)[i], f);
		}
	}
}

void ObjectSample::plot (DATAFILE *dat, QWidget *parent)
{
	QVBoxLayout *vbox = new QVBoxLayout (parent);

	SamplePlot *sp = new SamplePlot (parent);
	sp->setSample ((SAMPLE*)dat->dat);
	vbox->addWidget (sp);

	parent->show ();
}

bool ObjectSample::doubleClick (DATAFILE *dat)
{
	play_sample ((SAMPLE*)dat->dat, 255, 127, 1000, FALSE);
	return false;
}

/*****************************************************************************
 *
 * Class IOSample
 *
 *****************************************************************************/

class IOSample : public IOPlugin
{
public:
	IOSample ();
	~IOSample ();

	bool    typeSupported (int type);
	QString grabType      ();
	QString exportType    ();
	void    *grabFile     (const char *file, long *size);
	int     exportFile    (const char *file, DatafileItem *item);
};

IOSample::IOSample ()
{
}

IOSample::~IOSample ()
{
}

bool IOSample::typeSupported (int type)
{
	return type == DAT_SAMPLE;
}

QString IOSample::grabType ()
{
	return tr("Sample") + " (*.wav *.voc)";
}

QString IOSample::exportType ()
{
	return tr("Sample") + " (*.wav)";
}

void *IOSample::grabFile (const char *file, long *size)
{
	return load_sample (file);
}

int IOSample::exportFile (const char *file, DatafileItem *item)
{
	SAMPLE *spl = (SAMPLE *)item->data()->dat;
	int bps = spl->bits/8 * ((spl->stereo) ? 2 : 1);
	int len = spl->len * bps;
	int i;
	signed short s;
	PACKFILE *f;

	f = pack_fopen(file, F_WRITE);

	if ( f )
	{
		pack_fputs("RIFF", f);                 /* RIFF header */
		pack_iputl(36+len, f);                 /* size of RIFF chunk */
		pack_fputs("WAVE", f);                 /* WAV definition */
		pack_fputs("fmt ", f);                 /* format chunk */
		pack_iputl(16, f);                     /* size of format chunk */
		pack_iputw(1, f);                      /* PCM data */
		pack_iputw((spl->stereo) ? 2 : 1, f);  /* mono/stereo data */
		pack_iputl(spl->freq, f);              /* sample frequency */
		pack_iputl(spl->freq*bps, f);          /* avg. bytes per sec */
		pack_iputw(bps, f);                    /* block alignment */
		pack_iputw(spl->bits, f);              /* bits per sample */
		pack_fputs("data", f);                 /* data chunk */
		pack_iputl(len, f);                    /* actual data length */

		if (spl->bits == 8)
		{
			pack_fwrite(spl->data, len, f);     /* write the data */
		}
		else
		{
			for (i=0; i < (int)spl->len * ((spl->stereo) ? 2 : 1); i++)
			{
				s = ((signed short *)spl->data)[i];
				pack_iputw(s^0x8000, f);
			}
		}

		pack_fclose(f);
	}

	return (errno == 0);
}

/*****************************************************************************
 *
 * Class SamplePlugin
 *
 *****************************************************************************/

class SamplePlugin : public QGrabberPlugin
{
public:
	SamplePlugin() {}
	~SamplePlugin() {}

	QStringList keys() const
	{
		QStringList list;

		list << "Sample";

		return list;
	}

	QStringList keysObject() const
	{
		QStringList list;

		list << "Sample";

		return list;
	}

	ObjectPlugin *createObject(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "sample")
			return new ObjectSample;

		return 0;
	}

	QStringList keysIO() const
	{
		QStringList list;

		list << "Sample";

		return list;
	}

	IOPlugin *createIO(const QString &key)
	{
		QString plug = key.lower();

		if (plug == "sample")
			return new IOSample;

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

EXPORT_PLUGIN( SamplePlugin )

