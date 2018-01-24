/****************************************************************************
** Form interface generated from reading ui file 'imageplot.ui'
**
** Created: Чт 21. мар 16:44:36 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef IMAGEPLOT_H
#define IMAGEPLOT_H

#include <qvariant.h>
#include <qwidget.h>
#include "alleg.h"
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class BitmapView;
class QLabel;
class QSpinBox;

class ImagePlot : public QWidget
{ 
    Q_OBJECT

public:
    ImagePlot( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ImagePlot();

    QLabel* imageType;
    QLabel* imageInfo;
    QLabel* TextLabel8;
    QSpinBox* zoom;
    BitmapView* bitmapView;


signals:
    void bitmapChanged(BITMAP*);

public slots:
    virtual void setType(const QString & type);
    virtual void setBitmap(BITMAP * bmp, bool hasAlpha);

protected:
    QVBoxLayout* ImagePlotLayout;
    QHBoxLayout* Layout9;
};

#endif // IMAGEPLOT_H
