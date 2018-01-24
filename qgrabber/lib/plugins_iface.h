/***************************************************************************
                          plugins_iface.h  -  description
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

#ifndef PLUGINS_IFACE_H
#define PLUGINS_IFACE_H

#include <private/qcom_p.h>

class QString;

class ObjectPlugin;
class IOPlugin;
class MenuPlugin;

// {004d415a-504c-5547-4752-41420}
// {   M A Z- P L- U G- G R- A B }
#ifndef IID_QGrabberPlugin
#define IID_QGrabberPlugin QUuid(0x004d415a, 0x504c, 0x5547, 0x47, 0x52, 0x41, 0x42, 0x00, 0x00, 0x00, 0xa02)
#endif

struct QGrabberPluginInterface : public QFeatureListInterface
{
	virtual ObjectPlugin *createObject ( const QString& plug ) = 0;
	virtual IOPlugin     *createIO     ( const QString& plug ) = 0;
	virtual MenuPlugin   *createMenu   ( const QString& plug ) = 0;

	virtual QStringList keysObject () const = 0;
	virtual QStringList keysIO     () const = 0;
	virtual QStringList keysMenu   () const = 0;
};

#endif

