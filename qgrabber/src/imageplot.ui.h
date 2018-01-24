/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void ImagePlot::setType( const QString &type )
{
    imageType->setText(type);
}

void ImagePlot::setBitmap( BITMAP *bmp, bool hasAlpha )
{
    QString info = tr("(%1x%2, %3 bit)%4")
		   .arg(bmp->w)
		   .arg(bmp->h)
		   .arg(bmp->vtable->color_depth)
		   .arg( hasAlpha ? tr(" + alpha") : QString("") );
    
    imageInfo->setText(info);
    
    emit bitmapChanged(bmp);
}


