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

TILEINFO tinfo_back[MAX_TINFO];
TILEINFO tinfo_fore[MAX_TINFO];
TILEINFO tinfo_info[MAX_TINFO];

void destroy_tinfo()
{
   int i;

   for (i = 0; i < MAX_TINFO; i++)
   {
      if (strlen(tinfo_back[i].name))
      {
         free (tinfo_back[i].info);
         tinfo_back[i].info = NULL;
         strcpy (tinfo_back[i].name, "");
      }

      if (strlen(tinfo_fore[i].name))
      {
         free (tinfo_fore[i].info);
         tinfo_fore[i].info = NULL;
         strcpy (tinfo_fore[i].name, "");
      }

      if (strlen(tinfo_info[i].name))
      {
         free (tinfo_info[i].info);
         tinfo_info[i].info = NULL;
         strcpy (tinfo_info[i].name, "");
      }
   }
}

/*
==============================================================================
create_planes

Internal function for creating tile map planes.
It can allocate and free memory.
Returns 0 on success.
==============================================================================
*/
int create_planes(int back, int fore, int info)
{
   if(back)
   {
      //
      // Allocate memory...
      //
      if(!tile_map.back_plane)
      {
         tile_map.back_plane = (unsigned *)calloc(tile_map.w * (tile_map.h+2), sizeof(unsigned));
         if(tile_map.back_plane == NULL)
         {
            ustrcpy(ated_error, get_config_text("Out of memory for back plane!"));
            return 1;
         }
      }
   }
   else
   {
      //
      // free..
      //
      if(tile_map.back_plane)
      {
         free(tile_map.back_plane);
         tile_map.back_plane = NULL;
      }
   }

   if(fore)
   {
      //
      // so on...
      //
      if(!tile_map.fore_plane)
      {
         tile_map.fore_plane = (unsigned *)calloc(tile_map.w * (tile_map.h+2), sizeof(unsigned));
         if(tile_map.fore_plane == NULL)
         {
            ustrcpy(ated_error, get_config_text("Out of memory for fore plane!"));
            return 2;
         }
      }
   }
   else
   {
      if(tile_map.fore_plane)
      {
         free(tile_map.fore_plane);
         tile_map.fore_plane = NULL;
      }
   }

   if(info)
   {
      if(!tile_map.info_plane)
      {
         tile_map.info_plane = (unsigned *)calloc(tile_map.w * (tile_map.h+2), sizeof(unsigned));
         if(tile_map.info_plane == NULL)
         {
            ustrcpy(ated_error, get_config_text("Out of memory for info plane!"));
            return 3;
         }
      }
   }
   else
   {
      if(tile_map.info_plane)
      {
         free(tile_map.info_plane);
         tile_map.info_plane = NULL;
      }
   }

   return 0;
}

/*
==============================================================================
create_tile_map

Resets all the tilemap data
==============================================================================
*/
void create_tile_map()
{
   create_planes (FALSE, FALSE, FALSE);

   strcpy (tile_map.name, "");
   tile_map.w = tile_map.h = 0;
   tile_map.back_plane = tile_map.fore_plane = tile_map.info_plane = NULL;
}

/*
==============================================================================
destroy_tile_map

Frees all the memory, tile map occupies
Returns 0 on success.
==============================================================================
*/
void destroy_tile_map()
{
   create_tile_map ();
}

/*
==============================================================================
check_tile_map

When using different graphics for one map, current tile gfx
possible can contain lesser tiles, so errors may occure.
This function will fix it.
==============================================================================
*/
void check_tile_map()
{
   int x, y;
   unsigned loc;

   for(y = 0; y < tile_map.h; y++)
   {
      for(x = 0; x < tile_map.w; x++)
      {
         loc = (y * tile_map.w) + x;

         if(tile_map.back)
         {
            if(!(tile_map.back_plane[loc] < back_tile_gfx_count))
               tile_map.back_plane[loc] = 0;
         }
         if(tile_map.fore)
         {
            if(!(tile_map.fore_plane[loc] < fore_tile_gfx_count))
               tile_map.fore_plane[loc] = 0;
         }
      }
   }
}

/*
==============================================================================
get_background_tile

Returns value of the back plane
==============================================================================
*/
int get_background_tile (int x, int y)
{
   return tile_map.back_plane[(y * tile_map.w) + x];
}

/*
==============================================================================
get_foreground_tile

Returns value of the fore plane
==============================================================================
*/
int get_foreground_tile (int x, int y)
{
   return tile_map.fore_plane[(y * tile_map.w) + x];
}

/*
==============================================================================
get_info_plane_tile

Returns value of the back plane
==============================================================================
*/
unsigned get_info_plane_tile (int x, int y)
{
   return tile_map.info_plane[(y * tile_map.w) + x];
}

/*
==============================================================================
set_background_tile

Sets value to the back plane
==============================================================================
*/
void set_background_tile (int x, int y, int val)
{
   tile_map.back_plane[(y * tile_map.w) + x] = val;
}

/*
==============================================================================
set_foreground_tile

Sets value to the fore plane
==============================================================================
*/
void set_foreground_tile (int x, int y, int val)
{
   tile_map.fore_plane[(y * tile_map.w) + x] = val;
}

/*
==============================================================================
set_info_plane_tile

Sets value to the back plane
==============================================================================
*/
void set_info_plane_tile (int x, int y, unsigned val)
{
   tile_map.info_plane[(y * tile_map.w) + x] = val;
}
