#include "madi.h"
#include <math.h>


int sun_state = 0;

typedef struct Star
{
   int x, y;
   int col;
} Star;

#define STAR_NUM        500
Star    stars [STAR_NUM];

Sun::Sun ()
{
   angle = 0;
   r = SCREEN_W/2 - 100;
   ra = itofix(0);

   for (int i = 0; i < STAR_NUM; i++)
   {
      stars[i].x = rand() % SCREEN_W;
      stars[i].y = rand() % SCREEN_H;
      int t = rand () % 120;
      stars[i].col = makecol (t + rand() % 20 - 10, t + rand() % 20 - 10, t + rand() % 20 - 10);
   }
}

void Sun::move ()
{
   if (++angle > 180)
      angle = 0;

   if ((ra = fadd(ra, itofix(1))) > itofix(255))
      ra = itofix(0);

   float rad = angle * 3.14 / 180;
   sun_state = (int)(255 * sin (rad));
}

void Sun::draw (BITMAP *bmp)
{
   float rad = angle * 3.14 / 180;
   float x = -r * cos (rad) - (SCREEN_W/2 - r)/2 + SCREEN_W/2;
   float y = -r * sin (rad) + SCREEN_H/2;

   if (sun_state < 100)
   {
      for (int i = 0; i < STAR_NUM; i++)
         putpixel (bmp, stars[i].x, stars[i].y, stars[i].col);
   }

   BITMAP *tmp = (BITMAP *)store_dat ("/main/sun");

   rotate_sprite (bmp, tmp, (int)x, (int)y, ra);
}

bool Sun::done ()
{
   return false;
}
