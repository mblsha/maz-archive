/***************************************************************************
                          editor.h  -  description
                             -------------------
    begin                : Û¬‘ Ì¡  4 2002
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

#ifndef EDITOR_H
#define EDITOR_H

#include <qwidget.h>
#include <qvaluelist.h>

class QPopupMenu;

class LineItem;
class NodeItem;
class PolyItem;
typedef QValueList<LineItem *> LineItemList;
typedef QValueList<NodeItem *> NodeItemList;
typedef QValueList<PolyItem *> PolyItemList;
#include <qcanvas.h>

class Editor : public QCanvasView
{
	Q_OBJECT

private:
	typedef enum EditMode { normal, connectNodes, connectPoly, colorizer, getColor };
	EditMode mode;

	QColor color;

	QPopupMenu *menu;

	NodeItemList nodeItems;
	LineItemList lineItems;
	PolyItemList polyItems;

	int polyCount;
	QCanvasItem  *moving, *start[2];
	QPoint moving_start;

signals:
	void colorChanged(const QColor &);

private slots:
	void popupAddNode ();
	void popupDelete ();
	void popupSetMode (int mode);

public slots:
	void slotLoad (QString file);
	void slotSave (QString file);
	void setColor (const QColor &col);

	void slotClear ();

protected:
	void contentsMousePressEvent   (QMouseEvent*);
	void contentsMouseReleaseEvent (QMouseEvent*);
	void contentsMouseMoveEvent    (QMouseEvent*);

	void resizeEvent (QResizeEvent *e);

public:
	Editor (QCanvas *canvas, QWidget *parent = 0, const char *name = 0, WFlags f=0);
	~Editor ();

	QSize sizeHint() const;
};

#endif
