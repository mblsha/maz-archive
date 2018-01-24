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

char            *ated_error;
TILE_MAP        tile_map;
TILE_SET        tile_set;

DATAFILE *tile_gfx;
DATAFILE *back_tile_gfx;
DATAFILE *fore_tile_gfx;
DATAFILE *info_tile_gfx;

tiletype_t tiletype;

BITMAP **back_tile_bmp = NULL;
BITMAP **fore_tile_bmp = NULL;
BITMAP **info_tile_bmp = NULL;
//RLE_SPRITE **back_tile_rle;
//RLE_SPRITE **fore_tile_rle;
//RLE_SPRITE **info_tile_rle;

unsigned int    back_tile_gfx_count;
unsigned int    fore_tile_gfx_count;
unsigned int    info_tile_gfx_count;

/*
==============================================================================
load_tile_map

Load tile map from specified file, and initializes
internal variables. Returns 0 on success.
==============================================================================
*/
int load_tile_map(char *filename)
{
   int             b, f, i;
   int             tmp;
   unsigned        size;
   char            name[32];
   PACKFILE        *df;
   ATED_MAP_HEADER header;

   if(!(df = pack_fopen(filename, "rp")))
   {
      ustrcpy(ated_error, get_config_text("Could not open tile map file"));
      return 1;
   }

   create_tile_map();

   //
   // Loading header
   //
   pack_fread(&(header), sizeof(header), df);

   if(strcmp(header.id, "ATED"))
   {
      pack_fclose(df);
      ustrcpy(ated_error, get_config_text("Unknown tile map file format"));
      return 2;
   }

   /*if(head.v > 1)
   {
      pack_fclose(df);
      ustrcpy(ated_error, get_config_text("Not compatible with newer versions"));
      return 3;
   }*/
   tile_map.w = header.w;
   tile_map.h = header.h;

   strcpy(tile_map.name, header.name);

   //
   // Loading planes
   //
   pack_fread(&(b), sizeof(b), df);
   pack_fread(&(f), sizeof(f), df);
   pack_fread(&(i), sizeof(i), df);

   if(create_planes(b, f, i))
      return 4;

   if(b) pack_fread(tile_map.back_plane, sizeof(unsigned) * tile_map.w * tile_map.h, df);
   if(f) pack_fread(tile_map.fore_plane, sizeof(unsigned) * tile_map.w * tile_map.h, df);
   if(i) pack_fread(tile_map.info_plane, sizeof(unsigned) * tile_map.w * tile_map.h, df);

//
// FIXME: Move this data to tilesets!!!
//
   for (i = 0; i < MAX_TINFO; i++)
   {
      if(strlen(tinfo_back[i].name))
      {
         strcpy(tinfo_back[i].name, "");
         free(tinfo_back[i].info);
      }
      if(strlen(tinfo_fore[i].name))
      {
         strcpy(tinfo_fore[i].name, "");
         free(tinfo_fore[i].info);
      }
      if(strlen(tinfo_info[i].name))
      {
         strcpy(tinfo_info[i].name, "");
         free(tinfo_info[i].info);
      }
   }

   b = f = i = 0;
   while (!pack_feof(df))
   {
      pack_fread(&name, sizeof(name), df);
      pack_fread(&(tmp), sizeof(tmp), df);
      pack_fread(&(size), sizeof(size), df);

      switch (tmp)
      {
         case TINFO_BACK:
            tinfo_back[b].info = (unsigned *)calloc (size, sizeof(unsigned));
            pack_fread (tinfo_back[b].info, sizeof(unsigned) * size, df);
            strcpy (tinfo_back[b].name, name);
            tinfo_back[i].num = size;
            b++;
            break;

         case TINFO_FORE:
            tinfo_fore[f].info = (unsigned *)calloc (size, sizeof(unsigned));
            pack_fread (tinfo_fore[f].info, sizeof(unsigned) * size, df);
            strcpy (tinfo_fore[f].name, name);
            tinfo_fore[i].num = size;
            f++;
            break;

         case TINFO_INFO:
            tinfo_info[i].info = (unsigned *)calloc (size, sizeof(unsigned));
            pack_fread (tinfo_info[i].info, sizeof(unsigned) * size, df);
            strcpy (tinfo_info[i].name, name);
            tinfo_info[i].num = size;
            i++;
            break;
      }
   }

   //
   // That's all
   //
   pack_fclose(df);

   tile_map.back = b;
   tile_map.fore = f;
   tile_map.info = i;

   return 0;
}

/*
==============================================================================
unload_tile_map

Equivalent for destroy_tile_map function
==============================================================================
*/
void unload_tile_map()
{
   destroy_tile_map();
}


/*
==============================================================================

Image color depth conversion routines

==============================================================================
*/
/*
BITMAP *convert_bitmap(BITMAP *bitmap)
{
   int     source_depth;
   int     target_depth;
   BITMAP  *source_bmp, *target_bmp;
   PALETTE pal;

   get_palette(pal);

   source_depth = bitmap->vtable->color_depth;
   target_depth = screen->vtable->color_depth;

   if(source_depth == target_depth)
   {
      target_bmp = create_bitmap (bitmap->w,  bitmap->h);
      draw_sprite                (target_bmp, bitmap, 0, 0);
      return target_bmp;
   }

   source_bmp = create_bitmap_ex (source_depth, bitmap->w, bitmap->h);
   clear_to_color                (source_bmp, bitmap->vtable->mask_color);
   draw_sprite                   (source_bmp, bitmap, 0, 0);

   target_bmp = create_bitmap_ex (target_depth, bitmap->w, bitmap->h);

   pal[0].r = 63;
   pal[0].g = 0;
   pal[0].b = 63;

   select_palette(pal);
   blit(source_bmp, target_bmp, 0, 0, 0, 0, bitmap->w, bitmap->h);
   unselect_palette();

   destroy_bitmap (source_bmp);

   return target_bmp;//bitmap;
}

void convert_datafile_bitmap(DATAFILE *dat)
{
   int     source_depth;
   int     target_depth;
   BITMAP  *source_bmp, *target_bmp;
   BITMAP  *bitmap;
   PALETTE pal;

   bitmap = dat->dat;

   get_palette(pal);

   source_depth = bitmap->vtable->color_depth;
   target_depth = screen->vtable->color_depth;

   if(source_depth == target_depth)
   {
      return;
   }

   source_bmp = create_bitmap_ex (source_depth, bitmap->w, bitmap->h);
   clear_to_color                (source_bmp, bitmap->vtable->mask_color);
   draw_sprite                   (source_bmp, bitmap, 0, 0);

   target_bmp = create_bitmap_ex (target_depth, bitmap->w, bitmap->h);

   pal[0].r = 63;
   pal[0].g = 0;
   pal[0].b = 63;

   select_palette(pal);
   blit(source_bmp, target_bmp, 0, 0, 0, 0, bitmap->w, bitmap->h);
   unselect_palette();

   source_bmp = bitmap;

   dat->dat = target_bmp;

   destroy_bitmap (source_bmp);
}

void convert_bitmaps_in_datafile(DATAFILE *dat)
{
   int i;
   DATAFILE *d;
   DATAFILE *nested;

   for(i = 0; dat[i].type != DAT_END; i++)
   {
      d = dat + i;

      if(dat[i].type == DAT_BITMAP)
      {
         convert_datafile_bitmap(d);
      }
      else if(dat[i].type == DAT_FILE)
      {
         nested = dat[i].dat;
         convert_bitmaps_in_datafile(nested);
      }
   }
}

RLE_SPRITE *convert_rle_sprite(RLE_SPRITE *sprite)
{
   int     source_depth;
   int     target_depth;
   BITMAP  *source_bmp, *target_bmp;
   PALETTE pal;
   RLE_SPRITE *temp;

   get_palette(pal);

   source_depth = sprite->color_depth;
   target_depth = screen->vtable->color_depth;

   if(source_depth == target_depth)
   {
      target_bmp = create_bitmap (sprite->w,  sprite->h);
      draw_rle_sprite            (target_bmp, sprite, 0, 0);
      return sprite;
   }

   source_bmp = create_bitmap_ex (source_depth, sprite->w, sprite->h);
   clear_to_color                (source_bmp, source_bmp->vtable->mask_color);
   draw_rle_sprite               (source_bmp, sprite, 0, 0);

   target_bmp = create_bitmap_ex (target_depth, sprite->w, sprite->h);

   pal[0].r = 63;
   pal[0].g = 0;
   pal[0].b = 63;

   select_palette(pal);
   blit(source_bmp, target_bmp, 0, 0, 0, 0, sprite->w, sprite->h);
   unselect_palette();

   temp = get_rle_sprite(target_bmp);

   destroy_bitmap (source_bmp);
   destroy_bitmap (target_bmp);

   return temp;
}

void convert_datafile_rle_sprite(DATAFILE *dat)
{
   int     source_depth;
   int     target_depth;
   BITMAP  *source_bmp, *target_bmp;
   PALETTE pal;
   RLE_SPRITE *sprite;// *temp;

   get_palette(pal);

   sprite = dat->dat;

   source_depth = sprite->color_depth;
   target_depth = screen->vtable->color_depth;

   if(source_depth == target_depth)
   {
      return;
   }

   source_bmp = create_bitmap_ex (source_depth, sprite->w, sprite->h);
   clear_to_color                (source_bmp, source_bmp->vtable->mask_color);
   draw_rle_sprite               (source_bmp, sprite, 0, 0);

   target_bmp = create_bitmap_ex (target_depth, sprite->w, sprite->h);

   pal[0].r = 63;
   pal[0].g = 0;
   pal[0].b = 63;

   select_palette(pal);
   blit(source_bmp, target_bmp, 0, 0, 0, 0, sprite->w, sprite->h);
   unselect_palette();

   dat->dat = get_rle_sprite(target_bmp);

   destroy_rle_sprite (sprite);
   destroy_bitmap     (source_bmp);
   destroy_bitmap     (target_bmp);
}

void convert_rle_sprites_in_datafile(DATAFILE *dat)
{
   int i;
   DATAFILE *d;
   DATAFILE *nested;

   for(i = 0; dat[i].type != DAT_END; i++)
   {
      d = dat + i;

      if(dat[i].type == DAT_RLE_SPRITE)
      {
         convert_datafile_rle_sprite(d);
      }
      else if(dat[i].type == DAT_FILE)
      {
         nested = dat[i].dat;
         convert_rle_sprites_in_datafile(nested);
      }
   }
}

void convert_images_in_datafile(DATAFILE *dat)
{
   convert_bitmaps_in_datafile     (dat);
   convert_rle_sprites_in_datafile (dat);
}
*/

static BITMAP *make_bitmap (int w, int h)
{
   BITMAP *bmp = NULL;

   bmp = create_video_bitmap (w, h);

   if (!bmp)
   {
      bmp = create_bitmap (w, h);
   }

   clear_to_color (bmp, screen->vtable->mask_color);

   return bmp;
}

/*
==============================================================================
load_tile_gfx

Load GFX datafile and initialize some variables
Returns 0 on succeed, and error code, if fails
==============================================================================
*/
int load_tile_gfx(char *filename)
{
   RLE_SPRITE  *tile;
   unsigned int i;

   unload_tile_gfx ();
   
   if (!(tile_gfx = load_datafile(filename)))
   {
      ustrcpy(ated_error, get_config_text("Could not open tile gfx file"));
      return 1;
   }

   //
   // Load palette
   //
   if (tile_gfx[3].type == DAT_PALETTE)
   {
      //set_palette ((RGB *)tile_gfx[3].dat);

      //
      // Reload datafile
      //
      //unload_datafile (tile_gfx);
      //tile_gfx = load_datafile (filename);
   }

   //
   // Convert tiles' color depths
   //
   convert_images_in_datafile (tile_gfx);

   if (!(back_tile_gfx = (DATAFILE *)tile_gfx[0].dat)) return 2;
   if (!(fore_tile_gfx = (DATAFILE *)tile_gfx[1].dat)) return 3;
   if (!(info_tile_gfx = (DATAFILE *)tile_gfx[2].dat)) return 4;

   //
   // Count tiles
   //
   back_tile_gfx_count = fore_tile_gfx_count = info_tile_gfx_count = 0;
   while (back_tile_gfx[back_tile_gfx_count++].type != DAT_END);
   while (fore_tile_gfx[fore_tile_gfx_count++].type != DAT_END);
   while (info_tile_gfx[info_tile_gfx_count++].type != DAT_END);

   //
   // Set TileSize
   //
   tile = (RLE_SPRITE *)back_tile_gfx[0].dat;
   if (!(tile_map.tile_size = tile->w))
   {
      tile = (RLE_SPRITE *)fore_tile_gfx[0].dat;
      if (!(tile_map.tile_size = tile->w))
      {
         tile = (RLE_SPRITE *)info_tile_gfx[0].dat;
         if (!(tile_map.tile_size = tile->w))
         {
            tile_map.tile_size = 0;
            ustrcpy(ated_error, get_config_text("Could not determine what tile size that tile gfx file uses"));
            return 5;
         }
      }
   }

   if (gfx_capabilities & GFX_HW_SYS_TO_VRAM_BLIT_MASKED)
   {
      back_tile_bmp = (BITMAP **)malloc ((sizeof (BITMAP)) * (back_tile_gfx_count + 2));
      
      for (i = 0; i < back_tile_gfx_count-1; i++)
      {
         back_tile_bmp[i+1] = NULL;
         back_tile_bmp[i] = make_bitmap (tile_map.tile_size, tile_map.tile_size);
         draw_rle_sprite (back_tile_bmp[i], (RLE_SPRITE *)back_tile_gfx[i].dat, 0, 0);
      }
      
      fore_tile_bmp = (BITMAP **)malloc ((sizeof (BITMAP)) * (fore_tile_gfx_count + 2));
      
      for (i = 0; i < fore_tile_gfx_count-1; i++)
      {
         fore_tile_bmp[i+1] = NULL;
         fore_tile_bmp[i] = make_bitmap (tile_map.tile_size, tile_map.tile_size);
         draw_rle_sprite (fore_tile_bmp[i], (RLE_SPRITE *)fore_tile_gfx[i].dat, 0, 0);
      }
      
      info_tile_bmp = (BITMAP **)malloc ((sizeof (BITMAP)) * (info_tile_gfx_count + 2));
      
      for (i = 0; i < info_tile_gfx_count-1; i++)
      {
         info_tile_bmp[i+1] = NULL;
         info_tile_bmp[i] = make_bitmap (tile_map.tile_size, tile_map.tile_size);
         draw_rle_sprite (info_tile_bmp[i], (RLE_SPRITE *)info_tile_gfx[i].dat, 0, 0);
      }
      
      tiletype = tiletype_bitmap;
   }
   else
   {
      tiletype = tiletype_rle_sprite;
   }
   
   return 0;
}

void unload_tile_gfx(void)
{
   int i;
   
   if (back_tile_bmp)
   {
      for (i = 0; back_tile_bmp[i]; i++)
         destroy_bitmap (back_tile_bmp[i]);
         
      free (back_tile_bmp);
      back_tile_bmp[i] = NULL;
   }
   if (fore_tile_bmp)
   {
      for (i = 0; fore_tile_bmp[i]; i++)
         destroy_bitmap (fore_tile_bmp[i]);
         
      free (fore_tile_bmp);
      fore_tile_bmp[i] = NULL;
   }
   if (info_tile_bmp)
   {
      for (i = 0; info_tile_bmp[i]; i++)
         destroy_bitmap (info_tile_bmp[i]);
         
      free (info_tile_bmp);
      info_tile_bmp[i] = NULL;
   }

   if (tile_gfx)
   {
      unload_datafile(tile_gfx);

      tile_gfx = back_tile_gfx = fore_tile_gfx = info_tile_gfx = NULL;
      back_tile_gfx_count = fore_tile_gfx_count = info_tile_gfx_count = 0;
   }
}


