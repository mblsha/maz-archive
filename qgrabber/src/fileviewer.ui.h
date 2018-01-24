/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void FileViewer::setCaption(const QString &cap)
{
    QString c = QString("QGrabber: %1").arg(cap);
    QDialog::setCaption ( c );
}

void FileViewer::setFile( const QString &fileName, bool rich )
{
    QString text;
    
    QFile f (fileName);
    if (f.open(IO_ReadOnly)) 
    {
	QTextStream t(&f);
	while (!t.eof())
	    text += t.readLine() + '\n';
	f.close();

	textView->setTextFormat(rich ? QTextEdit::RichText : QTextEdit::PlainText);
    }
    else
    {
	text = tr("<big><b>Could not open '%1'</b></big>").arg(fileName);   
    }

    textView->setText (text);
}


void FileViewer::init()
{
    resize (580, 400);
}
