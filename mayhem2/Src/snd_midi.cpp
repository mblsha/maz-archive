#ifdef __SOUND_CPP__

static MIDI       *midi = NULL;

boolean midi_extension (char *ext)
{
   return !ustricmp (ext, "mid") ? true : false;
}

int midi_init(void)
{
   return 0;
}

void midi_shutdown(void)
{
   midi_play(NULL);
   midi = NULL;
}

int midi_play_file(char *file)
{
   if(file == NULL)
   {
      stop_midi();
      midi = NULL;

      // successfully stopped music output
      return -1;
   }
   else
   {
      midi = load_midi(file);
   }

   if (play_midi(midi, FALSE))
      return 1;

   return 0;
}

int midi_play(void *data)
{
   if(data == NULL)
   {
      stop_midi();
      midi = NULL;

      return -1;
   }
   else
   {
      midi = (MIDI *)data;
   }

   if(play_midi(midi, FALSE))
      return 1;

   return 0;
}

int midi_poll(int loop)
{
   char buf[64];

   if(midi)
   {
      if (midi_pos < -1)
      {
         if (loop == 1)
         {
            usprintf(buf, "music_play %d\n", mlist.current);
            cbuf_add_text(buf);
         }
         else if (loop == 0)
         {
            mlist_next_f();
         }
         else // (music_loop.value != 0) && (music_loop.value != 1)
         {
            midi_play(NULL);

            return 0;
         }
      }
   }
   else
   {
      return 0;
   }

   return 1;
}

void midi_volume(int vol)
{
   set_volume(-1, vol);
}

/*
void midi_pause()
{
   midi_pause();
}

void midi_resume()
{
   midi_resume();
}
*/

#endif
