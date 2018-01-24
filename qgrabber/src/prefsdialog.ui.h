/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void PrefsDialog::init()
{
	Prefs *prefs = Prefs::prefs();

    // Password list
    password->insertStringList ( prefs->passwords() );
    // Current password
    password->setCurrentText ( QString::fromUtf8( prefs->password() ) );
    // Compression level
    compressionLevel->setCurrentItem ( prefs->pack() );
    // Strip level
    stripLevel->setCurrentItem ( prefs->strip() );
    // Strip exceptions
    stripExceptions->setText ( prefs->stripExceptions() );
    // Backups
    doBackups->setChecked ( prefs->backups() );
    // Dither
    dither->setChecked ( prefs->dither() );
    // Save header
    saveHeader->setChecked ( prefs->saveHeader() );
    // Verbose
    verbose->setChecked ( prefs->verbose() );
    // Style
    styleSelector->insertStringList ( QStyleFactory::keys() );
    styleSelector->setCurrentText ( prefs->style() );    
    // Open nested
    openNested->setChecked ( prefs->openNested() );
    // Resources Directory
    resourcesDir->setText ( prefs->resourcesDir() );
}


void PrefsDialog::accept()
{
	Prefs *prefs = Prefs::prefs();

    // Password list
    addPassword(); 
    QStringList pass;
    for (int i = 0; i < password->count(); i++)
    {
		QString p = password->text( i );
		pass += p;
    }
    prefs->setPasswords ( pass );
    // Current password
    prefs->setPassword ( password->currentText().utf8() );
    // Compression level
    prefs->setPack ( compressionLevel->currentItem() );
    // Strip level
    prefs->setStrip ( stripLevel->currentItem() );
    // Strip exceptions
    prefs->setStripExceptions ( stripExceptions->text().latin1() );
    // Backups
    prefs->setBackups ( doBackups->isChecked() );
    // Dither
    prefs->setDither ( dither->isChecked () );
    // Save header
    prefs->setSaveHeader ( saveHeader->isChecked() );
    // Verbose
    prefs->setVerbose ( verbose->isChecked() );
    // Style
    prefs->setStyle ( styleSelector->currentText () );
    // Open Nested
    prefs->setOpenNested( openNested->isChecked() );
    // Resources Directory
    QString resDirOld = prefs->resourcesDir ();
    QString resDir = resourcesDir->text();
    prefs->setResourcesDir ( resDir );
    
    // Display Information msg box
    if ( resDirOld != resDir )
		QMessageBox::information (0, tr("Information"), tr("For some of your changes to take effect,\nyou need to restart program"), QMessageBox::Ok, QMessageBox::NoButton);;
    
    prefs->save ();
    
    QDialog::accept();
}




void PrefsDialog::addPassword()
{
    // Insert current password combobox item into its list
    bool found = false;
    QString pass = password->currentText ();
    
    for (int i = 0; i < password->count(); i++)
    {
		QString p = password->text( i );
		if (pass == p)
			found = true;
    }

    if ( !found )    
		password->insertItem ( pass );
}
