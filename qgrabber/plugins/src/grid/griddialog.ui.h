/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void GridDialog::accept()
{
    QSettings s;
    s.insertSearchPath ( QSettings::Windows, "/MAZsoft" );
    s.insertSearchPath ( QSettings::Unix, getHomeDir() );

    s.writeEntry ( "/QGrabber/GridPlugin/regularGrid", regularGrid->isChecked() );
    s.writeEntry ( "/QGrabber/GridPlugin/xSize",       xSize->value() );
    s.writeEntry ( "/QGrabber/GridPlugin/ySize",       ySize->value() );
    s.writeEntry ( "/QGrabber/GridPlugin/skipEmpties", skipEmpties->isChecked() );
    s.writeEntry ( "/QGrabber/GridPlugin/autocrop",    autocrop->isChecked() );
    s.writeEntry ( "/QGrabber/GridPlugin/name",        nameEdit->text() );
    s.writeEntry ( "/QGrabber/GridPlugin/type",        type->currentItem() );
    //s.writeEntry ( "/QGrabber/GridPlugin/colorDepth",  colorDepth->currentItem() );
    
    QDialog::accept ();
}

void GridDialog::init()
{
    QSettings s;
    s.insertSearchPath ( QSettings::Windows, "/MAZsoft" );
    s.insertSearchPath ( QSettings::Unix, getHomeDir() );
    
    // Grab method
    bool regular = s.readBoolEntry("/QGrabber/GridPlugin/regularGrid", false);
    regularGrid->setChecked ( regular );
    useColor->setChecked ( !regular );
    // Grid size
    xSize->setValue( s.readNumEntry("/QGrabber/GridPlugin/xSize", 32) );
    ySize->setValue( s.readNumEntry("/QGrabber/GridPlugin/ySize", 32) );
    // Skip empties
    skipEmpties->setChecked( s.readBoolEntry("/QGrabber/GridPlugin/skipEmpties", false) );
    // Autocrop
    autocrop->setChecked( s.readBoolEntry("/QGrabber/GridPlugin/autocrop", false) );
    // Name
    nameEdit->setText( s.readEntry("/QGrabber/GridPlugin/name", "") );
    // Type
    type->setCurrentItem( s.readNumEntry("/QGrabber/GridPlugin/type", 0) );
    // Color depth
    int depth = bitmap_color_depth ( Prefs::graphic() );
    int depthNum;
    switch ( depth )
    {
    case 8: depthNum = 0; break;
    case 15: depthNum = 1; break;
    case 16: depthNum = 2; break;
    case 24: depthNum = 3; break;
    case 32: depthNum = 4; break;
    }
    colorDepth->setCurrentItem( depthNum );
}
