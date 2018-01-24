/***************************************************************************
                          qgrabber.h  -  description
                             -------------------
    begin                : ðÎÄ áÐÒ  8 01:10:08 MSD 2002
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

#ifndef QGRABBER_H
#define QGRABBER_H

#include <qapp.h>
#include <qmainwindow.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qwhatsthis.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qmsgbox.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qpainter.h>

class DatafileView;
class PropView;
class DrawField;

class QGrabberApp : public QMainWindow
{
	Q_OBJECT

private:
	bool dirty;
	QString fileName;

	bool queryExit();

	void init ();
	void initActions ();
	void initMenuBar ();
	void initToolBar ();
	void initStatusBar ();
	void initView ();

	DatafileView *view;
	PropView     *propView;
	DrawField    *drawfield;

	// Menu
	QPopupMenu *fileMenu;
	QPopupMenu *recentFilesMenu;
	QPopupMenu *optionsMenu;
	QPopupMenu *pluginsMenu;
	QPopupMenu *helpMenu;

	// Toolbar
	QToolBar *fileToolbar;

	// Actions
	QAction *fileNew;
	QAction *fileOpen;
	QAction *fileSave;
	QAction *fileSaveAs;
	QAction *fileQuit;
	QAction *fileUpdate;
	QAction *fileUpdateSelection;
	QAction *fileForceUpdate;

	QAction *optionsToolBar;
	QAction *optionsStatusBar;
	QAction *optionsConfigureToolbars;
	QAction *optionsConfigure;

	QAction *helpReadme;
	QAction *helpLicense;
	QAction *helpAbout;
	QAction *helpAboutQt;

public slots:
	void setDirty ();
	void setDirty (bool);
	void setFileName (const QString &);
	void addRecentlyOpenedFile (const QString &);

private slots:
	void setupRecentFilesMenu ();
	void recentFileActivated (int);

	bool maybeSave ();

	void closeEvent (QCloseEvent *e);

	// Menu slots
	void slotFileNew();
	void slotFileOpen();
	void slotFileQuit();

	void slotOptionsToolBar(bool toggle);
	void slotOptionsStatusBar(bool toggle);
	void slotOptionsConfigureToolbars();
	void slotOptionsConfigure();

	void slotHelpReadme();
	void slotHelpLicense();
	void slotHelpAbout();
	void slotHelpAboutQt();

signals:
	void newFile ();
	void openFile ();
	void openFile (const QString &);
	void saveFile ();

public:
	QGrabberApp (int argc, char *argv[]);
	~QGrabberApp();
};

#endif

