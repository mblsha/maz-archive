#include "def.h"
#include "sound.h"
#include "cmd_base.h"
#include "screen.h"
#include "gui_dialog.h"
#include "gui.h"

CVAR (version, "MAYHEM2 v." VERSION_STR ", " ALLEGRO_PLATFORM_STR " (" __DATE__ ", " __TIME__ ")", CVAR_NOSET);

BEGIN_CUSTOM_CVAR (max_fps, "100", CVAR_ARCHIVE)
{
   if (var.value > 1000)
   {
      var.set (1000);
      return;
   }
   else if (var.value < 20)
   {
      var.set (20);
      return;
   }

   if (timer_initialized)
   {
      // reinit timers
      timer_uninit ();
      timer_init ();
   }
}
END_CUSTOM_CVAR (max_fps)

CVAR (screen_w,      "640", CVAR_ARCHIVE);
CVAR (screen_h,      "480", CVAR_ARCHIVE);
CVAR (screen_bpp,    "8",   CVAR_ARCHIVE);

CVAR (screen_windowed, "1", CVAR_ARCHIVE);

static FONT             *con_font               = NULL;
static BITMAP           *con_backbmp            = NULL;

extern void gui_init_colors();

void sel_palette(RGB *pal);

BEGIN_COMMAND (screenshot, "Takes a screenshot")
{
   int     i, j;
   char    pcxname[80], num[3], num2[3];
   BITMAP  *scr;
   PALETTE pal;

   if (argc >= 2 && !cmd_check_parm ("quiet"))
   {
      ustrcpy(pcxname, argv[1].get_s ());

      if ( exists (pcxname) )
      {
         console.printf (con_warning, "%s already exists!\n", pcxname);
      }
   }
   else
   {
      //
      // find a file name to save it to
      //
      for (i = 0; i <= 999; i++)
      {
         sprintf (num, "%d", i);
         strcpy  (num2, "");

         for (j = strlen (num); j < 3; j++)
         {
            strcat (num2, "0");
         }
         strcat (num2, num);

         strcpy (pcxname, "may2_");
         strcat (pcxname, num2);
         strcat (pcxname, ".pcx");

         if (!exists(pcxname))
            break;   // file doesn't exist, so we can use that name
      }
      
      if (i == 1000)
      {
         console.printf (con_error, "Can't save screenshot!\nYou already have too many!\n");

         return;
      }
   }

   //
   // Save screen
   //
   scr = create_bitmap(SCREEN_W, SCREEN_H);
   get_palette(pal);
   blit(screen, scr, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   save_pcx(pcxname, scr, pal);
   destroy_bitmap(scr);

   if ( !cmd_check_parm ("quiet") )
      console.printf (con_system, "Saved screenshot \"%s\"\n", pcxname);
}
END_COMMAND (screenshot)

static boolean          set_gfx_mode_lower = false;

static float    w = 0, h = 0, bpp = 0;

boolean  accel_system2screen_blits;
boolean  accel_system2screen_masked_blits;
boolean  accel_vram2vram_blits;
boolean  accel_vram2vram_masked_blits;

void set_gfx_mode_f (int argc, ustring *argv)
{
   int    i, f, x, y;
   BITMAP *bkg = NULL, *bkg_tile = NULL;

   if( !argv[1].case_compare ("\\?") || !argv[1].case_compare ("?") ||
       !argv[1].case_compare ("help"))
   {
      console.printf(con_description, "set_gfx_mode [width] [heigth] [bpp]: sets specified gfx mode\n"
                                      "Takes \"width\", \"heigth\" and \"bpp\" values from\n"
                                      "\"screen_w\", \"screen_h\" and \"screen_bpp\" cvars respectively\n");
      return;
   }

   if (argc >= 2)
      if( argv[1].length () )
         cvar_set ("screen_w",   argv[1]);

   if (argc >= 3)
      if( argv[2].length () )
         cvar_set ("screen_h",   argv[2]);

   if (argc >= 4)
      if( argv[3].length () )
         cvar_set ("screen_bpp", argv[3]);

   if ( w == screen_w.value && h == screen_h.value && bpp == screen_bpp.value )
   {
      return;
   }

   set_color_depth ((int)screen_bpp.value);

   if(set_gfx_mode (screen_windowed.value ? GFX_AUTODETECT_WINDOWED : GFX_AUTODETECT, (int)screen_w.value, (int)screen_h.value, 0, 0) )
   {
      console.printf("Cant set gfx mode %dx%dx%dbpp: %s\n", (int)screen_w.value, (int)screen_h.value, (int)screen_bpp.value, allegro_error);

      if(!set_gfx_mode_lower)
      {
         set_gfx_mode_lower = true;

         cvar_set("screen_w",   "640");
         cvar_set("screen_h",   "480");
         cvar_set("screen_bpp", "8" );

         console.printf("Trying to set %dx%dx%dbpp mode instead\n", (int)screen_w.value, (int)screen_h.value, (int)screen_bpp.value, allegro_error);

         cbuf_add_text("set_gfx_mode\n");
         cbuf_execute();

         // if something goes wrong we won't get to this point
         goto init;
      }
      else
         sys_error("Can't set even the lower resolution! Check your gfx card!");
   }
   else
   {

init:

       set_gfx_mode_lower = false;

       console.printf("Successfully set gfx mode %dx%dx%dbpp\n", (int)screen_w.value, (int)screen_h.value, (int)screen_bpp.value);

       accel_system2screen_blits        = (boolean)(gfx_capabilities & GFX_HW_SYS_TO_VRAM_BLIT);
       accel_system2screen_masked_blits = (boolean)(gfx_capabilities & GFX_HW_SYS_TO_VRAM_BLIT_MASKED);
       accel_vram2vram_blits            = (boolean)(gfx_capabilities & GFX_HW_VRAM_BLIT);
       accel_vram2vram_masked_blits     = (boolean)(gfx_capabilities & GFX_HW_VRAM_BLIT_MASKED);
       
       // bitmaps for console
       bkg = create_bitmap(SCREEN_W, SCREEN_H);
       bkg_tile = create_bitmap(32,  32);

       //
       // draw console back picture
       //
       clear_to_color(bkg,      screen->vtable->mask_color);
       clear_to_color(bkg_tile, screen->vtable->mask_color);

       // circle from center
       for(i = 0; i < SCREEN_W; i += 1)
       {
          circle(bkg, SCREEN_W/2-1, SCREEN_H/2,   i, makecol(255-(i/2),255-(i/2),255-(i/2)));
          circle(bkg, SCREEN_W/2,   SCREEN_H/2-1, i, makecol(255-(i/2),255-(i/2),255-(i/2)));
          circle(bkg, SCREEN_W/2+1, SCREEN_H/2,   i, makecol(255-(i/2),255-(i/2),255-(i/2)));
          circle(bkg, SCREEN_W/2,   SCREEN_H/2+1, i, makecol(255-(i/2),255-(i/2),255-(i/2)));
          circle(bkg, SCREEN_W/2,   SCREEN_H/2,   i, makecol(255-(i/2),255-(i/2),255-(i/2)));
       }

       // put transparent pixels
       f=0;
       for(y = 0; y < SCREEN_H; y++)
       {
          for(x = 0; x < SCREEN_W; x += 2)
          {
             if(f && !x)
                x = 1;
             putpixel(bkg, x, y, screen->vtable->mask_color);
          }

          f = f ? 0 : 1;
       }

       // rectangle
       rect(bkg, 0, 0, SCREEN_W,   SCREEN_H,   makecol( 96, 96, 96));
       rect(bkg, 1, 1, SCREEN_W-1, SCREEN_H-1, makecol(128,128,128));
       rect(bkg, 2, 2, SCREEN_W-2, SCREEN_H-2, makecol(128,128,128));
       rect(bkg, 3, 3, SCREEN_W-3, SCREEN_H-3, makecol( 96, 96, 96));
       rect(bkg, 4, 4, SCREEN_W-4, SCREEN_H-4, makecol( 48, 48, 48));

       console.set_background(bkg, makecol(128,128,128));

       // we don't more need it
       destroy_bitmap (bkg);
       destroy_bitmap (bkg_tile);
       console.set_height (SCREEN_H / 2);

       console.check_resize();

       // reinit gfx buffers
       scr_init_buffers();
       gui_init_colors();

       sel_palette(NULL);

       w   = screen_w.value;
       h   = screen_h.value;
       bpp = screen_bpp.value;
   }
}

BEGIN_COMMAND (set_gfx_mode, "Sets graphics mode")
{
   set_gfx_mode_f (argc, argv);
}
END_COMMAND (set_gfx_mode)

void con_init_f ()
{
   if(!con_font)
      con_font = font;

   console.set_properties (con_font, true);

   console.set_color (con_default,     makecol(255,155, 50));
   console.set_color (con_command,     makecol(255,195, 90));
   console.set_color (con_warning,     makecol(255, 55, 55));
   console.set_color (con_error,       makecol(255, 25, 25));
   console.set_color (con_system,      makecol(255,255,255));
   console.set_color (con_description, makecol(105,255,105));
   console.set_color (con_outline,     makecol(  0,  0,  0));
}

BEGIN_COMMAND (con_init, "Console initialization")
{
   con_init_f ();
}
END_COMMAND (con_init)

BEGIN_COMMAND (con_load_datafile, "Loads console datafile")
{
   ustring f;

   if (argc != 2)
      f = "data/console.dat";
   else
      f = argv[1];

   if(!exists(f.get_s ()))
   {
      console.printf(con_warning, "Can't read console datafile from \"%s\"!\n", f.get_s ());

      f = "data/console.dat";
   }

   store.add (f, "/mayhem/console/", "Console datafile");

   con_backbmp = (BITMAP *)store.dat ("/mayhem/console/background");
   con_font    = (FONT   *)store.dat ("/mayhem/console/font");

   con_init_f();
}
END_COMMAND (con_load_datafile)


void save_config ()
{
   unlink               ("config.cfg");
   
   console.debug_log    ("config.cfg", "// This file is generated automatically by MAYHEM2\n");
   console.debug_log    ("config.cfg", "// Do not hand edit!\n");

   console.debug_log    ("config.cfg", "\n// cvars\n");
   cvar_write_variables ("config.cfg");
   console.debug_log    ("config.cfg", "\n// bindings\n");
   key_write_bindings   ("config.cfg");
   console.debug_log    ("config.cfg", "\n// aliases\n");
   cmd_write_aliases    ("config.cfg");
}

void quit_f()
{
   console.printf ("Quitting, please wait...\n");

   save_config ();

   snd_shutdown ();
   //shut_down();
   allegro_exit();
   exit(0);
}

BEGIN_COMMAND (quit, "Terminates program")
{
   quit_f ();
}
END_COMMAND (quit)

BEGIN_COMMAND (exit, "Terminates program")
{
   quit_f ();
}
END_COMMAND (exit)


