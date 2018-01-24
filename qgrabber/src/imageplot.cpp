/****************************************************************************
** Form implementation generated from reading ui file 'imageplot.ui'
**
** Created: Чт 21. мар 16:45:19 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "imageplot.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "bitmapview.h"
#include "imageplot.ui.h"

/* 
 *  Constructs a ImagePlot which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
ImagePlot::ImagePlot( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "ImagePlot" );
    resize( 384, 225 ); 
    setCaption( trUtf8( "Image plot" ) );
    ImagePlotLayout = new QVBoxLayout( this, 0, 6, "ImagePlotLayout"); 

    Layout9 = new QHBoxLayout( 0, 0, 6, "Layout9"); 

    imageType = new QLabel( this, "imageType" );
    imageType->setText( trUtf8( "image type" ) );
    Layout9->addWidget( imageType );

    imageInfo = new QLabel( this, "imageInfo" );
    imageInfo->setText( trUtf8( "image info" ) );
    Layout9->addWidget( imageInfo );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout9->addItem( spacer );

    TextLabel8 = new QLabel( this, "TextLabel8" );
    TextLabel8->setText( trUtf8( "Zoom:" ) );
    Layout9->addWidget( TextLabel8 );

    zoom = new QSpinBox( this, "zoom" );
    zoom->setMaxValue( 10 );
    zoom->setMinValue( 1 );
    Layout9->addWidget( zoom );
    ImagePlotLayout->addLayout( Layout9 );

    bitmapView = new BitmapView( this, "bitmapView" );
    ImagePlotLayout->addWidget( bitmapView );

    // signals and slots connections
    connect( zoom, SIGNAL( valueChanged(int) ), bitmapView, SLOT( setZoom(int) ) );
    connect( bitmapView, SIGNAL( zoomChanged(int) ), zoom, SLOT( setValue(int) ) );
    connect( this, SIGNAL( bitmapChanged(BITMAP*) ), bitmapView, SLOT( setBitmap(BITMAP*) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ImagePlot::~ImagePlot()
{
    // no need to delete child widgets, Qt does it all for us
}

