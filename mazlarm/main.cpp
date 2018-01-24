/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : ‘Œ ·–“  5 17:21:05 MSD 2002
    copyright            : (C) 2002 by Michail Pishchagin
    email                : mblsha@rambler.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <klocale.h>
#include <kapp.h>
#include <kwin.h>

#include "mazlarm.h"

static const char *description =
	I18N_NOOP("MAZlarm");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

KAboutData *aboutData;

int main(int argc, char *argv[])
{
   KAboutData about_data ( "mazlarm", I18N_NOOP("MAZlarm"),
                            VERSION, description, KAboutData::License_GPL,
                            "(c) 2002, Michail Pishchagin", 0, 0, "mblsha@users.sourceforge.net");
   about_data.addAuthor ("Michail Pishchagin",0, "mblsha@users.sourceforge.net");
   aboutData = &about_data;

   KCmdLineArgs::init ( argc, argv, aboutData );
   KCmdLineArgs::addCmdLineOptions ( options ); // Add our own options.

   KApplication app;

   MAZlarm mazlarm;
   KWin::setSystemTrayWindowFor(mazlarm.winId(),0);
   mazlarm.show();

   app.setTopWidget(&mazlarm);
   KWin::appStarted();

   return app.exec();
}
