#include "def.h"
#include "sound.h"
//#include "string.h"

#define DAT_ALMP3               DAT_ID('M','P','3',' ')
#define DAT_FMOD                DAT_ID('F','M','O','D')

char snd_error[256];

static void mlist_next_f (void);

DATAFILE *music_data = NULL;

SND_DRIVER snd_drivers[] =
{
#ifdef USE_MIDI
   {
      "MIDI",
      false,
      false,

      DAT_MIDI,

      midi_init,
      midi_shutdown,
      midi_extension,
      midi_play_file,
      midi_play,
      midi_poll,
      midi_volume,
      midi_pause,
      midi_resume
   }
   ,
#endif

#ifdef USE_FMOD
   {
      "FMOD",
      false,
      false,

      DAT_FMOD,

      fmod_init,
      fmod_shutdown,
      fmod_extension,
      fmod_play_file,
      fmod_play,
      fmod_poll,
      fmod_volume,
      fmod_pause,
      fmod_resume
   }
   ,
#endif

#ifdef USE_ALMP3
   {
      "MP3",
      false,
      false,

      DAT_ALMP3,

      mp3_init,
      mp3_shutdown,
      mp3_extension,
      mp3_play_file,
      mp3_play,
      mp3_poll,
      mp3_volume,
      mp3_pause,
      mp3_resume
   }
   ,
#endif

#ifdef USE_JGMOD
   {
      "JGMOD",
      false,
      false,

      JGM_ID,

      jgmod_init,
      jgmod_shutdown,
      jgmod_extension,
      jgmod_play_file,
      jgmod_play,
      jgmod_poll,
      jgmod_volume,
      jgmod_pause,
      jgmod_resume
   }
   ,
#endif

   {
      NULL,
      false,
      false,

      0,

      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   }
};

BEGIN_CUSTOM_CVAR (sound_vol,           "255", CVAR_ARCHIVE)
{
   int t = (int)var.value;

   if(t < 0)
      t = 0;
   else if(t > 255)
      t = 255;

   if (t != (int)var.value)
      var.set (t);
}
END_CUSTOM_CVAR (sound_vol)

BEGIN_CUSTOM_CVAR (music_vol,           "255", CVAR_ARCHIVE)
{
   int i;
   int t = (int)var.value;

   if(t < 0)
      t = 0;
   else if(t > 255)
      t = 255;

   if (t != (int)var.value)
      var.set (t);

   for(i = 0; snd_drivers[i].name; i++)
   {
      if ( snd_drivers[i].initialized )
      {
         snd_drivers[i].volume(t);
      }
   }
}
END_CUSTOM_CVAR (music_vol)

CVAR (music_loop,          "0",   CVAR_ARCHIVE);
CVAR (mlist_autoadd,       "1",   CVAR_ARCHIVE);
CVAR (mlist_recurse,       "1",   CVAR_ARCHIVE);
CVAR (sound_delta,         "10",  CVAR_ARCHIVE);

typedef struct MUSIC_LIST_NODE
{
   struct MUSIC_LIST_NODE *next;
   struct MUSIC_LIST_NODE *prev;
   char                   file[256];
   int                    type;
   int                    num;
} MUSIC_LIST_NODE;

typedef struct MUSIC_LIST
{
   int             count;
   int             current;      // currently playing number
   int             type;         // currently played  type
   MUSIC_LIST_NODE *first;
   MUSIC_LIST_NODE *last;
} MUSIC_LIST;

MUSIC_LIST mlist;

#define __SOUND_CPP__
   #ifdef USE_MIDI
      #include "snd_midi.cpp"
   #endif

   #ifdef USE_FMOD
      #include "snd_fmod.cpp"
   #endif

   #ifdef USE_ALMP3
      #include "snd_mp3.cpp"
   #endif    
    
   #ifdef USE_JGMOD
      #include "snd_jgmod.cpp"
   #endif        
#undef __SOUND_CPP__

char       music_file[512];

static void add_to_mlist(AL_CONST char *file, int type)
{
   MUSIC_LIST_NODE *node   = NULL;
   MUSIC_LIST_NODE *insert = NULL;

   if(mlist.first == NULL)
   {
      mlist.first = (MUSIC_LIST_NODE *)malloc(sizeof(MUSIC_LIST_NODE));
      node = mlist.first;

      ustrcpy(node->file, file);
      node->type = type;
      node->next = NULL;
      node->prev = NULL;
      mlist.last = mlist.first;
   }
   else
   {
      node   = mlist.last;
      insert = (MUSIC_LIST_NODE *)malloc(sizeof(MUSIC_LIST_NODE));

      ustrcpy(insert->file, file);
      insert->type = type;
      insert->next = NULL;
      insert->prev = node;
      node->next   = insert;
      mlist.last   = node->next;
   }

   mlist.count++;
}

static boolean break_music_search = false;

static void check_music_file(AL_CONST char *filename, int attrib, int param)
{
   char buf[256];
   PACKFILE *f;
   DATAFILE *dat, *data;
   char *p;
   int i, j;

   if ( break_music_search || key[KEY_ESC] )
   {
      break_music_search = true;
      return;
   }

   if (attrib & FA_DIREC && mlist_recurse.value)
   {
      //
      // DIRECTORY
      //
      p = get_filename(filename);

      if ( ustricmp(p, ".") && ustricmp(p, "..") )
      {
         // recurse inside a directory
         ustrcpy(buf, filename);
         put_backslash(buf);
         strcat(buf, "*.*");

         for_each_file(buf, FA_ARCH | FA_RDONLY | FA_DIREC, check_music_file, param);

         return;
      }
   }

   //
   // Simple file
   //
   p = get_extension(filename);

   for (i = 0; snd_drivers[i].name; i++)
   {
      if ( snd_drivers[i].extension_supported (p) && snd_drivers[i].initialized )
      {
         f = pack_fopen (filename, F_READ);

         if (!f)
            return;

         pack_fclose (f);

         add_to_mlist (filename, snd_drivers[i].type);
      }
   }

   if ( !ustricmp(p, "dat") )
   {
      //
      // DATAFILE
      //
      dat = load_datafile(filename);

      if(dat)
      {
         for(i = 0; dat[i].type != DAT_END; i++)
         {
            data = dat + i;

            usprintf(buf, "%s#%s", filename, get_datafile_property(data, DAT_NAME));

            for(j = 0; snd_drivers[j].name; j++)
            {
               if( (dat[i].type == snd_drivers[j].type) && snd_drivers[j].initialized )
               {
                  add_to_mlist(buf, snd_drivers[j].type);
               }
            }
         }

         unload_datafile(dat);
      }
   }
}

BEGIN_COMMAND (mlist_add, "Adds directory to mlist")
{
   char buf[512], buf2[512];
   int  old_count;

   if(argc != 2)
   {
      console.printf (con_description, "%s <dir>: adds specified directory to mlist\n"
                                       "EXAMPLE: %s \"data\\music\"\n", argv[0].get_s (), argv[0].get_s ());
      return;
   }

   ustrcpy(buf, argv[1].get_s ());
   put_backslash(buf);
   ustrcat(buf, "*.*");
   ustrcpy(buf, fix_filename_slashes(buf));

   break_music_search = false;

   old_count = mlist.count;
   for_each_file (buf, FA_ARCH | FA_RDONLY | FA_DIREC, check_music_file, strlen(buf2)-3);
   console.printf (con_system, "Added %d music file(s) to list\n", mlist.count - old_count);
}
END_COMMAND (mlist_add)

static void remove_mlist_node(MUSIC_LIST_NODE *p)
{
   if(mlist.first == p) mlist.first = p->next;
   if(mlist.last  == p) mlist.last  = p->prev;
   if(p->prev) p->prev->next = p->next;
   if(p->next) p->next->prev = p->prev;

   ustrcpy(p->file, "");

   //delete(p);
   free (p);
   mlist.count--;
}

BEGIN_COMMAND (mlist_remove, "Removes mlist entries")
{
   MUSIC_LIST_NODE *node = mlist.first;
   MUSIC_LIST_NODE *x;
   int i, start, max;
   boolean start_it = false;

   if(argc == 1)
   {
      console.printf (con_description, "mlist_remove <first> [last]: removes mlist entries,\n"
                                       "\"first\" througth \"last\", if \"last\" is omitted, only removes \"first\"\n");
   }

   start = argv[1];
   if(!start)
   {
      console.printf ("\"first\" and \"last\" must be numbers!\n");
      return;
   }

   max = argv[2].get_int () ? argv[2].get_int () : start;

   for(i = 1; node && i <= max; i++)
   {
      if(i == start)
      {
         start_it = true;
      }

      x = node->next;

      if(start_it)
         remove_mlist_node(node);

      node = x;
   }
}
END_COMMAND (mlist_remove)


static void mlist_destroy ()
{
   MUSIC_LIST_NODE *node = mlist.first;
   MUSIC_LIST_NODE *x;

   while(node)
   {
      x = node->next;

      remove_mlist_node(node);
      node = NULL;

      node = x;
   }

   mlist.first   = NULL;

   mlist.count   = 0;
   mlist.current = 0;
}

BEGIN_COMMAND (mlist, "Displays its contents")
{
   MUSIC_LIST_NODE *node = mlist.first;
   int count = 0;

   while(node)
   {
      ++count;
      console.printf(count == mlist.current ? con_system : con_default, "%-3d: %s\n", count, node->file);
      node = node->next;
   }

   if(!count)
   {
      console.printf ("\"mlist\" is empty! Please try \"mlist_add\"\n");
   }
}
END_COMMAND (mlist)


static boolean music_paused = false;

void music_pause_f ()
{
   int i;

   for(i = 0; snd_drivers[i].name; i++)
   {
      if(snd_drivers[i].playing)
      {
         if(!music_paused)
         {
            console.printf ("%s paused\n", snd_drivers[i].name);
            music_paused = true;

            snd_drivers[i].pause();
         }
         else
         {
            console.printf ("%s resumed\n", snd_drivers[i].name);
            music_paused = false;

            snd_drivers[i].resume();
         }
      }
   }
}

BEGIN_COMMAND (music_pause, "Pause the music output")
{
   music_pause_f ();
}
END_COMMAND (music_pause)

BEGIN_COMMAND (music_play, "Plays a music file")
{
   int                  c, num, i, j;
   int                  type = 0;
   char                 file   [512];
   char                 buf    [512];
   char                 fname  [512];
   char                 objname[512];
   char                 *p;
   MUSIC_LIST_NODE      *node = mlist.first;
   DATAFILE             *data;
   boolean              from_mlist = false;
   boolean              is_number  = false;
   boolean              playing    = false;

   if (argc != 2)
   {
      console.printf (con_description, "music_play <music_file>: plays specified music.\n"
                                       "NOTE: 'music_file' can be a number in mlist\n");
      return;
   }

   ustrcpy (buf,  argv[1].get_s ());
   ustrcpy (file, argv[1].get_s ());

   if (!strcmp(file, "0"))
   {
      //console_warn();
      return;
   }

   if(music_paused)
   {
      music_pause_f();
   }

                                                    /* Don't even touch this ! */
   if (atoi(file) && (atoi(file) <= mlist.count) && !strstr(strlwr(file), ".dat"))
      is_number = true;

   for (i = 0; snd_drivers[i].name; i++)
   {
      snd_drivers[i].play (NULL);
      snd_drivers[i].playing = false;

      if (is_number)
      {
         if ( snd_drivers[i].extension_supported ( get_extension (file) ) && snd_drivers[i].initialized )
         {
            is_number = false;
         }
      }
   }

   if (music_data)
   {
      unload_datafile(music_data);

      music_data = NULL;
   }

   if (is_number)
   {
      //
      // Look in mlist for filename
      //
      num = atoi(file);

      c = 0;
      while (node)
      {
         if (++c == num)
         {
            mlist.current = num;

            from_mlist = true;

            //type = node->type;

            ustrcpy(file, node->file);
            break;
         }
         else if (c > mlist.count)
         {
            // That situation means error in code
            console.printf (con_error, "That number doesn't exist in mlist!\n"
                                       "Contact the developer about that error!\n");
            return;
         }

         node = node->next;
      }
   }

   /*for (i = 0; snd_drivers[i].type; i++)
   {
      if ( snd_drivers[i].play_file (file) )
      {
         snd_drivers[i].playing = playing = true;
         goto song_loaded;
      }
   }*/
                             /*  And This !!! */
   if (strchr (file, '#') && strstr(strlwr(file), ".dat"))
   {
      //
      // Loading from datafile
      //
      ustrcpy(fname, file);
      p = strchr(fname, '#');
      usetat(p, 0, '\0');
      //ustrcpy(objname, p);
      ustrcpy(objname, p+uwidth(p));

      music_data = load_datafile(fname);

      if (!music_data)
      {
         console.printf (con_error, "Can't open \"%s\" datafile!\n", fname);
         music_data = NULL;
         return;
      }

      for (i = 0; music_data[i].type != DAT_END; i++)
      {
         data = music_data + i;

         if ( !stricmp (objname, get_datafile_property (data, DAT_NAME) ) )
         {
            for (j = 0; snd_drivers[j].name; j++)
            {
               if ( music_data[i].type == snd_drivers[j].type )
               {
                  if ( snd_drivers[j].initialized )
                  {
                     if ( !snd_drivers[j].play (music_data[i].dat) )
                     {
                        snd_drivers[j].playing = playing = true;

                        goto song_loaded;
                     }
                  }
                  else
                  {
                     ustrcpy ( snd_error, snd_drivers[j].name );
                     ustrcat ( snd_error, " not initialized" );
                  }
               }
            }
         }
      }

song_loaded:

      if (!playing)
      {
         unload_datafile(music_data);
         music_data = NULL;

         if ( snd_error )
         {
            console.printf (con_error, "Error playing \"%s\": %s\n", objname, snd_error);
         }
         else
         {
            console.printf (con_error, "Can't load \"%s\" from \"%s\" datafile!\n", objname, fname);
         }
         return;
      }
   }
   else
   {
      //
      // Loading from file
      //
      for(i = 0; snd_drivers[i].name; i++)
      {
         if ( snd_drivers[i].extension_supported ( get_extension (file) ) && snd_drivers[i].initialized )
         {
            if ( !exists(file) )
            {
               console.printf (con_error, "\"%s\" file doesn't exist!\n", file);
               return;
            }

            if ( !snd_drivers[i].play_file(file) )
            {
               snd_drivers[i].playing = playing = true;
            }
            else
            {
               console.printf (con_error, "Error reading \"%s\" %s music file!\n", file, snd_drivers[i].name);
            }
         }
      }

      if(!playing)
      {
         console.printf (con_warning, "Unknown file type!\n");
         return;
      }
   }


   if(!from_mlist && mlist_autoadd.value)
   {
      //
      // Add to mlist
      //
      add_to_mlist(file, type);

      console.printf ("Added to mlist (%d)\n", mlist.count);

      sprintf(buf, "music_play %d\n", mlist.count);
      cbuf_add_text(buf);
      return;
   }

   for(i = 0; snd_drivers[i].name; i++)
   {
      if(snd_drivers[i].playing)
      {
         console.printf (con_system, "Starting playing %s \"%s\"\n", snd_drivers[i].name, get_filename(file));
      }
   }
}
END_COMMAND (music_play)

void mlist_next_f ()
{
   char buf[64];

   if(!mlist.count)
      return;

   mlist.current++;

   if(mlist.current > mlist.count)
      mlist.current = 1;

   sprintf(buf, "music_play %d\n", mlist.current);

   cbuf_add_text(buf);
}

BEGIN_COMMAND (mlist_next, "Plays next music track")
{
   mlist_next_f ();
}
END_COMMAND (mlist_next)

BEGIN_COMMAND (mlist_prev, "Plays previous music track")
{
   char buf[64];

   if(!mlist.count)
      return;

   mlist.current--;

   if(mlist.current <= 0)
      mlist.current = mlist.count;

   sprintf(buf, "music_play %d\n", mlist.current);

   cbuf_add_text(buf);
}
END_COMMAND (mlist_prev)


BEGIN_COMMAND (music_stop, "Completely stops music output")
{
   int i;

   for(i = 0; snd_drivers[i].name; i++)
      snd_drivers[i].play(NULL);
}
END_COMMAND (music_stop)


BEGIN_COMMAND (music_vol_up, "Increases music volume")
{
  char buf[64];

  sprintf(buf, "music_vol %d", (int)(music_vol.value + sound_delta.value));

  cbuf_add_text(buf);
}
END_COMMAND (music_vol_up)


BEGIN_COMMAND (music_vol_down, "Decreases music volume")
{
  char buf[64];

  sprintf(buf, "music_vol %d", (int)(music_vol.value - sound_delta.value));

  cbuf_add_text(buf);
}
END_COMMAND (music_vol_down)


BEGIN_COMMAND (digi_vol_up, "Increases sound volume")
{
  char buf[64];

  sprintf(buf, "sound_vol %d", (int)(sound_vol.value + sound_delta.value));

  cbuf_add_text(buf);
}
END_COMMAND (digi_vol_up)


BEGIN_COMMAND (digi_vol_down, "Decreases sound volume")
{
  char buf[64];

  sprintf(buf, "sound_vol %d", (int)(sound_vol.value - sound_delta.value));

  cbuf_add_text(buf);
}
END_COMMAND (digi_vol_down)

void snd_init()
{
   int i;

   for(i = 0; snd_drivers[i].name != NULL; i++)
   {
      if(!snd_drivers[i].init())
      {
         ustrcpy ( snd_error, empty_string );

         snd_drivers[i].initialized = true;
         snd_drivers[i].playing     = false;

         console.printf ("%s music driver initialized.\n", snd_drivers[i].name);
      }
      else
      {
         snd_drivers[i].initialized = false;
         snd_drivers[i].playing     = false;

         if ( ustrlen (snd_error) )
         {
            console.printf (con_warning, "Error inialising %s music driver:\n\"%s\"\n", snd_drivers[i].name, snd_error);
         }
         else
         {
            console.printf (con_warning, "Error inialising %s music driver.\n", snd_drivers[i].name);
         }
      }
   }
}

void snd_shutdown()
{
   int i;

   for(i = 0; snd_drivers[i].name; i++)
   {
      if(snd_drivers[i].initialized)
      {
         snd_drivers[i].play (NULL);
         snd_drivers[i].shutdown ();
      }
   }

   if (music_data)
   {
      unload_datafile (music_data);
      ustrcpy (music_file, NULL);
   }
}

void snd_poll()
{
   int i;

   for(i = 0; snd_drivers[i].name; i++)
   {
      if(snd_drivers[i].playing)
      {
         if( !snd_drivers[i].poll ( (int)music_loop.value ) )
         {
            snd_drivers[i].playing = false;
         }
      }
   }
}

void play_sound(SAMPLE *spl, int pan, int loop)
{
   play_sample(spl, (int)sound_vol.value, pan, 1000, loop);
}

