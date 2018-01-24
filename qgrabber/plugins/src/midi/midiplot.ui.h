/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void MidiPlot::stopPressed()
{
    stop_midi ();
}

void MidiPlot::playPressed()
{
    play_midi (midi, false);
}

void MidiPlot::setMidi( MIDI *m )
{
    midi = m;
}
