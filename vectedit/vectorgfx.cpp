/***************************************************************************
                          vector.cpp  -  description
                             -------------------
    begin                : óÂÔ íÁÊ 4 2002
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

#include "vectorgfx.h"

/*****************************************************************************
 *
 * Class Vertex
 *
 *****************************************************************************/

Vertex::Vertex (mfloat x, mfloat y, int _color)
{
	color = _color;
	pos.set (x, y);
}

Vertex::Vertex (const Vector &_pos, int _color)
{
	color = _color;
	pos = _pos;
}

Vertex::~Vertex ()
{
}

Vector Vertex::getPos ()
{
	return pos;
}

mfloat Vertex::x ()
{
	return pos.x;
}

mfloat Vertex::y ()
{
	return pos.y;
}

void Vertex::setActive (bool _active)
{
	active = _active;
}

void Vertex::draw (QPaintDevice *device)
{
	QPainter paint (device);

	int r = active ? 10 : 5;

	paint.setPen (Qt::red);
	paint.drawEllipse ((int)pos.x, (int)pos.y, r, r);
}

/*****************************************************************************
 *
 * Class Line
 *
 *****************************************************************************/

Line::Line (Vertex *v1, Vertex *v2, int _color)
{
	v[0] = v1;
	v[1] = v2;
	color = _color;
}

Line::Line (Vertex *_v[2], int _color)
{
	v[0] = _v[0];
	v[1] = _v[1];
	color = _color;
}

Line::~Line ()
{
}

void Line::setColor (int _color)
{
	color = _color;
}

void Line::setActive (bool _active)
{
	active = _active;
}

void Line::draw (QPaintDevice *device)
{
	if (!v[0] || !v[1])
		return;

	QPainter paint (device);

	paint.setPen (active ? Qt::red : Qt::black);
	paint.drawLine ((int)v[0]->x(), (int)v[0]->y(), (int)v[1]->x(), (int)v[1]->y());
}
