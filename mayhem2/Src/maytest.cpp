#include <allegro.h>
#include "def.h"
#include "console.h"
#include "gui.h"
#include "screen.h"
#include "cmd_base.h"
#include "sound.h"

#ifdef BALL_DEMO
       #include "balls.h"
#endif

VIEWPORT *view[16];

sv_state_t    sv_state = sv_disconnected;
game_state_t  game_state = not_playing;

char mouse_pointer_data[256] =
{
   1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   3,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
   3,4,5,1,0,0,0,0,0,0,0,0,0,0,0,0,
   3,2,4,5,1,0,0,0,0,0,0,0,0,0,0,0,
   3,2,4,4,5,1,0,0,0,0,0,0,0,0,0,0,
   3,2,2,4,4,5,1,0,0,0,0,0,0,0,0,0,
   3,2,2,4,4,4,5,1,0,0,0,0,0,0,0,0,
   3,2,2,2,4,4,4,5,1,0,0,0,0,0,0,0,
   3,2,2,2,4,4,5,5,5,1,0,0,0,0,0,0,
   3,2,2,4,4,3,1,1,1,0,0,0,0,0,0,0,
   3,2,4,1,2,4,1,0,0,0,0,0,0,0,0,0,
   3,4,1,0,1,2,4,1,0,0,0,0,0,0,0,0,
   0,1,0,0,1,2,4,1,0,0,0,0,0,0,0,0,
   0,0,0,0,0,1,2,4,1,0,0,0,0,0,0,0,
   0,0,0,0,0,1,4,4,1,0,0,0,0,0,0,0,
   0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0
};

char busy_pointer_data[256] =
{
   0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
   0,0,0,0,0,1,1,2,2,1,1,0,0,0,0,0,
   0,0,0,0,1,2,2,4,4,4,4,1,0,0,0,0,
   0,0,0,1,2,4,4,3,3,4,4,4,1,0,0,0,
   0,0,1,2,4,3,3,0,0,3,6,4,4,1,0,0,
   0,1,2,4,3,0,0,0,0,1,1,6,4,4,1,0,
   0,1,2,4,3,0,0,0,1,1,0,3,4,4,1,0,
   1,2,4,3,0,0,0,1,1,0,0,0,3,4,5,1,
   1,2,4,3,0,0,0,1,1,0,0,0,3,4,5,1,
   0,1,4,4,3,0,0,0,1,1,0,3,4,5,1,0,
   0,1,4,4,3,0,0,0,0,1,1,6,4,5,1,0,
   0,0,1,4,4,3,3,0,0,3,6,4,5,1,0,0,
   0,0,0,1,4,4,4,3,3,4,4,5,1,0,0,0,
   0,0,0,0,1,4,4,4,4,5,5,1,0,0,0,0,
   0,0,0,0,0,1,1,5,5,1,1,0,0,0,0,0,
   0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0
};
int busy_mouse = FALSE;

BITMAP *mouse_pointer;
BITMAP *busy_pointer;

int gui_col1;
int gui_col2;
int gui_col3;
int gui_col4;
int gui_col5;
int gui_col6;

void sel_palette(RGB *pal)
{
   int c, x, y;

   if(pal)
   {
      //memcpy(ated_current_palette, pal, sizeof(PALETTE));
      //set_palette(ated_current_palette);
   }

   gui_col1   = makecol(196,196,196);
   gui_col2   = makecol(128,128,128);
   gui_col3   = makecol( 64, 64, 64);

   gui_col4   = makecol(196,128, 96);

   gui_col5   = makecol( 32, 32, 32);
   gui_col6   = makecol(150,150,150);

   gui_fg_color = makecol(0, 0, 0);
   gui_mg_color = makecol(0x80, 0x80, 0x80);
   gui_bg_color = makecol(0xFF, 0xFF, 0xFF);

   if(mouse_pointer)
      destroy_bitmap(mouse_pointer);
   if(busy_pointer)
      destroy_bitmap(busy_pointer);

   mouse_pointer = create_bitmap(16, 16);
   busy_pointer  = create_bitmap(16, 16);

   for (y=0; y<16; y++)
   {
      for (x=0; x<16; x++)
      {
	      switch (mouse_pointer_data[x+y*16])
	      {
	         case 1:  c = black; break;
	         case 2:  c = white; break;
	         case 3:  c = gui_col3; break;
      	   case 4:  c = gui_col1; break;
	         case 5:  c = gui_col6; break;
	         case 6:  c = gui_col5; break;
	         default: c = screen->vtable->mask_color; break;
	      }
	      putpixel(mouse_pointer, x, y, c);

      	switch (busy_pointer_data[x+y*16])
	      {
	         case 1:  c = black; break;
	         case 2:  c = white; break;
	         case 3:  c = gui_col3; break;
	         case 4:  c = gui_col1; break;
	         case 5:  c = gui_col6; break;
	         case 6:  c = gui_col5; break;
	         default: c = screen->vtable->mask_color; break;
	      }
	      putpixel(busy_pointer, x, y, c);
      }
   }

   set_mouse_sprite(mouse_pointer);
   busy_mouse = FALSE;
}

COLOR_MAP color_map_menu;
/*
DATAFILE *gui_data;
DATAFILE *back_bmp;
DATAFILE *may_data;
DATAFILE *tmp_data;
DATAFILE *snd_data;
DATAFILE *font_data;
*/
/*
void init_vars()
{
  cvar_register_variable(&wait_vsync);
  cvar_register_variable(&show_fps);

  cvar_register_variable(&screen_w);
  cvar_register_variable(&screen_h);
  cvar_register_variable(&screen_bpp);

  cvar_register_variable(&max_fps);

}*/

BEGIN_COMMAND (load_tilegfx, "Loads tile graphics file")
{
   int ret;

   if (argc != 2)
   {
      console.printf (con_description, "%s <filename> : Load tilegfx from filename\n", argv[0].get_s ());
      return;
   }

   if (exists(argv[1].get_s ()))
   {
      if ( (ret = load_tile_gfx(argv[1].get_s ())) )
      {
         console.dprintf ("load_tilegfx = %d, %s\n", ret, ated_error);
      }
   }
   else
   {
      console.printf (con_error, "%s doesn't exist!\n", argv[1].get_s ());
   }
}
END_COMMAND (load_tilegfx)


BEGIN_COMMAND (load_tilemap, "Loads tile map file")
{
   int ret;

   if (argc != 2)
   {
      console.printf (con_description, "%s <filename> : Load tilemap from filename\n", argv[0].get_s ());
      return;
   }

   if (exists(argv[1].get_s ()))
   {
      if( (ret = load_tile_map(argv[1].get_s ())) )
      {
         console.dprintf ("load_tilemap = %d, %s\n", ret, ated_error);
      }

      check_tile_map ();
   }
   else
   {
      console.printf (con_error, "%s doesn't exist!\n", argv[1].get_s ());
   }
}
END_COMMAND (load_tilemap)

key_button but_left   = {"Left",  { 0, 0 }, 0, 0, 0};
key_button but_right  = {"Right", { 0, 0 }, 0, 0, 0};
key_button but_up     = {"Up",    { 0, 0 }, 0, 0, 0};
key_button but_down   = {"Down",  { 0, 0 }, 0, 0, 0};

BEGIN_CUSTOM_COMMAND (left_press,    NULL, "+left")  { key_down (&but_left,  argv); } END_COMMAND (left_press)
BEGIN_CUSTOM_COMMAND (left_release,  NULL, "-left")  { key_up   (&but_left,  argv); } END_COMMAND (left_release)
BEGIN_CUSTOM_COMMAND (right_press,   NULL, "+right") { key_down (&but_right, argv); } END_COMMAND (right_press)
BEGIN_CUSTOM_COMMAND (right_release, NULL, "-right") { key_up   (&but_right, argv); } END_COMMAND (right_release)
BEGIN_CUSTOM_COMMAND (up_press,      NULL, "+up")    { key_down (&but_up,    argv); } END_COMMAND (up_press)
BEGIN_CUSTOM_COMMAND (up_release,    NULL, "-up")    { key_up   (&but_up,    argv); } END_COMMAND (up_release)
BEGIN_CUSTOM_COMMAND (down_press,    NULL, "+down")  { key_down (&but_down,  argv); } END_COMMAND (down_press)
BEGIN_CUSTOM_COMMAND (down_release,  NULL, "-down")  { key_up   (&but_down,  argv); } END_COMMAND (down_release)

BEGIN_COMMAND (viewport, "Change viewports' parameters")
{
   int num;
   int x, y, w, h, zoom;

   num = argv[1].get_int ();

   if (argc < 2)
   {
      console.printf (con_description, "%s <viewport #> [[x] [y] [w] [h] [zoom]]\n", argv[0].get_s ());

      return;
   }
   else if (argc == 2)
   {
      console.printf ("viewport %d: x = %d; y = %d; w = %d; h = %d; dx = %d; dy = %d; zoom = %d\n", num, view[num]->x,view[num]->y, view[num]->w, view[num]->h, view[num]->dx, view[num]->dy, view[num]->zoom);

      return;
   }

   if(argc >= 3 && argv[2].length ())
      x = argv[2].get_int ();
   else
      x = view[num]->x;

   if(argc >= 4 && argv[3].length ())
      y = argv[3].get_int ();
   else
      y = view[num]->y;

   if(argc >= 5 && argv[4].length ())
      w = argv[4].get_int ();
   else
      w = view[num]->w;

   if(argc >= 6 && argv[5].length ())
      h = argv[5].get_int ();
   else
      h = view[num]->h;

   if(argc >= 7 && argv[6].length ())
      zoom = argv[6].get_int ();
   else
      zoom = view[num]->zoom;

   //
   // Error checking
   //

   if (x < 0)
      x = 0;

   if (y < 0)
      y = 0;

   if (w < 0)
      w = 0;

   if (h < 0)
      h = 0;

   if (w > SCREEN_W)
      w = SCREEN_W;

   if (h > SCREEN_H)
      h = SCREEN_H;

   if (x + w > SCREEN_W)
      x = SCREEN_W - w;

   if (y + h > SCREEN_H)
      h = SCREEN_H - h;

   if (zoom < 100)
      zoom = 100;
   else if (zoom > 1000)
      zoom = 1000;

   view[num]->x = x;
   view[num]->y = y;
   view[num]->w = w;
   view[num]->h = h;

   view[num]->zoom = zoom;

   view[num]->x1 = 0;
   view[num]->y1 = 0;
   view[num]->x2 = w;
   view[num]->y2 = h;

   // check dx & dy
   check_viewport_edges (view[num]);
}
END_COMMAND (viewport)

/*
DATAFILE *my_load_datafile(char *file, char *desc)
{
   DATAFILE *data;

   if(!exists(file))
   {
      //console_printf ("%s datafile doesn't exist!\n", file);
      sys_error ("\"%s\" datafile doesn't exist!\nPlease reinstall", file);
   }

   data = load_datafile(file);

   if(!data)
   {
      sys_error ("Please reinstall, corrupted datafile \"%s\"\n", file);
   }

   convert_images_in_datafile (data);

   console.printf ("%s successfully loaded.\n", desc);

   return data;
}
*/
void col_map (int pos)
{
   console.printf ("+");
   //console_draw   (screen, console_lines, true);
}

extern cvar screen_bpp;

static boolean init_done = false;

BEGIN_COMMAND (init, "Initialization command (DON'T USE THIS !!!)")
{
   int i;
   PALETTE pal;

   if (init_done)
      return;

   init_done = true;

   console.printf ("----------------------------------------\n");
   console.printf ("Mouse    driver: %s\n", ((i=install_mouse()) == -1) ?           "MOUSE NOT FOUND" : (i == 2) ?  "2 BUTTONS" : (i == 3) ? "3 BUTTONS" : "1 BUTTON");
   console.printf ("Keyboard driver: %s\n", install_keyboard() ?                    "ERROR" :                       "OK");
   console.printf ("Joystick driver: %s\n", install_joystick(JOY_TYPE_AUTODETECT) ? allegro_error :                 "OK");
   console.printf ("Timer    driver: %s\n", install_timer() ?                       "ERROR" :                       "OK");
   console.printf ("Sound    driver: %s\n", install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) ? allegro_error : "OK");
   snd_init  ();
   console.printf ("----------------------------------------\n");
   set_volume(255, (int)music_vol.value);
   console.printf ("Loading datafiles...\n");

   cvar_enable_callbacks ();

   set_color_conversion(COLORCONV_NONE);
/*
   may_data = my_load_datafile ("data/mayhem.dat",       "Main  datafile");
   gui_data = my_load_datafile ("data/gui/original.dat", "GUI   datafile");
   snd_data = my_load_datafile ("data/sound/mayhem.dat", "Sound datafile");
*/
   store.add ("data/mayhem.dat",       "/mayhem/",     "Main datafile");
   store.add ("data/gui/original.dat", "/mayhem/gui/", "GUI datafile");
   store.add ("data/sound/mayhem.dat", "/sound/",      "Sound datafile");

   console.printf("----------------------------------------\n");

   font = (FONT *)store.dat ("/mayhem/fonts/ocrana");

   text_mode(-1);

   console.printf ("Please wait, inititializing game data...\n");
   console.set_action (con_closed);

   // Create COLOR MAP
   if(screen_bpp.value == 8)
   {
      get_palette(pal);
      console.printf("Creating color map...");
      create_trans_table(&color_map_menu, pal, 100, 100, 100, col_map);
      color_map = &color_map_menu;
      console.printf("DONE\n"
                     "----------------------------------------\n");
   }
   else
      set_trans_blender(128,128,128,128);

   timer_init();
   //timer = 0xFFFFFFFF; // i think, no one will continuosly run program for 1.5 month :-)

   #ifdef BALL_DEMO
          init_balls();
   #endif

   view[0] = create_viewport (0,            0,            SCREEN_W / 2, SCREEN_H / 2, TRUE, TRUE, FALSE, NULL);
   view[1] = create_viewport (SCREEN_W / 2, 0,            SCREEN_W / 2, SCREEN_H / 2, TRUE, TRUE, FALSE, NULL);
   view[2] = create_viewport (0,            SCREEN_H / 2, SCREEN_W / 2, SCREEN_H / 2, TRUE, TRUE, FALSE, NULL);
   view[3] = create_viewport (SCREEN_W / 2, SCREEN_H / 2, SCREEN_W / 2, SCREEN_H / 2, TRUE, TRUE, FALSE, NULL);
}
END_COMMAND (init)

void init()
{
   allegro_init();
   set_config_file("maytest.cfg");

   text_mode(-1);

   com_init();

   if ( !exists ("data/mayhem.dat") )
   {
      sys_error ("\"data/mayhem.dat\" does not exist! Please reinstall!\n");
   }

   cbuf_add_text ("exec \"data/mayhem.dat#mayhem2.rc\"\n");
   cbuf_execute  ();

   sv_state = sv_connected;
   game_state = game_playing;

   //cbuf_add_text ("menu Main_Menu\n");

   gui_intro_f();
}

CVAR (scroll_speed, "300", CVAR_ARCHIVE);

inline void actual_game_logic ()
{
   //
   // Game logic
   //
   #ifdef BALL_DEMO
          move_balls();
   #else
   #endif

   // test feature
   float x = (key_state (&but_right) - key_state (&but_left)) * frame_time * scroll_speed.value;
   float y = (key_state (&but_down)  - key_state (&but_up))   * frame_time * scroll_speed.value;
   for (int i = 0; i < 4; i++)
   {
      move_viewport (view[i], x, y, FALSE);
   }
}

inline void get_input ()
{
   //
   // since keyboard is in autopoll (tm) mode, 
   // our only need is to poll joystick
   //
   key_joystick_poll();
}

inline void get_mouse_input ()
{
   //
   // mouse is used in menu system, so we can't always do key_mouse_poll ()
   // for example -- gui_idle_logic()
   //
   key_mouse_poll();
}

inline void update_things ()
{
   //
   // execute console commands
   //
   cbuf_execute();

   //
   // move console
   //
   console.animate ();

   //
   // poll our music (& possibly sound) drivers
   //
   snd_poll();
}

static void game_logic()
{
   actual_game_logic ();
   get_input ();
   get_mouse_input ();
   update_things ();
}

int gui_idle_logic(int msg, DIALOG *d, int c)
{
   boolean update = false;
   int     counter = 0;           // to avoid infinite looping

   if(msg != MSG_UPDATE)
      return D_O_K;

   while (game_time)
   {
      if (counter++ > 100)
         break;
   
      actual_game_logic ();
      get_input ();
      //get_mouse_input ();
      update_things ();

      game_time--;

      update = true;
   }

   if (update)
   {
      scr_draw();

      frame_count++;
      frames++;
   }
         
   return D_O_K;
}




//#include "game.cpp"

int main(int argc, char *argv[])
{
   boolean update;

   com_argc = argc;
   com_argv = argv;

   init();

   key_dest = key_game;

   scr_draw();

   gui_set_bmp(screen);

   cbuf_add_text("menu Main_Menu\n");

   update = false;
   while(1)
   {
      while (game_time)
      {
         game_logic();

         game_time--;
         
         update = true;
      }
      
      if (update)
      {
         scr_draw();

         frame_count++;
         frames++;

         update = false;
      }
   }

   return 0;
}
END_OF_MAIN();
