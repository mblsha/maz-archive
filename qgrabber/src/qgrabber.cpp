/***************************************************************************
                          qgrabber.cpp  -  description
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

#include "alleg.h"

#include <qaccel.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qstringlist.h>

#include "qgrabber.h"

#include "common.h"
#include "prefs.h"
#include "plugins.h"
#include "factory.h"

#include "datafileview.h"
#include "propview.h"
#include "drawfield.h"

#include "prefsdialog.h"
#include "aboutdialog.h"
#include "fileviewer.h"

QGrabberApp::QGrabberApp (int argc, char *argv[])
{
	fileName = "";
	dirty = false;

	init ();

	if ( argc == 2 )
		emit openFile ( QFile::decodeName ( argv[1] ) );
	else
		emit newFile ();
}

QGrabberApp::~QGrabberApp()
{
	Prefs::prefs()->save ();
	delete Prefs::prefs();
}

void QGrabberApp::init ()
{
	setName ("QGrabberApp");
	QMainWindow::setCaption ( "QGrabber" );

	allegro_init ();
	set_color_conversion (COLORCONV_NONE);

	initView (); // since it initializes custom widgets, it must be called first
	initActions ();
	initToolBar ();
	initStatusBar ();
	initMenuBar ();
	initPlugins ();

	QStringList keys = ObjectFactory::keys ();
	for (QStringList::Iterator it = keys.begin(); it != keys.end(); it++)
	{
		ObjectPlugin *plug = ObjectFactory::create ( *it );
		connect (plug, SIGNAL(dirty()), this, SLOT(setDirty()));
	}
}

void QGrabberApp::initActions()
{
	Prefs *prefs = Prefs::prefs();

	//
	// File Menu
	//
	fileNew = new QAction(tr("New File"), QPixmap(getResourcesDir()+"/icons/main/filenew.png"), tr("&New"), QAccel::stringToKey(tr("Ctrl+N")), this);
	fileNew->setStatusTip(tr("Creates a new datafile"));
	fileNew->setWhatsThis(tr("New File\n\nCreates a new datafile"));
	connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));

	fileOpen = new QAction(tr("Open File"), QPixmap(getResourcesDir()+"/icons/main/fileopen.png"), tr("&Open..."), 0, this);
	fileOpen->setStatusTip(tr("Opens an existing datafile"));
	fileOpen->setWhatsThis(tr("Open File\n\nOpens an existing datafile"));
	connect(fileOpen, SIGNAL(activated()), this, SLOT(slotFileOpen()));

	fileSave = new QAction(tr("Save File"), QPixmap(getResourcesDir()+"/icons/main/filesave.png"), tr("&Save"), QAccel::stringToKey(tr("Ctrl+S")), this);
	fileSave->setStatusTip(tr("Saves the datafile"));
	fileSave->setWhatsThis(tr("Save File.\n\nSaves the datafile"));
	connect(fileSave, SIGNAL(activated()), view, SLOT(saveFile()));

	fileSaveAs = new QAction(tr("Save File As"), QPixmap(getResourcesDir()+"/icons/main/filesaveas.png"), tr("Save &as..."), 0, this);
	fileSaveAs->setStatusTip(tr("Saves the datafile under a new filename"));
	fileSaveAs->setWhatsThis(tr("Save As\n\nSaves the datafile under a new filename"));
	connect(fileSaveAs, SIGNAL(activated()), view, SLOT(saveFileAs()));

	fileUpdate = new QAction(tr("Update"), QPixmap(getResourcesDir()+"/icons/main/reload.png"), tr("Update"), QAccel::stringToKey("Ctrl+U"), this);
	fileUpdate->setStatusTip(tr("Updates datafile"));
	//fileUpdate->setWhatsThis(tr(""));
	connect(fileUpdate, SIGNAL(activated()), view, SLOT(update()));

	fileUpdateSelection = new QAction(tr("Update Selection"), QPixmap(getResourcesDir()+"/icons/main/reload.png"), tr("Update Selection"), 0, this);
	fileUpdateSelection->setStatusTip(tr("Updates selected datafile item(s)"));
	//fileUpdateSelection->setWhatsThis(tr(""));
	connect(fileUpdateSelection, SIGNAL(activated()), view, SLOT(updateSelection()));

	fileForceUpdate = new QAction(tr("Force Update"), QPixmap(getResourcesDir()+"/icons/main/reload.png"), tr("&Force Update"), QAccel::stringToKey("Ctrl+U"), this);
	fileForceUpdate->setStatusTip(tr("Forces updating of datafile"));
	//fileForceUpdate->setWhatsThis(tr(""));
	connect(fileForceUpdate, SIGNAL(activated()), view, SLOT(forceUpdate()));

	fileQuit = new QAction(tr("Exit"), QPixmap(getResourcesDir()+"/icons/main/exit.png"), tr("E&xit"), QAccel::stringToKey(tr("Ctrl+Q")), this);
	fileQuit->setStatusTip(tr("Quits the application"));
	fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
	connect(fileQuit, SIGNAL(activated()), this, SLOT(slotFileQuit()));

	//
	// Options Menu
	//
	optionsToolBar = new QAction(tr("Toolbar"), tr("Show Tool&bar"), 0, this, 0, true);
	optionsToolBar->setStatusTip(tr("Enables/disables the toolbar"));
	optionsToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
	connect(optionsToolBar, SIGNAL(toggled(bool)), this, SLOT(slotOptionsToolBar(bool)));
	connect(optionsToolBar, SIGNAL(toggled(bool)), prefs, SLOT(setToolbarOn(bool)));

	optionsStatusBar = new QAction(tr("Statusbar"), tr("Show &Statusbar"), 0, this, 0, true);
	optionsStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
	optionsStatusBar->setWhatsThis(tr("Statusbar\n\nEnables/disables the statusbar"));
	connect(optionsStatusBar, SIGNAL(toggled(bool)), this, SLOT(slotOptionsStatusBar(bool)));
	connect(optionsStatusBar, SIGNAL(toggled(bool)), prefs, SLOT(setStatusbarOn(bool)));

	optionsConfigure = new QAction (tr("Configure QGrabber"), QPixmap(getResourcesDir()+"/icons/main/configure.png"), tr("Configure QGrabber"), 0, this);
	optionsConfigure->setStatusTip(tr("Configures QGrabber"));
	//optionsConfigure->setWhatsThis(tr("Print File\n\nPrints out the datafile"));
	connect(optionsConfigure, SIGNAL(activated()), this, SLOT(slotOptionsConfigure()));

	optionsConfigureToolbars = new QAction (tr("Configure toolbars"), QPixmap(getResourcesDir()+"/icons/main/configuretoolbars.png"), tr("Configure toolbars"), 0, this);
	optionsConfigureToolbars->setStatusTip (tr("Configures toolbars"));
	connect(optionsConfigureToolbars, SIGNAL(activated()), this, SLOT(slotOptionsConfigureToolbars()));

	//
	// Help Menu
	//
	helpReadme = new QAction(tr("Readme"), QPixmap(getResourcesDir()+"/icons/main/help.png"), tr("&Readme"), 0, this);
	//helpReadme->setStatusTip(tr("About the application"));
	//helpReadme->setWhatsThis(tr("About\n\nAbout the application"));
	connect(helpReadme, SIGNAL(activated()), this, SLOT(slotHelpReadme()));

	helpLicense = new QAction(tr("License"), tr("&License"), 0, this);
	//helpLicense->setStatusTip(tr("About the application"));
	//helpLicense->setWhatsThis(tr("About\n\nAbout the application"));
	connect(helpLicense, SIGNAL(activated()), this, SLOT(slotHelpLicense()));

	helpAbout = new QAction(tr("About"), tr("&About..."), 0, this);
	helpAbout->setStatusTip(tr("About the application"));
	helpAbout->setWhatsThis(tr("About\n\nAbout the application"));
	connect(helpAbout, SIGNAL(activated()), this, SLOT(slotHelpAbout()));

	helpAboutQt = new QAction(tr("About Qt"), tr("About &Qt..."), 0, this);
	helpAboutQt->setStatusTip(tr("About the Qt"));
	helpAboutQt->setWhatsThis(tr("About\n\nAbout the Qt"));
	connect(helpAboutQt, SIGNAL(activated()), this, SLOT(slotHelpAboutQt()));
}

void QGrabberApp::initMenuBar()
{
	//
	// File Menu
	//
	fileMenu = new QPopupMenu(this);
	fileNew->addTo(fileMenu);
	fileOpen->addTo(fileMenu);

	fileMenu->insertSeparator();
	fileSave->addTo(fileMenu);
	fileSaveAs->addTo(fileMenu);

	fileMenu->insertSeparator();
	fileUpdate->addTo (fileMenu);
	fileUpdateSelection->addTo (fileMenu);
	fileForceUpdate->addTo (fileMenu);

	QStringList filePlugs = MenuFactory::menuKeys (MenuPlugin::File);
	if ( filePlugs.size() )
	{
		fileMenu->insertSeparator();
		for (QStringList::Iterator it = filePlugs.begin(); it != filePlugs.end(); it++)
		{
			MenuPlugin *plug = MenuFactory::create (*it);
			plug->menuAction()->addTo (fileMenu);
		}
	}

	fileMenu->insertSeparator();
	recentFilesMenu = new QPopupMenu (this);
	fileMenu->insertItem (tr("Re&cently opened files"), recentFilesMenu);
	connect (recentFilesMenu, SIGNAL(aboutToShow()), this, SLOT(setupRecentFilesMenu()));
	connect (recentFilesMenu, SIGNAL(activated(int)), this, SLOT(recentFileActivated(int)));

	fileMenu->insertSeparator();
	fileQuit->addTo(fileMenu);

	//
	// Options Menu
	//
	optionsMenu = new QPopupMenu(this);
	optionsMenu->insertItem ( tr("Tool&bars"), createDockWindowMenu() );
	//optionsToolBar->addTo (optionsMenu);
	optionsStatusBar->addTo (optionsMenu);

	QStringList optionsPlugs = MenuFactory::menuKeys (MenuPlugin::Options);
	if ( optionsPlugs.size() )
	{
		optionsMenu->insertSeparator();
		for (QStringList::Iterator it = optionsPlugs.begin(); it != optionsPlugs.end(); it++)
		{
			MenuPlugin *plug = MenuFactory::create (*it);
			plug->menuAction()->addTo (optionsMenu);
		}
	}

	optionsMenu->insertSeparator();
	//optionsConfigureToolbars->addTo (optionsMenu);
	optionsConfigure->addTo (optionsMenu);

	//
	// Plugins menu
	//
	pluginsMenu = 0;
	QStringList pluginsPlugs = MenuFactory::menuKeys (MenuPlugin::Plugins);
	if ( pluginsPlugs.size() )
	{
		pluginsMenu = new QPopupMenu(this);

		for (QStringList::Iterator it = pluginsPlugs.begin(); it != pluginsPlugs.end(); it++)
		{
			MenuPlugin *plug = MenuFactory::create (*it);
			plug->menuAction()->addTo (pluginsMenu);
		}
	}

	//
	// Help Menu
	//
	QStringList helpPlugs = MenuFactory::menuKeys (MenuPlugin::Help);
	if ( helpPlugs.size() )
	{
		for (QStringList::Iterator it = helpPlugs.begin(); it != helpPlugs.end(); it++)
		{
			MenuPlugin *plug = MenuFactory::create (*it);
			plug->menuAction()->addTo (helpMenu);
		}
		helpMenu->insertSeparator();
	}

	helpMenu = new QPopupMenu(this);
	helpReadme->addTo(helpMenu);
	helpLicense->addTo(helpMenu);

	helpMenu->insertSeparator();
	helpAbout->addTo(helpMenu);
	helpAboutQt->addTo(helpMenu);

	menuBar()->insertItem (tr("&File"), fileMenu);
	menuBar()->insertItem (tr("&Options"), optionsMenu);
	if ( pluginsMenu )
		menuBar()->insertItem (tr("&Plugins"), pluginsMenu);
	menuBar()->insertSeparator ();
	menuBar()->insertItem (tr("&Help"), helpMenu);
}

void QGrabberApp::initToolBar()
{
	fileToolbar = new QToolBar( tr("Main Toolbar"), this );
	fileNew->addTo(fileToolbar);
	fileOpen->addTo(fileToolbar);
	fileSave->addTo(fileToolbar);
	fileToolbar->addSeparator();
	optionsConfigure->addTo(fileToolbar);

	bool toolbar = Prefs::prefs()->toolbarOn ();

	optionsToolBar->setOn (true);

	if ( !toolbar )
		optionsToolBar->toggle ();
}

void QGrabberApp::initStatusBar()
{
	statusBar()->message(tr("Ready."), 2000);

	bool statusbar = Prefs::prefs()->statusbarOn ();

	optionsStatusBar->setOn (true);

	if ( !statusbar )
		optionsStatusBar->toggle ();
}

void QGrabberApp::initView()
{
	resize (640, 480);

	setCentralWidget ( new QWidget(this, "central_widget") );
	QVBoxLayout *formLayout = new QVBoxLayout (centralWidget(), 2, 5, "formLayout");

	QSplitter *hsplit = new QSplitter (centralWidget(), "hsplit");
	hsplit->setOrientation ( QSplitter::Horizontal );

	formLayout->addWidget (hsplit);

	view = new DatafileView (hsplit, "DatafileView");
	hsplit->setResizeMode ( view, QSplitter::FollowSizeHint );

	QSplitter *vsplit = new QSplitter ( hsplit, "vsplit" );
	vsplit->setOrientation ( QSplitter::Vertical );

	propView = new PropView ( vsplit, "PropView" );
	vsplit->setResizeMode ( propView, QSplitter::FollowSizeHint );

	drawfield = new DrawField (vsplit, "Drawfield");

	connect (view, SIGNAL(selectionChanged ( QListViewItem * )), propView, SLOT(displayItem ( QListViewItem * )));
	connect (view, SIGNAL(selectionChanged ( QListViewItem * )), drawfield, SLOT(displayItem ( QListViewItem * )));
	connect (propView, SIGNAL(updated ( QListViewItem * )), view, SLOT(updateSelection ( QListViewItem * )));

	connect (propView, SIGNAL(dirty()), this, SLOT(setDirty()));

	connect (view, SIGNAL(dirty(bool)), this, SLOT(setDirty(bool)));
	connect (this, SIGNAL(newFile()), view, SLOT(newFile()));
	connect (this, SIGNAL(openFile()), view, SLOT(openFile()));
	connect (this, SIGNAL(openFile(const QString &)), view, SLOT(openFile(const QString &)));
	connect (this, SIGNAL(saveFile()), view, SLOT(saveFile()));
	connect (view, SIGNAL(changeCaption(const QString &)), this, SLOT(setFileName(const QString &)));
	connect (view, SIGNAL(addRecentlyOpenedFile(const QString &)), this, SLOT(addRecentlyOpenedFile(const QString &)));
}

void QGrabberApp::setDirty ()
{
	setDirty (true);
}

void QGrabberApp::setDirty (bool d)
{
	dirty = d;

	setFileName (fileName);
}

void QGrabberApp::setupRecentFilesMenu ()
{
	recentFilesMenu->clear();
	int id = 0;

	QStringList lst = Prefs::prefs()->recentFiles ();
	for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
	{
		recentFilesMenu->insertItem ( *it, id );
		id++;
	}
}

void QGrabberApp::recentFileActivated (int id)
{
	if ( id != -1 ) {
		if ( maybeSave() )
			emit openFile( *Prefs::prefs()->recentFiles().at( id ) );
	}
}

void QGrabberApp::addRecentlyOpenedFile ( const QString &fn )
{
	QStringList lst = Prefs::prefs()->recentFiles();

	if ( lst.find( fn ) != lst.end() )
		return;	// file already in list
	if ( lst.count() >= 10 )
		lst.remove( lst.begin() );
	lst << fn;

	Prefs::prefs()->setRecentFiles ( lst );
}

void QGrabberApp::setFileName (const QString &f)
{
	fileName = f;

	QString cap = QString("%1%2 - QGrabber").arg(fileName).arg(dirty ? tr(" [modified]") : QString(""));
	QMainWindow::setCaption ( cap );
}

bool QGrabberApp::queryExit()
{
	int exit = QMessageBox::information(this, tr("Quit..."),
					tr("You have modified data.\nDo you want to save it?"),
					QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

	if (exit == QMessageBox::Yes)
	{	// save data
		emit saveFile();
	}

	return exit == QMessageBox::Yes || exit == QMessageBox::No;
}

void QGrabberApp::closeEvent (QCloseEvent *e)
{
	if ( maybeSave() )
		e->accept ();
	else
		e->ignore ();
}

bool QGrabberApp::maybeSave ()
{
	if ( dirty )
	{
		switch ( QMessageBox::information (this, tr("QGrabber"),
					tr("You have modified data.\nDo you want to save it?"),
					QMessageBox::Yes | QMessageBox::Default,
					QMessageBox::No,
					QMessageBox::Cancel | QMessageBox::Escape ) )
		{
			case QMessageBox::Cancel:
				return false;
			case QMessageBox::Yes:
				emit saveFile();
				break;
			case QMessageBox::No:
				break;
		}
	}

	return true;
}

void QGrabberApp::slotFileNew ()
{
	if ( maybeSave() )
		emit newFile();
}

void QGrabberApp::slotFileOpen ()
{
	if ( maybeSave() )
		emit openFile();
}

void QGrabberApp::slotFileQuit()
{
	statusBar()->message(tr("Exiting application..."));

	if ( maybeSave() )
		qApp->quit();

	statusBar()->message(tr("Ready."));
}


void QGrabberApp::slotOptionsToolBar(bool toggle)
{
	statusBar()->message(tr("Toggle toolbar..."));

	if (toggle == false)
		fileToolbar->hide();
	else
		fileToolbar->show();

	statusBar()->message(tr("Ready."));
}

void QGrabberApp::slotOptionsStatusBar(bool toggle)
{
	statusBar()->message(tr("Toggle statusbar..."));

	if (toggle == false)
		statusBar()->hide();
	else
		statusBar()->show();

	statusBar()->message(tr("Ready."));
}

void QGrabberApp::slotOptionsConfigure ()
{
	PrefsDialog *dlg = new PrefsDialog (this, "PrefsDialog", true);
	dlg->exec();
}

void QGrabberApp::slotOptionsConfigureToolbars ()
{
}

void QGrabberApp::slotHelpReadme()
{
	FileViewer *fv = new FileViewer (this);
	fv->setCaption (tr("Readme"));
	fv->setFile (getResourcesDir()+"/Readme.html", true);
	fv->exec ();
}

void QGrabberApp::slotHelpLicense()
{
	FileViewer *fv = new FileViewer (this);
	fv->setCaption (tr("License"));
	fv->setFile (getResourcesDir()+"/COPYING", false);
	fv->exec ();
}

void QGrabberApp::slotHelpAbout()
{
	AboutDialog *about = new AboutDialog (this);
	about->exec ();
}

void QGrabberApp::slotHelpAboutQt()
{
	QMessageBox::aboutQt (this, tr("QGrabber"));
}

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

