/***************************************************************************
                          editor.cpp  -  description
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

#include "editor.h"

/*****************************************************************************
 *
 * Class
 *
 *****************************************************************************/

#include <stdlib.h>
#include <qdatetime.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlabel.h>
#include <qlayout.h>

#include <qpopupmenu.h>
#include <qcursor.h>

#include <qfile.h>
#include <qtextstream.h>

#include <qmessagebox.h>

#include <allegro.h>

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static QBrush *tb = 0;
static QPen *tp = 0;

class LineItem;
class NodeItem;
class PolyItem;

static const int lineRTTI = DAT_ID('L','I','N','E');

class LineItem: public QCanvasLine
{
private:
	QColor color;
	QPen *pen;
	void init ( NodeItem *from, NodeItem *to, int x, int y, QColor *col = 0 );

public:
	LineItem ( NodeItem *from, NodeItem *to, QCanvas *, QColor *col = 0 );
	LineItem ( NodeItem *from, int x, int y, QCanvas *, QColor *col = 0 );
	~LineItem ();

	void setColor (QColor &col);
	QColor getColor ();

	int rtti() const { return lineRTTI; }

	void setFromPoint ( int x, int y ) ;
	void setToPoint ( int x, int y );
	void setNodes (NodeItem *from, NodeItem *to);
	void moveBy (double dx, double dy);
	void remove (NodeItem *node);

	void save (QTextStream &s, NodeItemList &nodes);

	NodeItem *from;
	NodeItem *to;
};

static const int nodeRTTI = DAT_ID('N','O','D','E');

class NodeItem: public QCanvasEllipse
{
private:
	QCanvas *canvas;
	LineItemList inList;
	LineItemList outList;
	PolyItemList polyList;

public:
	NodeItem( QCanvas * );
	~NodeItem();

	int rtti() const { return nodeRTTI; }

	void addInLine     (LineItem *line) { inList.append  ( line ); }
	void addOutLine    (LineItem *line) { outList.append ( line ); }
	void deleteInLine  (LineItem *line) { inList.remove  ( line ); }
	void deleteOutLine (LineItem *line) { outList.remove ( line ); }

	void addPoly    (PolyItem *poly) { polyList.append ( poly ); }
	void deletePoly (PolyItem *poly) { polyList.remove ( poly ); }

	void moveBy(double dx, double dy);

	void save (QTextStream &s);
};

static const int polyRTTI = DAT_ID('P','O','L','Y');

class PolyItem : public QCanvasPolygon
{
private:
	QColor color;
	QBrush *brush;

	NodeItem *nodes[3];

	void init (NodeItem *nodes[3], QPointArray &points, QColor &col);

public:
	PolyItem (NodeItem *nodes[3], QCanvas *canvas, QColor *col = 0);
	PolyItem (NodeItem *start, QCanvas *canvas, QColor *col = 0);
	~PolyItem ();

	int rtti() const { return polyRTTI; }

	void addToNodes ();

	void setPoint (int index, NodeItem *node, int x, int y);
	void remove (NodeItem *node);

	void save (QTextStream &s, NodeItemList &nodes);

	void setColor (QColor &color);
	QColor getColor ();
};


/*****************************************************************************
 *
 * Class PolyItem
 *
 *****************************************************************************/

void PolyItem::setPoint (int index, NodeItem *node, int x, int y)
{
	if (node)
	{
		if (index == -1)
		{
			for (int i = 0; i < 3; i++)
				if (nodes[i] == node)
				{
					index = i;
					break;
				}

			if (index == -1)
				return;
		}
		nodes[index] = node;
		QPointArray pa;
		pa = points();

		pa[index] = QPoint((int)nodes[index]->x(), (int)nodes[index]->y()); //(QPoint &)nodes[index];
		setPoints (pa);
	}
	else
	{
		QPointArray pa;
		pa = points();

		pa[index] = QPoint(x, y);
		setPoints (pa);
	}
}

void PolyItem::addToNodes ()
{
	for (int i = 0; i < 3; i++)
		nodes[i]->addPoly (this);
}

void PolyItem::init (NodeItem *_nodes[3], QPointArray &points, QColor &col)
{
	for (int i = 0; i < 3; i++)
		nodes[i] = _nodes[i];
	setPoints (points);

	if (nodes[0] && nodes[1] && nodes[2])
	{
		for (int i = 0; i < 3; i++)
			nodes[i]->addPoly (this);
	}

	brush = NULL;
	setColor (col);

	setZ ( 110 );
}

PolyItem::PolyItem (NodeItem *nodes[3], QCanvas *canvas, QColor *col)
			: QCanvasPolygon (canvas)
{
	QPointArray pa(3);
	for (int i = 0; i < 3; i++)
		pa[i] = QPoint( (int)nodes[i]->x(), (int)nodes[i]->y() );

	init (nodes, pa, *col);
}

PolyItem::PolyItem (NodeItem *start, QCanvas *canvas, QColor *col)
			: QCanvasPolygon (canvas)
{
	QPointArray pa(3);
	for (int i = 0; i < 3; i++)
		pa[i] = QPoint( (int)start->x() + i, (int)start->y() - i );

	NodeItem *nodes[3];
	for (int i = 0; i < 3; i++)
		nodes[i] = NULL;

	nodes[0] = start;

	init (nodes, pa, *col);
}

void PolyItem::setColor (QColor &col)
{
	if (brush)
		delete brush;

	color = col;
	brush = new QBrush (color);
	setBrush (*brush);
}

QColor PolyItem::getColor ()
{
	return color;
}

void PolyItem::remove (NodeItem *node)
{
	for (int i = 0; i < 3; i++)
		if (nodes[i] != node)
		{
			nodes[i]->deletePoly (this);
			nodes[i] = NULL;
		}
}

PolyItem::~PolyItem ()
{
	for (int i = 0; i < 3; i++)
		if (!nodes[i])
			return;

	for (int i = 0; i < 3; i++)
		nodes[i]->deletePoly (this);
}

void PolyItem::save (QTextStream &s, NodeItemList &nodeList)
{
	int count = 0, i = 1;
	NodeItemList::Iterator ni;
	for (  ni = nodeList.begin(); ni != nodeList.end(); ni++, i++ )
	{
		for (int j = 0; j < 3; j++)
		{
			if (*ni == nodes[j])
			{
				s << i;
				s << " ";
			if (++count == 3)
				goto out;
			}
		}
	}
out:
	s << color.red() << " " << color.green() << " " << color.blue();
	s << "\n";
}


/*****************************************************************************
 *
 * Class LineItem
 *
 *****************************************************************************/

void LineItem::moveBy (double, double)
{
	//nothing
}

void LineItem::init (NodeItem *fromItem, NodeItem *toItem, int x, int y, QColor *col)
{
	from = fromItem;
	to = toItem;

	pen = NULL;
	setColor (*col);

	setBrush (*tb);

	if (to)
	{
		from->addOutLine (this);
		to->addInLine (this);

		setPoints ( int(from->x()), int(from->y()), int(to->x()), int(to->y()) );
	}
	else
	{
		setPoints ( int(from->x()), int(from->y()), x, y );
	}

	setZ ( 120 );
}

LineItem::LineItem( NodeItem *fromItem, NodeItem *toItem, QCanvas *canvas, QColor *col )
			: QCanvasLine( canvas )
{
	init (fromItem, toItem, 0, 0, col);
}

LineItem::LineItem (NodeItem *_from, int x, int y, QCanvas *canvas, QColor *col)
			: QCanvasLine (canvas)
{
	init (_from, 0, x, y, col);
}

void LineItem::setColor (QColor &col)
{
	if (pen)
		delete pen;

	color = col;
	pen = new QPen (color);
	setPen (*pen);
}

QColor LineItem::getColor ()
{
	return color;
}

void LineItem::setNodes (NodeItem *_from, NodeItem *_to)
{
	from = _from;
	to = _to;

	from->addOutLine (this);
	to->addInLine (this);
}

void LineItem::remove (NodeItem *node)
{
	//if (!node)
	//	return;

	if (node == from)
	{
		to->deleteInLine (this);
		to = NULL;
	}

	if (node == to)
	{
		from->deleteOutLine (this);
		from = NULL;
	}
}

LineItem::~LineItem ()
{
	if (from && to)
	{
		remove (from);
		remove (to);
	}

	delete pen;
}

void LineItem::setFromPoint ( int x, int y )
{
	setPoints ( x,y, endPoint().x(), endPoint().y() );
}

void LineItem::setToPoint( int x, int y )
{
	setPoints( startPoint().x(), startPoint().y(), x, y );
}

void LineItem::save (QTextStream &s, NodeItemList &nodes)
{
	int count = 0, i = 1;
	NodeItemList::Iterator ni;
	for (  ni = nodes.begin(); ni != nodes.end(); ni++, i++ )
	{
		if ((*ni == from) || (*ni == to))
		{
			s << i;
			s << " ";
			if (++count == 2)
				break;
		}
	}
	s << color.red() << " " << color.green() << " " << color.blue();
	s << "\n";
}

/*****************************************************************************
 *
 * Class NodeItem
 *
 *****************************************************************************/

#define NODE_RADIUS 7

NodeItem::NodeItem( QCanvas *_canvas )
			: QCanvasEllipse( NODE_RADIUS, NODE_RADIUS, _canvas )
{
	setPen( *tp );
	setBrush( *tb );

	setZ( 128 );
}

NodeItem::~NodeItem ()
{
	{
		LineItemList::Iterator it;
		for (  it = inList.begin(); it != inList.end(); ++it )
		{
			(*it)->remove (this);
			delete *it;
		}
		for (  it = outList.begin(); it != outList.end(); ++it )
		{
			(*it)->remove (this);
			delete *it;
		}
	}
	{
		PolyItemList::Iterator it;
		for (  it = polyList.begin(); it != polyList.end(); ++it )
		{
			(*it)->remove (this);
			delete *it;
		}
	}
}

void NodeItem::moveBy(double dx, double dy)
{
	QCanvasEllipse::moveBy (dx, dy);

	LineItemList::Iterator it;
	for (  it = inList.begin(); it != inList.end(); ++it )
		(*it)->setToPoint( int(x()), int(y()) );
	for (  it = outList.begin(); it != outList.end(); ++it )
		(*it)->setFromPoint( int(x()), int(y()) );

	PolyItemList::Iterator pi;
	for (  pi = polyList.begin(); pi != polyList.end(); ++pi )
		(*pi)->setPoint (-1, this, 0, 0);
}

void NodeItem::save (QTextStream &s)
{
	s << x() << " " << y() << "\n";
}

/*****************************************************************************
 *
 * Class Editor
 *
 *****************************************************************************/


Editor::Editor (QCanvas *_canvas, QWidget *parent = 0, const char *name, WFlags f)
			: QCanvasView (_canvas, parent, name, f)
{
	menu = NULL;
	mode = normal;

	if ( !tb ) tb = new QBrush (Qt::red);
	if ( !tp ) tp = new QPen   (Qt::black);

	canvas()->update ();
}

Editor::~Editor ()
{
	if (menu)
		delete menu;
}

void Editor::contentsMousePressEvent (QMouseEvent* e)
{
	moving_start = e->pos();

	if (e->button() == Qt::LeftButton)
	{
		if (mode == normal)
		{
			QCanvasItemList l = canvas()->collisions (e->pos());
			for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
			{
				if ((*it)->rtti() == nodeRTTI)
				{
					moving = *it;
					return;
				}
			}
			moving = NULL;
		}
		else if (mode == connectNodes)
		{
			QCanvasItemList l = canvas()->collisions (e->pos());
			for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
			{
				if ((*it)->rtti() == nodeRTTI)
				{
					start[0] = (*it);
					moving = new LineItem (static_cast<NodeItem *>(*it), e->pos().x(), e->pos().y(), canvas(), &color);
					moving->show ();

					return;
				}
			}
			moving = NULL;
		}
		else if (mode == connectPoly)
		{
			QCanvasItemList l = canvas()->collisions (e->pos());
			for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
			{
				if ((*it)->rtti() == nodeRTTI)
				{
					start[polyCount] = (*it);

					if (!polyCount)
					{
						moving = new PolyItem (static_cast<NodeItem *>(*it), canvas(), &color);
						moving->show ();
					}

					return;
				}
			}
			if (moving)
				delete moving;
			polyCount = 0;
			moving = NULL;
		}
		else if (mode == colorizer)
		{
			QCanvasItemList l = canvas()->collisions (e->pos());
			for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
			{
				if ((*it)->rtti() == lineRTTI)
				{
					static_cast<LineItem *>(*it)->setColor( color );
					return;
				}
				else if ((*it)->rtti() == polyRTTI)
				{
					static_cast<PolyItem *>(*it)->setColor( color );
					return;
				}
			}
		}
		else if (mode == getColor)
		{
			QCanvasItemList l = canvas()->collisions (e->pos());
			for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
			{
				if ((*it)->rtti() == lineRTTI)
				{
					emit colorChanged ( static_cast<LineItem *>(*it)->getColor() );
					return;
				}
				else if ((*it)->rtti() == polyRTTI)
				{
					emit colorChanged ( static_cast<PolyItem *>(*it)->getColor() );
					return;
				}
			}
		}
	}
	else if (e->button() == Qt::RightButton)
	{
		if (menu)
			delete menu;

		menu = new QPopupMenu;

		menu->insertItem ( tr("Add Node"), this, SLOT(popupAddNode()) );

		QCanvasItemList l = canvas()->collisions (e->pos());
		for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
		{
			menu->insertItem ( tr("Delete"), this, SLOT(popupDelete())  );
			break;
		}

		menu->insertSeparator ();
		menu->setCheckable (true);

		menu->insertItem ( tr("Normal Mode"),  this, SLOT(popupSetMode(int)), 0, 1000 );
		menu->setItemParameter (1000, normal);
		if (mode == normal) menu->setItemChecked (1000, true);
		menu->insertItem ( tr("Connect Mode"), this, SLOT(popupSetMode(int)), 0, 1001 );
		menu->setItemParameter (1001, connectNodes);
		if (mode == connectNodes) menu->setItemChecked (1001, true);
		menu->insertItem ( tr("Connect Polygon Mode"), this, SLOT(popupSetMode(int)), 0, 1002 );
		menu->setItemParameter (1002, connectPoly);
		if (mode == connectPoly) menu->setItemChecked (1002, true);
		menu->insertItem ( tr("Colorizer"), this, SLOT(popupSetMode(int)), 0, 1003 );
		menu->setItemParameter (1003, colorizer);
		if (mode == colorizer) menu->setItemChecked (1003, true);
		menu->insertItem ( tr("Get Color"), this, SLOT(popupSetMode(int)), 0, 1004 );
		menu->setItemParameter (1004, getColor);
		if (mode == getColor) menu->setItemChecked (1004, true);

		menu->exec (QCursor::pos ());
	}
}

void Editor::contentsMouseReleaseEvent (QMouseEvent *e)
{
	if (mode == normal)
	{
		if ( moving )
			moving = NULL;
	}
	else if (mode == connectNodes)
	{
		if (moving)
		{
			QCanvasItemList l = canvas()->collisions (e->pos());
			for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
			{
				if ((*it)->rtti() == nodeRTTI)
				{
					if (start[0] != (*it))
					{
						static_cast<LineItem *>(moving)->setNodes (static_cast<NodeItem *>(start[0]), static_cast<NodeItem *>(*it));
						static_cast<NodeItem *>(start[0])->moveBy (0, 0);
						static_cast<NodeItem *>(*it)->moveBy (0, 0);

						lineItems.append (static_cast<LineItem *>(moving));
						return;
					}
				}
			}

			delete moving;
			moving = NULL;
		}
	}
	else if (mode == connectPoly)
	{
		if (moving)
		{
			QCanvasItemList l = canvas()->collisions (e->pos());
			for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
			{
				if ((*it)->rtti() == nodeRTTI)
				{
					if (start[0] != (*it) && start[1] != (*it))
					{
						static_cast<PolyItem *>(moving)->setPoint (polyCount+1, static_cast<NodeItem *>(*it), 0, 0);

						if (polyCount++ == 1)
						{
							static_cast<PolyItem *>(moving)->setPoint (0, static_cast<NodeItem *>(start[0]), 0, 0);
							static_cast<PolyItem *>(moving)->setPoint (1, static_cast<NodeItem *>(start[1]), 0, 0);
							static_cast<PolyItem *>(moving)->setPoint (2, static_cast<NodeItem *>(*it), 0, 0);

							static_cast<PolyItem *>(moving)->addToNodes ();

							static_cast<NodeItem *>(start[0])->moveBy (0, 0);
							static_cast<NodeItem *>(start[1])->moveBy (0, 0);
							static_cast<NodeItem *>(*it)->moveBy (0, 0);

							polyItems.append (static_cast<PolyItem *>(moving));

							start[0] = start[1] = NULL;
							moving = NULL;
							polyCount = 0;
						}

						return;
					}
				}
			}

			start[0] = start[1] = NULL;
			polyCount = 0;
			delete moving;
			moving = NULL;
		}
	}
}

void Editor::contentsMouseMoveEvent (QMouseEvent* e)
{
	if (mode == normal)
	{
		if ( moving )
		{
			moving->moveBy (e->pos().x() - moving_start.x(), e->pos().y() - moving_start.y());
			moving_start = e->pos();
			canvas()->update();
		}
	}
	else if (mode == connectNodes)
	{
		if (moving)
			static_cast<LineItem *>(moving)->setToPoint (e->pos().x(), e->pos().y());
	}
	else if (mode == connectPoly)
	{
		if (moving)
			static_cast<PolyItem *>(moving)->setPoint (polyCount + 1, 0, e->pos().x(), e->pos().y());
	}
/*	else if (mode == colorizer || mode == getColor)
	{
		contentsMousePressEvent ( e );
	}*/
}


QSize Editor::sizeHint() const
{
    return QSize ( 1024, 768 );
}

void Editor::resizeEvent( QResizeEvent* e )
{
	if ( canvas() )
		canvas()->resize( QMAX( 590, contentsRect().width()), QMAX(390, contentsRect().height() ));

	QCanvasView::resizeEvent (e);
}


void Editor::popupAddNode ()
{
	NodeItem *ni = new NodeItem( canvas() );
	ni->move (moving_start.x(), moving_start.y());
	ni->show ();

	nodeItems.append (ni);
}

void Editor::popupDelete ()
{
	QCanvasItemList l = canvas()->collisions (moving_start);
	for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
	{
		switch ((*it)->rtti())
		{
			case nodeRTTI:
				nodeItems.remove (static_cast<NodeItem *>(*it));
				break;
			case lineRTTI:
				lineItems.remove (static_cast<LineItem *>(*it));
				break;
			case polyRTTI:
				polyItems.remove (static_cast<PolyItem *>(*it));
				break;
		}

		delete *it;
		return;
	}
}

void Editor::popupSetMode (int _mode)
{
	polyCount = 0;
	mode = (EditMode)_mode;
}

#define FILE_VER 3

void Editor::slotLoad (QString filename)
{
	QFile f (filename);

	if ( f.open(IO_ReadOnly) )
	{
		slotClear ();

		QTextStream s (&f);

		QString id;
		int ver;
		s >> id >> ver;
		if (id != "VectEdit")
			return;

		//
		// Nodes first
		//
		int nodeCount;
		s >> nodeCount;
		for (int i = 0; i < nodeCount; i++)
		{
			int x, y;
			s >> x >> y;

			NodeItem *ni = new NodeItem( canvas() );
			ni->move (x, y);
			ni->show ();

			nodeItems.append (ni);
		}

		int lineCount;
		s >> lineCount;
		for (int i = 0; i < lineCount; i++)
		{
			int n[2];
			int r = 0, g = 0, b = 0;
			s >> n[0] >> n[1];

			if (ver > 1)
				s >> r >> g >> b;

			NodeItem *ni[2];
			int count = 0, i = 1;;

			NodeItemList::Iterator it;
			for (  it = nodeItems.begin(); it != nodeItems.end(); it++, i++ )
				if (i == n[count])
				{
					ni[count] = (*it);
					if (++count == 2)
						break;
				}

			QColor col;
			col.setRgb (r, g, b);
			LineItem *line = new LineItem (ni[0], ni[1], canvas(), &col);
			line->show ();
			lineItems.append (line);
		}

		if (ver > 2)
		{
			int polyCount;
			s >> polyCount;

			for (int i = 0; i < polyCount; i++)
			{
				int n[3];
				int r, g, b;

				s >> n[0] >> n[1] >> n[2];
				s >> r >> g >> b;

				NodeItem *ni[3];
				int count = 0, i = 1;;

				NodeItemList::Iterator it;
				for (  it = nodeItems.begin(); it != nodeItems.end(); it++, i++ )
					if (i == n[count])
					{
						ni[count] = (*it);
						if (++count == 3)
							break;
					}

				QColor col;
				col.setRgb (r, g, b);
				PolyItem *poly = new PolyItem (ni, canvas(), &col);
				poly->show ();
				polyItems.append (poly);
			}
		}
	}
}

void Editor::slotSave (QString filename)
{
	QFile f (filename);

	if ( f.open(IO_WriteOnly) )
	{
		QTextStream s (&f);

		s << "VectEdit" << " " << FILE_VER << "\n";

		// first count
		NodeItemList::Iterator ni;
		int count = 0;
		for (  ni = nodeItems.begin(); ni != nodeItems.end(); ++ni )
			count++;

		s << count << "\n";

		// then save
		for (  ni = nodeItems.begin(); ni != nodeItems.end(); ++ni )
			(*ni)->save (s);

		// first count
		LineItemList::Iterator li;
		count = 0;
		for (  li = lineItems.begin(); li != lineItems.end(); ++li )
			count++;

		s << count << "\n";

		// then save
		for (  li = lineItems.begin(); li != lineItems.end(); ++li )
			(*li)->save (s, nodeItems);

		// first count
		PolyItemList::Iterator pi;
		count = 0;
		for (  pi = polyItems.begin(); pi != polyItems.end(); ++pi )
			count++;

		s << count << "\n";

		// then save
		for (  pi = polyItems.begin(); pi != polyItems.end(); ++pi )
			(*pi)->save (s, nodeItems);
	}
}

void Editor::slotClear ()
{
	NodeItemList::Iterator it;
	for (  it = nodeItems.begin(); it != nodeItems.end(); ++it )
		delete (*it);

	nodeItems.clear ();
	lineItems.clear ();
	polyItems.clear ();
}


void Editor::setColor (const QColor &col)
{
	color = col;
}
