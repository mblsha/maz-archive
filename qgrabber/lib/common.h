/***************************************************************************
                          common.h  -  description
                             -------------------
    begin                : þÔ× á×Ç 22 2002
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

#ifndef COMMON_H
#define COMMON_H

#include <qobject.h>

#include "export.h"

#include "version.h"

#ifdef DEBUG
#	define VERSION		APP_VERSION "debug"
#else
#	define VERSION		APP_VERSION
#endif

class QString;

EXPORT QString getResourcesDir();
EXPORT QString getHomeDir();

#endif

