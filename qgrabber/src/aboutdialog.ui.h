/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/




void AboutDialog::init()
{
    QString l = label->text().arg ( VERSION );
    label->setText (l);
    
    int h = pixmapLabel->pixmap()->height();
    setMaximumSize ( QSize(maximumWidth(), h) );
}
