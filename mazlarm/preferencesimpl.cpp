/***************************************************************************
                          preferencesimpl.cpp  -  description
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

#include "preferencesimpl.h"

#include <qpushbutton.h>
#include <qlineedit.h>
#include <qfiledialog.h>

#include <klocale.h>

PreferencesImpl::PreferencesImpl ( QWidget* parent, const char* name,
      bool modal, WFlags fl ) : Preferences( parent, name, modal, fl)
{
   testPressed = false;
}

void PreferencesImpl::browseFile ()
{
   QString s( QFileDialog::getOpenFileName( QString::null, i18n("Music (*.wav *.mp3 *.ogg)"), 0, i18n("Select Music file") ) );
   if ( s.isEmpty() )
      return;

   alarmFile->setText (s);
}

void PreferencesImpl::testAlarm ()
{
   if (!testPressed)
   {
      alarmTest->setText ( i18n("&Stop") );

      emit dispatchMusic (alarmFile->text ());
   }
   else
   {
      alarmTest->setText ( i18n("&Test") );

      emit dispatchMusic (NULL);
   }

   testPressed = !testPressed;
}
