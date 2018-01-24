/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void SaveProgress::init()
{
    totalUnpacked = totalPacked = -1;
    
    numItems = -1;
    saved = 0;
}

static int percent(int a, int b)
{
    float result;
    if (a)
	result = (b * 100) / a;
    else
	result = 0;
    return (int)result;
}

void SaveProgress::savedItem( long total, long packed )
{
    saved++;
    
    progress->setProgress ( percent(numItems, saved) );
    
    totalUnpacked += total;
    totalPacked += packed;
    
    packRatio->setText ( QString("%1").arg( percent(totalUnpacked, totalPacked) ) );
    
    totalSize->setText  ( QString("%1").arg(totalUnpacked) );
    packedSize->setText ( QString("%1").arg(totalPacked) );
    
    if (saved >= numItems)
    {
	button->setText ( tr("&Done") );
	button->setEnabled (true);
    }
}

void SaveProgress::savingItem( QString name )
{
    itemName->setText (name);
}

void SaveProgress::setNumItems( int num )
{
    numItems = num;
}
