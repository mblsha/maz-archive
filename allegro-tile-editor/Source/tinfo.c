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
 *   Routines for TINFO Editing are here
 *
 * CHANGES:
 *
 *   04.01.02
 *      Creation date
 */
 
#include "ated.h"

extern int tinfo_current_plane;
//int     tinfo_current_info;

char tinfo_edit_val[32];

static DIALOG TINFO_EDIT_VAL[] =
{
    /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
    { box,            0,            0, 100,  70,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { edit_text,      5,           25,  90,  20,   0,   0,     0,D_EXIT,      32,           0,    tinfo_edit_val,            NULL,  NULL  },
    { button,        25,           45,  50,  20,   0,   0,    'o',D_EXIT,      0,           0,        "&OK",                 NULL,  NULL  },
    { label,         50,            1,   0,   0,   0,   0,     0,     0,     TRUE,TEXT_OUTLINE_2,     "",                    NULL,  NULL  },
    { label,         50,           10,   0,   0,   0,   0,     0,     0,     TRUE,TEXT_OUTLINE_2,     "Enter value:",        NULL,  NULL  },
    { NULL,           0,            0,   0,   0,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};


char *tinfo_info_getter(int index, int *list_size);

char tinfo_edit_int[32];
char tinfo_edit_hex[32];

DIALOG TINFO_EDIT[] =
{
   // (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)               (dp2) (dp3)
   { box,           0,   0,   320, 200, 0,   0,   0,    0,      0,   0,   NULL,              NULL, NULL },
   { button,        130, 175, 80,  20,  0,   0,   'd',  D_EXIT, 0,   0,   "&Done",           NULL, NULL },
   { listbox,       10,  25,  115, 145, 0,   0,   0,    D_EXIT, 0,   0,   tinfo_info_getter, NULL, NULL },
   { label,         10,  10,  115, 10,  0,   0,   0,    0,      0,   0,   "Select TINFO:",   NULL, NULL },
   { button,        135, 120, 175, 20,  0,   0,   0,    D_EXIT, 0,   0,   tinfo_edit_int,    NULL, NULL },
   { button,        135, 145, 175, 20,  0,   0,   0,    D_EXIT, 0,   0,   tinfo_edit_hex,    NULL, NULL },
   { rle_icon,      135, 25,  35,  30,  0,   0,   0,    0,      TRUE,0,   NULL,              NULL, NULL },
   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,              NULL, NULL }
};

#define TINFOEDIT_DONE             1
#define TINFOEDIT_LIST             2
#define TINFOEDIT_INT              4
#define TINFOEDIT_HEX              5
#define TINFOEDIT_TILE             6

int tinfo_edit_layer = 0;

static RLE_SPRITE *tinfo_edit_tile_getter (int tile)
{
   switch (tinfo_current_plane)
   {
      case TINFO_BACK:
         return (RLE_SPRITE *)gfx_t [ tile ].dat;
         break;
         
      case TINFO_FORE:
         return (RLE_SPRITE *)gfx_tm [ tile ].dat;
         break;
         
      case TINFO_INFO:
         return (RLE_SPRITE *)gfx_i [ tile ].dat;
         break;
   }

   return 0;
}

static int tinfo_edit_getter (int tile)
{
   if (tinfo_edit_layer <= -1)
      return 0;

   switch (tinfo_current_plane)
   {
      case TINFO_BACK:
         return tinfo_back[tinfo_edit_layer].info[tile];
         break;
         
      case TINFO_FORE:
         return tinfo_fore[tinfo_edit_layer].info[tile];
         break;
         
      case TINFO_INFO:
         return tinfo_info[tinfo_edit_layer].info[tile];
         break;
   }

   return 0;
}

//          it can be "setter" :-)
static void tinfo_edit_putter (int tile, unsigned val)
{
   if (tinfo_edit_layer <= -1)
      return;
      
   switch (tinfo_current_plane)
   {
      case TINFO_BACK:
         tinfo_back[tinfo_edit_layer].info[tile] = val;
         break;
         
      case TINFO_FORE:
         tinfo_fore[tinfo_edit_layer].info[tile] = val;
         break;
         
      case TINFO_INFO:
         tinfo_info[tinfo_edit_layer].info[tile] = val;
         break;
   }
}

static void tinfo_edit_update (int tile)
{
   unsigned val;
   
   val = tinfo_edit_getter (tile);

   itoa (val, tinfo_edit_int, 10);
   itoa (val, tinfo_edit_hex, 16);
   strcpy (tinfo_edit_hex, strupr(tinfo_edit_hex));

   TINFO_EDIT[TINFOEDIT_TILE].dp = tinfo_edit_tile_getter (tile);
}

void do_tinfo_edit_tile (unsigned tile)
{
   int ret;

   tinfo_edit_update (tile);

   centre_dialog (TINFO_EDIT);
   
   while ((ret = do_dialog (TINFO_EDIT, -1)) != TINFOEDIT_DONE)
   {
      switch (ret)
      {
         case TINFOEDIT_LIST:
            tinfo_edit_layer = TINFO_EDIT[TINFOEDIT_LIST].d1;
            tinfo_edit_update (tile);
            break;
            
         case TINFOEDIT_INT:
            centre_dialog(TINFO_EDIT_VAL);
            strcpy (tinfo_edit_val, tinfo_edit_int);
            if (do_dialog(TINFO_EDIT_VAL, 1) == -1)
               return;
               
            if (!strlen(tinfo_edit_val))
               return;

            tinfo_edit_putter (tile, atoi (tinfo_edit_val));
            tinfo_edit_update (tile);
            break;
            
         case TINFOEDIT_HEX:
            centre_dialog(TINFO_EDIT_VAL);
            strcpy (tinfo_edit_val, tinfo_edit_hex);
            if (do_dialog(TINFO_EDIT_VAL, 1) == -1)
               return;
               
            if (!strlen(tinfo_edit_val))
               return;
               
            tinfo_edit_putter (tile, strtol (tinfo_edit_val, NULL, 16));
            tinfo_edit_update (tile);
            break;
      }
   }
}

void do_tinfo_edit_map (int x, int y)
{
   unsigned loc;
   unsigned tile;
   int count = 0, i;

   if(((abs(planeton) + abs(planemon) + abs(planeion)) > 1))
   {
      ated_alert("I will only allow TINFO Editing",
                 "one plane at a time; you have",
                 "more than one plane selected.", "&OK", 0, 0, 0);
      return;
   }
   else if ((abs(planeton) + abs(planemon) + abs(planeion)) <= 0)
   {
      return;
   }
   
   loc = (y * mapwidth) + x;

   if      (planeton)
   {
      tinfo_current_plane = TINFO_BACK;
      tile = MapBkgnd  [loc];
   }
   else if (planemon)
   {
      tinfo_current_plane = TINFO_FORE;
      tile = MapFrgnd  [loc];
   }
   else if (planeion)
   {
      tinfo_current_plane = TINFO_INFO;
      tile = MapInfoPl [loc];
   }

   for (i = 0; i < MAX_TINFO; i++)
   {
      switch (tinfo_current_plane)
      {
         case TINFO_BACK:
            if(!strlen(tinfo_back[i].name))
               continue;
               
            count++;
            return;

         case TINFO_FORE:
            if(!strlen(tinfo_fore[i].name))
               continue;
               
            count++;
            return;

         case TINFO_INFO:
            if(!strlen(tinfo_info[i].name))
               continue;
               
            count++;
            return;
      }
   }

   if (!count)
   {
      ated_alert("No TINFO layers",
                 "on selected plane.",
                 "Please make some.", "&OK", 0, 0, 0);
      return;
   }
   
   while (mouse_b);

   do_tinfo_edit_tile (tile);
}

