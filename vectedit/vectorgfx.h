/***************************************************************************
                          vector.h  -  description
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


#ifndef VECTOR_GFX_H
#define VECTOR_GFX_H

#include <qpainter.h>

#include "matrix.h"

class Vertex
{
private:
	Vector pos;
	int color;
	bool active;

public:
	Vertex (mfloat x = 0, mfloat y = 0, int color = 0);
	Vertex (const Vector &vec, int color = 0);
	~Vertex ();

	void set (const Vector &vec);
	Vector getPos ();
	mfloat x ();
	mfloat y ();
	void setActive (bool active);
	void draw (QPaintDevice *device);
};

class Line
{
private:
	Vertex *v[2];
	int color;
	bool active;

public:
	Line (Vertex *v1, Vertex *v2, int color = 0);
	Line (Vertex *v[2], int color = 0);
	~Line ();

	void setColor (int color);
	void setActive (bool active);
	void draw (QPaintDevice *device);
};

#endif
