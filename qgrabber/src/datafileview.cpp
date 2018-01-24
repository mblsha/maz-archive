/***************************************************************************
                          datafileview.cpp  -  description
                             -------------------
    begin                : Mon Apr 8 2002
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

#include <qfile.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qpoint.h>
#include <qmessagebox.h>
#include <qdragobject.h>
#include <qmime.h>
#include <qstrlist.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <qheader.h>
#include <qfiledialog.h>
#include <qlineedit.h>
#ifdef DRAGNDROP_TEST
#	include <qdragobject.h>
#endif

#include <allegro/internal/aintern.h>
#include <string.h>
#include <stdio.h>

#include "datafileview.h"
#include "plugins.h"
#include "factory.h"

#include "common.h"

#include "prefs.h"
#include "edititem.h"
#include "saveprogress.h"
#include "passworddialog.h"

/*****************************************************************************
 *
 * Class DatafileRoot
 *
 *****************************************************************************/

class DatafileRoot : public Datafile
{
private:
	QString n;

public:
	DatafileRoot (QListView *parent, QString name, DATAFILE *data);

	QString name () const;
	QString text (int col) const;
	void setName (QString name);
};


DatafileRoot::DatafileRoot (QListView *parent, QString name, DATAFILE *data)
			: Datafile (parent, data)
{
	n = name;
	setOpen ( true );

	setDragEnabled (false);
}

QString DatafileRoot::text (int col) const
{
	if (col == 0)
		return name();
	else
		return "";
}

QString DatafileRoot::name () const
{
	return n;
}

void DatafileRoot::setName (QString name)
{
	n = name;
}

/*****************************************************************************
 *
 * Class DatafileView
 *
 *****************************************************************************/

DatafileView::DatafileView ( QWidget *parent, const char *name, bool sdo )
			: QListView( parent, name )
{
	contextMenu = 0;
	root = 0;

	setAcceptDrops (true);
#ifdef DRAGNDROP_TEST
	viewport()->setAcceptDrops (true);
#endif
	setTreeStepSize (20);
	setSorting (0, true);
	setDefaultRenameAction (QListView::Accept);

	addColumn ( tr("Name") );
	addColumn ( tr("Type") );
	setTreeStepSize ( 20 );

	connect (this, SIGNAL(itemRenamed(QListViewItem*, int,const QString &)), this, SLOT(renamedItem(QListViewItem*, int, const QString &)));
	connect (this, SIGNAL(mouseButtonPressed(int, QListViewItem*, const QPoint &, int)), this, SLOT(singleClick(int, QListViewItem*, const QPoint &, int)));
	connect (this, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(doubleClick(QListViewItem*)));

	// initialize `Create New Item` context menu
	contextMenuNew  = new QPopupMenu;
	QStringList k = ObjectFactory::keys ();
	int i = 0;
	QStringList::Iterator it;
	for (it = k.begin(); it != k.end(); it++, i++)
	{
		ObjectPlugin *plug = ObjectFactory::create (*it);

		contextMenuNew->insertItem (plug->pixmap(), plug->menuDesc (), this, SLOT(contextNew (int)), 0, i);
		contextMenuNew->setItemParameter ( i, plug->type () );
	}

	k = MenuFactory::keys ();
	for (it = k.begin(); it != k.end(); it++)
	{
		MenuPlugin *plug = MenuFactory::create (*it);
		connect (plug, SIGNAL(insertItem(DATAFILE*, DatafileItem*)), this, SLOT(insertItem(DATAFILE*, DatafileItem*)));
		connect (plug, SIGNAL(insertItem(DATAFILE*)), this, SLOT(insertItem(DATAFILE*)));
		connect (plug, SIGNAL(dirty()), this, SLOT(menuPluginDirty()));
	}


	// initialize actions
	grabAction      = new QAction("", QPixmap(getResourcesDir()+"/icons/main/fileimport.png"), tr("&Grab"), 0, this);
	connect (grabAction, SIGNAL(activated()), SLOT(contextGrab()));
	exportAction    = new QAction("", QPixmap(getResourcesDir()+"/icons/main/fileexport.png"), tr("&Export"), 0, this);
	connect (exportAction, SIGNAL(activated()), SLOT(contextExport()));
	deleteAction    = new QAction("", QPixmap(getResourcesDir()+"/icons/main/editdelete.png"), tr("&Delete"), QListView::Key_Delete, this);
	connect (deleteAction, SIGNAL(activated()), SLOT(contextDelete()));
	renameAction    = new QAction("", tr("&Rename"), QListView::Key_F2, this);
	connect (renameAction, SIGNAL(activated()), SLOT(contextRename()));
	shellEditAction = new QAction("", QPixmap(getResourcesDir()+"/icons/main/exec.png"), tr("&Shell Edit"), 0, this);
	connect (shellEditAction, SIGNAL(activated()), SLOT(contextShellEdit()));
}


DatafileView::~DatafileView ()
{
	if (root)
		delete root;
}

void DatafileView::keyPressEvent (QKeyEvent *e)
{
	DatafileItem *item = static_cast<DatafileItem *>( selectedItem() );
	if ( !item )
		return;

	if (e->key() == QListView::Key_Enter || e->key() == QListView::Key_Return)
	{
		if ( item->isDatafile() )
			item->setOpen ( !item->isOpen() );
		else
			doubleClick ( item );
	}
	else if (e->key() == QListView::Key_Space)
		displayContextMenu (item, viewport()->mapToGlobal(QPoint(32, itemPos(item))));
	else
		QListView::keyPressEvent(e);
}

#ifdef DRAGNDROP_TEST
QDragObject *DatafileView::dragObject ()
{
	DatafileItem *item = dynamic_cast<DatafileItem *>(selectedItem());
	return item ? new DatafileItemDrag (item, this) : 0;
}
#else
void DatafileView::viewportMousePressEvent (QMouseEvent *e)
{
	QListView::viewportMousePressEvent (e);

	if (e->button() == QListView::LeftButton)
		mousePressPos = e->pos();
}

void DatafileView::viewportMouseMoveEvent (QMouseEvent *e)
{
	DatafileItem *item = static_cast<DatafileItem *>( selectedItem() );
	if ( !item )
		return;

	// FIXME: hmmm... maybe enable dragging of root?
	// it may be useful for dragging to other QGrabber windows...
	if (!item->isRoot()                      &&
	    (e->state() & QListView::LeftButton) &&
	    !mousePressPos.isNull()              &&
	    (QPoint(e->pos() - mousePressPos).manhattanLength() > 8))
	{
		QDragObject *d = new DatafileItemDrag (item, this);
		d->dragCopy ();
	}
}

void DatafileView::dragEnterEvent (QDragEnterEvent *e)
{
	bool accept = false;

	if ( DatafileViewDrag::canDecode(e) )
		accept = true;

	e->accept (accept);
}

void DatafileView::dropEvent (QDropEvent *e)
{
	bool takeItem = false;
	DatafileItem *item;
	if ( item = DatafileViewDrag::decode(e, takeItem) )
	{
		if ( !item )
		{
			qWarning ("Whoops! Drag'n'Drop item is empty!");
			return;
		}

		DatafileItem *i = static_cast<DatafileItem *>( itemAt(e->pos()) );
		if ( !i )
			i = root;

		DatafileItem *dest;
		if ( i->isDatafile() )
			dest = i;
		else
			dest = static_cast<DatafileItem *>( i->parent() );

		if ( takeItem )
		{
			DatafileItem *parent = static_cast<DatafileItem *>( item->parent() );

			if ( item->isDatafile() )
			{	// we need to checkup that the place we are moving item to
				// is not child of item
				DatafileItem *it = dest;
				while ( it != root )
				{
					if ( it == item )
						return;

					it = static_cast<DatafileItem *>( it->parent() );
				}
			}

			parent->takeItem (item);
		}

		dest->insertItem (item);

		emit selectionChanged (0);
		emit dirty (true);
	}
}
#endif

void DatafileView::loadDatafile (const char *file)
{
	if (root)
	{
		delete root;
		root = NULL;
	}

	QString cap;

	if ( !strlen(file) )
	{	// Create new datafile
		cap = tr("New Datafile");
		root = new DatafileRoot (this, cap, NULL);
	}
	else
	{	// Load an existing one
		DATAFILE *d = 0;
		while ( !d )
		{
			d = datedit_load_datafile ((AL_CONST char *)file, TRUE, Prefs::prefs()->password());

			if ( !d )
			{
				PasswordDialog *pd = new PasswordDialog (this, "PasswordDialog", true);
				if (pd->exec() == QDialog::Rejected)
					return;
			}
		}

		cap = QFile::decodeName( get_filename((AL_CONST char *)file) );
		root = new DatafileRoot (this, cap, d);

		root->setData (0);
		unload_datafile (d);

		emit addRecentlyOpenedFile(QFile::decodeName(file));
	}

	emit selectionChanged(0);
	emit dirty(false);
	emit changeCaption(cap);
}

extern "C" {
extern int datedit_striptype (int strip);
extern int datedit_packtype (int pack);
}

static int itemCount;

static void countItems (QListViewItem *item)
{
	QListViewItem *it = item->firstChild ();

	while ( it )
	{
		if ( it->childCount() )
			countItems (it);
		else
			itemCount++;

		it = it->nextSibling ();
	}
}

void DatafileView::saveDatafile (const char *name)
{
	// Count datafile items
	itemCount = 0;
	countItems ( root );

	SaveProgress *saveProg = new SaveProgress (this, "SaveProgress", true);
	saveProg->setNumItems (itemCount);
	saveProg->show ();

	QStringList k = ObjectFactory::keys ();
	for (QStringList::Iterator it = k.begin(); it != k.end(); it++)
	{
		ObjectPlugin *plug = ObjectFactory::create (*it);

		connect (plug, SIGNAL(savingItem (QString)), saveProg, SLOT(savingItem (QString)));
		connect (plug, SIGNAL(savedItem (long, long)), saveProg, SLOT(savedItem (long, long)));
	}

	char backup_name[256];
	char *pretty_name;

	packfile_password ( Prefs::prefs()->password() );

	Prefs::prefs()->setStrip ( datedit_striptype ( Prefs::prefs()->strip() ) );
	Prefs::prefs()->setPack  ( datedit_packtype  ( Prefs::prefs()->pack()  ) );

	strcpy (backup_name, datedit_pretty_name ((AL_CONST char *)name, "bak", TRUE));
	pretty_name = datedit_pretty_name ((AL_CONST char *)name, "dat", FALSE);

	delete_file (backup_name);
	rename (pretty_name, backup_name);

	PACKFILE *f = pack_fopen (pretty_name, (Prefs::prefs()->pack() >= 2) ? F_WRITE_PACKED : F_WRITE_NOPACK);

	if (f)
	{
		pack_mputl (DAT_MAGIC, f);
		ObjectPlugin::fileDatasize = 12;

		pack_mputl (root->childCount(), f);

		DatafileItem *item = static_cast<DatafileItem *>(root->firstChild ());

		while ( item )
		{
			ObjectPlugin *plug = ObjectFactory::create ( item->type() );
			plug->save (item, (Prefs::prefs()->pack() >= 2), (Prefs::prefs()->pack() >= 1), Prefs::prefs()->strip(), Prefs::prefs()->verbose(), (Prefs::prefs()->strip() <= 0), f);

			item = static_cast<DatafileItem *>(item->nextSibling ());
		}

		if ( Prefs::prefs()->strip() <= 0 )
		{
			datedit_set_property (&datedit_info, DAT_NAME, "GrabberInfo");
			//save_object (&datedit_info, FALSE, FALSE, FALSE, FALSE, f);
		}

		pack_fclose (f);
	}

	if (errno)
	{
		delete_file (pretty_name);
		QMessageBox::warning (this, tr("Error"), tr("Error writing '%1'").arg( QFile::decodeName(pretty_name) ), QMessageBox::Ok, QMessageBox::NoButton);
		packfile_password (NULL);
	}
	else
	{
		if ( !Prefs::prefs()->backups() )
			delete_file (backup_name);

		if ( Prefs::prefs()->verbose() )
		{
			int fileFilesize = file_size(pretty_name);
			//datedit_msg ("%-28s%7d bytes into %-7d (%d%%)", "- GLOBAL COMPRESSION -",
			//             file_datasize, file_filesize, percent(file_datasize, file_filesize));
		}

		QString n = QFile::decodeName ( get_filename((AL_CONST char *)name) );
		root->setName (n);

		emit changeCaption(n);
		emit dirty(false);
	}

	packfile_password (NULL);
}


void DatafileView::displayContextMenu (DatafileItem *item, const QPoint &pos)
{
	if ( contextMenu )
		delete contextMenu;
	contextMenu = new QPopupMenu;

	if ( item && item->isRoot() )
		item = 0;

	QStringList pl;
	if ( item && (pl = MenuFactory::contextKeys ( item->type() )).size() )
	{
		connect (contextMenu, SIGNAL(activated(int)), this, SLOT(contextMenuPlugin(int)));

		int id = 0;
		for (QStringList::Iterator it = pl.begin(); it != pl.end(); it++, id++)
		{
			MenuPlugin *plug = MenuFactory::create (*it);
			
			QString    text = plug->contextMenuText ();
			QPopupMenu *pop = plug->contextMenuPopup ( item );

			if ( pop )
				contextMenu->insertItem (text, pop);
			else
				contextMenu->insertItem (text, id);
		}
		contextMenu->insertSeparator ();
	}

	if ( item )
	{
		grabAction->addTo ( contextMenu );
		exportAction->addTo ( contextMenu );
		deleteAction->addTo ( contextMenu );
		renameAction->addTo ( contextMenu );
		//shellEditAction->addTo ( contextMenu );
		contextMenu->insertSeparator ();
	}

	contextMenu->insertItem ( QPixmap(getResourcesDir()+"/icons/main/new_item.png"), tr("&New"), contextMenuNew);
	contextMenu->popup ( pos );
}

void DatafileView::contextMenuPlugin (int id)
{
	if ( id != -1 )
	{
		DatafileItem *item = static_cast<DatafileItem*> ( selectedItem() );
		QStringList pl = MenuFactory::contextKeys ( item->type() );
		int i = 0;
		for (QStringList::Iterator it = pl.begin(); it != pl.end(); it++, i++)
			if ( i == id )
			{
				MenuPlugin *plug = MenuFactory::create (*it);
				plug->contextMenuActivated ( item );
			}
	}
}

void DatafileView::insertItem (DATAFILE *dat)
{
	DatafileItem *item = static_cast<DatafileItem*>( selectedItem() );
	if ( !item )
		item = root;

	if ( !item->isDatafile() )
		item = static_cast<DatafileItem*>( item->parent() );

	insertItem (dat, item);
}

void DatafileView::insertItem (DATAFILE *dat, DatafileItem *parent)
{
	if ( !parent )
		parent = root;

	ObjectPlugin *plug = ObjectFactory::create (dat->type);
	plug->open (parent, dat);
}

void DatafileView::singleClick ( int button, QListViewItem *it, const QPoint &pos, int c )
{
	if ( !root )
		return;

	mousePressPos = pos;

	DatafileItem *item = static_cast<DatafileItem *>( it );
	if (button == 2)
		displayContextMenu (item, pos);
}

void DatafileView::contextGrab ()
{
	DatafileItem *item = static_cast<DatafileItem *>( currentItem() );
	if ( !item || item->isRoot() )
		return;

	IOPlugin *io = IOFactory::create (item->type());

	if (io)
	{
		QString file = QFileDialog::getOpenFileName( QFile::decodeName(get_datafile_property(item->data (), DAT_ORIG)),
		                                             io->grabType(), this, "grab_dialog", tr("Grab") );

		if ( !file.isEmpty () )
		{
			if ( io->grab ( QFile::encodeName(file), item ) )
			{
				emit selectionChanged ( item );
				emit dirty(true);
			}
		}
	}
}

void DatafileView::contextExport ()
{
	DatafileItem *item = static_cast<DatafileItem *>( currentItem() );
	if ( !item || item->isRoot() )
		return;

	IOPlugin *io = IOFactory::create (item->type());

	if (io)
	{
		QString file = QFileDialog::getSaveFileName( QFile::decodeName(get_datafile_property(item->data (), DAT_ORIG)),
		                                             io->exportType(), this, "export_dialog", tr("Export") );

		if ( !file.isEmpty () )
		{
			int ret = io->exportFile ( QFile::encodeName(file), item );

			if ( !ret )
				QMessageBox::warning (0, tr("Warning"), tr("File could not be saved!"));
		}
	}
}

void DatafileView::contextDelete ()
{
	DatafileItem *item = static_cast<DatafileItem *>( currentItem() );
	if ( !item || item->isRoot() )
		return;

	switch( QMessageBox::warning ( this, tr("QGrabber"),
	        tr ("Do you really want\n"
	        "to delete this datafile item?"),
	        tr("&Yes"),
	        tr("&No"), 0, 0, 1 ) )
	{
		case 0:
			delete item;
			emit selectionChanged ( root );
			emit dirty(true);
			break;
		case 1:
			break;
	}
}


void DatafileView::contextRename ()
{
	DatafileItem *item = static_cast<DatafileItem *>( currentItem() );
	if ( !item || item->isRoot() )
		return;

/*	// bah! this code isn't working for some reason :(

	item->setRenameEnabled (0, true);
	item->startRename (0);
	item->setRenameEnabled (0, false);
*/
	EditItem *edit = new EditItem (this, "rename item", true);
	edit->setCaption ( tr("Rename item") );

	edit->typeEdit->setText ( item->strType () );
	edit->typeEdit->setReadOnly (true);

	edit->nameEdit->setText ( item->name () );

	if ( edit->exec() == QDialog::Accepted )
	{
		item->setName ( edit->nameEdit->text () );

		repaint ();
		emit selectionChanged ( item ); // need to update PropView
		emit dirty(true);
	}
}

void DatafileView::renamedItem (QListViewItem *it, int i, const QString &t)
{
	DatafileItem *item = static_cast<DatafileItem *>( it );
	if ( !item || item->isRoot() )
		return;

	item->setName ( t );

	emit selectionChanged(item); // need to update PropView
	emit dirty(true);

}

void DatafileView::contextShellEdit ()
{
	DatafileItem *item = static_cast<DatafileItem *>( currentItem() );
	if ( !item || item->isRoot() )
		return;

	// TODO: write this out
}

void DatafileView::contextNew (int type)
{
	EditItem *newInput = new EditItem (this, "EditItem", true);
	newInput->setCaption ( tr("Create new datafile item...") );

	if ( !type )
	{
		newInput->typeEdit->setText ( "DATA" );
	}
	else
	{
		char buf[5];
		sprintf (buf, "%c%c%c%c", type >> 24, type >> 16, type >> 8, type);

		newInput->typeEdit->setText ( buf );
		newInput->typeEdit->setReadOnly (true);
	}

	if ( newInput->exec() == QDialog::Accepted )
	{
		type = datedit_clean_typename ( (AL_CONST char *)newInput->typeEdit->text().latin1() );

		ObjectPlugin *plug = ObjectFactory::create (type);

		DATAFILE *dat = (DATAFILE *) malloc ( sizeof (DATAFILE) );
		dat->type = type;
		dat->size = 0;
		dat->prop = NULL;
		dat->dat = plug->makeNew ( &dat->size );

		// nasty MSVC trick
		QCString utf8 = newInput->nameEdit->text().utf8();
		char *c = new char[ utf8.size() ];
		strcpy ( c, (const char *)utf8 );
		datedit_set_property ( dat, DAT_NAME, (AL_CONST char *)c );
		delete[] c;
		
		DatafileItem *parent;
		DatafileItem *item = static_cast<DatafileItem *>( currentItem() );
		if ( !item )
			item = root;

		if ( item->isDatafile() || item->isRoot() )
			parent = static_cast<DatafileItem *>(item);
		else
			parent = static_cast<DatafileItem *>(item->parent ());

		plug->open (parent, dat);

		emit dirty(true);
	}

	delete newInput;
}


void DatafileView::updateSelection (QListViewItem *item)
{
	repaint ();
}

QSize DatafileView::sizeHint () const
{
	int size = 16;

	QHeader *h = header ();

	for (int i = 0; i < h->count(); i++)
		size += h->sectionSize (i);

	size += 4;

	return QSize ( size, 150 );
}

void DatafileView::doubleClick (QListViewItem *it)
{
	DatafileItem *item = static_cast<DatafileItem *>( it );
	if ( !item || item->isRoot() )
		return;

	ObjectPlugin *plug = ObjectFactory::create (item->type());

	if (plug)
	{
		if ( plug->doubleClick ( item->data () ) )
		{
			emit selectionChanged ( item );
		}
	}
}

void DatafileView::newFile ()
{
	Prefs::prefs()->setDatafileName ( "" );
	loadDatafile ( "" );
}

void DatafileView::openFile (const QString &file)
{
	Prefs::prefs()->setDatafileName ( file );
	loadDatafile ( QFile::encodeName (file) );
}

void DatafileView::openFile ()
{
	QString fileName = QFileDialog::getOpenFileName (Prefs::prefs()->datafileName(), tr("Datafile") + " (*.dat)", this, tr("Open"));
	if ( !fileName.isEmpty() )
	{
		Prefs::prefs()->setDatafileName ( fileName );
		loadDatafile ( QFile::encodeName (fileName) );
	}
}

void DatafileView::saveFile ()
{
	if ( !strlen( Prefs::prefs()->datafileName() ) )
	{
		saveFileAs ();
	}
	else
	{
		saveDatafile ( QFile::encodeName(Prefs::prefs()->datafileName()) );
	}

}

void DatafileView::saveFileAs ()
{
	QString fileName = QFileDialog::getSaveFileName (Prefs::prefs()->datafileName(), tr("Datafile") + " (*.dat)", this, tr("Save"));
	if ( !fileName.isEmpty() )
	{
		Prefs::prefs()->setDatafileName ( fileName );
		saveDatafile ( QFile::encodeName(fileName) );
	}
}

bool DatafileView::updateDataHelper (DATAFILE *dat, const char *filename, int x, int y, int w, int h)
{
	DATAFILE *tmp = IOPlugin::grab (filename, "dummyname", dat->type, x, y, w, h, -1);
	DATAFILE_PROPERTY *tmp_prop;

	if (tmp->dat)
	{
		// exchange properties
		tmp_prop = tmp->prop;
		tmp->prop = dat->prop;
		dat->prop = tmp_prop;

		datedit_set_property (tmp, DAT_DATE, get_datafile_property(dat, DAT_DATE));

		// adjust color depth?
		if ( (dat->type == DAT_BITMAP) || (dat->type == DAT_RLE_SPRITE) ||
		     (dat->type == DAT_C_SPRITE) || (dat->type == DAT_XC_SPRITE) )
		{
			int src_depth, dest_depth;

			if (dat->type == DAT_RLE_SPRITE)
			{
				dest_depth = ((RLE_SPRITE *)dat->dat)->color_depth;
				src_depth = ((RLE_SPRITE *)tmp->dat)->color_depth;
			}
			else
			{
				dest_depth = bitmap_color_depth ( (BITMAP *)dat->dat );
				src_depth = bitmap_color_depth ( (BITMAP *)tmp->dat );
			}

			if ( src_depth != dest_depth )
			{
				BITMAP *b1, *b2;

				if (dat->type == DAT_RLE_SPRITE)
				{
					RLE_SPRITE *spr = (RLE_SPRITE *)tmp->dat;
					b1 = create_bitmap_ex(src_depth, spr->w, spr->h);
					clear_to_color(b1, b1->vtable->mask_color);
					draw_rle_sprite(b1, spr, 0, 0);
					destroy_rle_sprite(spr);
				}
				else
					b1 = (BITMAP *)tmp->dat;

				if ( dest_depth == 8 )
					qWarning ("Warning: lossy conversion from truecolor to 256 colors!");

				if ( (dat->type == DAT_RLE_SPRITE) ||
				     (dat->type == DAT_C_SPRITE) || (dat->type == DAT_XC_SPRITE))
				{
					datedit_last_read_pal[0].r = 63;
					datedit_last_read_pal[0].g = 0;
					datedit_last_read_pal[0].b = 63;
				}

				select_palette(datedit_last_read_pal);
				b2 = create_bitmap_ex(dest_depth, b1->w, b1->h);
				blit(b1, b2, 0, 0, 0, 0, b1->w, b1->h);
				unselect_palette();

				if (dat->type == DAT_RLE_SPRITE)
				{
					tmp->dat = get_rle_sprite(b2);
					destroy_bitmap(b1);
					destroy_bitmap(b2);
				}
				else
				{
					tmp->dat = b2;
					destroy_bitmap(b1);
				}
			}
		}

		_unload_datafile_object(dat);
		*dat = *tmp;
		return true;
	}
	else
		return false;
}

bool DatafileView::updateData (DATAFILE *dat)
{
	const char *name   = get_datafile_property(dat, DAT_NAME);
	const char *origin = get_datafile_property(dat, DAT_ORIG);
	const char *date   = get_datafile_property(dat, DAT_DATE);
	time_t origt, datat;

	if ( !*origin )
	{
		qWarning ("%s has no origin data - skipping", name);
		return true;
	}

	if ( !exists((AL_CONST char *)origin) )
	{
		qWarning ("%s: %s not found - skipping", name, origin);
		return true;
	}

	if (*date)
	{
		origt = file_time((AL_CONST char *)origin);
		datat = datedit_asc2ftime((AL_CONST char *)date);

		if ( (origt/60) <= (datat/60) )
		{
			//if (verbose)
				qWarning("%s: %s has not changed - skipping", name, origin);
			return true;
		}
	}

	qWarning("Updating %s -> %s", origin, name);

	return updateDataHelper ( dat, origin,
			     datedit_numprop (dat, DAT_XPOS),
			     datedit_numprop (dat, DAT_YPOS),
			     datedit_numprop (dat, DAT_XSIZ),
			     datedit_numprop (dat, DAT_YSIZ) );
}

void DatafileView::updateHelper (DatafileItem *r, bool force)
{
	DatafileItem *item = static_cast<DatafileItem *>(  root->firstChild() );

	while ( item )
	{
		DATAFILE *dat = item->data();

		updateData (dat);

		if ( dat->type == DAT_FILE )
			updateHelper ( item, force );

		item = static_cast<DatafileItem *>( item->nextSibling () );
	}
}

void DatafileView::update()
{
	updateHelper ( root, false );
}

void DatafileView::forceUpdate()
{
	updateHelper ( root, true );
}

void DatafileView::updateSelection()
{
	DatafileItem *item = static_cast<DatafileItem *>( selectedItem() );
	if ( !item || item->isRoot() )
		return;

	updateData ( item->data() );
}

void DatafileView::menuPluginDirty ()
{
	emit dirty(true);
	emit selectionChanged ( selectedItem() );
}

