/***************************************************************************
                          prefs.cpp  -  description
                             -------------------
    begin                : ðÎÄ áÐÒ 29 2002
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
#include <string.h>

#include <qstring.h>
#include <qcstring.h>
#include <qtextcodec.h>
#include <qmessagebox.h>
#include <qapp.h>
#include <qsettings.h>

#include "common.h"
#include "prefs.h"

static Prefs *globalPrefs = 0;

Prefs::Prefs (QObject *parent, const char *name)
			: QObject (parent, name)
{
	DatafileName = "";

	load ();
}

Prefs::~Prefs ()
{
	save ();
}

Prefs *Prefs::prefs ()
{
	if ( !globalPrefs )
		globalPrefs = new Prefs (0, "globalPrefs");

	return globalPrefs;
}

void Prefs::load ()
{
	QSettings settings;
	settings.insertSearchPath ( QSettings::Windows, "/MAZsoft" );
	settings.insertSearchPath ( QSettings::Unix, getHomeDir() );

	Passwords =		settings.readListEntry("/QGrabber/passwords");
	strcpy (Password,	settings.readEntry("/QGrabber/password",        "").utf8() );
	strcpy (StripExceptions,settings.readEntry("/QGrabber/stripExceptions", "NAME;").latin1() );

	RecentFiles =		settings.readListEntry("/QGrabber/recentFiles");

	setStyle ( StyleName =	settings.readEntry("/QGrabber/style", "Windows") );

	Pack         = settings.readNumEntry  ("/QGrabber/pack",        2);
	Strip        = settings.readNumEntry  ("/QGrabber/strip",       0);
	Backups      = settings.readBoolEntry ("/QGrabber/backups",     true);
	Dither       = settings.readBoolEntry ("/QGrabber/dither",      true);
	SaveHeader   = settings.readBoolEntry ("/QGrabber/saveHeader",  false);
	Verbose      = settings.readBoolEntry ("/QGrabber/verbose",     true);
	OpenNested   = settings.readBoolEntry ("/QGrabber/openNested",  false);
	ToolbarOn    = settings.readBoolEntry ("/QGrabber/toolbarOn",   true);
	StatusbarOn  = settings.readBoolEntry ("/QGrabber/statusbarOn", false);
	ResourcesDir =     settings.readEntry ("/QGrabber/resourcesDir", "");
}

void Prefs::save ()
{
	QSettings settings;
	settings.insertSearchPath ( QSettings::Windows, "/MAZsoft" );
	settings.insertSearchPath ( QSettings::Unix, getHomeDir() );

	settings.writeEntry ("/QGrabber/password",        QString::fromUtf8( Password ) );
	settings.writeEntry ("/QGrabber/passwords",       Passwords );
	settings.writeEntry ("/QGrabber/stripExceptions", StripExceptions);

	settings.writeEntry ("/QGrabber/recentFiles", RecentFiles);

	settings.writeEntry ("/QGrabber/style",        StyleName);

	settings.writeEntry ("/QGrabber/pack",         Pack);
	settings.writeEntry ("/QGrabber/strip",        Strip);
	settings.writeEntry ("/QGrabber/backups",      Backups);
	settings.writeEntry ("/QGrabber/dither",       Dither);
	settings.writeEntry ("/QGrabber/saveHeader",   SaveHeader);
	settings.writeEntry ("/QGrabber/verbose",      Verbose);
	settings.writeEntry ("/QGrabber/openNested",   OpenNested);
	settings.writeEntry ("/QGrabber/toolbarOn",    ToolbarOn);
	settings.writeEntry ("/QGrabber/statusbarOn",  StatusbarOn);
	settings.writeEntry ("/QGrabber/resourcesDir", ResourcesDir);
}

void Prefs::setDatafileName (const QString &name)
{
	DatafileName = name;
}

QString Prefs::datafileName () const
{
	return DatafileName;
}

void Prefs::setPassword (const char *pass)
{
	strcpy (Password, pass);
}

char *Prefs::password () const
{
	return (char *)Password;
}

QStringList Prefs::passwords () const
{
	return Passwords;
}

void Prefs::setPasswords (const QStringList &p)
{
	Passwords = p;
}

void Prefs::setStrip (int s)
{
	Strip = s;
}

int Prefs::strip () const
{
	return Strip;
}

void Prefs::setStripExceptions (const char *ex)
{
	strcpy (StripExceptions, ex);
}

char *Prefs::stripExceptions () const
{
	return (char *)StripExceptions;
}

void Prefs::setPack (int p)
{
	Pack = p;
}

int Prefs::pack () const
{
	return Pack;
}

void Prefs::setBackups (bool _backups)
{
	Backups = _backups;
}

bool Prefs::backups () const
{
	return Backups;
}

void Prefs::setDither (bool _dither)
{
	Dither = _dither;
}

bool Prefs::dither () const
{
	return Dither;
}

void Prefs::setSaveHeader (bool save)
{
	SaveHeader = save;
}

bool Prefs::saveHeader () const
{
	return SaveHeader;
}

void Prefs::setVerbose (bool _verbose)
{
	Verbose = _verbose;
}

bool Prefs::verbose () const
{
	return Verbose;
}

void Prefs::setStyle (const QString style)
{
	StyleName = style;
	qApp->setStyle (style);
}

QString Prefs::style () const
{
	return StyleName;
}

void Prefs::setOpenNested (bool open)
{
	OpenNested = open;
}

bool Prefs::openNested () const
{
	return OpenNested;
}

void Prefs::setToolbarOn (bool on)
{
	ToolbarOn = on;
	save ();
}

void Prefs::setStatusbarOn (bool on)
{
	StatusbarOn = on;
	save ();
}

bool Prefs::toolbarOn () const
{
	return ToolbarOn;
}

bool Prefs::statusbarOn () const
{
	return StatusbarOn;
}

QString Prefs::resourcesDir () const
{
	return ResourcesDir;
}

void Prefs::setResourcesDir (const QString &d)
{
	ResourcesDir = d;
}

QStringList Prefs::recentFiles () const
{
	return RecentFiles;
}

void Prefs::setRecentFiles (const QStringList &l)
{
	RecentFiles = l;
	save ();
}


BITMAP *Prefs::Graphic = 0;
PALETTE Prefs::Palette;
char Prefs::ImportFile[1024];
char Prefs::GraphicOrigin[1024];
char Prefs::GraphicDate[1024];

BITMAP *Prefs::graphic ()
{
	return Graphic;
}

RGB *Prefs::palette ()
{
	return (RGB *)Palette;
}

char *Prefs::importFile ()
{
	return ImportFile;
}

char *Prefs::graphicOrigin ()
{
	return GraphicOrigin;
}

char *Prefs::graphicDate ()
{
	return GraphicDate;
}

void Prefs::setGraphic (BITMAP *gfx)
{
	Graphic = gfx;
}

void Prefs::setPalette (RGB *rgb)
{
	for (int i = 0; i < sizeof(PALETTE)/sizeof(RGB); i++)
		Palette[i] = rgb[i];
	//Palette = rgb;
}

void Prefs::setImportFile (const char *file)
{
	strcpy (ImportFile, file);
}

void Prefs::setGraphicOrigin (const char *orig)
{
	strcpy (GraphicOrigin, orig);
}

void Prefs::setGraphicDate (const char *c)
{
	strcpy (GraphicDate, c);
}
