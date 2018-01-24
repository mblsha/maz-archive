/***************************************************************************
                          mazlarm.cpp  -  description
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

#include "mazlarm.h"
#include "preferencesimpl.h"
#include "alarm.h"

#include <fmod_errors.h>

#include <qcombobox.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qmessagebox.h>
#include <qcheckbox.h>

#include <kmessagebox.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kconfig.h>
#include <kapp.h>

#include <stdio.h>

#include "mazlarm/normal_clock.xpm"
#include "mazlarm/disabled_clock.xpm"

MAZlarm::MAZlarm() : KSystemTray ()
{
   setBackgroundMode (X11ParentRelative);

   timer = NULL;

   menu = new KPopupMenu ();
   //menu->setCheckable (true);

   KConfig *config = kapp->config();

   config->setGroup ("General");
   volume = config->readNumEntry ("volume", 255);
   alarmMessage = config->readEntry ("message", "Place your message here");

   config->setGroup ("FMOD");
   fmodOutput = config->readNumEntry ("output", 1);
   fmodDriver = config->readNumEntry ("driver", 0);

   config->setGroup ("Alarm");
   alarmFile = config->readPathEntry ("file", "");
   alarmTime.setHMS (config->readNumEntry ("timeHour", 0), config->readNumEntry ("timeMinute", 0), 0);
   fileIsApp = config->readNumEntry ("fileIsApplication", 0);

   // setup help menu
   helpMenu = new KHelpMenu (this, KGlobal::instance()->aboutData(), false);
   KPopupMenu *helpMnu = helpMenu->menu();

   // create popup menu
   menu->insertTitle(kapp->miniIcon(), KGlobal::instance()->aboutData()->programName());
   menu->insertItem (SmallIcon("configure"), i18n("&Configure..."), this, SLOT(config()));
   menu->insertSeparator ();
   menu->insertItem(SmallIconSet("help"), i18n("&Help"), helpMnu);
   //menu->insertItem (i18n("&Help"), this, SLOT(help()));
   //menu->insertItem (i18n("&About..."), this, SLOT(about()));
   menu->insertSeparator ();
   menu->insertItem (SmallIcon("exit"), i18n("Quit"), kapp, SLOT(quit()));

   normalPixmap = new QPixmap ((const char**)normal_clock);
   disabledPixmap = new QPixmap ((const char**)disabled_clock);

   initFmod ();

   enabled = true;
   alarmOver = true;

   timer = timer2 = NULL;

   start ();
}

MAZlarm::~MAZlarm()
{
   playFile (NULL);

   delete menu;
   delete normalPixmap;
   delete disabledPixmap;
}


void MAZlarm::mousePressEvent (QMouseEvent *event)
{
   if (event->button() == 1)
   {
      enabled = !enabled;
      alarmOver = true;

      start ();
      paintEvent (NULL);
   }
   else
   {
      menu->popup (QCursor::pos());
   }
}


void MAZlarm::paintEvent (QPaintEvent *)
{
   QPixmap *pm;

   if (enabled)
      pm = normalPixmap;
   else
      pm = disabledPixmap;

   QPainter p;
   p.begin (this);
   int x = 1 + (12 - pm->width()/2);
   int y = 1 + (12 - pm->height()/2);
   p.drawPixmap (x , y, *pm);
   p.end ();
}

void MAZlarm::initFmod ()
{
   if (FSOUND_GetVersion() < FMOD_VERSION)
   {
      QString err = i18n("You are using the wrong FMOD.DLL version!\n") +
                    i18n("You should be using FMOD %.02f").arg(FMOD_VERSION);

      KMessageBox::error (0, err, i18n("Error!"));
      return;
   }

   switch ( fmodOutput )
   {
#ifdef _WIN32
      case '1' :  FSOUND_SetOutput (FSOUND_OUTPUT_DSOUND);
                  break;
      case '2' :  FSOUND_SetOutput (FSOUND_OUTPUT_WINMM);
                  break;
      case '3' :  FSOUND_SetOutput (FSOUND_OUTPUT_A3D);
                  break;
#elif defined(__linux__)
      case '1' :  FSOUND_SetOutput (FSOUND_OUTPUT_OSS);
                  break;
      case '2' :  FSOUND_SetOutput (FSOUND_OUTPUT_ESD);
                  break;
      case '3' :  FSOUND_SetOutput (FSOUND_OUTPUT_ALSA);
                  break;
#endif
      case '4' :  FSOUND_SetOutput (FSOUND_OUTPUT_NOSOUND);
                  break;
   }

   FSOUND_SetDriver ( fmodDriver );

   if ( !FSOUND_Init (44100, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH) )
   {
      QString err = i18n("FMOD cannot be initialized\n") +
                    FMOD_ErrorString ( FSOUND_GetError () );

      KMessageBox::error (0, err, i18n("Error!"));
      return;
   }

   mod    = NULL;
   stream = NULL;
}

void MAZlarm::playFile (const char *file)
{
   //
   // Stop the music
   //
   if (mod || stream)
   {
      if (mod)
      {
         FMUSIC_StopSong (mod);
         FMUSIC_FreeSong (mod);
      }

      if (stream)
      {
         FSOUND_Stream_Stop  (stream);
         FSOUND_Stream_Close (stream);
      }

      mod    = NULL;
      stream = NULL;
   }

   //
   // Exit, if no filename specified
   //
   if (!file)
      return;

   //
   // Load either module or stream file
   //
   mod = FMUSIC_LoadSong (file);

   if (!mod)
      stream = FSOUND_Stream_OpenFile (file, FSOUND_NORMAL | FSOUND_2D |
 FSOUND_LOOP_NORMAL, 0);

   if ( mod )
   {
      FMUSIC_SetMasterVolume  ( mod, volume / (FMUSIC_GetType(mod) ==
 FMUSIC_TYPE_IT ? 2 : 1) );
      FMUSIC_SetPanSeperation ( mod, 0.85f );

      if ( FMUSIC_PlaySong (mod) )
         return;
   }
   else if ( stream )
   {
      FSOUND_SetVolume ( FSOUND_ALL, volume );

      if ( (fsound_channel = FSOUND_Stream_Play (FSOUND_FREE, stream)) >= 0 )
      {
         return;
      }
   }
   else
   {
      QString err = i18n("FMOD Error:\n") + file + "\n" +
                  FMOD_ErrorString ( FSOUND_GetError () );
      KMessageBox::error (0, err, i18n("Error!"));

      return;
   }

   QString err = i18n("FMOD Error:\n") +
                 FMOD_ErrorString ( FSOUND_GetError () );
   KMessageBox::error (0, err, i18n("Error!"));
}

void MAZlarm::stop ()
{
   if (timer)
   {
      timer->stop ();

      delete timer;
      timer = NULL;
   }
}

void MAZlarm::start ()
{
   stop ();

   if (enabled)
   {
      timer = new QTimer (this);
      connect (timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
      timer->start (30*1000, FALSE); // do the time check every 30 seconds

      char buf[64];
      sprintf (buf, " - %d:%02d", alarmTime.hour (), alarmTime.minute ());

      setToolTip (i18n("MAZlarm") + buf);
   }
   else if (!alarmOver)
      setToolTip (i18n("MAZlarm") + " - " + i18n("Temporarily disabled"));
   else
      setToolTip (i18n("MAZlarm") + " - " + i18n("Disabled"));

   paintEvent (NULL);
}

void MAZlarm::timer2Event ()
{
   enabled = true;
   if (alarmOver)
   {
      alarmOver = false;
      start ();
   }

   paintEvent (NULL);
}

void MAZlarm::timerEvent ()
{
   QTime currentTime;
   currentTime = QTime::currentTime ();

   if (alarmTime.hour ()   == currentTime.hour () &&
       alarmTime.minute () == currentTime.minute ())
   {
      stop ();

      enabled   = false;
      alarmOver = false;

      //
      // Eliminate "double alarm"!
      //
      timer2 = new QTimer (this);
      connect (timer2, SIGNAL(timeout()), this, SLOT(timer2Event()));
      timer2->start (60*1000, TRUE); // restore the state in 1 minute

      paintEvent (NULL);
      setToolTip (i18n("MAZlarm") + " - " + i18n("Temporarily disabled"));

      if ( !fileIsApp )
      {
         doAlarm ();

         //
         // Display "ALARM!" dialog
         //
         Alarm *alarm = new Alarm (0, "alarm", true);

         alarm->alarmVolume->setValue (volume);
         alarm->alarmMessage->setText (alarmMessage);

         connect (alarm->alarmVolume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));

         alarm->exec ();
      }
      else
      {
         system ( alarmFile );
      }

      alarmOver = true;

      doAlarm (false);
      start ();
   }
}

void MAZlarm::config (void)
{
   PreferencesImpl *prefs = new PreferencesImpl (0, "config", true);

   //
   // Set up the current values
   //
   prefs->alarmVolume->setValue (volume);
   int oldVolume = volume;
   connect (prefs->alarmVolume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
   prefs->alarmFile->setText (alarmFile);
   prefs->hour->setValue (alarmTime.hour ());
   prefs->minute->setValue (alarmTime.minute ());
   prefs->alarmMessage->setText (alarmMessage);
   prefs->fileIsApplication->setChecked ( fileIsApp );

#ifdef _WIN32 // this is very unlikely, but who knows :-)
   prefs->fmodOutput->insertItem ( tr("Direct Sound") );
   prefs->fmodOutput->insertItem ( tr("Windows Media Waveout") );
   prefs->fmodOutput->insertItem ( tr("A3D") );
#elif defined(__linux__)
   prefs->fmodOutput->insertItem ( tr("Open Sound System") );
   prefs->fmodOutput->insertItem ( tr("Elightment Sound Daemon") );
   prefs->fmodOutput->insertItem ( tr("Alsa Sound System") );
#endif
   prefs->fmodOutput->insertItem ( tr("No Sound") );

   prefs->fmodOutput->setCurrentItem ( fmodOutput-1 );

   for (int i=0; i < FSOUND_GetNumDrivers(); i++)
   {
      QString str;
      str.sprintf ("%s", FSOUND_GetDriverName (i) );
      prefs->fmodDriver->insertItem ( str );
   }
   prefs->fmodDriver->setCurrentItem ( fmodDriver );

   QObject::connect (prefs, SIGNAL(dispatchMusic(const char *)), this,
 SLOT(playMusic(const char *)));

   if (prefs->exec () == QDialog::Accepted)
   {
      //
      // Apply the prefs
      //
      volume = prefs->alarmVolume->value ();
      alarmFile = prefs->alarmFile->text ();
      alarmMessage = prefs->alarmMessage-> text ();
      alarmTime.setHMS (prefs->hour->value (), prefs->minute->value (), 0);
      fileIsApp = prefs->fileIsApplication->isChecked ();

      fmodOutput = prefs->fmodOutput->currentItem () + 1;
      fmodDriver = prefs->fmodDriver->currentItem ();
      initFmod ();

      //
      // Save the prefs
      //
      KConfig *config = kapp->config ();
      config->setGroup ("General");
      config->writeEntry ("volume", volume);
      config->writeEntry ("message", alarmMessage);

      config->setGroup ("FMOD");
      config->writeEntry ("output", fmodOutput);
      config->writeEntry ("driver", fmodDriver);

      config->setGroup ("Alarm");
      config->writeEntry ("file", alarmFile);
      config->writeEntry ("timeHour", alarmTime.hour());
      config->writeEntry ("timeMinute", alarmTime.minute());
      config->writeEntry ("fileIsApplication", fileIsApp);
      config->sync ();
   }
   else
   {
      initFmod (); // stop the sound
      setVolume ( oldVolume );
   }
   start ();
}

void MAZlarm::setToolTip (const QString &text)
{
   if (tooltip == text)
      return;

   tooltip = text;
   QToolTip::remove (this);
   QToolTip::add    (this, tooltip);
}

void MAZlarm::doAlarm (bool on, const char *file)
{
   if (on)
   {
      if (file)
         playFile (file);
      else if (!alarmFile.isEmpty ())
         playFile (alarmFile);
   }
   else
   {
      playFile (NULL);
   }
}

void MAZlarm::setVolume (int newVolume)
{
   volume = newVolume;

   if (mod)
      FMUSIC_SetMasterVolume ( mod, volume / (FMUSIC_GetType(mod) ==
 FMUSIC_TYPE_IT ? 2 : 1) );
   else if (stream)
      FSOUND_SetVolume ( FSOUND_ALL, volume );
}

void MAZlarm::playMusic (const char *file)
{
   if (file)
      doAlarm (true, file);
   else
      doAlarm (false);
}

