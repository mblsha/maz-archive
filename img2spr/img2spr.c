#include <stdio.h>
#include <allegro.h>

DATAFILE *data;

void save_sprite (char *file, int x1, int y1, int x2, int y2)
{
   FILE *fp;
   int x, y, c;
   
   fp = fopen (file, "w");
   
   fprintf (fp, "<ASCII_SPRITE>\n");
   fprintf (fp, "%d %d\n", x2 - x1, y2 - y1);

   for (y = y1; y <= y2; y++)
   {
      for (x = x1; x <= x2; x++)
      {
         c = getpixel (screen, x, y);
         if (c == 16)
            c = 0;

         fprintf (fp, "%d ", c);
      }

      fprintf (fp, "\n");
   }
      
   fclose (fp);
}

void lets_do_it (int argc, char **argv)
{
   int x, y, i, c;
   char buf[128];
   BITMAP *bmp;
  /*
   if (argc == 2)
   {
      get_palette (pal);
      bmp = load_bitmap (argv[1], NULL);
//      set_palette (pal);

      if (!bmp)
         exit (6);

      draw_sprite (screen, bmp, 0, 0);

      replace_extension (buf, argv[1], "spr", sizeof(buf));
      save_sprite (buf, 0, 0, bmp->w, bmp->h);

      destroy_bitmap (bmp);

      readkey ();
   }
*/

   data = load_datafile ("bip.dat");

   {
      bmp = data[0].dat;
      draw_sprite (screen, bmp, 0, 0);

      save_sprite (argv[1], 0, 0, bmp->w, bmp->h);
      readkey ();
   }
   
/*
   //
   // Corkscrew gfx
   //
   {
      BITMAP *tmp;
      
      for (i = 0; data[i].type != DAT_END; i++)
      {
         if (data[i].type != DAT_BITMAP)
            continue;

         bmp = data[i].dat;
         tmp = create_bitmap (bmp->w, bmp->h);
         draw_sprite_h_flip (tmp, bmp, 0, 0);
         clear_to_color (screen, 0);
      
         rotate_sprite (screen, tmp, 0, 0, itofix(64));
         
         sprintf (buf, "plane_%d.spr", i+1);
         save_sprite (buf, 0, 0, bmp->w, bmp->h);

         destroy_bitmap (tmp);

         readkey ();
      }
   }
*/
/*
   //
   // Do loop :-)
   //
   {
      int ang;
      BITMAP *tmp;
      
      bmp = data[0].dat;
      tmp = create_bitmap (bmp->w, bmp->h);
      draw_sprite_h_flip (tmp, bmp, 0, 0);
      
      for (ang = 0, i = 1; ang <= 254; ang += 16, i++)
      {
         clear_to_color (screen, 0);
         rotate_sprite (screen, tmp, 0, 0, itofix(256-ang));
         
         sprintf (buf, "plane_%d.spr", i);
         save_sprite (buf, 0, 0, tmp->w, tmp->h);
      
         readkey ();
      }

      destroy_bitmap (tmp);
   }
*/
/*
   //
   // Barrel graphics :-)
   //
   {
      for (i = 0; data[i].type != DAT_END; i++)
      {
         if (data[i].type != DAT_BITMAP)
            continue;

         bmp = data[i].dat;
         clear_to_color (screen, 0);
         draw_sprite_h_flip (screen, bmp, 0, 0);
      
         sprintf (buf, "plane_%d.spr", i+1);
         save_sprite (buf, 0, 0, bmp->w, bmp->h);

         readkey ();
      }
   }
*/
   unload_datafile (data);
}

void set_our_palette ()
{
   int i;
   RGB rgb = {0, 0, 0};

   //
   // This convertor uses standard EGA palette, so we need to clear all colors past 15
   //

   for (i = 16; i < 255; i++)
      set_color (i, &rgb);
}

int main (int argc, char **argv)
{
   allegro_init ();
   install_keyboard ();

   if (argc < 2)
   {
      printf ("You should use makespr.bat!\n");
      return 1;
   }

   if (set_gfx_mode (GFX_AUTODETECT, 640, 480, 0, 0))
   {
      printf ("Can't initialize graphics mode:\n%s\n", allegro_error);
      return 1;
   }

   set_color_conversion (COLORCONV_TOTAL | COLORCONV_DITHER);

   set_our_palette ();
   lets_do_it (argc, argv);

   return 0;
}
