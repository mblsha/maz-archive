/***************************************************************************
                          preferencesimpl.h  -  description
                             -------------------
    begin                : Sat Apr 6 2002
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

#ifndef PREFERENCESIMPL_H
#define PREFERENCESIMPL_H

#include <preferences.h>

#include <qwidget.h>

/**
  *@author Michail Pishchagin
  */

class PreferencesImpl : public Preferences
{
   Q_OBJECT

private:
   bool testPressed;

public: 
   PreferencesImpl ( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );

signals:
   void dispatchMusic (const char *);

public slots:
   virtual void browseFile();

protected slots:
   virtual void testAlarm();
};

#endif
