#ifdef __GUI_C__     // to be included only from "gui.c"

#ifndef __DIALOGS_H__
#define __DIALOGS_H__

#define D_SD            (D_SELECTED | D_DISABLED)

#define V               (void *)

int dummy(int msg, DIALOG *d, int c)
{
  return D_O_K;
}

DIALOG d_main_menu[] =
                {       /// (proc)        (x)  (y)  (w)  (h)  (fg)   (bg)   (key)  (flags) (d1) (d2) (dp)                                       (dp2)                   (dp3) ////////////
                        //{ background,     0,   0,   0,   0,   0,     0,     0,     0,      1,   0,   NULL,                                      NULL,                   NULL              },
                        { dummy,          0,   0,   640, 480, 0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { frame,          200, 155, 245, 166, 0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },
                        { button,         245, 200, 150, 20,  0,     0,     'p',   D_EXIT, 0,   0,   (void *) "&Play Game",                            NULL,                   (void *) "menu start_game" },
                        { button,         245, 230, 150, 20,  0,     0,     'o',   D_EXIT, 0,   0,   (void *) "&Options",                              NULL,                   (void *) "menu options"    },
                        { button,         245, 260, 150, 20,  0,     0,     'x',   D_EXIT, 0,   0,   (void *) "E&xit",                                 NULL,                   (void *) "menu quit"       },
                        { label,          5,   465, 0,   0,   -4,    -5,    0,     0,      0,   1,   (void *) "(c) 2001 &M&A&Zsoft, All Rights Reserved",NULL,                 NULL              },
                        { gui_idle_logic, 0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { NULL,           0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              }
                };

DIALOG d_quit[] =
                {       /// (proc)        (x)  (y)  (w)  (h)  (fg)   (bg)   (key)  (flags) (d1) (d2) (dp)                                       (dp2)                   (dp3) ////////////
                        //{ background,     0,   0,   0,   0,   0,     0,     0,     0,      1,   0,   NULL,                                      NULL,                   NULL              },
                        { dummy,          0,   0,   640, 480, 0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { frame,          200, 155, 245, 166, 0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },
                        { label,          320, 200, 0,   0,   -4,    -4,    0,     0,      1,   1,   (void *) "Are you sure?",                         NULL,                   NULL              },
                        { button,         245, 230, 150, 20,  0,     0,     'p',   D_EXIT, 0,   0,   (void *) "&Yes",                                  NULL,                   (void *) "quit"          },
                        { button,         245, 260, 150, 20,  0,     0,     'o',   D_EXIT, 0,   0,   (void *) "&No",                                   NULL,                   (void *) "menu main_menu"},
                        { gui_idle_logic, 0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { NULL,           0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              }
                };

DIALOG d_start_menu[] =
                {       /// (proc)        (x)  (y)  (w)  (h)  (fg)   (bg)   (key)  (flags) (d1) (d2) (dp)                                       (dp2)                   (dp3) ////////////
                        //{ background,     0,   0,   0,   0,   0,     0,     0,     0,      1,   0,   NULL,                                      NULL,                   NULL              },
                        //{ frame,          5,   385, 80,  75,  0,     0,     0,     0,     10,   0,   NULL,                                      NULL,                   NULL              },
                        { dummy,          0,   0,   640, 480, 0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { frame,          20,  20,  220, 360, 0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },
                        { label,          130, 30,  0,   0,   -4,    -5,    0,     0,      1,   1,   (void *) "Choose &M&A&P",                         NULL,                   NULL              },
                        { frame,          260, 20,  360, 360, 0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },
                        { label,          440, 30,  0,   0,   -4,    -5,    0,     0,      1,   1,   (void *) "Choose &S&H&I&P",                       NULL,                   NULL              },
                        { frame,          90,  400, 530, 60,  0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },
                        { button,         110, 420, 150, 20,  0,     0,     's',   D_EXIT, 0,   0,   (void *) "&Start MAYHEM!",                        NULL,                   (void *) "play_game"     },
                        { button,         280, 420, 150, 20,  0,     0,     'e',   D_EXIT, 0,   0,   (void *) "&Edit Players",                         NULL,                   NULL              },
                        { button,         450, 420, 150, 20,  0,     0,     'b',   D_EXIT, 0,   0,   (void *) "&Back",                                 NULL,                   (void *) "menu main_menu"},
                        { gui_idle_logic, 0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { NULL,           0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              }
                };

DIALOG d_options[] =
                {       /// (proc)        (x)  (y)  (w)  (h)  (fg)   (bg)   (key)  (flags) (d1) (d2) (dp)                                       (dp2)                   (dp3) ////////////
                        //{ background,     0,   0,   0,   0,   0,     0,     0,     0,      1,   0,   NULL,                                      NULL,                   NULL              },
                        { dummy,          0,   0,   640, 480, 0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { frame,          20,  20,  600, 440, 0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },
                        { hor_line,       21,  67,  597,   0, 0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },

                        { button,         40,  35,  125, 20,  0,     0,     'g',   D_SD,   0,   0,   (void *) "&General",                                NULL,                 (void *) "menu options"    },
                        { button,         185, 35,  125, 20,  0,     0,     'p',   D_EXIT, 0,   0,   (void *) "Gra&phics",                               NULL,                 (void *) "menu gfx_options"},
                        { button,         330, 35,  125, 20,  0,     0,     's',   D_EXIT, 0,   0,   (void *) "&Sound",                                  NULL,                 (void *) "menu snd_options"},
                        { button,         475, 35,  125, 20,  0,     0,     'c',   D_EXIT, 0,   0,   (void *) "&Controls",                               NULL,                 (void *) "menu cnt_options"},

                        { checkbox,       40,  90,  0,   20,  0,     0,     'z',   0,      0,   0,   (void *) "Check for colli&zions",                   NULL,                   NULL              },
                        { checkbox,       40,  120, 0,   20,  0,     0,     'm',   0,      0,   0,   (void *) "Allow &Multiplayer",                      NULL,                   NULL              },
                        { checkbox,       40,  150, 0,   20,  0,     0,     'l',   0,      0,   0,   (void *) "Automatic &look for MAYHEM server",       NULL,                   NULL              },

                        { button,         280, 420, 150, 20,  0,     0,     'c',   D_EXIT, 0,   0, (void *) "&Go to Console",                          NULL,                 (void *) "menu console"    },
                        { button,         450, 420, 150, 20,  0,     0,     'b',   D_EXIT, 0,   0, (void *) "&Back",                                   NULL,                 (void *) "menu main_menu"  },
                        { gui_idle_logic, 0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { NULL,           0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              }
                };


typedef struct GFX_MODE_DATA
{
   int  w, h;
   int  bpp;
   char *s;
   char *cmd;
} GFX_MODE_DATA;

static GFX_MODE_DATA gfx_mode_data[] =
{
   //{ 320,   200,  "320x200"   },
   //{ 320,   240,  "320x240"   },
   //{ 640,   400,  "640x400"   },
   { 640,   480,  0, "640x480",   "640  480"  },
   { 800,   600,  0, "800x600",   "800  600"  },
   { 1024,  768,  0, "1024x768",  "1024 768"  },
   { 1280,  1024, 0, "1280x1024", "1280 1024" },
   { 1600,  1200, 0, "1600x1200", "1600 1200" }
};

static char *gfx_mode_getter(int index, int *list_size)
{
   char buf[64];

   if (index == -1) {
      if (list_size)
	 *list_size = sizeof(gfx_mode_data) / sizeof(GFX_MODE_DATA) - 1;
      return NULL;
   }
   else if(index < -1)
   {
     //console_dprintf("index = %d, guessed = %d\n", index, -(index+2));

     sprintf(buf, "set_gfx_mode %s; menu reinit\n", gfx_mode_data[-(index+2)].cmd);
     cbuf_add_text(buf);

     return NULL;
   }

   return (char *)get_config_text(gfx_mode_data[index].s);
}

static char *gfx_anim_type(int index, int *list_size)
{
  static char *anim_list[] =
  {
    "Double Buffer",
    "Page Flip",
    "Triple Buffer"
  };

  if(index < 0)
  {
    if (list_size)
      *list_size = sizeof(anim_list) / sizeof(char *);

    return NULL;
  }

  return anim_list[index];
}

DIALOG d_gfx_options[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags)           (d1) (d2) (dp)                                 (dp2)                 (dp3)                        */
   { dummy, 0, 0, 640, 480, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
   { frame, 20, 20, 600, 440, 0, 0, 0, 0, 10, 0, NULL, NULL, NULL },
   { hor_line, 21, 67, 597, 0, 0, 0, 0, 0, 10, 0, NULL, NULL, NULL },
   { button, 40, 35, 125, 20, 0, 0, 'g', D_EXIT, 0, 0, (void *) "&General", NULL, (void *) "menu options" },
   { button, 185, 35, 125, 20, 0, 0, 'p', D_SD, 0, 0, (void *) "Gra&phics", NULL, (void *) "menu gfx_options" },
   { button, 330, 35, 125, 20, 0, 0, 's', D_EXIT, 0, 0, (void *) "&Sound", NULL, (void *) "menu snd_options" },
   { button, 475, 35, 125, 20, 0, 0, 'c', D_EXIT, 0, 0, (void *) "&Controls", NULL, (void *) "menu cnt_options" },
   { label, 40, 90, 270, 20, -4, -4, 0, 0, 0, 1, (void *) "Max. FPS", NULL, NULL },
   { slider, 340, 90, 250, 20, 0, 100, 0, 0, 10, 0, NULL, NULL, (void *) "max_fps" },
   { checkbox, 40, 120, 270, 20, 0, 0, 'v', 0, 0, 0, (void *) "&VSYNC every frame", NULL, (void *) "wait_vsync" },
   { checkbox, 40, 150, 270, 20, 0, 0, 't', 0, 0, 0, (void *) "&Transparent GUI windows", NULL, (void *) "gui_trans" },
   { checkbox, 40, 180, 270, 20, 0, 0, 'f', 0, 0, 0, (void *) "Show FPS", NULL, (void *) "show_fps" },
   { listbox, 40, 310, 270, 80, 0, 0, 0, LISTBOX_CALL_DP, 0, 0, gfx_mode_getter, NULL, NULL },
   { listbox, 330, 310, 270, 80, 0, 0, 0, LISTBOX_EXEC_DP3, 0, 0, gfx_anim_type, NULL, (void *) "animation_type" },
   { button, 450, 420, 150, 20, 0, 0, 'b', D_EXIT, 0, 0, (void *) "&Back", NULL, (void *) "menu main_menu" },
   { gui_idle_logic, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
   { label, 40, 290, 270, 20, -4, -4, 0, 0, 0, 1, (void*)"Resolution:", NULL, NULL },
   { label, 330, 290, 270, 20, -4, -4, 0, 0, 0, 1, (void*)"Drawing method:", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

char *snd_digi_card_list(int index, int *list_size)
{
  static char *digi_list[] =
  {
    "NONE",
    "Adlib 1.0",
    "SoundBlaster 1",
    "SoundBlaster 2",
    "SoundBlaster 3",
    "SoundBlaster PRO",
    "SoundBlaster 16",
    "SoundBlaster AWE 32",
    "SoundForge",
    "SoundMaster",
    "SoundForce",
    "SoundMan",
    "SoundBlaster LIVE (tm)",
    "Snd Card",
    "I don't know!",
    "Super-Force",
    "Enhanced Equalizer",
    "Visio 3d",
    "EAX compatible",
    "SFX compatible"
  };

  if(index < 0)
  {
    if (list_size)
      *list_size = sizeof(digi_list) / sizeof(char *);

    return NULL;
  }

  return digi_list[index];
}

char *snd_midi_card_list(int index, int *list_size)
{
  static char *midi_list[] =
  {
    "NONE",
    "Adlib 1.0",
    "SoundBlaster 1",
    "SoundBlaster 2",
    "SoundBlaster 3",
    "SoundBlaster PRO",
    "SoundBlaster 16",
    "SoundBlaster AWE 32",
    "SoundForge",
    "SoundMaster",
    "SoundForce",
    "SoundMan",
    "SoundBlaster LIVE (tm)",
    "Snd Card",
    "I don't know!",
    "Super-Force",
    "Enhanced Equalizer",
    "Visio 3d",
    "EAX compatible",
    "SFX compatible"
  };

  if(index < 0)
  {
    if (list_size)
      *list_size = sizeof(midi_list) / sizeof(char *);

    return NULL;
  }

  return midi_list[index];
}

DIALOG d_snd_options[] =
{
   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                               (dp2) (dp3)                        */
   { dummy, 0, 0, 640, 480, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
   { frame, 20, 20, 600, 440, 0, 0, 0, 0, 10, 0, NULL, NULL, NULL },
   { hor_line, 21, 67, 597, 0, 0, 0, 0, 0, 10, 0, NULL, NULL, NULL },
   { button, 40, 35, 125, 20, 0, 0, 'g', D_EXIT, 0, 0, (void *) "&General", NULL, (void *) "menu options" },
   { button, 185, 35, 125, 20, 0, 0, 'p', D_EXIT, 0, 0, (void *) "Gra&phics", NULL, (void *) "menu gfx_options" },
   { button, 330, 35, 125, 20, 0, 0, 's', D_SD, 0, 0, (void *) "&Sound", NULL, (void *) "menu snd_options" },
   { button, 475, 35, 125, 20, 0, 0, 'c', D_EXIT, 0, 0, (void *) "&Controls", NULL, (void *) "menu cnt_options" },
   { checkbox, 40, 190, 260, 20, 0, 0, 'l', 0, 0, 0, (void *) "&Loop SoundTracks", NULL, (void *) "music_loop" },
   { label, 40, 90, 260, 20, -4, -4, 0, 0, 0, 2, (void *) "Sound Volume", NULL, NULL },
   { slider, 340, 90, 250, 20, 0, 255, 0, 0, 10, 0, NULL, NULL, (void *) "sound_vol" },
   { label, 40, 120, 260, 20, -4, -4, 0, 0, 0, 1, (void *) "Music Volume", NULL, NULL },
   { slider, 340, 120, 250, 20, 0, 255, 0, 0, 10, 0, NULL, NULL, (void *) "music_vol" },
   { label, 40, 150, 260, 20, -4, -4, 0, 0, 0, 1, (void *) "JukeBox Controls", NULL, NULL },
   { button, 330, 150, 80, 20, 0, 0, 0, D_EXIT, 0, 0, (void *) "Previous", NULL, (void *) "mlist_prev" },
   { button, 425, 150, 80, 20, 0, 0, 0, D_EXIT, 0, 0, (void *) "Pause", NULL, (void *) "music_pause" },
   { button, 520, 150, 80, 20, 0, 0, 0, D_EXIT, 0, 0, (void *) "Next", NULL, (void *) "mlist_next" },
   { listbox, 40, 350, 270, 50, 0, 0, 0, 0, 0, 0, snd_digi_card_list, NULL, NULL },
   { button, 450, 420, 150, 20, 0, 0, 'b', D_EXIT, 0, 0, (void *) "&Back", NULL, (void *) "menu main_menu" },
   { gui_idle_logic, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
   { checkbox, 330, 190, 270, 20, 0, 0, 0, 0, 10, 0, (void*)"AutoAdd to MLIST", NULL, (void*)"mlist_autoadd" },
   { checkbox, 330, 220, 270, 20, 0, 0, 0, 0, 10, 0, (void*)"Recurse subdirectories", NULL, (void*)"mlist_recurse" },
   { label, 40, 330, 270, 20, -4, -4, 0, 0, 0, 1, (void*)"Test ListBox:", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG d_cnt_options[] =
                {       /// (proc)        (x)  (y)  (w)  (h)  (fg)   (bg)   (key)  (flags) (d1) (d2) (dp)                                       (dp2)                   (dp3) ////////////
                        //{ background,     0,   0,   0,   0,   0,     0,     0,     0,      1,   0,   NULL,                                      NULL,                   NULL              },
                        { dummy,          0,   0,   640, 480, 0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { frame,          20,  20,  600, 440, 0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },
                        { hor_line,       21,  67,  597,   0, 0,     0,     0,     0,      10,  0,   NULL,                                      NULL,                   NULL              },

                        { button,         40,  35,  125, 20,  0,     0,     'g',   D_EXIT, 0,   0, (void *) "&General",                                NULL,                 (void *) "menu options"    },
                        { button,         185, 35,  125, 20,  0,     0,     'p',   D_EXIT, 0,   0, (void *) "Gra&phics",                               NULL,                 (void *) "menu gfx_options"},
                        { button,         330, 35,  125, 20,  0,     0,     's',   D_EXIT, 0,   0, (void *) "&Sound",                                  NULL,                 (void *) "menu snd_options"},
                        { button,         475, 35,  125, 20,  0,     0,     'c',   D_SD,   0,   0, (void *) "&Controls",                               NULL,                 (void *) "menu cnt_options"},

                        { button,         450, 420, 150, 20,  0,     0,     'b',   D_EXIT, 0,   0, (void *) "&Back",                                   NULL,                 (void *) "menu main_menu"  },
                        { gui_idle_logic, 0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              },
                        { NULL,           0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              }
                };

typedef struct MAY2_DIALOGS
{
  char   *name;
  DIALOG *d;
  int    focus_obj;
} MAY2_DIALOGS;

MAY2_DIALOGS dialogs[] =
{
        {
                "main_menu",
                d_main_menu,
                -1
        }
        ,
        {
                "quit",
                d_quit,
                -1
        }
        ,
        {
                "start_game",
                d_start_menu,
                -1
        }
        ,
        {
                "options",
                d_options,
                -1
        }
        ,
        {
                "gfx_options",
                d_gfx_options,
                -1
        }
        ,
        {
                "snd_options",
                d_snd_options,
                -1
        }
        ,
        {
                "cnt_options",
                d_cnt_options,
                -1
        }
        ,
        {
                NULL,
                NULL,
                0
        }
};


#endif

#endif
