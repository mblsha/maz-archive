#include "madi.h"
#include <string.h>
#include <stdio.h>

volatile int game_time;
volatile int frames;
volatile int fps;

void timer_proc ()
{
   game_time++;
}
END_OF_FUNCTION (timer_proc);

void fps_proc ()
{
   fps = frames;
   frames = 0;
}
END_OF_FUNCTION (fps_proc);

RLE_SPRITE *madi;
BITMAP *buf;
List bottom_list;
List front_list;

extern int sun_state; // 0 to 255

void screenshot ()
{
   int     i, j;
   char    pcxname[80], num[3], num2[3];
   BITMAP  *scr;
   PALETTE pal;

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

      strcpy (pcxname, "madi_");
      strcat (pcxname, num2);
      strcat (pcxname, ".pcx");

      if (!exists(pcxname))
         break;   // file doesn't exist, so we can use that name
   }
      
   if (i == 1000)
   {
      return;
   }

   //
   // Save screen
   //
   scr = create_bitmap (SCREEN_W, SCREEN_H);
   get_palette (pal);
   blit (screen, scr, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   save_bitmap (pcxname, scr, pal);
   destroy_bitmap (scr);
}

void draw ()
{
   clear_to_color (buf, makecol (sun_state/2,sun_state/2,sun_state));

   bottom_list.draw_all (buf);
   if (!key[KEY_SPACE])
      draw_rle_sprite (buf, madi, 0, SCREEN_H - madi->h);
   front_list.draw_all (buf);

   if (key[KEY_F1])
   {
      int x = SCREEN_W - text_length (font, "FPS: 000");
      textprintf (buf, font, x, 0, makecol (255,255,255), "FPS: %d", fps);
   }

   if (key[KEY_ENTER])
      textout (buf, font, "На правах рекламы", 0, 0, makecol (255,255,255));

   if (key[KEY_F12])
      screenshot ();
      
   blit (buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void logic ()
{
   if (rand () % 100 > 75)
      front_list.add (new Student);

   front_list.move_all ();
   front_list.remove_dead_nodes ();
   
   bottom_list.move_all ();
   bottom_list.remove_dead_nodes ();
}

int color_depths[] =
{
   32,
   24,
   16,
   15,
   8,
   0
};

int main (int argc, char **argv)
{
   allegro_init ();
   install_keyboard ();
   install_timer ();

   bool gfx_initialized = false;

   for (int i = 0; color_depths[i]; i++)
   {
      set_color_depth (color_depths[i]);
      if (!set_gfx_mode (GFX_AUTODETECT, 640, 480, 0, 0))
      {
         gfx_initialized = true;
         break;
      }
   }

   if (!gfx_initialized)
   {
      allegro_message ("Error: %s\n", allegro_error);
      return (1); // the resolution isn't supported :-(
   }

   set_color_conversion (COLORCONV_TOTAL | COLORCONV_DITHER | COLORCONV_KEEP_TRANS);

   store_init (500);
   store_load ("main.dat",    "/main/");
   store_load ("student.dat", "/student/");

   text_mode (-1);
   font = (FONT *)      store_dat ("/main/font");
   madi = (RLE_SPRITE *)store_dat ("/main/madi");

   buf = create_bitmap (SCREEN_W, SCREEN_H);

   install_int (timer_proc, 50);
   install_int (fps_proc,   1000);
   
   bottom_list.add (new Sun);
   bottom_list.add (new Scroller);

   while (!key[KEY_ESC])
   {
      while (game_time > 0)
      {
         logic ();
         game_time--;
      }
      
      draw ();
      frames++;
   }

   return 0;
}
END_OF_MAIN ();
