/***************************************************************************
                          plugins_p.cpp  -  description
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

#include "plugins_p.h"

#include "plugins_iface.h"
#include <qobjectcleanuphandler.h>
#include "plugins.h"

/*****************************************************************************
 *
 * Class QGrabberPlugin
 *
 *****************************************************************************/

class QGrabberPluginPrivate : public QGrabberPluginInterface, public QLibraryInterface
{
public:
	QGrabberPluginPrivate( QGrabberPlugin *p )
		: plugin( p )
	{
	}

	virtual ~QGrabberPluginPrivate();

	QRESULT queryInterface( const QUuid &iid, QUnknownInterface **iface );
	Q_REFCOUNT;

	QStringList featureList() const;

	ObjectPlugin *createObject ( const QString& plug );
	IOPlugin     *createIO     ( const QString& plug );
	MenuPlugin   *createMenu   ( const QString& plug );

	QStringList keysObject () const;
	QStringList keysIO     () const;
	QStringList keysMenu   () const;

	bool init();
	void cleanup();
	bool canUnload() const;

private:
	QGrabberPlugin *plugin;
	QObjectCleanupHandler plugs;
};

QRESULT QGrabberPluginPrivate::queryInterface( const QUuid &iid, QUnknownInterface **iface )
{
	*iface = 0;

	if ( iid == IID_QUnknown )
		*iface = (QGrabberPluginInterface*)this;
	else if ( iid == IID_QFeatureList )
		*iface = (QFeatureListInterface*)this;
	else if ( iid == IID_QGrabberPlugin )
		*iface = (QGrabberPluginInterface*)this;
	else if ( iid == IID_QLibrary )
		*iface = (QLibraryInterface*) this;
	else
		return QE_NOINTERFACE;

	(*iface)->addRef();
	return QS_OK;
}

QGrabberPluginPrivate::~QGrabberPluginPrivate()
{
	delete plugin;
}

QStringList QGrabberPluginPrivate::featureList() const
{
	return plugin->keys();
}

QStringList QGrabberPluginPrivate::keysObject() const
{
	return plugin->keysObject();
}

QStringList QGrabberPluginPrivate::keysIO() const
{
	return plugin->keysIO();
}

QStringList QGrabberPluginPrivate::keysMenu() const
{
	return plugin->keysMenu();
}

ObjectPlugin *QGrabberPluginPrivate::createObject( const QString &key )
{
	ObjectPlugin *st = plugin->createObject( key );
	plugs.add( st );
	return st;
}

IOPlugin *QGrabberPluginPrivate::createIO( const QString &key )
{
	IOPlugin *st = plugin->createIO( key );
	plugs.add( st );
	return st;
}

MenuPlugin *QGrabberPluginPrivate::createMenu( const QString &key )
{
	MenuPlugin *st = plugin->createMenu( key );
	plugs.add( st );
	return st;
}

bool QGrabberPluginPrivate::init()
{
	return TRUE;
}

void QGrabberPluginPrivate::cleanup()
{
	plugs.clear();
}

bool QGrabberPluginPrivate::canUnload() const
{
    return plugs.isEmpty();
}


QGrabberPlugin::QGrabberPlugin()
			: QGPlugin( (QGrabberPluginInterface*)(d = new QGrabberPluginPrivate( this )) )
{
}

QGrabberPlugin::~QGrabberPlugin()
{
	// don't delete d, as this is deleted by d
}
