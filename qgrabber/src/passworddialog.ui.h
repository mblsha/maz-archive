/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void PasswordDialog::accept()
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
    
    prefs->save ();
    
    QDialog::accept();
}

void PasswordDialog::init()
{
	Prefs *prefs = Prefs::prefs();

    // Password list
    password->insertStringList ( prefs->passwords() );
    // Current password
    password->setCurrentText ( QString::fromUtf8( prefs->password() ) );
}

void PasswordDialog::addPassword()
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
