/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void SamplePlot::setSample( SAMPLE *s )
{
    samp = s;
    
    long sec = (samp->len + samp->freq/2) * 10 / MAX(samp->freq, 1);
    QString info = tr("(%1 bit %2, %3, %4.%5 sec)")
		   .arg(samp->bits)
		   .arg(samp->stereo ? tr("stereo") : tr("mono"))
		   .arg(samp->freq)
		   .arg(sec/10)
		   .arg(sec%10);
    sampleInfo->setText (info);
}


void SamplePlot::stopPressed()
{
    stop_sample (samp);
}

void SamplePlot::playPressed()
{
    play_sample (samp, 255, 127, 1000, FALSE);
}
