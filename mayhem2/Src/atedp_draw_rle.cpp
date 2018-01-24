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

void draw_vp_back_rle (VIEWPORT *vp, BITMAP *bmp)
{
   int x,  y, x1, x2, y1, y2;
   int xx, yy;
   int tx1, ty1, tx2, ty2;
   unsigned loc, loc2, tile, tile2;
//   int clip;
   int cx1, cy1, cx2, cy2;

   if (!bmp)
   {
      bmp = vp->back_bmp;

      clear_to_color (bmp, screen->vtable->mask_color);
   }

   cx1 = bmp->cl;
   cy1 = bmp->ct;
   cx2 = bmp->cr;
   cy2 = bmp->cb;

   // calculate tiles
   tx1 = ((vp->dx + vp->x1 - ((vp->dx + vp->x1) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   ty1 = ((vp->dy + vp->y1 - ((vp->dy + vp->y1) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   tx2 = ((vp->dx + vp->x2 - ((vp->dx + vp->x2) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   ty2 = ((vp->dy + vp->y2 - ((vp->dy + vp->y2) % tile_map.tile_size)) / tile_map.tile_size); //- 1;

   // set clipping rectangle
   set_clip(bmp, vp->x1 + vp->x, vp->y1 + vp->y, vp->x2 + vp->x, vp->y2 + vp->y);

   //
   // draw clipped tiles
   //

   //
   // Horizontal clipping
   //
   loc  = (ty1 * tile_map.w) + tx2;
   loc2 = (ty2 * tile_map.w) + tx2;
   y1   = (ty1 * tile_map.tile_size) - vp->dy + vp->y;
   y2   = (ty2 * tile_map.tile_size) - vp->dy + vp->y;
   xx   = (tx2 * tile_map.tile_size) - vp->dx + vp->x;

   for(x = (tx2 - tx1); x >= 0; x--)
   {
      if((tile  = tile_map.back_plane[loc])  > 0)
         draw_rle_sprite(bmp, (RLE_SPRITE *)back_tile_gfx[tile].dat,  xx, y1);

      if((tile2 = tile_map.back_plane[loc2]) > 0)
         draw_rle_sprite(bmp, (RLE_SPRITE *)back_tile_gfx[tile2].dat, xx, y2);

      xx -= tile_map.tile_size;
      loc--;
      loc2--;
   }

   //
   // Vertical clipping
   //
   loc  = (ty2 * tile_map.w) + tx1;
   loc2 = (ty2 * tile_map.w) + tx2;
   x1   = (tx1 * tile_map.tile_size) - vp->dx + vp->x;
   x2   = (tx2 * tile_map.tile_size) - vp->dx + vp->x;
   yy   = (ty2 * tile_map.tile_size) - vp->dy + vp->y;

   for(y = (ty2 - ty1); y >= 0; y--)
   {
      if((tile  = tile_map.back_plane[loc])  > 0)
         draw_rle_sprite(bmp, (RLE_SPRITE *)back_tile_gfx[tile].dat,  x1, yy);

      if((tile2 = tile_map.back_plane[loc2]) > 0)
         draw_rle_sprite(bmp, (RLE_SPRITE *)back_tile_gfx[tile2].dat, x2, yy);

      yy   -= tile_map.tile_size;
      loc  -= tile_map.w;
      loc2 -= tile_map.w;
   }

   // that's for speed
   set_clip(bmp, 0, 0, 0, 0);

   //
   // draw tiles, that are not clipped
   //
   tx1++;
   ty1++;
   loc = (ty1 * tile_map.w) + tx1;
   yy  = (ty1 * tile_map.tile_size) - vp->dy + vp->y;

   for(y = (ty2 - ty1); y > 0; y--)
   {
      xx = (tx1 * tile_map.tile_size) - vp->dx + vp->x;

      for(x = (tx2 - tx1); x > 0; x--)
      {
         if((tile = tile_map.back_plane[loc]) > 0)
            draw_rle_sprite(bmp, (RLE_SPRITE *)back_tile_gfx[tile].dat, xx, yy);

         xx += tile_map.tile_size;
         loc++;
      }

      yy  += tile_map.tile_size;
      loc += tile_map.w - tx2 + tx1;
   }

   // set clipping rectangle back to original
   set_clip(bmp, cx1, cy1, cx2, cy2);
}

void draw_vp_planes_rle (VIEWPORT *vp, BITMAP *bmp)
{
   int x,  y, x1, x2, y1, y2;
   int xx, yy;
   int tx1, ty1, tx2, ty2;
   unsigned loc, loc2, tile, tile2;
   BITMAP *back, *fore;
   int cx1, cy1, cx2, cy2;

   if (!bmp)
   {
      back = vp->back_bmp;
      fore = vp->fore_bmp;

      clear_to_color (back, screen->vtable->mask_color);
      clear_to_color (fore, screen->vtable->mask_color);
   }
   else
   {
      back = bmp;
      fore = bmp;
   }

   // calculate tiles
   tx1 = ((vp->dx + vp->x1 - ((vp->dx + vp->x1) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   ty1 = ((vp->dy + vp->y1 - ((vp->dy + vp->y1) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   tx2 = ((vp->dx + vp->x2 - ((vp->dx + vp->x2) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   ty2 = ((vp->dy + vp->y2 - ((vp->dy + vp->y2) % tile_map.tile_size)) / tile_map.tile_size); //- 1;

   cx1 = back->cl;
   cy1 = back->ct;
   cx2 = back->cr;
   cy2 = back->cb;

   // set clipping rectangle
   set_clip(back, vp->x1 + vp->x, vp->y1 + vp->y, vp->x2 + vp->x, vp->y2 + vp->y);
   set_clip(fore, vp->x1 + vp->x, vp->y1 + vp->y, vp->x2 + vp->x, vp->y2 + vp->y);
   //
   // draw clipped tiles
   //

   //
   // Horizontal clipping
   //
   loc  = (ty1 * tile_map.w) + tx2;
   loc2 = (ty2 * tile_map.w) + tx2;
   y1   = (ty1 * tile_map.tile_size) - vp->dy + vp->y;
   y2   = (ty2 * tile_map.tile_size) - vp->dy + vp->y;
   xx   = (tx2 * tile_map.tile_size) - vp->dx + vp->x;

   for(x = (tx2 - tx1); x >= 0; x--)
   {
      if(vp->back)
      {
         if((tile  = tile_map.back_plane[loc])  > 0)
            draw_rle_sprite(back, (RLE_SPRITE *)back_tile_gfx[tile].dat,  xx, y1);

         if((tile2 = tile_map.back_plane[loc2]) > 0)
            draw_rle_sprite(back, (RLE_SPRITE *)back_tile_gfx[tile2].dat, xx, y2);
      }
      if(vp->fore)
      {
         if((tile  = tile_map.fore_plane[loc])  > 0)
            draw_rle_sprite(fore, (RLE_SPRITE *)fore_tile_gfx[tile].dat,  xx, y1);

         if((tile2 = tile_map.fore_plane[loc2]) > 0)
            draw_rle_sprite(fore, (RLE_SPRITE *)fore_tile_gfx[tile2].dat, xx, y2);
      }

      xx -= tile_map.tile_size;
      loc--;
      loc2--;
   }
   //
   // Vertical clipping
   //
   loc  = (ty2 * tile_map.w) + tx1;
   loc2 = (ty2 * tile_map.w) + tx2;
   x1   = (tx1 * tile_map.tile_size) - vp->dx + vp->x;
   x2   = (tx2 * tile_map.tile_size) - vp->dx + vp->x;
   yy   = (ty2 * tile_map.tile_size) - vp->dy + vp->y;

   for(y = (ty2 - ty1); y >= 0; y--)
   {
      if(vp->back)
      {
         if((tile  = tile_map.back_plane[loc])  > 0)
            draw_rle_sprite(back, (RLE_SPRITE *)back_tile_gfx[tile].dat,  x1, yy);

         if((tile2 = tile_map.back_plane[loc2]) > 0)
            draw_rle_sprite(back, (RLE_SPRITE *)back_tile_gfx[tile2].dat, x2, yy);
      }
      if(vp->fore)
      {
         if((tile  = tile_map.fore_plane[loc])  > 0)
            draw_rle_sprite(fore, (RLE_SPRITE *)fore_tile_gfx[tile].dat,  x1, yy);

         if((tile2 = tile_map.fore_plane[loc2]) > 0)
            draw_rle_sprite(fore, (RLE_SPRITE *)fore_tile_gfx[tile2].dat, x2, yy);
      }

      yy   -= tile_map.tile_size;
      loc  -= tile_map.w;
      loc2 -= tile_map.w;
   }

   // that's for speed
   set_clip(back, 0, 0, 0, 0);
   set_clip(fore, 0, 0, 0, 0);

   //
   // draw tiles, that are not clipped
   //
   tx1++;
   ty1++;
   loc = (ty1 * tile_map.w) + tx1;
   yy  = (ty1 * tile_map.tile_size) - vp->dy + vp->y;

   for(y = (ty2 - ty1); y > 0; y--)
   {
      xx = (tx1 * tile_map.tile_size) - vp->dx + vp->x;

      for(x = (tx2 - tx1); x > 0; x--)
      {
         if(vp->back)
         {
            if((tile = tile_map.back_plane[loc]) > 0)
               draw_rle_sprite(back, (RLE_SPRITE *)back_tile_gfx[tile].dat, xx, yy);
         }
         if(vp->fore)
         {
            if((tile = tile_map.fore_plane[loc]) > 0)
               draw_rle_sprite(fore, (RLE_SPRITE *)fore_tile_gfx[tile].dat, xx, yy);
         }
         xx += tile_map.tile_size;
         loc++;
      }

      yy  += tile_map.tile_size;
      loc += tile_map.w - tx2 + tx1;
   }

   // set clipping rectangle back to original
   set_clip(back, cx1, cy1, cx2, cy2);
   set_clip(fore, cx1, cy1, cx2, cy2);
}

void draw_vp_fore_rle (VIEWPORT *vp, BITMAP *bmp)
{
   int x,  y, x1, x2, y1, y2;
   int xx, yy;
   int tx1, ty1, tx2, ty2;
   unsigned loc, loc2, tile, tile2;
   int cx1, cy1, cx2, cy2;

   if (!bmp)
   {
      bmp = vp->fore_bmp;

      clear_to_color (bmp, screen->vtable->mask_color);
   }

   cx1 = bmp->cl;
   cy1 = bmp->ct;
   cx2 = bmp->cr;
   cy2 = bmp->cb;

   // calculate tiles
   tx1 = ((vp->dx + vp->x1 - ((vp->dx + vp->x1) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   ty1 = ((vp->dy + vp->y1 - ((vp->dy + vp->y1) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   tx2 = ((vp->dx + vp->x2 - ((vp->dx + vp->x2) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   ty2 = ((vp->dy + vp->y2 - ((vp->dy + vp->y2) % tile_map.tile_size)) / tile_map.tile_size); //- 1;

   // set clipping rectangle
   set_clip(bmp, vp->x1 + vp->x, vp->y1 + vp->y, vp->x2 + vp->x, vp->y2 + vp->y);

   //
   // draw clipped tiles
   //

   //
   // Horizontal clipping
   //
   loc  = (ty1 * tile_map.w) + tx2;
   loc2 = (ty2 * tile_map.w) + tx2;
   y1   = (ty1 * tile_map.tile_size) - vp->dy + vp->y;
   y2   = (ty2 * tile_map.tile_size) - vp->dy + vp->y;
   xx   = (tx2 * tile_map.tile_size) - vp->dx + vp->x;

   for(x = (tx2 - tx1); x >= 0; x--)
   {
      if((tile  = tile_map.fore_plane[loc])  > 0)
         draw_rle_sprite(bmp, (RLE_SPRITE *)fore_tile_gfx[tile].dat,  xx, y1);

      if((tile2 = tile_map.fore_plane[loc2]) > 0)
         draw_rle_sprite(bmp, (RLE_SPRITE *)fore_tile_gfx[tile2].dat, xx, y2);

      xx -= tile_map.tile_size;
      loc--;
      loc2--;
   }
   //
   // Vertical clipping
   //
   loc  = (ty2 * tile_map.w) + tx1;
   loc2 = (ty2 * tile_map.w) + tx2;
   x1   = (tx1 * tile_map.tile_size) - vp->dx + vp->x;
   x2   = (tx2 * tile_map.tile_size) - vp->dx + vp->x;
   yy   = (ty2 * tile_map.tile_size) - vp->dy + vp->y;

   for(y = (ty2 - ty1); y >= 0; y--)
   {
      if((tile  = tile_map.fore_plane[loc])  > 0)
         draw_rle_sprite(bmp, (RLE_SPRITE *)fore_tile_gfx[tile].dat,  x1, yy);

      if((tile2 = tile_map.fore_plane[loc2]) > 0)
         draw_rle_sprite(bmp, (RLE_SPRITE *)fore_tile_gfx[tile2].dat, x2, yy);

      yy   -= tile_map.tile_size;
      loc  -= tile_map.w;
      loc2 -= tile_map.w;
   }

   // that's for speed
   set_clip(bmp, 0, 0, 0, 0);

   //
   // draw tiles, that are not clipped
   //
   tx1++;
   ty1++;
   loc = (ty1 * tile_map.w) + tx1;
   yy  = (ty1 * tile_map.tile_size) - vp->dy + vp->y;

   for(y = (ty2 - ty1); y > 0; y--)
   {
      xx = (tx1 * tile_map.tile_size) - vp->dx + vp->x;

      for(x = (tx2 - tx1); x > 0; x--)
      {
         if((tile = tile_map.fore_plane[loc]) > 0)
            draw_rle_sprite(bmp, (RLE_SPRITE *)fore_tile_gfx[tile].dat, xx, yy);

         xx += tile_map.tile_size;
         loc++;
      }

      yy  += tile_map.tile_size;
      loc += tile_map.w - tx2 + tx1;
   }

   // set clipping rectangle back to original
   set_clip(bmp, cx1, cy1, cx2, cy2);
}

void draw_vp_info_rle (VIEWPORT *vp, BITMAP *bmp)
{
   int x,  y, x1, x2, y1, y2;
   int xx, yy;
   int tx1, ty1, tx2, ty2;
   unsigned loc, loc2, tile, tile2;
   int cx1, cy1, cx2, cy2;

   if (!bmp)
   {
      bmp = vp->info_bmp;

      clear_to_color (bmp, screen->vtable->mask_color);
   }

   cx1 = bmp->cl;
   cy1 = bmp->ct;
   cx2 = bmp->cr;
   cy2 = bmp->cb;

   // calculate tiles
   tx1 = ((vp->dx + vp->x1 - ((vp->dx + vp->x1) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   ty1 = ((vp->dy + vp->y1 - ((vp->dy + vp->y1) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   tx2 = ((vp->dx + vp->x2 - ((vp->dx + vp->x2) % tile_map.tile_size)) / tile_map.tile_size); //- 1;
   ty2 = ((vp->dy + vp->y2 - ((vp->dy + vp->y2) % tile_map.tile_size)) / tile_map.tile_size); //- 1;

   // set clipping rectangle
   set_clip(bmp, vp->x1 + vp->x, vp->y1 + vp->y, vp->x2 + vp->x, vp->y2 + vp->y);

   //
   // draw clipped tiles
   //

   //
   // Horizontal clipping
   //
   loc  = (ty1 * tile_map.w) + tx2;
   loc2 = (ty2 * tile_map.w) + tx2;
   y1   = (ty1 * tile_map.tile_size) - vp->dy + vp->y;
   y2   = (ty2 * tile_map.tile_size) - vp->dy + vp->y;
   xx   = (tx2 * tile_map.tile_size) - vp->dx + vp->x;

   for(x = (tx2 - tx1); x >= 0; x--)
   {
      if(((tile  = tile_map.info_plane[loc])  > 0) && (tile  < info_tile_gfx_count))
         draw_rle_sprite(bmp, (RLE_SPRITE *)info_tile_gfx[tile].dat,  xx, y1);

      if(((tile2 = tile_map.info_plane[loc2]) > 0) && (tile2 < info_tile_gfx_count))
         draw_rle_sprite(bmp, (RLE_SPRITE *)info_tile_gfx[tile2].dat, xx, y2);

      xx -= tile_map.tile_size;
      loc--;
      loc2--;
   }
   //
   // Vertical clipping
   //
   loc  = (ty2 * tile_map.w) + tx1;
   loc2 = (ty2 * tile_map.w) + tx2;
   x1   = (tx1 * tile_map.tile_size) - vp->dx + vp->x;
   x2   = (tx2 * tile_map.tile_size) - vp->dx + vp->x;
   yy   = (ty2 * tile_map.tile_size) - vp->dy + vp->y;

   for(y = (ty2 - ty1); y >= 0; y--)
   {
      if(((tile  = tile_map.info_plane[loc])  > 0) && (tile  < info_tile_gfx_count))
         draw_rle_sprite(bmp, (RLE_SPRITE *)info_tile_gfx[tile].dat,  x1, yy);

      if(((tile2 = tile_map.info_plane[loc2]) > 0) && (tile2 < info_tile_gfx_count))
         draw_rle_sprite(bmp, (RLE_SPRITE *)info_tile_gfx[tile2].dat, x2, yy);

      yy   -= tile_map.tile_size;
      loc  -= tile_map.w;
      loc2 -= tile_map.w;
   }

   // that's for speed
   //set_clip(bmp, 0, 0, 0, 0);

   //
   // draw tiles, that are not clipped
   //
   tx1++;
   ty1++;
   loc = (ty1 * tile_map.w) + tx1;
   yy  = (ty1 * tile_map.tile_size) - vp->dy + vp->y;

   for(y = (ty2 - ty1); y > 0; y--)
   {
      xx = (tx1 * tile_map.tile_size) - vp->dx + vp->x;

      for(x = (tx2 - tx1); x > 0; x--)
      {
         if(((tile = tile_map.info_plane[loc]) > 0) && (tile < info_tile_gfx_count))
            draw_rle_sprite(bmp, (RLE_SPRITE *)info_tile_gfx[tile].dat, xx, yy);

         xx += tile_map.tile_size;
         loc++;
      }

      yy  += tile_map.tile_size;
      loc += tile_map.w - tx2 + tx1;
   }

   // set clipping rectangle back to original
   set_clip(bmp, cx1, cy1, cx2, cy2);
}

