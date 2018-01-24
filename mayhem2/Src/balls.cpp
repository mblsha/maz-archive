#include "def.h"

#ifdef BALL_DEMO

#include "balls.h"

BITMAP *ball_bmp[MAX_BALLS];

typedef struct BALL
{
   BITMAP        *bmp;
   float         x, y, vx, vy;
   int           r;
} BALL;

BALL balls[MAX_BALLS];

CVAR (balls_move,    "1",   CVAR_ARCHIVE);
CVAR (balls_speed,   "10",  CVAR_ARCHIVE);

BEGIN_CUSTOM_CVAR (balls_display, "100", CVAR_ARCHIVE)
{
   if ( var.value > MAX_BALLS )
      var.set ( (float)MAX_BALLS );
}
END_CUSTOM_CVAR (balls_display)

void init_balls()
{
   int i, r, g, b, rr, gg, bb;
   int j;

   for(i = 0; i < MAX_BALLS; i++)
   {
      balls[i].x = rand()%SCREEN_W;
      balls[i].y = rand()%SCREEN_H;
      balls[i].vx = (float)(rand()%5000) - 2500;
      balls[i].vy = (float)(rand()%5000) - 2500;
      balls[i].r = rand()%60+25;
      balls[i].bmp = make_bitmap (balls[i].r*2+1, balls[i].r*2+1, 1, true);
      clear_to_color(balls[i].bmp, screen->vtable->mask_color);

      r = rand()%200+55;
      g = rand()%200+55;
      b = rand()%200+55;
      for(j = 0; j < balls[i].r; j++)
      {
         //t = r/BALL_GRAD;
         //l = balls[i].r / t;
         //for(k = 0; k*t < balls[i].r; k+=l)
         rr = r -j*2;//- k*t;

         //t = g/BALL_GRAD;
         //l = balls[i].r / t;
         //for(k = 0; k*t < balls[i].r; k+=l)
         gg = g -j*2;//- k*t;

         //t = b/BALL_GRAD;
         //l = balls[i].r / t;
         //for(k = 0; k*t < balls[i].r; k+=l)
         bb = b -j*2;//- k*t;

         rr = rr < 0 ? 0 : rr;
         gg = gg < 0 ? 0 : gg;
         bb = bb < 0 ? 0 : bb;

         circle(balls[i].bmp, balls[i].r-1, balls[i].r,   j, makecol(rr, gg, bb));
         circle(balls[i].bmp, balls[i].r,   balls[i].r-1, j, makecol(rr, gg, bb));
         circle(balls[i].bmp, balls[i].r+1, balls[i].r,   j, makecol(rr, gg, bb));
         circle(balls[i].bmp, balls[i].r,   balls[i].r+1, j, makecol(rr, gg, bb));
         circle(balls[i].bmp, balls[i].r,   balls[i].r,   j, makecol(rr, gg, bb));
      }
   }
}

void draw_balls(BITMAP *bmp)
{
   int i;

   for(i = 0; i < balls_display.value && i < MAX_BALLS; i++)
   {
      draw_sprite(bmp, balls[i].bmp, (int)(balls[i].x-balls[i].r), (int)(balls[i].y-balls[i].r));
   }
}

void move_balls()
{
   int i;

   if (!balls_move.value)
      return;

   for(i = 0; i < MAX_BALLS; i++)
   {
      balls[i].x += balls[i].vx*balls_speed.value/1000*frame_time;
      balls[i].y += balls[i].vy*balls_speed.value/1000*frame_time;

      if(balls[i].x < 0)
      {
         balls[i].x   = 0;
         balls[i].vx *= -1;
      }
      if(balls[i].x > SCREEN_W)
      {
         balls[i].x   = SCREEN_W;
         balls[i].vx *= -1;
      }

      if(balls[i].y < 0)
      {
         balls[i].y   = 0;
         balls[i].vy *= -1;
      }
      if(balls[i].y > SCREEN_H)
      {
         balls[i].y   = SCREEN_H;
         balls[i].vy *= -1;
      }
   }
}

#endif
