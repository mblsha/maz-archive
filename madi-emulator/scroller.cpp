#include "madi.h"

static char *text[] =
{  // The russian text in utf-8 :-)
   "Беги скорей в МАДИ, абитуриент!",
   "Здесь ты получишь студенческий билет!",
   "Учиться будешь на отлично,",
   "И красный диплом получишь ты лично!",
   ""
};

Scroller::Scroller ()
{
   y = SCREEN_H/2;
   h = 0;

   for (int i = 0; text[i]; i++)
      h += text_height (font) + 5;
}

Scroller::~Scroller ()
{
}

void Scroller::draw (BITMAP *bmp)
{
   int i, yy;
   
   if (key[KEY_RSHIFT])
      return;

   for (i = 0, yy = y; /*ustrlen (*/text[i]/*)*/; i++, yy += text_height (font) + 5)
   {
      textout_centre (bmp, font, text[i], SCREEN_W/2-1, yy,   makecol (0,0,0));
      textout_centre (bmp, font, text[i], SCREEN_W/2,   yy-1, makecol (0,0,0));
      textout_centre (bmp, font, text[i], SCREEN_W/2+1, yy,   makecol (0,0,0));
      textout_centre (bmp, font, text[i], SCREEN_W/2,   yy+1, makecol (0,0,0));
      
      textout_centre (bmp, font, text[i], SCREEN_W/2, yy, makecol (255,255,255));
   }
}

void Scroller::move ()
{
   if ((--y + h) < 0)
      y = SCREEN_H/2;
}

bool Scroller::done ()
{
   return false;
}
