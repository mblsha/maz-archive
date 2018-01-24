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
 *   Code to handle key presses is here
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 */

#include "ated.h"

void check_map_edges()
{
   if (mapwidth < screenw)
      xbase = 0;

   if (mapheight < screenh)
      ybase = 0;

   if (xbase < 0)
   {
      xbase = 0;
   }
   else if (xbase > (mapwidth - screenw))
   {
      xbase = mapwidth - screenw;
   }

   if (ybase < 0)
   {
      ybase = 0;
   }
   else if (ybase > (mapheight - screenh))
   {
      ybase = mapheight - screenh;
   }
}

/*----------------------------------------------------------------------------------------*/
int check_for_keys_proc(int msg, DIALOG *d, int c) {
  int x, y;

  if(key[KEY_SPACE])
  {
    //
    // Tile Select
    //
    Item_SelectTile();
    wait_for_keys();
    return D_REDRAW;
  }

  if(key[KEY_ESC])
  {
    //
    // Clear modes & ask for quit
    //
    if(Item_ESC() == D_EXIT) {
      wait_for_keys();
      return D_EXIT;
    }
    wait_for_keys();
    return D_REDRAW;
  }

  if(key[KEY_ENTER])
  {
    if(SelectMode)
    {
      //
      // Copy selected area into cut buffer
      //
      SelectMode = FALSE;

      if((SelX1 != -1) && (SelY1 != -1) && (SelX2 != -1) && (SelY2 != -1)) {
        //
        // Copy selected area into cut buffer
        //
        for(y = SelY1; y <= SelY2; y++) {
          for(x = SelX1; x <= SelX2; x++) {
            if(planeton)
              CutBkgnd[(y*mapwidth) + x]  = MapBkgnd[(y*mapwidth) + x];
            if(planemon)
              CutFrgnd[(y*mapwidth) + x]  = MapFrgnd[(y*mapwidth) + x];
            if(planeion)
              CutInfoPl[(y*mapwidth) + x] = MapInfoPl[(y*mapwidth) + x];
          }
        }
        TileCopy.MapOrTileSelect = 0;
        TileCopy.back = planeton;
        TileCopy.fore = planemon;
        TileCopy.info = planeion;
        TileCopy.x = SelX1;
        TileCopy.y = SelY1;
        TileCopy.w = SelX2 - SelX1 + 1;
        TileCopy.h = SelY2 - SelY1 + 1;

        SelX1 = SelY1 = SelX2 = SelY2 = -1;
      }
    }
    else if(BfillMode)
    {
      //
      // Fill selected area
      //
      BfillMode = FALSE;

      if((SelX1 != -1) && (SelY1 != -1) && (SelX2 != -1) && (SelY2 != -1)) {
        //
        // Flood fill the area
        //
        for(y = SelY1; y <= SelY2; y++) {
          for(x = SelX1; x <= SelX2; x++) {
            if(MapBkgnd && planeton)
              MapBkgnd[(y*mapwidth) + x] = which_t;
            if(MapFrgnd && planemon)
              MapFrgnd[(y*mapwidth) + x] = which_tm;
            if(MapInfoPl && planeion)
              MapInfoPl[(y*mapwidth) + x] = which_i;
          }
        }

        UndoRegion.x = SelX1;
        UndoRegion.y = SelY1;
        UndoRegion.w = SelX2 - SelX1 + 1;
        UndoRegion.h = SelY2 - SelY1 + 1;

        DirtyFlag = 1;

        SelX1 = SelY1 = SelX2 = SelY2 = -1;
      }
    }
    else
    {
      //
      // Enter infoplane value
      //
      Item_InputInfoplane();
    }

    while(key[KEY_ENTER]);
    return D_REDRAW;
  }

  return D_O_K;
}
/*----------------------------------------------------------------------------------------*/
int toggle_layers_proc(int msg, DIALOG *d, int c) {
  int update = FALSE;

  if(msg != MSG_IDLE)
    return D_O_K;

  if(key[KEY_1] || key[KEY_2] || key[KEY_3] || key[KEY_4] || key[KEY_5] || key[KEY_6])
    update = TRUE;

  //
  // Edit planes
  //
  if(key[KEY_1])
    planeton = planeton ? FALSE : (viewton ? (MapBkgnd ? TRUE : FALSE) : FALSE);
  if(key[KEY_2])
    planemon = planemon ? FALSE : (viewmon ? (MapFrgnd ? TRUE : FALSE) : FALSE);
  if(key[KEY_3])
    planeion = planeion ? FALSE : (viewion ? (MapInfoPl ? TRUE : FALSE) : FALSE);

  //
  // View planes
  //
  if(key[KEY_4])
  {
    viewton = viewton ? FALSE : (MapBkgnd ? TRUE : FALSE);
    if(!viewton)
      planeton = FALSE;
  }
  if(key[KEY_5])
  {
    viewmon = viewmon ? FALSE : (MapFrgnd ? TRUE : FALSE);
    if(!viewmon)
      planemon = FALSE;
  }
  if(key[KEY_6])
  {
    viewion = viewion ? FALSE : (MapInfoPl ? TRUE : FALSE);
    if(!viewion)
      planeion = FALSE;
  }

  if(update)
  {
    broadcast_dialog_message(MSG_UPDATE_INFO, 0);
    while(key[KEY_1] || key[KEY_2] || key[KEY_3] || key[KEY_4] || key[KEY_5] || key[KEY_6]);
  }

  return D_O_K;
}
/*----------------------------------------------------------------------------------------*/
int scroll_map_proc(int msg, DIALOG *d, int c) {
  int scroll       = FALSE;
  int left_scroll  = FALSE;
  int right_scroll = FALSE;
  int up_scroll    = FALSE;
  int down_scroll  = FALSE;
  int max_x = (mapwidth  - screenw);
  int max_y = (mapheight - screenh);
  int dx, dy;

  if(msg != MSG_IDLE)
    return D_O_K;

  if(key[KEY_LEFT])  left_scroll  = TRUE;
  if(key[KEY_RIGHT]) right_scroll = TRUE;
  if(key[KEY_UP])    up_scroll    = TRUE;
  if(key[KEY_DOWN])  down_scroll  = TRUE;

  if(key[KEY_LEFT] || key[KEY_RIGHT] || key[KEY_UP] || key[KEY_DOWN])
    scroll = TRUE;

  if((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && (key[KEY_ALT] || key[KEY_ALTGR]))
  {
    //
    // Scroll to the edges
    //
    dx = dy = 0;
    if(left_scroll)  xbase = 0;
    if(right_scroll) xbase = max_x;
    if(up_scroll)    ybase = 0;
    if(down_scroll)  ybase = max_y;
  }
  else if(key[KEY_LCONTROL] || key[KEY_RCONTROL])
  {
    //
    // Screen scrolling
    //
    dx = screenw;
    dy = screenh;
  }
  else
  {
    //
    // Usual scrolling
    //
    dx = dy = 1;
  }

  if(left_scroll)  xbase -= dx;
  if(right_scroll) xbase += dx;
  if(up_scroll)    ybase -= dy;
  if(down_scroll)  ybase += dy;

  if(scroll)
  {
    check_map_edges();
    broadcast_dialog_message(MSG_UPDATE_INFO, 0);
  }

  if(key[KEY_LSHIFT] || key[KEY_RSHIFT] || key[KEY_LCONTROL] || key[KEY_RCONTROL])
  {
    //
    // Wait to release arrow keys
    //
    while(key[KEY_LEFT] || key[KEY_RIGHT] || key[KEY_UP] || key[KEY_DOWN]);
  }

  return D_O_K;
}

