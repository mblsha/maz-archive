#ifndef IMAGEPLOT_H
#define IMAGEPLOT_H

#include "export.h"

#include "alleg.h"
#include "imageplotbase.h"
#include <qstring.h>

class EXPORT ImagePlot : public ImagePlotBase
{
	Q_OBJECT

public:
	ImagePlot (QWidget *parent = 0, const char *name = 0, WFlags fl = 0);
	~ImagePlot ();

public slots:
	void setBitmap (BITMAP *bmp, bool hasAlpha);
	void setType (const QString &type);
};

#endif
