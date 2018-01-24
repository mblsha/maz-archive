/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void ViewBitmap::init()
{
    resize (640, 480);
    
    imagePlot->setBitmap ( Prefs::graphic(), false );
    imagePlot->setType ( tr("Bitmap") );
}
