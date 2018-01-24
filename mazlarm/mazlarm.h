/***************************************************************************
                          mazlarm.h  -  description
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

#ifndef MAZLARM_H
#define MAZLARM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fmod.h>

#include <kapp.h>
#include <kaboutdata.h>
#include <kpopupmenu.h>
#include <khelpmenu.h>

#include <qdatetime.h>
#include <qtimer.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <ksystemtray.h>

extern KAboutData *aboutData;

class MAZlarm : public KSystemTray
{
   Q_OBJECT

private:
   QPixmap *normalPixmap, *disabledPixmap;
   KPopupMenu *menu;
   QString tooltip;
   KHelpMenu *helpMenu;

   int fmodDriver, fmodOutput;
   bool fileIsApp;
   QString alarmMessage;
   QString alarmFile;
   QTime alarmTime;
   QTimer *timer, *timer2; // timer2 is for eliminating "double alarm" feature

   bool alarmOver;

   bool enabled;

   int volume;

   FSOUND_STREAM *stream;
   int fsound_channel;
   FMUSIC_MODULE *mod;

   void initFmod ();
   void playFile (const char *file);
   void doAlarm (bool on = true, const char *file = NULL);

private slots:
   void config ();
   void setToolTip (const QString &text);

   void timerEvent ();
   void timer2Event ();

public slots:
   void setVolume (int);     // 0 - 255
   void playMusic (const char *);

protected:
   void paintEvent (QPaintEvent *);
   void mousePressEvent (QMouseEvent *);

   void start ();
   void stop ();

public:
   MAZlarm ();
   ~MAZlarm ();
};

#endif
