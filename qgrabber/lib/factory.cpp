/***************************************************************************
                          factory.cpp  -  description
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

#include "factory.h"
#include "plugins.h"
#include "plugins_iface.h"
#include <private/qpluginmanager_p.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qapplication.h>
#include <qmap.h>

#include "common.h"

#include "dat_data.h"
#include "dat_file.h"

static void printKeys (const char *prefix, QStringList k)
{
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
		qWarning ("%s::%s", prefix, (*it).latin1());
}

/*****************************************************************************
 *
 * Class QGrabberFactoryPrivate
 *
 *****************************************************************************/

class QGrabberFactoryPrivate : public QObject
{
private:
	static QPluginManager<QGrabberPluginInterface> *manager;

	QMap<QString, ObjectPlugin*> plugsObject;
	QMap<QString, IOPlugin*>     plugsIO;
	QMap<QString, MenuPlugin*>   plugsMenu;

public:
	QGrabberFactoryPrivate();
	~QGrabberFactoryPrivate();

	QStringList keys();

	QStringList keysObject();
	QStringList keysIO();
	QStringList keysMenu();

	ObjectPlugin *createObject(const QString &key);
	IOPlugin     *createIO(const QString &key);
	MenuPlugin   *createMenu(const QString &key);
};

QPluginManager<QGrabberPluginInterface> *QGrabberFactoryPrivate::manager = 0;
static QGrabberFactoryPrivate *instance = 0;

QGrabberFactoryPrivate::QGrabberFactoryPrivate()
			: QObject( qApp )
{
	QStringList path;
	path += getResourcesDir();
	path += getHomeDir();

	manager = new QPluginManager<QGrabberPluginInterface>( IID_QGrabberPlugin, path, "/plugins", FALSE );
}

QGrabberFactoryPrivate::~QGrabberFactoryPrivate()
{
	delete manager;
	manager = 0;

	instance = 0;
}

QStringList QGrabberFactoryPrivate::keys ()
{
	QStringList list;

	list = manager->featureList();

	return list;
}

QStringList QGrabberFactoryPrivate::keysObject ()
{
	QStringList list;

	QStringList k = keys ();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		QInterfacePtr<QGrabberPluginInterface> iface;
		//manager->queryInterface( (*it).lower(), &iface );
		manager->queryInterface( *it, &iface );

		if ( iface )
			list += iface->keysObject();
	}

	list += "Datafile";
	list += "Default";

	list.sort ();
	return list;
}

QStringList QGrabberFactoryPrivate::keysIO ()
{
	QStringList list;

	QStringList k = keys ();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		QInterfacePtr<QGrabberPluginInterface> iface;
		manager->queryInterface( *it, &iface );

		if ( iface )
			list += iface->keysIO();
	}

	list += "Datafile";
	list += "Default";

	list.sort ();
	return list;
}

QStringList QGrabberFactoryPrivate::keysMenu ()
{
	QStringList list;

	QStringList k = keys ();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		QInterfacePtr<QGrabberPluginInterface> iface;
		manager->queryInterface( *it, &iface );

		if ( iface )
			list += iface->keysMenu();
	}

	list.sort ();
	return list;
}

ObjectPlugin *QGrabberFactoryPrivate::createObject (const QString &key)
{
	ObjectPlugin *ret = 0;
	QString plug = key.lower();

	if ( plugsObject[plug] )
		return plugsObject[plug]; // plugin already created

	if (plug == "datafile")
		ret = new ObjectFile;
	else
	if (plug == "default")
		ret = new ObjectData;

	if(!ret) {
		QStringList k = keys();
		for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
		{
			QInterfacePtr<QGrabberPluginInterface> iface;
			manager->queryInterface( *it, &iface );

			if ( iface )
				ret = iface->createObject( plug );

			if ( ret )
				break;
		}
	}

	if(ret)
	{
		ret->setName (key);
		plugsObject[plug] = ret; // add plugin to list
	}

	return ret;
}

IOPlugin *QGrabberFactoryPrivate::createIO (const QString &key)
{
	IOPlugin *ret = 0;
	QString plug = key.lower();

	if ( plugsIO[plug] )
		return plugsIO[plug]; // plugin already created

	if (plug == "datafile")
		ret = new IOFile;
	else
	if (plug == "default")
		ret = new IOData;

	if(!ret) {
		QStringList k = keys();
		for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
		{
			QInterfacePtr<QGrabberPluginInterface> iface;
			manager->queryInterface( *it, &iface );

			if ( iface )
				ret = iface->createIO( plug );

			if ( ret )
				break;
		}
	}

	if(ret)
	{
		ret->setName (key);
		plugsIO[plug] = ret; // add plugin to list
	}

	return ret;
}

MenuPlugin *QGrabberFactoryPrivate::createMenu (const QString &key)
{
	MenuPlugin *ret = 0;
	QString plug = key.lower();

	if ( plugsMenu[plug] )
		return plugsMenu[plug]; // plugin already created

	if(!ret) {
		QStringList k = keys();
		for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
		{
			QInterfacePtr<QGrabberPluginInterface> iface;
			manager->queryInterface( *it, &iface );

			if ( iface )
				ret = iface->createMenu( plug );

			if ( ret )
				break;
		}
	}

	if(ret)
	{
		ret->setName (key);
		plugsMenu[plug] = ret; // add plugin to list
	}

	return ret;
}

/*****************************************************************************
 *
 * Class ObjectFactory
 *
 *****************************************************************************/

ObjectPlugin *ObjectFactory::create (const QString &key)
{
	if ( !instance )
		instance = new QGrabberFactoryPrivate;

	return instance->createObject (key);
}

QStringList ObjectFactory::keys ()
{
	if ( !instance )
		instance = new QGrabberFactoryPrivate;

	return instance->keysObject ();
}

ObjectPlugin *ObjectFactory::create (const int type)
{
	ObjectPlugin *ret = 0;

	QStringList k = keys ();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		if ( (*it).lower() == "default" )
			continue;

		ret = create (*it);

		if ( ret && ret->typeSupported(type) )
			break;
		else
			ret = 0;
	}

	if ( !ret )
	{
		ret = create ("default");

		if ( !ret )
			qWarning ("ObjectFactory::create (const int): Cannot create plugin!");
	}

	return ret;
}

/*****************************************************************************
 *
 * Class IOFactory
 *
 *****************************************************************************/

IOPlugin *IOFactory::create (const QString &key)
{
	if ( !instance )
		instance = new QGrabberFactoryPrivate;

	return instance->createIO (key);
}

QStringList IOFactory::keys ()
{
	if ( !instance )
		instance = new QGrabberFactoryPrivate;

	return instance->keysIO ();
}

IOPlugin *IOFactory::create (const int type)
{
	IOPlugin *ret = 0;

	QStringList k = keys ();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		if ( (*it).lower() == "default" )
			continue;

		ret = create (*it);

		if ( ret && ret->typeSupported(type) )
			break;
		else
			ret = 0;
	}

	if ( !ret )
	{
		ret = create ("default");

		if ( !ret )
			qWarning ("IOFactory::create (const int): Cannot create plugin!");
	}

	return ret;
}

/*****************************************************************************
 *
 * Class MenuFactory
 *
 *****************************************************************************/

MenuPlugin *MenuFactory::create (const QString &key)
{
	if ( !instance )
		instance = new QGrabberFactoryPrivate;

	return instance->createMenu (key);
}

QStringList MenuFactory::keys ()
{
	if ( !instance )
		instance = new QGrabberFactoryPrivate;

	return instance->keysMenu ();
}

QStringList MenuFactory::menuKeys ()
{
	QStringList ret;

	QStringList k = keys();
	printKeys ("MenuFactory::keys", ret);
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		MenuPlugin *plug = create ( *it );

		if ( plug && plug->menuAction() )
		{
			ret << *it;
		}
	}

	return ret;
}

QStringList MenuFactory::contextKeys ()
{
	QStringList ret;

	QStringList k = keys();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		MenuPlugin *plug = create ( *it );

		if ( plug && !plug->contextMenuText().isEmpty() )
		{
			ret << *it;
		}
	}

	return ret;
}

QStringList MenuFactory::contextKeys (const int type)
{
	QStringList ret;

	QStringList k = contextKeys();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		MenuPlugin *plug = create ( *it );

		if ( plug && plug->contextMenuTypeSupported( type ) )
		{
			ret << *it;
		}
	}

	return ret;
}

QStringList MenuFactory::menuKeys (int dest)
{
	QStringList ret;

	QStringList k = menuKeys();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		MenuPlugin *plug = create ( *it );

		if ( plug && (plug->menuAddTo() == dest) )
		{
			ret << *it;
		}
	}

	return ret;
}
