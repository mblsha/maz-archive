#ifndef FONT_H
#define FONT_H

#include "alleg.h"

#include <qlistbox.h>
#include <qlistview.h>

#include "plugins.h"

/*****************************************************************************
 *
 * Class FontView
 *
 *****************************************************************************/

class FontView : public QWidget {
	Q_OBJECT

private:
	DATAFILE *d;

	void paintRange (QPainter *painter, int range, int *x, int *y, int *maxh);

public:
	FontView (QWidget* parent = 0, const char *name = 0);

public slots:
	void setData (DATAFILE *);

protected:
	void paintEvent( QPaintEvent* );
};

/*****************************************************************************
 *
 * Class FontRangeSelect
 *
 *****************************************************************************/

class FontRangeSelect : public QListBox
{
	Q_OBJECT

private:
	DATAFILE *d;
	QMap<QListBoxItem*,int> items;
	int range;

public:
	FontRangeSelect (QWidget *parent = 0, const char *name = 0);
	~FontRangeSelect ();

signals:
	void selectionChanged (int);
	void updated ();
	void update (int); // internal

public slots:
	void updateList (int);
	void setData (DATAFILE *);

	void importRange ();
	void deleteRange ();

private slots:
	void changedSelection (QListBoxItem *);
};

/*****************************************************************************
 *
 * Class FontRangeView
 *
 *****************************************************************************/

class FontRangeView : public QListView
{
	Q_OBJECT

private:
	DATAFILE *d;
	int range;

public:
	FontRangeView (QWidget *parent = 0, const char *name = 0);
	~FontRangeView ();

public slots:
	void setData (DATAFILE *);

public slots:
	void rangeChanged (int);
};

/*****************************************************************************
 *
 * Class ObjectFont
 *
 *****************************************************************************/

class ObjectFont : public ObjectPlugin
{
	Q_OBJECT

public:
	ObjectFont ();
	~ObjectFont ();

	bool    typeSupported (int type);
	int     type          ();
	QString menuDesc      ();
	QPixmap pixmap        ();
	void    open          (DatafileItem *parent, DATAFILE *data);
	void    *makeNew      (long *size);
	void    *copyData     (DATAFILE *dat, long *size);
	void    plot          (DATAFILE *dat, QWidget *parent);
	void    saveObject    (DatafileItem *item, int packed, int packkids, int strip, int verbose, int extra, PACKFILE *f);
};

#endif
