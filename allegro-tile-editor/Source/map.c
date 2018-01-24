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
 *   Undo routines and flood filling code
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 */

#include "ated.h"

void DoFloodFill(int mx, int my) 
{
   #define NUMSPORES    50000
   int obx[NUMSPORES], oby[NUMSPORES];

   int i, j, k;
   int width, height;
   int used, highest;
   int org_t, org_m, org_i;
   int Ton, Mon, Ion;
   int nx, ny, new_t, new_m, new_i;
   int copy_fill = FALSE;

   unsigned ptr, newoff;
   int vectx[4] = {0, 1, 0, -1}, vecty[4]={-1, 0, 1, 0};

   if ((mouse_b & 2) || key[KEY_LCONTROL] || key[KEY_RCONTROL]) 
   {
      copy_fill = TRUE;
   }

   if (copy_fill && !TileCopy.x && !TileCopy.y && !TileCopy.w && !TileCopy.h) 
   {
      ated_alert ("Hey!","I can't use CopyFill mode", "without the source!", "Oops", 0, 0, 0);
      FillMode = 0;
      return;
   }

   if (((abs(planeton) + abs(planemon) + abs(planeion)) > 1)) 
   {
      ated_alert("I will only allow Flood Filling",
                 "one plane at a time; you have",
                 "more than one plane selected.", "&OK", 0, 0, 0);
      return;
   }

   for (i = 0; i < NUMSPORES; i++)
      obx[i] = oby[i] = -1;

   obx[0] = mx;
   oby[0] = my;

   newoff = ptr = (oby[0]*mapwidth) + obx[0];

   width  = mapwidth;
   height = mapheight;
   used = 1;
   highest = 1;

   if ((obx[0] < 0) || (oby[0] < 0) || (obx[0] > width) || (oby[0] > height)) 
   {
      FillMode = 0;
      return;
   }

   Ton = planeton;
   Mon = planemon;
   Ion = planeion;

   org_t = MapBkgnd [ptr];
   org_m = MapFrgnd [ptr];
   org_i = MapInfoPl[ptr];

   if (((Ton ? (which_t  == org_t) : 0) ||
        (Mon ? (which_tm == org_m) : 0) ||
        (Ion ? (which_i  == org_i) : 0)) && !copy_fill) 
   {
      FillMode = 0;
      return;
   }

   set_mouse_sprite(busy_pointer);
   busy_mouse = TRUE;

   CopyUndoRegion();
   UndoRegion.x = UndoRegion.y = 0;
   UndoRegion.w = mapwidth;
   UndoRegion.h = mapheight;

   if (copy_fill) 
   {
      unsigned from = (TileCopy.y + (my % TileCopy.h))*mapwidth + TileCopy.x + (mx % TileCopy.w);

      Ton = (TileCopy.back && planeton);
      Mon = (TileCopy.fore && planemon);
      Ion = (TileCopy.info && planeion);

      if (TileCopy.MapOrTileSelect) 
      {
         //
         // From TileSelect
         //
         unsigned loc = (TileCopy.y * TileCopy.tile_select_w) + TileCopy.x;

         if(Ton) 
         {
            new_t = (loc < gfx_t_count) ? loc : 0;
         }
         if(Mon) 
         {
            new_m = (loc < gfx_tm_count) ? loc : 0;
         }
         if(Ion) 
         {
            new_i = (loc < gfx_i_count) ? loc : 0;
         }
      }
      else 
      {
         //
         // From Copy Buffer
         //
         new_t = CutBkgnd [from];
         new_m = CutFrgnd [from];
         new_i = CutInfoPl[from];
      }
   } 
   else 
   {
      new_t = which_t;
      new_m = which_tm;
      new_i = which_i;
   }

   if (Ton) MapBkgnd [newoff] = new_t;
   if (Mon) MapFrgnd [newoff] = new_m;
   if (Ion) MapInfoPl[newoff] = new_i;

   do 
   { // while(used)
      for (i = 0;i <= highest; i++) 
      {
         //
         // SEE IF SPORE EXISTS
         //
         if (obx[i] != -1) 
         {
            //
            // FREE THIS SPORE
            //
            mx = obx[i];
            my = oby[i];
            obx[i] = -1;
            used--;

            //
            // SEARCH 4 QUADRANTS FOR A SPORE TO FILL
            // (ONLY 4 QUADS SO WE DON'T FILL THRU DIAGONALS)
            //
            if (!copy_fill) 
            {
               //
               // Normal Fill mode
               //
               for (j = 0; j < 4; j++) 
               {
                  newoff=(my + vecty[j])*mapwidth + mx + vectx[j];

                  if ((Ton ? (MapBkgnd [newoff] == org_t) : 1) &&
                      (Mon ? (MapFrgnd [newoff] == org_m) : 1) &&
                      (Ion ? (MapInfoPl[newoff] == org_i) : 1)) 
                  {

                     for (k = 0; k < NUMSPORES; k++) 
                     {
                        if (obx[k] == -1) 
                        {
                           obx[k] = mx + vectx[j];
                           oby[k] = my + vecty[j];

                           if ((obx[k] < 0) || (obx[k] > width) || (oby[k] < 0) || (oby[k] > height)) 
                           {
                              obx[k] = -1;
                              break;
                           }

                           used++;

                           if (Ton) MapBkgnd [newoff] = which_t;
                           if (Mon) MapFrgnd [newoff] = which_tm;
                           if (Ion) MapInfoPl[newoff] = which_i;
   
		                     DirtyFlag=1;

		                     if (k > highest) 
                           {
		                        highest = k;
		                        break;
                           }
                        } // if (obx[k] == -1)
	                  } // for (k)

	                  if (key[KEY_ESC])    // ESC OUT
                     {  
	                     while(key[KEY_ESC]);

	                     goto done;
	                  }
	               } // if
               } // for (j)
            } 
            else 
            {
               //
               // Copy Fill mode
               //
	            for (j = 0; j < 4; j++) 
               {
	               unsigned from;

	               ny = my + vecty[j];
	               nx = mx + vectx[j];

                  newoff = (ny*mapwidth) + nx;

                  if ((Ton ? (MapBkgnd [newoff] == org_t) : 1) &&
                      (Mon ? (MapFrgnd [newoff] == org_m) : 1) &&
                      (Ion ? (MapInfoPl[newoff] == org_i) : 1)) 
                  {
                     for (k = 0; k < NUMSPORES; k++) 
                     {
                        if (obx[k] == -1) 
                        {
                           obx[k] = nx;
                           oby[k] = ny;

                           if ((obx[k] < 0) || (obx[k] > width) || (oby[k] < 0) || (oby[k] > height)) 
                           {
                              obx[k] = -1;
                              break;
                           }

                           from = (TileCopy.y + (ny % TileCopy.h))*mapwidth + TileCopy.x + (nx % TileCopy.w);

                           Ton = TileCopy.back;
                           Mon = TileCopy.fore;
                           Ion = TileCopy.info;

                           if(!TileCopy.MapOrTileSelect) 
                           {
                              //
                              // From Map
                              //
                              new_t = CutBkgnd[from];
                              new_m = CutFrgnd[from];
                              new_i = CutInfoPl[from];
                           }
                           else 
                           {
                              //
                              // From TileSelect
                              //
                              unsigned loc = ((ny%TileCopy.h)+TileCopy.y)*selectcols+TileCopy.x+(nx%TileCopy.w);

                              if(Ton) new_t = (loc < gfx_t_count)  ? loc : 0;
                              if(Mon) new_m = (loc < gfx_tm_count) ? loc : 0;
                              if(Ion) new_i = (loc < gfx_i_count)  ? loc : 0;
                           }

                           if (((!new_t && !F3_flag) || new_t) && viewton && Ton) MapBkgnd [newoff] = new_t;
                           if (((!new_m && !F3_flag) || new_m) && viewmon && Mon) MapFrgnd [newoff] = new_m;
                           if (((!new_i && !F3_flag) || new_i) && viewion && Ion) MapInfoPl[newoff] = new_i;

                           used++;

                           DirtyFlag = 1;

                           if(k > highest) 
                           {
                              highest = k;
		                        break;
                           }
	                     } // if (obx[k] == -1)

	                     if (key[KEY_ESC])    // ESC OUT
                        {    
   	                     while(key[KEY_ESC]);
	                        goto done;
	                     }
                     } // for (k)
	               } // if
	            } // for (j)
            } // if / else (copy_fill)
         } // if (obx[i] == -1)
      } // for (i)
   } while(used);

done:

   set_mouse_sprite(mouse_pointer);
   busy_mouse = FALSE;

   FillMode = 0;
}

void RestoreUndo(void) {
  unsigned x, y, loc;

  for (y = UndoRegion.y; y < UndoRegion.y + UndoRegion.h; y++)  {
    for(x = UndoRegion.x; x < UndoRegion.x + UndoRegion.w; x++) {
      loc = (y * mapwidth) + x;

      if(MapBkgnd)  MapBkgnd [loc] = undoB[loc];
      if(MapFrgnd)  MapFrgnd [loc] = undoF[loc];
      if(MapInfoPl) MapInfoPl[loc] = undoI[loc];
    }
  }
}

void CopyUndoRegion() 
{
   if(UndoRegion.x == -1)
      return;

   SaveUndo(UndoRegion.x, UndoRegion.y, UndoRegion.w, UndoRegion.h);
}

void SaveUndo(int xx, int yy, int w, int h) 
{
   unsigned x, y, loc;

   for (y = yy; y < (yy + h); y++)  
   {
      for(x = xx; x < (xx + w); x++) 
      {
         loc = (y * mapwidth) + x;

         if(MapBkgnd)  undoB[loc] = MapBkgnd [loc];
         if(MapFrgnd)  undoF[loc] = MapFrgnd [loc];
         if(MapInfoPl) undoI[loc] = MapInfoPl[loc];
      }
   }
}
