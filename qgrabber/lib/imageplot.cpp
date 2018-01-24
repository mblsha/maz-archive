#include "imageplot.h"

ImagePlot::ImagePlot (QWidget *parent, const char *name, WFlags fl)
	: ImagePlotBase (parent, name, fl)
{
}

ImagePlot::~ImagePlot ()
{
}

void ImagePlot::setBitmap (BITMAP *bmp, bool hasAlpha)
{
	ImagePlotBase::setBitmap (bmp, hasAlpha);
}

void ImagePlot::setType (const QString &type)
{
	ImagePlotBase::setType (type);
}
