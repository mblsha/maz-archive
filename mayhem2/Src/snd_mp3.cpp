#ifdef __SOUND_CPP__

#include "mp3.cpp"

static MP3FILE    *mp3  = NULL;

boolean mp3_extension (char *ext)
{
   return !ustricmp (ext, "mp3") ? true : false;
}

int mp3_init(void)
{
   return 0;
}

void mp3_shutdown(void)
{
   mp3_play(NULL);
   mp3 = NULL;
}

int mp3_play_file(char *file)
{
   if(file == NULL)
   {
      close_mp3_file(mp3);
      mp3 = NULL;

      // successfully stopped music output
      return -1;
   }
   else
   {
      mp3 = open_mp3_file(file);
   }

   if(!mp3 || play_mp3_file(mp3, BUFSZ, 128))
      return 1;

   return 0;
}

int mp3_play(void *data)
{
   if(data == NULL)
   {
      close_mp3_file(mp3);
      mp3 = NULL;

      return -1;
   }
   else
   {
      mp3 = (MP3FILE *)data;
   }

   if(play_mp3_file(mp3, BUFSZ, 128))
      return 1;

   return 0;
}

int mp3_poll(int loop)
{
   int  mp3_state;
   char buf[64];

   if(mp3)
   {
      switch (mp3_state = poll_mp3_file(mp3))
      {
         case ALMP3_OK:
            break;

         case ALMP3_POLL_PLAYJUSTFINISHED:
            //console_dprintf("MP3: Just finished\n");
            if (loop == 1)
            {
               usprintf(buf, "music_play %d\n", mlist.current);
               cbuf_add_text(buf);
            }
            else if (loop == 0)
            {
               mlist_next_f();
            }
            else // (loop != 0) && (loop != 1)
            {
               goto close;
            }
            break;

         case ALMP3_POLL_NOTPLAYING:
            // it's just paused
            //console_dprintf("MP3: Not playing\n");
            break;

         case ALMP3_POLL_FRAMECORRUPT:
            console.dprintf("MP3: Frame corrupt\n");
            goto close;

         case ALMP3_POLL_INTERNALERROR:
            console.dprintf("MP3: Internal error\n");
            goto close;
      }
   }
   else
   {
      return 0;
   }

   return 1;

close:

   mp3_play(NULL);

   return 0;
}

void mp3_volume(int vol)
{
   if(mp3)
      adjust_mp3stream(mp3->s, (int)vol, 128, 1000);
}

void mp3_pause()
{
   if(mp3)
      stop_mp3stream(mp3->s);
}

void mp3_resume()
{
   if(mp3)
      play_mp3_file(mp3, BUFSZ, 128);
}

#endif
