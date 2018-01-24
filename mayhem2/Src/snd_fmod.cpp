#ifdef __SOUND_CPP__

//#include "def.h" // should be removed later

#include "fmod.h"
#include "fmod_errors.h"

static FMUSIC_MODULE *mod;
static FSOUND_STREAM *stream;
static int           fsound_channel;
static boolean       stream_end = false;

static char *fmod_extensions[] =
{
   // FMUSIC Interface
   "mod",
   "s3m",
   "xm",
   "it",
#ifndef USE_MIDI
   "mid",
#endif
   "rmi",
   "sgt",

   // FSOUND_Stream Interface
   "wav",
   "mp2",
#ifndef USE_ALMP3
   "mp3",
#endif
   "ogg",
   "wma",
   "asf",
   NULL
};

BEGIN_COMMAND (fmod_output_desc, "List fmod sound output method")
{
#ifdef ALLEGRO_WINDOWS
   console.printf("1 - Direct Sound\n");
   console.printf("2 - Windows Multimedia Waveout\n");
   console.printf("3 - A3D\n");
#else  
   console.printf("1 - Open Sound System (OSS) (Linux, Solaris, freebsd)\n");
   console.printf("2 - Elightment Sound Daemon (ESD, Linux, more ...)\n");
   console.printf("3 - Alsa Sound System (Linux)\n");
#endif
   
   console.printf("4 - NoSound\n");
}
END_COMMAND (fmod_output_desc)

BEGIN_COMMAND (fmod_driver_desc, "Lists available sound drivers")
{
   int   i;
   char  buf[128];

   for (i=0; i < FSOUND_GetNumDrivers(); i++)
   {
      // print driver names
      console.printf("%d - %s\n", i+1, uconvert ((const char *)FSOUND_GetDriverName(i), U_ASCII_CP, buf, U_CURRENT, sizeof(buf)));  
   }
}
END_COMMAND (fmod_driver_desc)

BEGIN_CUSTOM_CVAR (fmod_output, "0", CVAR_ARCHIVE)
{

   if (var.value < 1)
   {
      var.set ( (float)1 );
      return;
   }
   
   if (var.value > 4)
   {
      var.set ( (float)4 );
      return;
   }

   switch ( (int)var.value )
   {
#ifdef ALLEGRO_WINDOWS
      case 1 :  FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
                break;
      case 2 :  FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
                break;
      case 3 :  FSOUND_SetOutput(FSOUND_OUTPUT_A3D);
                break;
#else
      case 1 :  FSOUND_SetOutput(FSOUND_OUTPUT_OSS);
                break;
      case 2 :  FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
                break;
      case 3 :  FSOUND_SetOutput(FSOUND_OUTPUT_ALSA);
                break;                
#endif
      case 4 :  FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
                break;
   }
}
END_CUSTOM_CVAR (fmod_output)

BEGIN_CUSTOM_CVAR (fmod_driver, "0", CVAR_ARCHIVE)
{
   if ( var.value < 1 )
   {
      var.set ( (float)1 );
      return;
   }
   
   if ( var.value >= FSOUND_GetNumDrivers() )
   {
      var.set ( (float)(FSOUND_GetNumDrivers() - 1) );
      return;
   }
   
   FSOUND_SetDriver ( (int)var.value );
}
END_CUSTOM_CVAR (fmod_driver)

int fmod_init (void)
{
   char buf[256];

   if (FSOUND_GetVersion() < FMOD_VERSION)
   {
      usprintf ( buf, "You are using the wrong FMOD.DLL version!\nYou should be using FMOD %.02f", FMOD_VERSION );
      ustrcpy  ( snd_error, buf );
      
      return 1;
   }

   fmod_output.set (fmod_output.value);
   fmod_driver.set (fmod_driver.value);

   if ( !FSOUND_Init (44100, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH) )
   {
      ustrcpy ( snd_error, FMOD_ErrorString ( FSOUND_GetError () ) );
      
      return 1;
   }      

   return 0;
}

void fmod_shutdown (void)
{
   fmod_play (NULL);

   FSOUND_Close ();
}

boolean fmod_extension (char *ext)
{
   int i;

   for (i = 0; fmod_extensions[i]; i++)
      if ( !ustricmp (ext, fmod_extensions[i] ) )
         return true;

   return false;
}

static signed char fmod_stream_callback (FSOUND_STREAM *stream, void *buff, int len, int param)
{
   stream_end = true;

   return 0;
}

int fmod_play_file (char *file)
{
   mod = FMUSIC_LoadSong (file);

   if (!mod)
      stream = FSOUND_Stream_OpenFile (file, FSOUND_NORMAL /*| FSOUND_LOOP_NORMAL*/ | FSOUND_2D /*| FSOUND_MPEGACCURATE*/, 0);

   /*if ( !mod && !stream )
   {
      console.dprintf ("Neither mod or stream has been loaded\n");
   }*/

   if ( mod )
   {
      FMUSIC_SetMasterVolume  ( mod, (int)music_vol.value / (FMUSIC_GetType(mod) == FMUSIC_TYPE_IT ? 2 : 1) );
      FMUSIC_SetPanSeperation ( mod, 0.85f );
      
      if ( FMUSIC_PlaySong (mod) )
         return 0;
   }
   else if ( stream )
   {
      stream_end = false;

      FSOUND_SetVolume ( FSOUND_ALL, (int)music_vol.value );
      
      if ( (fsound_channel = FSOUND_Stream_Play (FSOUND_FREE, stream)) >= 0 )
      {
         FSOUND_Stream_SetEndCallback ( stream, fmod_stream_callback, 0 );

         return 0;
      }
   }

   strcpy ( snd_error, FMOD_ErrorString ( FSOUND_GetError () ) );

   return 1;
}

int fmod_play (void *data)
{
   if (mod)
   {
      FMUSIC_StopSong (mod);
      FMUSIC_FreeSong (mod);
   }

   if (stream)
   {
      FSOUND_Stream_Stop  (stream);
      FSOUND_Stream_Close (stream);
   }
   
   mod    = NULL;
   stream = NULL;
   
   return 0;
}

static int fmod_poll_helper ( int loop )
{
   char buf[64];

   stream_end = false;
      
   //console.dprintf ("Now we will change tracks!\n");

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
      fmod_play (NULL);

      return 0;
   }

   return 1;
}

int fmod_poll (int loop)
{
   if ( mod )
   {
      if ( !FMUSIC_IsPlaying ( mod ) )
         return fmod_poll_helper ( loop );
   }
   else if ( stream )
   {
      if ( stream_end )
         return fmod_poll_helper ( loop );
   }
   else
   {
      return 0;
   }

   return 1;
}

void fmod_volume (int vol)
{
   if (mod)
      FMUSIC_SetMasterVolume ( mod, vol / (FMUSIC_GetType(mod) == FMUSIC_TYPE_IT ? 2 : 1) );
   else if (stream)
      FSOUND_SetVolume ( FSOUND_ALL, vol );
}

void fmod_pause (void)
{
   if (mod)
   {
      FMUSIC_SetPaused (mod, TRUE);
   }
   else if (stream)
   {
      FSOUND_SetPaused (fsound_channel, true);
   }
}

void fmod_resume (void)
{
   if (mod)
   {
      FMUSIC_SetPaused (mod, FALSE);
   }
   else if (stream)
   {
      FSOUND_SetPaused (fsound_channel, false);
   }
}

#endif // __SOUND_CPP__
