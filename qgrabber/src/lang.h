/***************************************************************************
                          lang.h  -  description
                             -------------------
    begin                : Пон Сен 30 16:45:16 MSD 2002
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

#ifndef LANG_H
#define LANG_H

#include "varlist.h"
#include <qstringlist.h>

class Lang
{
private:
	VarList langs;

	QStringList searchPaths();

public:
	Lang ()
	~Lang ();

	VarList list () const;
	void set (const QString &lang);
};

#endif
