/***************************************************************************
                          prefs.h  -  description
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

#ifndef PREFS_H
#define PREFS_H

#include "export.h"

#include "alleg.h"

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

class EXPORT Prefs : public QObject
{
	Q_OBJECT

private:
	QString DatafileName;	// name of currently editing datafile
	char Password[512];	// datafile password
	QStringList Passwords;	// history of entered passwords
	int Strip, Pack;	// property strip and datafile compression method
	char StripExceptions[512];	// properties, that must not be stripped
	bool Backups, Dither;	// create backupfiles (with '.bak' extension) and dither images on color conversion
	bool SaveHeader;	// save datafile header
	bool Verbose;
	bool OpenNested;	// if this is true, all nested datafiles set to open state on loading of existing datafile
	QString StyleName;	// visual style
	bool ToolbarOn, StatusbarOn;
	QString ResourcesDir;
	QStringList RecentFiles;	// List with last open and created datafiles

	static BITMAP *Graphic;
	static PALETTE Palette;

	static char ImportFile[1024];
	static char GraphicOrigin[1024];
	static char GraphicDate[1024];

public:
	Prefs (QObject *parent = 0, const char *name = 0);
	~Prefs ();

	void load ();
	void save ();

	static Prefs *prefs ();

	static BITMAP *graphic ();
	static RGB *palette ();

	static char *importFile ();
	static char *graphicOrigin ();
	static char *graphicDate ();

	static void setGraphic (BITMAP *);
	static void setPalette (RGB *);

	static void setImportFile (const char *);
	static void setGraphicOrigin (const char *);
	static void setGraphicDate (const char *);
	
	QString		datafileName () const;
	char		*password () const;
	QStringList	passwords () const;
	int		strip () const;
	char		*stripExceptions () const;
	int		pack () const;
	bool		backups () const;
	bool		dither () const;
	bool		saveHeader () const;
	bool		verbose () const;
	QString		style () const;
	bool		openNested () const;
	bool		toolbarOn () const;
	bool		statusbarOn () const;
	QString		resourcesDir () const;
	QStringList	recentFiles () const;

public slots:
	void setDatafileName (const QString &);
	void setPassword (const char *pass);
	void setPasswords (const QStringList &);
	void setStrip (int strip);
	void setStripExceptions (const char *ex);
	void setPack (int pack);
	void setBackups (bool backups);
	void setDither (bool dither);
	void setSaveHeader (bool save);
	void setVerbose (bool verbose);
	void setStyle (const QString style);
	void setOpenNested (bool open);
	void setToolbarOn (bool on);
	void setStatusbarOn (bool on);
	void setResourcesDir (const QString &);
	void setRecentFiles (const QStringList &);
};

#endif
