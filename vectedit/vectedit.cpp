/***************************************************************************
                          vectedit.cpp  -  description
                             -------------------
    begin                : Û¬‘ Ì¡   4 11:04:57 MSD 2002
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qaccel.h>
#include "vectedit.h"
#include "filesave.xpm"
#include "fileopen.xpm"
#include "filenew.xpm"

VectEditApp::VectEditApp()
{
  setCaption(tr("VectEdit " VERSION));

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initActions();
  initMenuBar();
  initToolBar();
  initStatusBar();

  initDoc();
  initView();

  viewToolBar->setOn(true);
  viewStatusBar->setOn(true);

  viewStatusBar->toggle ();
}

VectEditApp::~VectEditApp()
{
}

/** initializes all QActions of the application */
void VectEditApp::initActions(){

  QPixmap openIcon, saveIcon, newIcon;
  newIcon = QPixmap(filenew);
  openIcon = QPixmap(fileopen);
  saveIcon = QPixmap(filesave);


  fileNew = new QAction(tr("New File"), newIcon, tr("&New"), QAccel::stringToKey(tr("Ctrl+N")), this);
  fileNew->setStatusTip(tr("Creates a new document"));
  fileNew->setWhatsThis(tr("New File\n\nCreates a new document"));
  connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));

  fileOpen = new QAction(tr("Open File"), openIcon, tr("&Open..."), 0, this);
  fileOpen->setStatusTip(tr("Opens an existing document"));
  fileOpen->setWhatsThis(tr("Open File\n\nOpens an existing document"));
  connect(fileOpen, SIGNAL(activated()), this, SLOT(slotFileOpen()));

  fileOpenBackground = new QAction(tr("Open Background File"), openIcon, tr("&Open Background..."), 0, this);
  //fileOpenBackground->setStatusTip(tr("Opens an existing document"));
  //fileOpenBackground->setWhatsThis(tr("Open File\n\nOpens an existing document"));
  connect(fileOpenBackground, SIGNAL(activated()), this, SLOT(slotFileOpenBackground()));

  fileSave = new QAction(tr("Save File"), saveIcon, tr("&Save"), QAccel::stringToKey(tr("Ctrl+S")), this);
  fileSave->setStatusTip(tr("Saves the actual document"));
  fileSave->setWhatsThis(tr("Save File.\n\nSaves the actual document"));
  connect(fileSave, SIGNAL(activated()), this, SLOT(slotFileSave()));

  fileSaveAs = new QAction(tr("Save File As"), tr("Save &as..."), 0, this);
  fileSaveAs->setStatusTip(tr("Saves the actual document under a new filename"));
  fileSaveAs->setWhatsThis(tr("Save As\n\nSaves the actual document under a new filename"));
  connect(fileSaveAs, SIGNAL(activated()), this, SLOT(slotFileSaveAs()));

  fileClose = new QAction(tr("Close File"), tr("&Close"), QAccel::stringToKey(tr("Ctrl+W")), this);
  fileClose->setStatusTip(tr("Closes the actual document"));
  fileClose->setWhatsThis(tr("Close File\n\nCloses the actual document"));
  connect(fileClose, SIGNAL(activated()), this, SLOT(slotFileClose()));

  filePrint = new QAction(tr("Print File"), tr("&Print"), QAccel::stringToKey(tr("Ctrl+P")), this);
  filePrint->setStatusTip(tr("Prints out the actual document"));
  filePrint->setWhatsThis(tr("Print File\n\nPrints out the actual document"));
  connect(filePrint, SIGNAL(activated()), this, SLOT(slotFilePrint()));

  fileQuit = new QAction(tr("Exit"), tr("E&xit"), QAccel::stringToKey(tr("Ctrl+Q")), this);
  fileQuit->setStatusTip(tr("Quits the application"));
  fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(fileQuit, SIGNAL(activated()), this, SLOT(slotFileQuit()));

  editCut = new QAction(tr("Cut"), tr("Cu&t"), QAccel::stringToKey(tr("Ctrl+X")), this);
  editCut->setStatusTip(tr("Cuts the selected section and puts it to the clipboard"));
  editCut->setWhatsThis(tr("Cut\n\nCuts the selected section and puts it to the clipboard"));
  connect(editCut, SIGNAL(activated()), this, SLOT(slotEditCut()));

  editCopy = new QAction(tr("Copy"), tr("&Copy"), QAccel::stringToKey(tr("Ctrl+C")), this);
  editCopy->setStatusTip(tr("Copies the selected section to the clipboard"));
  editCopy->setWhatsThis(tr("Copy\n\nCopies the selected section to the clipboard"));
  connect(editCopy, SIGNAL(activated()), this, SLOT(slotEditCopy()));

  editPaste = new QAction(tr("Paste"), tr("&Paste"), QAccel::stringToKey(tr("Ctrl+V")), this);
  editPaste->setStatusTip(tr("Pastes the clipboard contents to actual position"));
  editPaste->setWhatsThis(tr("Paste\n\nPastes the clipboard contents to actual position"));
  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));

  viewToolBar = new QAction(tr("Toolbar"), tr("Tool&bar"), 0, this, 0, true);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  connect(viewToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewToolBar(bool)));

  viewStatusBar = new QAction(tr("Statusbar"), tr("&Statusbar"), 0, this, 0, true);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(tr("Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)), this, SLOT(slotViewStatusBar(bool)));

  helpAboutApp = new QAction(tr("About"), tr("&About..."), 0, this);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), this, SLOT(slotHelpAbout()));

}

void VectEditApp::initMenuBar()
{
  ///////////////////////////////////////////////////////////////////
  // MENUBAR

  ///////////////////////////////////////////////////////////////////
  // menuBar entry fileMenu
  fileMenu=new QPopupMenu();
  fileNew->addTo(fileMenu);
  fileOpen->addTo(fileMenu);
  fileOpenBackground->addTo(fileMenu);
  //fileClose->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileSave->addTo(fileMenu);
  fileSaveAs->addTo(fileMenu);
  //fileMenu->insertSeparator();
  //filePrint->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileQuit->addTo(fileMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry editMenu
  editMenu=new QPopupMenu();
  editCut->addTo(editMenu);
  editCopy->addTo(editMenu);
  editPaste->addTo(editMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry viewMenu
  viewMenu=new QPopupMenu();
  viewMenu->setCheckable(true);
  viewToolBar->addTo(viewMenu);
  viewStatusBar->addTo(viewMenu);

  ///////////////////////////////////////////////////////////////////
  // EDIT YOUR APPLICATION SPECIFIC MENUENTRIES HERE

  ///////////////////////////////////////////////////////////////////
  // menuBar entry helpMenu
  helpMenu=new QPopupMenu();
  helpAboutApp->addTo(helpMenu);

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
  menuBar()->insertItem(tr("&File"), fileMenu);
  //menuBar()->insertItem(tr("&Edit"), editMenu);
  menuBar()->insertItem(tr("&View"), viewMenu);
  menuBar()->insertSeparator();
  menuBar()->insertItem(tr("&Help"), helpMenu);

}

void VectEditApp::initToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // TOOLBAR
  fileToolbar = new QToolBar(this, "file operations");
  fileNew->addTo(fileToolbar);
  fileOpen->addTo(fileToolbar);
  fileSave->addTo(fileToolbar);
  //fileToolbar->addSeparator();
  //QWhatsThis::whatsThisButton(fileToolbar);

}

void VectEditApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  //STATUSBAR
  statusBar()->message(tr("Ready."), 2000);
}

void VectEditApp::initDoc()
{
   //doc=new VectEditDoc();
}

void VectEditApp::initView()
{
  ////////////////////////////////////////////////////////////////////
  // set the main widget here
  view=new VectEditView(this, "VectEditView");
  setCentralWidget(view);
}

bool VectEditApp::queryExit()
{
  int exit=QMessageBox::information(this, tr("Quit..."),
                                    tr("Do your really want to quit?"),
                                    QMessageBox::Ok, QMessageBox::Cancel);

  if (exit==1)
  {

  }
  else
  {

  };

  return (exit==1);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void VectEditApp::slotFileNew()
{
  statusBar()->message(tr("Creating new file..."));
  //doc->newDoc();
  view->slotClear ();
  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotFileOpen()
{
  statusBar()->message(tr("Opening file..."));

  QString fileName = QFileDialog::getOpenFileName(0,0,this);
  if (!fileName.isEmpty())
  {
    //doc->load(fileName);
    view->slotLoad (fileName);

    setCaption(fileName);
    QString message=tr("Loaded document: ")+fileName;
    statusBar()->message(message, 2000);
  }
  else
  {
    statusBar()->message(tr("Opening aborted"), 2000);
  }
}


void VectEditApp::slotFileOpenBackground()
{
  statusBar()->message(tr("Opening Background file..."));

  QString fileName = QFileDialog::getOpenFileName(0,0,this);
  if (!fileName.isEmpty())
  {
    //doc->load(fileName);
    view->slotLoadBackground (fileName);

    //setCaption(fileName);
    //QString message=tr("Loaded document: ")+fileName;
    //statusBar()->message(message, 2000);
  }
  else
  {
    statusBar()->message(tr("Opening aborted"), 2000);
  }
}


void VectEditApp::slotFileSave()
{
  statusBar()->message(tr("Saving file..."));
  //doc->save();
  view->slotSave ("");
  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotFileSaveAs()
{
  statusBar()->message(tr("Saving file under new filename..."));
  QString fn = QFileDialog::getSaveFileName(0, 0, this);
  if (!fn.isEmpty())
  {
    //doc->saveAs(fn);
    view->slotSave (fn);
  }
  else
  {
    statusBar()->message(tr("Saving aborted"), 2000);
  }

  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotFileClose()
{
  statusBar()->message(tr("Closing file..."));

  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotFilePrint()
{
  statusBar()->message(tr("Printing..."));
  QPrinter printer;
  if (printer.setup(this))
  {
    QPainter painter;
    painter.begin(&printer);

    ///////////////////////////////////////////////////////////////////
    // TODO: Define printing by using the QPainter methods here

    painter.end();
  };

  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotFileQuit()
{
  statusBar()->message(tr("Exiting application..."));
  ///////////////////////////////////////////////////////////////////
  // exits the Application
/*  if(doc->isModified())
  {
    if(queryExit())
    {
      qApp->quit();
    }
    else
    {

    };
  }
  else
  {
    qApp->quit();
  };
*/
  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));

  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotEditCopy()
{
  statusBar()->message(tr("Copying selection to clipboard..."));

  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotEditPaste()
{
  statusBar()->message(tr("Inserting clipboard contents..."));

  statusBar()->message(tr("Ready."));
}


void VectEditApp::slotViewToolBar(bool toggle)
{
  statusBar()->message(tr("Toggle toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off

  if (toggle== false)
  {
    fileToolbar->hide();
  }
  else
  {
    fileToolbar->show();
  };

  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotViewStatusBar(bool toggle)
{
  statusBar()->message(tr("Toggle statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off

  if (toggle == false)
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  statusBar()->message(tr("Ready."));
}

void VectEditApp::slotHelpAbout()
{
  QMessageBox::about(this,tr("About..."),
                      tr("VectEdit\nVersion " VERSION "\n(c) 2002 by Michail Pishchagin") );
}

