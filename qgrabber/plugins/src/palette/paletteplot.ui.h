/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void PalettePlot::setPalette( RGB *pal )
{
    bool paletteInUse = true;
    RGB *palette = Prefs::palette ();
    for (int i = 0; i < 256; i++)
    {
	if ( pal[i].r != palette[i].r ||
	     pal[i].g != palette[i].g ||
	     pal[i].b != palette[i].b )
	{
	    paletteInUse = false;
	}
    }
    
    if ( paletteInUse )
	paletteNotInUse->hide ();
    
    emit paletteChanged(pal);
}
