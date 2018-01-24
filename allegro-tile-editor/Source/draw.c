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
 *                                     by MAZsoft
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * DESCRIPTION:
 *
 *   Routines to draw tile map & infobar are here
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 */

#include "ated.h"

int infobarh;

/*----------------------------------------------------------------------------------------*/
int draw_infobar_proc(int msg, DIALOG *d, int c) 
{
   BITMAP *infobar;
   int mx, my;     //mouse mickeys

   switch (msg) 
   {
      case MSG_START:
      case MSG_SCREEN_RESIZE:
         if(d->dp) 
         {
            destroy_bitmap(d->dp);
         }
         infobarh = d->h = (tsize < 16) ? 16 : tsize;
         d->w = SCREEN_W;
         d->x = 0;
         d->y = SCREEN_H - d->h;

         if(!(infobar = create_bitmap(d->w, d->h))) 
         {
            fat_error("Out of memory!");
         }
         d->dp = infobar;
         break;

      case MSG_END:
         destroy_bitmap(d->dp);
         break;

      case MSG_IDLE:
         get_mouse_mickeys(&mx, &my);
         if(mx || my || (mouse_b & 2))
            SEND_MESSAGE(d, MSG_DRAW, 0);
         break;

      case MSG_DRAW:
      case MSG_UPDATE_INFO:
         if(!infobaron || !tsize)
            return D_O_K;

         if(!(infobar = create_bitmap(d->w, d->h))) 
         {
            fat_error("Out of memory!");
            //ated_alert("ERROR!","Out of memory!","ERROR!","Exit",NULL,NULL,NULL);
            //return D_EXIT;
         }
         blit(d->dp, infobar, 0, 0, 0, 0, d->w, d->h);

         //clear_to_color(infobar, white);
         drawing_mode(DRAW_MODE_COPY_PATTERN, skin, d->x, d->y);
         rectfill(infobar, 0, 0, d->w, d->h, gray);
         drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
         if (SelectMode) 
         {
            print(infobar, "Copy Mode",        8, 0);
            print(infobar, "Esc to exit",      8, 8);
         } 
         else if (TsearchMode) 
         {
            print(infobar, "Tile Search Mode", 8, 0);
            print(infobar, "Esc to exit",      8, 8);
         } 
         else if (BfillMode) 
         {
            print(infobar, "Block Fill Mode",  8, 0);
            print(infobar, "Esc to exit",      8, 8);
         }
         else if (PasteMode) 
         {
            print(infobar, "Paste Mode",       8, 0);
            print(infobar, "Esc to exit",      8, 8);
         }
         else if (FillMode) 
         {
            print(infobar, "Flood Fill Mode",  8, 0);
            print(infobar, "Esc to exit",      8, 8);
         }
         else if (TINFOMode) 
         {
            print(infobar, "TINFO Edit Mode",  8, 0);
            print(infobar, "Esc to exit",      8, 8);
         }
         else 
         {
            //
            // Draw selected tiles
            //
            if(which_t) 
            {
               if(which_t < gfx_t_count) 
               {
                  rectfill       (infobar, 0, 0, tsize-1, tsize-1, BkgndColor);
                  draw_rle_sprite(infobar, gfx_t [which_t].dat, 0, 0);
               }
               printhex       (infobar, which_t, tsize, 0, black);
               printint       (infobar, which_t, tsize, 8, black);
            } 
            else 
            {
               print          (infobar, " No",  tsize, 0);
               print          (infobar, "Tile", tsize, 8);
            }
            if(which_tm) 
            {
               if(which_tm < gfx_tm_count) 
               {
                  rectfill       (infobar, tsize + 40, 0, 2*tsize+39, tsize-1, BkgndColor);
                  draw_rle_sprite(infobar, gfx_tm[which_tm].dat, tsize + 40, 0);
               }
               printhex       (infobar, which_tm, 2*tsize + 40, 0, black);
               printint       (infobar, which_tm, 2*tsize + 40, 8, black);
            }
            else 
            {
               print          (infobar, " No",  2*tsize + 40, 0);
               print          (infobar, "Tile", 2*tsize + 40, 8);
            }
            if(which_i) 
            {
               rectfill       (infobar, 2*tsize + 80, 0, 3*tsize+79, tsize-1, BkgndColor);
               if(which_i < gfx_i_count) 
               {
                  draw_rle_sprite(infobar, gfx_i[which_i].dat, 2*tsize + 80, 0);
               }
               else 
               {  //
                  // print hex
                  //
                  draw_info_val(infobar, which_i, 2*tsize + 80, 0);
               }

               if(which_i <= 0xFFFF) 
               {
                  printhex       (infobar, which_i, 3*tsize + 80, 0, black);
                  printint       (infobar, which_i, 3*tsize + 80, 8, black);
               }
               else 
               {
                  printhex       (infobar, which_i >> 16,        3*tsize + 80, 0, black);
                  printhex       (infobar, which_i & 0x0000ffff, 3*tsize + 80, 8, black);
               }
            }
            else 
            {
               print          (infobar, " No",  3*tsize + 80, 0);
               print          (infobar, "Icon", 3*tsize + 80, 8);
            }
         }
         //
         // Draw mouse coords
         //
         print(infobar, "X=", d->w - 88, 0);
         print(infobar, "Y=", d->w - 88, 8);
         if((mouse_y > 8) && (mouse_y < d->y)) 
         {
            mx = mouse_x/tsize + xbase;
            my = (mouse_y-8)/tsize + ybase;
            printint  (infobar, mx,   d->w - 72, 0, black);
            printint  (infobar, my,   d->w - 72, 8, black);
            printhex  (infobar, mx,   d->w - 40, 0, black);
            printhex  (infobar, my,   d->w - 40, 8, black);
         }
         else 
         {
            print     (infobar, "??? ?????", d->w - 72, 0);
            print     (infobar, "??? ?????", d->w - 72, 8);
            //print     (infobar, "?????", d->w - 40, 0);
            //print     (infobar, "?????", d->w - 40, 8);
         }
         //
         // Draw plane options
         //
         print     (infobar, "123456",               d->w - 144, 8);
         print     (infobar, (planeton) ? "B" : " ", d->w - 144, 0);
         print     (infobar, (planemon) ? "F" : " ", d->w - 136, 0);
         print     (infobar, (planeion) ? "I" : " ", d->w - 128, 0);
         print     (infobar, (viewton)  ? "b" : " ", d->w - 120, 0);
         print     (infobar, (viewmon)  ? "f" : " ", d->w - 112, 0);
         print     (infobar, (viewion)  ? "i" : " ", d->w - 104, 0);
         //
         // Map name
         //
         if(d->w > 320) 
         {
            gui_textout_highlight(infobar, map_name, d->w/2, 0, white, white, black, TRUE, TEXT_OUTLINE_4, font);
         }
         //
         // Additional options
         //
         if (SnapMode) 
         {
            print   (infobar, "S", d->w - 160, 8);
         }
         if (GridMode) 
         {
            print   (infobar, "G", d->w - 152, 8);
         }

         scare_mouse();
         blit(infobar, screen, 0, 0, d->x, d->y, d->w, d->h);
         unscare_mouse();
   
         destroy_bitmap(infobar);
         break;
   }
   return D_O_K;
}
/*----------------------------------------------------------------------------------------*/
//
// Draws Map's background layer in window (x,y)-(x+ox,y+oy), with offsets in ox & oy
//
/*void Draw_MapBackground(BITMAP *dest, int ox, int oy, int x, int y, int w, int h) {
  int i, j, tsize, oldclpx1, oldclpy1, oldclpx2, oldclpy2, vclip, hclip;

  //tsize = tile.size;

  // Save old clipping range
  oldclpx1 = dest->cl;  oldclpy1 = dest->cr;  oldclpx2 = dest->ct;  oldclpy2 = dest->cb;

  vclip = oy%tsize;  hclip = ox%tsize;

  // Left Clipped
  i = (x - hclip);
  for (j = (y - vclip); j < ((y + h) - tsize); j += tsize) {

  }
  // Bottom Clipped
  for (; i < ((x + w) - tsize); i += tsize)) {

  }
  // Right Clipped
  for (; j > (y - vclip); j -= tsize) {

  }
  // Top Clipped
  for (; i > (x - hclip); i -= tsize) {

  }
  set_clip(dest, x, y, x+ox-1, y+oy-1);                               // Speed up process
  // Unclipped
  for   (j = ((y - vclip) + tsize); j < ((y + h) - tsize); j += tsize) {
    for (i = ((x - hclip) + tsize); i < ((x + w) - tsize); i += tsize) {

    }
  }
  set_clip(dest, oldclpx1, oldclpy1, oldclpx2+1, oldclpy2+1);         // Set up old clipping
}*/                                                                     // range
/*----------------------------------------------------------------------------------------*/
int  draw_rect_proc(int msg, DIALOG *d, int c) {
  return D_O_K;
}
/*----------------------------------------------------------------------------------------*/
#define PL_BACK                 1
#define PL_FORE                 2
#define PL_INFO                 3

static unsigned get_copy_tile(int plane, int x, int y)
{
   int xx, yy;
   unsigned loc;

   //xx = px + TileCopy.x - x;//x + TileCopy.x - px;
   //yy = py + TileCopy.y - y;//y + TileCopy.y - py;
   xx = (x + xbase) + TileCopy.x - px;
   yy = (y + ybase) + TileCopy.y - py;

   if((xx < TileCopy.x) || (xx > (TileCopy.x + TileCopy.w - 1)) ||
      (yy < TileCopy.y) || (yy > (TileCopy.y + TileCopy.h - 1)))
   {
      return -1;
   }

   if(TileCopy.MapOrTileSelect) 
   {
      loc = (yy*TileCopy.tile_select_w) + xx;

      switch(plane) 
      {
         case PL_BACK:
            if(TileCopy.back && planeton) 
            {
               if(loc < gfx_t_count) 
               {
                  return loc;
               }
               else 
               {
                  return 0;
               }
            }
            break;

         case PL_FORE:
            if(TileCopy.fore && planemon) 
            {
               if(loc < gfx_tm_count) 
               {
                  return loc;
               }
               else
               {
                  return 0;
               }
            }
            break;

         case PL_INFO:
            if(TileCopy.info && planeion) 
            {
               if(loc < gfx_i_count) 
               {
                  return loc;
               }
               else 
               {
                  return 0;
               }
            }
            break;
      }
   } else 
   {
      switch(plane) 
      {
         case PL_BACK:
            if(planeton)
               return CutBkgnd[(yy*mapwidth) + xx];
            break;

         case PL_FORE:
            if(planemon)
               return CutFrgnd[(yy*mapwidth) + xx];
            break;

         case PL_INFO:
            if(planeion)
               return CutInfoPl[(yy*mapwidth) + xx];
            break;
      }
   }

   return -1;
}
/*----------------------------------------------------------------------------------------*/
void draw_info_val(BITMAP *bmp, unsigned val, int x, int y)
{
   if(tsize >= 32)
   {
      printhex (bmp, val, x, y, black);
   } 
   else if(tsize >= 16)
   {
      printhexb (bmp, val >> 8,   x, y,     black);
      printhexb (bmp, val & 0xff, x, y + 8, black);
   } 
   else if(tsize >= 8)
   {
      print(bmp, "#", x, y);
   }
}
/*----------------------------------------------------------------------------------------*/
int  draw_map_proc(int msg, DIALOG *d, int c) 
{
   BITMAP *tmp;
   int x, y, xmax, ymax, mx, my;
   unsigned loc, tile_t, tile_m, tile_i;
   int t_flag;

   if(!tsize)
      return D_O_K;

   switch(msg) 
   {
      case MSG_START:
      case MSG_SCREEN_RESIZE:
         if(d->dp) 
         {
            destroy_bitmap(d->dp);
         }
         d->h = SCREEN_H - 8 - ((infobaron) ? infobarh : 0);
         d->w = SCREEN_W;
         d->x = 0;
         d->y = 8;
         screenw = d->w / tsize;
         screenh = d->h / tsize;

         if(!(tmp = create_bitmap(d->w, d->h))) 
         {
            fat_error("Out of memory!");
         }
         d->dp = tmp;
         break;

      case MSG_END:
         destroy_bitmap(d->dp);
         break;

      case MSG_IDLE:
         if(PasteMode) 
         {
            mx = mouse_x;
            my = mouse_y;
            //
            // Determine clicked tile x & y
            //
            for(x = 0; x < (mx - d->x); x += tsize);
               x /= tsize;
            if(x)
               x--;
            for(y = 0; y < (my - d->y); y += tsize);
               y /= tsize;
            if(y)
               y--;

            mx = x + xbase;
            my = y + ybase;

            if(((mx + 1) > mapwidth) || ((my + 1) > mapheight))
               return D_O_K;

            if(SnapMode && PasteMode) 
            {
               if((snapx != (mx - (mx/snapxsize) * snapxsize)) ||
                  (snapy != (my - (my/snapysize) * snapysize))) 
               {
                  if((snapx == -1) && (snapy == -1)) 
                  {
                     snapx = mx - (mx/snapxsize) * snapxsize;
                     snapy = my - (my/snapysize) * snapysize;

                     px = mx;
                     py = my;
                  }
                  else 
                  {
                     //px = snapx = mx - (mx/snapxsize) * snapxsize;
                     //py = snapy = my - (my/snapysize) * snapysize;
                     mx = (mx/snapxsize)*snapxsize + snapx;
                     my = (my/snapysize)*snapysize + snapy;

                     if(mx < xbase)
                        mx += snapxsize;
                     if(my < ybase)
                        my += snapysize;
                     if(mx > (xbase + screenw))
                        mx -= snapxsize;
                     if(my > (ybase + screenh))
                        my -= snapysize;

                     if(((px == mx) && (py == my)))
                        return D_O_K;

                     if((mx > mapwidth) || (my > mapheight))
                        return D_O_K;

                     px = mx;
                     py = my;
                  }

                  SEND_MESSAGE(d, MSG_DRAW, 0);
               }
            }
            else 
            {
               if((px != mx) || (py != my)) 
               {
                  px = mx;
                  py = my;

                  SEND_MESSAGE(d, MSG_DRAW, 0);
               }
            }
         }
         break;

      case MSG_CLICK:
         mx = mouse_x;
         my = mouse_y;
         //
         // Determine clicked tile x & y
         //
         for(x = 0; x < (mx - d->x); x += tsize);
            x /= tsize;
         if(x)
            x--;
         for(y = 0; y < (my - d->y); y += tsize);
            y /= tsize;
         if(y)
            y--;

         mx = x + xbase;
         my = y + ybase;

         if(((mx + 1) > mapwidth) || ((my + 1) > mapheight))
            return D_O_K;

         loc = (my*mapwidth) + mx;

         if(SelectMode || BfillMode) 
         {
            if(mouse_b & 1) 
            {
               SelX1 = mx;
               SelY1 = my;

               if(((SelX2 == -1) && (SelY2 == -1)) || ((SelX1 > SelX2) || (SelY1 > SelY2))) 
               {
                  SelX2 = mx;
                  SelY2 = my;
               }
            }

            if(mouse_b & 2) 
            {
               SelX2 = mx;
               SelY2 = my;

               if(((SelX1 == -1) && (SelY1 == -1)) || ((SelX1 > SelX2) || (SelY1 > SelY2))) 
               {
                  SelX1 = mx;
                  SelY1 = my;
               }
            }
         }
         else if(TsearchMode) 
         {
            if(mouse_b & 2) 
            {
               //
               // Grab tile
               //
               if(planeton) which_t  = MapBkgnd [loc];
               if(planemon) which_tm = MapFrgnd [loc];
               if(planeion) which_i  = MapInfoPl[loc];
            }
         }
         else if(PasteMode) 
         {
            if(SnapMode) 
            {
               mx = (mx / snapxsize)*snapxsize + snapx;
               my = (my / snapysize)*snapysize + snapy;
            }

            if(TileCopy.MapOrTileSelect) // TileSelect
            {
               CopyUndoRegion();
               UndoRegion.x = mx;
               UndoRegion.y = my;
               UndoRegion.w = (((TileCopy.x + TileCopy.w) > mapwidth)  ? (mapwidth  - TileCopy.x) : TileCopy.w);
               UndoRegion.h = (((TileCopy.y + TileCopy.h) > mapheight) ? (mapheight - TileCopy.y) : TileCopy.h);

               for(y = 0; y < TileCopy.h; y++) 
               {
                  for(x = 0; x < TileCopy.w; x++) 
                  {
                     int theT, theM, theI;
                     unsigned loc;

                     if(((y + my) >= mapheight) || ((x + mx) >= mapwidth))
                        continue;
                        
                     loc = ((y + TileCopy.y)*TileCopy.tile_select_w) + (x + TileCopy.x);

                     if(TileCopy.back && planeton)
                     {
                        theT = (loc < gfx_t_count) ? loc : 0;
                        if(theT || F3_flag)
                           MapBkgnd[((y + my)*mapwidth) + x + mx] = theT;
                     }
                     if(TileCopy.fore && planemon)
                     {
                        theM = (loc < gfx_tm_count) ? loc : 0;
                        if(theM || F3_flag)
                           MapFrgnd[((y + my)*mapwidth) + x + mx] = theM;
                     }
                     if(TileCopy.info && planeion)
                     {
                        theI = (loc < gfx_i_count) ? loc : 0;
                        if(theI || F3_flag)
                           MapInfoPl[((y + my)*mapwidth) + x + mx] = theI;
                     }
                  }
               }

               DirtyFlag = 1;
            } 
            else  // MapSelect
            { 
               CopyUndoRegion();
               UndoRegion.x = mx;
               UndoRegion.y = my;
               UndoRegion.w = (((TileCopy.x + TileCopy.w) > mapwidth)  ? (mapwidth  - TileCopy.x) : TileCopy.w);
               UndoRegion.h = (((TileCopy.y + TileCopy.h) > mapheight) ? (mapheight - TileCopy.y) : TileCopy.h);

               for(y = 0; y < TileCopy.h; y++) 
               {
                  for(x = 0; x < TileCopy.w; x++) 
                  {
                     int theT, theM, theI;

                     if(((y + my) >= mapheight) || ((x + mx) >= mapwidth))
                        continue;
                        
                     if(TileCopy.back && planeton)
                     {
                        theT = CutBkgnd[((y + TileCopy.y)*mapwidth) + x + TileCopy.x];
                        if(theT || F3_flag)
                           MapBkgnd[((y + my)*mapwidth) + x + mx] = theT;
                     }
                     if(TileCopy.fore && planemon)
                     {
                        theM = CutFrgnd[((y + TileCopy.y)*mapwidth) + x + TileCopy.x];
                        if(theM || F3_flag)
                           MapFrgnd[((y + my)*mapwidth) + x + mx] = theM;
                     }
                     if(TileCopy.info && planeion)
                     {
                        theI = CutInfoPl[((y + TileCopy.y)*mapwidth) + x + TileCopy.x];
                        if(theI || F3_flag)
                           MapInfoPl[((y + my)*mapwidth) + x + mx] = theI;
                     }
                  }
               }

               DirtyFlag = 1;
            }
         }
         else if(FillMode) 
         {
            if(mouse_b) 
            {
               //
               // Flood fill
               //
               DoFloodFill(mx, my);

               broadcast_dialog_message(MSG_UPDATE_INFO, 0);
               while(mouse_b)
                  ;
            }
         }
         else if (TINFOMode) 
         {
            if (mouse_b)
            {
               do_tinfo_edit_map (mx, my);
            }
         }
         else 
         {
            if(mouse_b & 1) 
            {
               //
               // Modify tilemap
               //
               if(planeton) MapBkgnd [loc] = which_t;
               if(planemon) MapFrgnd [loc] = which_tm;
               if(planeion) MapInfoPl[loc] = which_i;

               DirtyFlag = 1;
            }
            else if(mouse_b & 2) 
            {
               //
               // Grab tile
               //
               if(planeton) which_t  = MapBkgnd [loc];
               if(planemon) which_tm = MapFrgnd [loc];
               if(planeion) which_i  = MapInfoPl[loc];
            }
         }

         broadcast_dialog_message(MSG_UPDATE_INFO, 0);
         break;

      case MSG_DRAW:
      case MSG_UPDATE_INFO:
         if(!(tmp = create_bitmap(d->w, d->h))) 
         {
            fat_error("Out of memory!");
         }

         blit(d->dp, tmp, 0, 0, 0, 0, d->w, d->h);
         clear_to_color(tmp, BkgndColor);

         xmax = ((screenw+1) > mapwidth)  ? mapwidth  : (screenw+1);
         ymax = ((screenh+1) > mapheight) ? mapheight : (screenh+1);

         for(x = 0; x < xmax; x++)
         {
            for(y = 0; y < ymax; y++)
            {
               loc = ((ybase + y)*mapwidth) + (xbase + x);
               t_flag = FALSE;

               if(viewton)
               {
                  if(TsearchMode && planeton)
                  {
                     if((tile_t = MapBkgnd[loc]) == which_t)
                        t_flag = TRUE;
                  }
                  else if(PasteMode && TileCopy.back)
                  {
                     tile_t = get_copy_tile(PL_BACK, x, y);

                     if((tile_t == -1) || (!F3_flag && !tile_t)) 
                     {
                        tile_t = MapBkgnd[loc];
                     }
                  }
                  else
                  {
                     tile_t = MapBkgnd[loc];
                  }

                  if(TsearchMode && t_flag)
                  {
                     draw_info_val(tmp, which_t, x*tsize, y*tsize);
                  }
                  else if(tile_t < gfx_t_count)
                  {
                     draw_rle_sprite(tmp, gfx_t [tile_t].dat, x * tsize, y * tsize);
                  }
               }

               if(viewmon && !t_flag)
               {
                  if(TsearchMode && planemon)
                  {
                     if((tile_m = MapFrgnd[loc]) == which_tm)
                        t_flag = TRUE;
                  }
                  else if(PasteMode && TileCopy.fore)
                  {
                     tile_m = get_copy_tile(PL_FORE, x, y);
   
                     if((tile_m == -1) || (!F3_flag && !tile_m))
                     {
                        tile_m = MapFrgnd[loc];
                     }
                  }
                  else
                  {
                     tile_m = MapFrgnd[loc];
                  }

                  if(TsearchMode && t_flag)
                  {
                     draw_info_val(tmp, which_tm, x*tsize, y*tsize);
                  }
                  else if(tile_m < gfx_tm_count)
                  {
                     draw_rle_sprite(tmp, gfx_tm[tile_m].dat, x * tsize, y * tsize);
                  }
               }

               if(GridMode)
               {
                  hline(tmp, x * tsize, y * tsize, x * tsize + tsize, white);
                  vline(tmp, x * tsize, y * tsize, y * tsize + tsize, white);
               }

               if(viewion && !t_flag)
               {
                  if(TsearchMode && planeion)
                  {
                     if((tile_i = MapInfoPl[loc]) == which_i)
                        t_flag = TRUE;
                  }
                  else if(PasteMode && TileCopy.info)
                  {
                     tile_i = get_copy_tile(PL_INFO, x, y);
   
                     if((tile_i == -1) || (!F3_flag && !tile_i))
                     {
                        tile_i = MapInfoPl[loc];
                     }
                  }
                  else
                  {
                     tile_i = MapInfoPl[loc];
                  }

                  if(TsearchMode && t_flag)
                  {
                     draw_info_val(tmp, which_i, x*tsize, y*tsize);
                  }
                  else if(tile_i < gfx_i_count)
                  {
                     draw_rle_sprite(tmp, gfx_i [tile_i].dat, x * tsize, y * tsize);
                  }
                  else
                  {
                     //
                     // print hex
                     //
                     draw_info_val(tmp, tile_i, x*tsize, y*tsize);
                  }
               }
            }
         }
         if((BfillMode || SelectMode) && (SelX1 != -1) && (SelY1 != -1) && (SelX2 != -1) && (SelY2 != -1)) 
         {
            //
            // Draw selected area
            //
            rect(tmp, (SelX1*tsize) - (xbase*tsize),         (SelY1*tsize) - (ybase*tsize),
                      (SelX2*tsize) - (xbase*tsize) + tsize, (SelY2*tsize) - (ybase*tsize) + tsize,
                      black);
            rect(tmp, (SelX1*tsize) - (xbase*tsize) + 1,         (SelY1*tsize) - (ybase*tsize) + 1,
                      (SelX2*tsize) - (xbase*tsize) + tsize - 1, (SelY2*tsize) - (ybase*tsize) + tsize - 1,
                      gray);
            rect(tmp, (SelX1*tsize) - (xbase*tsize) + 2,         (SelY1*tsize) - (ybase*tsize) + 2,
                      (SelX2*tsize) - (xbase*tsize) + tsize - 2, (SelY2*tsize) - (ybase*tsize) + tsize - 2,
                      white);
            rect(tmp, (SelX1*tsize) - (xbase*tsize) + 3,         (SelY1*tsize) - (ybase*tsize) + 3,
                      (SelX2*tsize) - (xbase*tsize) + tsize - 3, (SelY2*tsize) - (ybase*tsize) + tsize - 3,
                      gray);
            rect(tmp, (SelX1*tsize) - (xbase*tsize) + 4,         (SelY1*tsize) - (ybase*tsize) + 4,
                      (SelX2*tsize) - (xbase*tsize) + tsize - 4, (SelY2*tsize) - (ybase*tsize) + tsize - 4,
                      black);
         }
         scare_mouse();
         blit(tmp, screen, 0, 0, d->x, d->y, d->w, d->h);
         //blit(tmp, screen, 0, 0, d->x, d->y, d->w, (!infobaron) ? d->h : (d->h - tsize));
         unscare_mouse();
         destroy_bitmap(tmp);
         break;
   }

   return D_O_K;
}

