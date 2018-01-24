/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void FontPlot::setData( DATAFILE *dat, ObjectPlugin *plug )
{
    d = dat;
    emit dataChanged(d);
    
    connect (rangeSelect, SIGNAL(updated()), plug, SIGNAL(dirty()));
}

void FontPlot::updateInfo()
{
    if ( !d )
    {
	qWarning ("FontPlot::updateInfo(): no data!");
	return;
    }
    
    FONT *fnt = (FONT *)d->dat;
    QString mono = (fnt->vtable == font_vtable_mono) ? tr("Mono") : tr("Color");
    int ranges = 0;
    int glyphs = 0;

    if(fnt->vtable == font_vtable_mono) {
	FONT_MONO_DATA* mf = (FONT_MONO_DATA *)fnt->data;
	
	while(mf) {
	    ranges++;
	    glyphs += mf->end - mf->begin;
	    mf = mf->next;
	}
    } else {
	FONT_COLOR_DATA* cf = (FONT_COLOR_DATA *)fnt->data;
	
	while(cf) {
	    ranges++;
	    glyphs += cf->end - cf->begin;
	    cf = cf->next;
	}
    }
    
    QString info = tr("%1 font, %2 %3, %4 glyphs").arg(mono)
	    .arg(ranges)
	    .arg((ranges == 1) ? tr("range") : tr("ranges"))
	    .arg(glyphs);    
    
    fontInfo->setText(info);
}
