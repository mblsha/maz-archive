#include "madi.h"

Student::Student ()
{
   direction = rand () % 2;
   if (!direction)
      direction = -1;

   x = (direction > 0) ? 0 : SCREEN_W;

   speed = (float) (rand () % 10) / 2 + 2;

   fc = 0.0;

/*
   int i;

   DATAFILE *s = (DATAFILE *)store_dat ("/student/");
   for (i = 0; s[i].type != DAT_END; i++)
      ;
   //gfx = (DATAFILE *)s[rand () % i].dat;
*/
   gfx = (DATAFILE *)store_dat ("/student/data");//s[0].dat;

   for (max_gfx = 0; gfx[max_gfx].type != DAT_END; max_gfx++)
      ;
   frame = 0;

   remove = false;
}

void Student::draw (BITMAP *bmp)
{
   BITMAP *buf = (BITMAP *)gfx[frame].dat;

   if (direction > 0)
      draw_sprite (bmp, buf, (int)x - buf->w/2, SCREEN_H - buf->h);
   else
      draw_sprite_h_flip (bmp, buf, (int)x - buf->w/2, SCREEN_H - buf->h);
}

void Student::move ()
{
   float s = (float)speed; //* ((SCREEN_W/2 - (x > SCREEN_W/2 ? x - SCREEN_W/2 : x) + 25) / SCREEN_W/2);

   x += (float)s * direction;

   if ((direction > 0 && x > SCREEN_W/2) ||
       (direction < 0 && x < SCREEN_W/2))
   {
      remove = true;
   }

   fc += (float)s / 10;

   if (fc > 1)
   {
      fc = 0;
      
      if (++frame >= max_gfx)
         frame = 0;
   }
}

bool Student::done ()
{
   return remove;
}
