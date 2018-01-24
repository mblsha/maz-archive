#ifndef ATTR_H
#define ATTR_H

#include "common.h"

class NTFSPrivate;

class Attribute
{
public:
	Attribute(uchar *data, NTFSPrivate *p); // ���樠������
	~Attribute();

	ulong type() const;
	char *name() const;
	bool resident() const;
	ullong size() const;
	ullong allocated() const; // ᪮�쪮 ���� �� ��᪥ �������� �����
	ullong initialized() const; // ���樠����஢���� ࠧ��� ������ ��⮪�
	bool compressed() const;

	uchar *data(); // �� �����
	void read_data(ulong offs, ulong len, uchar *buf);

private:
	class Private;
	Private *d;
};

#endif

