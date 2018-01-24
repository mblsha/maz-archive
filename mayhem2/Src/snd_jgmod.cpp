#ifdef __SOUND_CPP__

#include <jgmod.h>

static JGMOD *mod;
static int   reserved_channels;

static char *jgmod_extensions[] =
{
   "mod",
   "s3m",
   "xm",
   "umx",
   "jgm",
   NULL
};

int jgmod_init (void)
{
   register_datafile_jgmod ();
   
   return 0;
}

void jgmod_shutdown (void)
{
   if (mod)
   {
      stop_mod ();
      destroy_mod (mod);
      mod = NULL;
   }

   return;
}

boolean jgmod_extension (char *ext)
{
   int i;

   for (i = 0; jgmod_extensions[i]; i++)
      if ( !ustricmp (ext, jgmod_extensions[i] ) )
         return true;

   return false;
}

static int jgmod_play_helper (JGMOD *data)
{
   if ( mod->no_chn > MAX_ALLEG_VOICE)
      reserved_channels = MAX_ALLEG_VOICE;
   else
      reserved_channels = mod->no_chn;

   reserve_voices ( reserved_channels, -1 );

   if ( install_mod ( reserved_channels ) < 0 )
   {
      usprintf ( snd_error, "Unable to allocate %d voices", reserved_channels );
      return 1;
   }

   set_mod_speed  ( 100 );
   set_mod_pitch  ( 100 );
   set_mod_volume ( (int)music_vol.value );
   play_mod ( data, FALSE );

   return 0;
}

int jgmod_play_file (char *file)
{
   mod = load_mod (file);
   if ( !mod )
   {
      ustrcpy ( snd_error, jgmod_error );
      return 1;
   }

   return jgmod_play_helper (mod);

   return 0;
}

int jgmod_play (void *data)
{
   if ( mod )
   {
      stop_mod ();
      destroy_mod (mod);
      mod = NULL;
   }
   
   if ( data )
   {
      //mod = (JGMOD *)malloc ( sizeof (data) );
      //mod = (JGMOD *)memcpy ( (JGMOD *)mod, (JGMOD *)data, sizeof (data) );
   
      return jgmod_play_helper ((JGMOD *)mod);
   }
   
   return 1;
}

int jgmod_poll (int loop)
{
   char buf[64];

   if ( !is_mod_playing () )
   {
      console.dprintf ("mod not playing\n");
   
      if (loop == 1)
      {
         usprintf (buf, "music_play %d\n", mlist.current);
         cbuf_add_text (buf);
      }
      else if (loop == 0)
      {
         mlist_next_f ();
      }
      else // (music_loop.value != 0) && (music_loop.value != 1)
      {
         jgmod_play (NULL);

         return 0;
      }
   }
   else
   {
      return 0;
   }
   
   return 1;
}

void jgmod_volume (int vol)
{
   set_mod_volume ( vol );
}

void jgmod_pause (void)
{
   pause_mod ();
}

void jgmod_resume (void)
{
   resume_mod ();
}

#endif // __SOUND_CPP__
