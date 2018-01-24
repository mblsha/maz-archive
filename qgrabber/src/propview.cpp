/***************************************************************************
                          proptable.cpp  -  description
                             -------------------
    begin                : Wed Apr 10 2002
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

#include "alleg.h"

#include <qlistview.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include <stdio.h>

#include "propview.h"
#include "plugins.h"
#include "datafileitem.h"

#include "prefs.h"
#include "edititem.h"

/*****************************************************************************
 *
 * Class PropViewItem
 *
 *****************************************************************************/

class PropViewItem : public QListViewItem
{
private:
	DATAFILE *data;
	int      type;

public:
	PropViewItem (QListView *parent, int type, DATAFILE *data);
	~PropViewItem ();

	QString text ( int column ) const;

	QString getType () const;
	QString getValue () const;
	void setValue (QString val);
};

PropViewItem::PropViewItem (QListView *parent, int _type, DATAFILE *_data)
			: QListViewItem (parent)
{
	data = _data;
	type = _type;
}

PropViewItem::~PropViewItem ()
{
}

QString PropViewItem::getType () const
{
	char buf[16];
	sprintf (buf, "%c%c%c%c", type >> 24, type >> 16, type >> 8, type);

	QString str = buf;
	return str;
}

QString PropViewItem::getValue () const
{
	return QString::fromUtf8( get_datafile_property (data, type) );
}

void PropViewItem::setValue (QString value)
{
	// nasty MSVC trick
	QCString utf8 = value.utf8();
	char *c = new char[ utf8.size() ];
	strcpy ( c, (const char *)utf8 );
	datedit_set_property (data, type, (AL_CONST char *)c );
	delete[] c;
}

QString PropViewItem::text ( int column ) const
{
	if ( column == 0 )
		return getType ();
	else
		return getValue ();
}

/*****************************************************************************
 *
 * Class PropView
 *
 *****************************************************************************/

PropView::PropView (QWidget *parent, const char *name)
			: QListView (parent, name)
{
	setSizePolicy ( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

	addColumn( tr("Type") );
	addColumn( tr("Value") );

	item = NULL;
	propItem = NULL;

	menu = 0;

	//setSelectionMode( QListView::Extended );
	setAllColumnsShowFocus( TRUE );

	connect (this, SIGNAL(mouseButtonPressed ( int, QListViewItem *, const QPoint &, int)), this, SLOT(handleMousePress ( int, QListViewItem *, const QPoint &, int)));
	connect (this, SIGNAL(selectionChanged (QListViewItem *)), this, SLOT(updateSelection (QListViewItem *)));
	connect (this, SIGNAL(doubleClicked ( QListViewItem * )), this, SLOT (doubleClick ( QListViewItem * )));
}

PropView::~PropView ()
{
}

QSize PropView::sizeHint () const
{
	return QListView::minimumSizeHint();
}

void PropView::displayItem (QListViewItem *it)
{
	item = 0;
	clear ();

	if ( !it )
		return;

	item = static_cast<DatafileItem *>(it);
	DATAFILE *dat = item->data ();

	if (!dat)
		return;

	if (item->isRoot ())
		return; // root doesn't have any properties at all

	DATAFILE_PROPERTY *prop = dat->prop;
	if (prop)
	{
		while (prop->dat)
		{
			PropViewItem *vi = new PropViewItem (this, prop->type, dat);
			prop++;
		}
	}
}

void PropView::handleMousePress ( int button, QListViewItem *it, const QPoint &pos, int c )
{
	if ( !item || (item && item->isRoot ()) )
		return;

	propItem = 0;

	if ( it )
		propItem = static_cast<PropViewItem *>(it);

	if (button == 1)
	{
	}
	else
	{
		if ( menu )
			delete menu;

		menu = new QPopupMenu;

		menu->insertItem ( tr("&Add"), this, SLOT( popupAdd() ));

		if ( it )
		{
			menu->insertItem ( tr("&Delete"), this, SLOT( popupDelete() ));
			menu->insertItem ( tr("&Modify"), this, SLOT( popupModify() ));
		}

		menu->popup ( pos );
	}
}

void PropView::updateSelection (QListViewItem *_item)
{
	propItem = static_cast<PropViewItem *>(_item);
}

void PropView::doubleClick (QListViewItem *item)
{
	EditItem *edit = new EditItem (this, "edit props", true);
	edit->setCaption ( tr("Edit property") );

	edit->typeEdit->setText ( propItem->getType () );
	edit->typeEdit->setReadOnly (true);

	edit->nameEdit->setText ( propItem->getValue () );

	if ( edit->exec() == QDialog::Accepted )
	{
		propItem->setValue ( edit->nameEdit->text () );

		if ( edit->nameEdit->text().isEmpty() )
			delete propItem;

		emit updated ( item );
		emit dirty ();
	}
}

void PropView::popupAdd ()
{
	EditItem *edit = new EditItem (this, "edit props", true);
	edit->setCaption ( tr("Add property") );

	edit->typeEdit->setText ( "" );
	edit->nameEdit->setText ( "" );

	if ( edit->exec() == QDialog::Accepted )
	{
		bool unique = true;
		QListViewItem *it = firstChild ();

		QString type  = edit->typeEdit->text ();
		QString value = edit->nameEdit->text ();

		while ( it )
		{
			if ( type == static_cast<PropViewItem *>(it)->getType() )
			{
				unique = false;
				break;
			}

			it = it->nextSibling ();
		}

		if ( !unique )
		{
			QMessageBox::warning (this, tr("Error!"), tr("The property type must be unique!") );
			return;
		}

		char buf[5];
		strcpy (buf, type);

		PropViewItem *vi = new PropViewItem ( this, DAT_ID(buf[0], buf[1], buf[2], buf[3]), item->data() );
		vi->setValue ( value );

		emit dirty ();
	}
}

void PropView::popupDelete ()
{
	propItem->setValue ( "" );
	delete propItem;

	emit updated ( item );
	emit dirty ();
}

void PropView::popupModify ()
{
	doubleClick ( propItem );
}

