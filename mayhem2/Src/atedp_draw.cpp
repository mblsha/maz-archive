/*  ______   ___    ___
 * /\  _  \ /\_ \  /\_ \
 * \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *  \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *   \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *    \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *     \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                    /\____/
 *                                    \_/__/
 *           ______     ___               ______      __        __
 *          /\__  _\__ /\_ \             /\  ___\    /\ \  __  /\ \__
 *          \/_/\ \/\_\\//\ \      __    \ \ \__/    \_\ \/\_\/\__  _\  ___   _ __
 *             \ \ \/_/_ \ \ \   /'__'\   \ \  __\   / __ \/_/\/_/\ \/ / __'\/\''__\
 *              \ \ \/\ \ \_\ \_/\  __/    \ \ \_/__/\ \L\ \/\ \ \ \ \/\ \L\ \ \ \/
 *               \ \_\ \_\/\____\ \____\    \ \_____\ \_____\ \_\ \ \_\ \____/\ \ \
 *                \/_/\/_/\/____/\/____/     \/_____/\/_____/\/_/  \/_/\/___/  \/_/
 *
 *                                  Playback Library
 *
 *                                     by MAZsoft
 */

#include "atedp.h"

VIEWPORT *create_viewport(int x, int y, int w, int h, int back, int fore, int info, BITMAP *bmp)
{
   VIEWPORT *vp;

   vp = (VIEWPORT *)malloc (sizeof (VIEWPORT));

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

   vp->x1 = 0;
   vp->y1 = 0;
   vp->x2 = w;
   vp->y2 = h;

   vp->x  = x;
   vp->y  = y;

   vp->w  = w;
   vp->h  = h;

   vp->dx     = 0;
   vp->dy     = 0;
   vp->dx_old = 0;
   vp->dy_old = 0;

   vp->back = back; //&& tile_map.back;
   vp->fore = fore; //&& tile_map.fore;
   vp->info = info; //&& tile_map.info;

   if(bmp)
   {
      if(back)
         vp->back_bmp = bmp;

      if(fore)
         vp->fore_bmp = bmp;

      if(info)
         vp->info_bmp = bmp;
   }
   else
   {
      if(back)
         vp->back_bmp = create_bitmap(w, h);

      if(fore)
         vp->fore_bmp = create_bitmap(w, h);

      if(info)
         vp->info_bmp = create_bitmap(w, h);
   }

   if (back)
      clear (vp->back_bmp);

   if (fore)
      clear (vp->fore_bmp);

   if (info)
      clear (vp->info_bmp);

   return vp;
}

void destroy_viewport(VIEWPORT *vp)
{
   if(vp)
   {
      free(vp);

      vp = NULL;
   }
}

void viewport2bmp(VIEWPORT *vp, BITMAP *bmp, int back, int fore, int info)
{
   if(vp->back && back)
      draw_sprite (bmp, vp->back_bmp, vp->x, vp->y);

   if(vp->fore && fore)
      draw_sprite (bmp, vp->fore_bmp, vp->x, vp->y);

   if(vp->info && info)
      draw_sprite (bmp, vp->info_bmp, vp->x, vp->y);
}

void move_viewport(VIEWPORT *vp, int x, int y, int is_absolute)
{
   if (is_absolute)
   {
      vp->dx_old = vp->dx;
      vp->dy_old = vp->dy;

      vp->dx = x;
      vp->dy = y;
   }
   else if (!x && !y)
   {
      return;
   }
   else
   {
      vp->dx_old = vp->dx;
      vp->dy_old = vp->dy;

      vp->dx += x;
      vp->dy += y;
   }

   vp->vx = vp->dx - vp->dx_old;
   vp->vy = vp->dy - vp->dy_old;

   if((vp->dx != vp->dx_old) || (vp->dy != vp->dy_old))
   {
      //vp->dx = x;
      //vp->dy = y;

      check_viewport_edges(vp);
   }
}

void check_viewport_edges(VIEWPORT *vp)
{
   if((vp->dx + vp->w) > (tile_map.w * tile_map.tile_size))
   {
      vp->dx = (tile_map.w * tile_map.tile_size) - vp->w;
   }

   if(vp->dx < 0)
   {
      vp->dx = 0;
   }

   if((vp->dy + vp->h) > (tile_map.h * tile_map.tile_size))
   {
      vp->dy = (tile_map.h * tile_map.tile_size) - vp->h;
   }

   if(vp->dy < 0)
   {
      vp->dy = 0;
   }
}

void update_viewport(VIEWPORT *vp)
{/*
   int ox = vp->dx - vp->dx_old;
   int oy = vp->dy - vp->dy_old;
   int x1, y1, x2, y2, x3, y3, x4, y4;
   int sx, sy, dx, dy;
   int w, h;

   vp->dx_old = vp->dx;
   vp->dy_old = vp->dy;

   if(!ox && !oy)
      return;

   if(oy < 0)
   {
      sy = 0;
      dy = -oy;
      h  = vp->h + oy;

      x1 = 0;
      x2 = vp->w;
      y1 = 0;
      y2 = -oy;
   }
   else
   {
      sy = oy;
      dy = 0;
      h  = vp->h - oy;

      x1 = 0;
      x2 = vp->w;
      y1 = h;
      y2 = vp->h;
   }

   if(ox < 0)
   {
      sx = 0;
      dx = -ox;
      w  = vp->w + ox;

      x3 = 0;
      x4 = dx;
      y3 = dy;
      y4 = dy + h;
   }
   else
   {
      sx = ox;
      dx = 0;
      w  = vp->w - ox;

      x3 = w;
      x4 = vp->w;
      y3 = dy;
      y4 = dy + h;
   }

   if(vp->back)
   {
      clear(vp->back_tmp);
      blit(vp->back_bmp, vp->back_tmp, sx, sy, sx, sy, w, h);
      clear(vp->back_bmp);
      blit(vp->back_tmp, vp->back_bmp, sx, sy, dx, dy, w, h);
   }
   if(vp->fore)
   {
      clear(vp->fore_tmp);
      blit(vp->fore_bmp, vp->fore_tmp, sx, sy, sx, sy, w, h);
      clear(vp->fore_bmp);
      blit(vp->fore_tmp, vp->fore_bmp, sx, sy, dx, dy, w, h);
   }
   if(vp->info)
   {
      clear(vp->info_tmp);
      blit(vp->info_bmp, vp->info_tmp, sx, sy, sx, sy, w, h);
      clear(vp->info_bmp);
      blit(vp->info_tmp, vp->info_bmp, sx, sy, dx, dy, w, h);
   }

   if(oy)
   {
      vp->x1 = x1;
      vp->y1 = y1;
      vp->x2 = x2;
      vp->y2 = y2;

      draw_planes(vp);
   }

   if(ox)
   {
      vp->x1 = x3;
      vp->y1 = y3;
      vp->x2 = x4;
      vp->y2 = y4;

      draw_planes(vp);
   }

   vp->x1 = 0;
   vp->y1 = 0;
   vp->x2 = vp->w;
   vp->y2 = vp->h;*/
   /*
  if(ox < 0)
  {
    sx = 0;
    dx = -ox;
    w  = vp->w + ox;
  }
  else
  {
    sx = ox;
    dx = 0;
    w  = vp->w - ox;

    x3 = 0;
  }

  if(oy < 0)
  {
    sy = 0;
    dy = -oy;
    h  = vp->h + oy;
  }
  else
  {
    sy = oy;
    dy = 0;
    h  = vp->h - oy;
  }
  */
  /*
  int xm = vp->dx - vp->dx_old;
  int ym = vp->dy - vp->dy_old;

  int xoff1;// = (xm < 0) ? -xm : 0;
  int yoff1;// = (ym < 0) ? -ym : 0;
  int xoff2;// = (xm < 0) ? sx  : sx - xm;
  int yoff2;// = (ym < 0) ? sy  : sy - ym;

  int sz_x;// = xoff2- xoff1;
  int sz_y;// = yoff2- yoff1;

  //int x1_old = vp->x1;
  //int y1_old = vp->y1;
  //int x2_old = vp->x2;
  //int y2_old = vp->y2;

  struct {
    int x1, y1, x2, y2;
  } draw_area[2];

  if((vp->dx_old == vp->dx) && (vp->dy_old == vp->dy))
    return;

  if(xm < 0) {
    xoff1 = -xm;
    xoff2 = vp->w;

    draw_area[0].x1 = 0;
    draw_area[0].x2 = xoff1;
  } else {
    xoff1 = 0;
    xoff2 = vp->w - xm;

    draw_area[0].x1 = xoff2;
    draw_area[0].x2 = vp->w;
  }
  if(ym < 0) {
    yoff1 = -ym;
    yoff2 = vp->h;

    draw_area[0].y1 = 0;
    draw_area[0].y2 = yoff1;
  } else {
    yoff1 = 0;
    yoff2 = vp->h - ym;

    draw_area[0].y1 = yoff2;
    draw_area[0].y2 = vp->h;
  }

  if(tile_map.back) {
    //blit(vp->back_bmp, vp->back_bmp, xoff1, yoff1, xoff2, yoff2, xoff1, yoff1, sz_x, sz_y);
    blit(vp->back_bmp, vp->back_bmp, xoff1, yoff1, xoff1, yoff1, sz_x, sz_y);
    if(xm) {
      vp->x1 = draw_area[0].x1;
      vp->y1 = draw_area[0].y1;
      vp->x2 = draw_area[0].x2;
      vp->y2 = draw_area[0].y2;

      draw_background(vp);
    }
    if(ym) {
      vp->x1 = draw_area[1].x1;
      vp->y1 = draw_area[1].y1;
      vp->x2 = draw_area[1].x2;
      vp->y2 = draw_area[1].y2;

      draw_background(vp);
    }
  }

  vp->dx_old = vp->dx;
  vp->dy_old = vp->dy;

  vp->x1 = 0;//x1_old;
  vp->y1 = 0;//y1_old;
  vp->x2 = vp->w;//x2_old;
  vp->y2 = vp->h;//y2_old;
  */
}

#include "atedp_draw_bmp.cpp"
#include "atedp_draw_rle.cpp"

void draw_background (VIEWPORT *vp, BITMAP *bmp)
{
   if (tiletype == tiletype_rle_sprite)
   {
      draw_vp_back_rle (vp, bmp);
   }
   else // tiletype == tiletype_bitmap
   {
      draw_vp_back_bmp (vp, bmp);
   }
}

void draw_planes(VIEWPORT *vp, BITMAP *bmp)
{
   if (tiletype == tiletype_rle_sprite)
   {
      draw_vp_planes_rle (vp, bmp);
   }
   else // tiletype == tiletype_bitmap
   {
      draw_vp_planes_bmp (vp, bmp);
   }
}

void draw_foreground(VIEWPORT *vp, BITMAP *bmp)
{
   if (tiletype == tiletype_rle_sprite)
   {
      draw_vp_fore_rle (vp, bmp);
   }
   else // tiletype == tiletype_bitmap
   {
      draw_vp_fore_bmp (vp, bmp);
   }
}

void draw_info_plane(VIEWPORT *vp, BITMAP *bmp)
{
   if (tiletype == tiletype_rle_sprite)
   {
      draw_vp_info_rle (vp, bmp);
   }
   else // tiletype == tiletype_bitmap
   {
      draw_vp_info_bmp (vp, bmp);
   }
}

