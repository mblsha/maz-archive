#ifndef ATTR_H
#define ATTR_H

#include "common.h"

class NTFSPrivate;

class Attribute
{
public:
	Attribute(uchar *data, NTFSPrivate *p); // инициализация
	~Attribute();

	ulong type() const;
	char *name() const;
	bool resident() const;
	ullong size() const;
	ullong allocated() const; // сколько места на диске занимают данные
	ullong initialized() const; // инициализированный размер данных потока
	bool compressed() const;

	uchar *data(); // все данные
	void read_data(ulong offs, ulong len, uchar *buf);

private:
	class Private;
	Private *d;
};

#endif

