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
 *   Routines available from MENU are here
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 *
 *   29.10.01 - v0.2.0
 *     Added "View Map & Goto", "Review Map & Goto" and "Map Info"
 *     do_viewmap(), Item_ViewMap(), Item_ReviewMap(), Item_MapStats()
 *
 *     Doing TILEINFO adding, deleting and renaming -- Item_AddDelTinfo()
 */

#include "ated.h"
extern DIALOG ATED_MAIN[];


char map_size_x [10];
char map_size_y [10];
char map_planes [255];
char num_back   [10];
char num_fore   [10];
char num_icons  [10];
char map_size   [10];


static void ResetModes()
{
   SelectMode  = FALSE;
   TsearchMode = FALSE;
   BfillMode   = FALSE;
   PasteMode   = FALSE;
   FillMode    = FALSE;
   TINFOMode   = FALSE;
}

int skins_count;
INDEX_LIST skin_list[1000];

char *skin_list_proc (int index, int *list_size)
{
   if(index < 0)
   {
      *list_size = skins_count;
      return NULL;
   }

   return skin_list[index].name;
}

int Item_Skin()
{
   DIALOG SKIN[] = {
      /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
      { background,     0,            0, 320, 200,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
      { box,            0,            0, 320, 200,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
      { listbox,        5,           15, 310, 155,   0,   0,     0,D_EXIT,       0,           0,         skin_list_proc,       NULL,  NULL  },
      { label,          5,            1,   0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,       "Skins:",             NULL,  NULL  },
      { button,       145,          175,  50,  20,   0,   0,   'd',D_EXIT,       0,           0,         "&Done",              NULL,  NULL  },
      { NULL,           0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
   };

   skins_count = skin_build_list (skin_list, &SKIN[2].d1);
   centre_dialog (SKIN);

   while (do_dialog (SKIN, -1) != 4)
   {
      strcpy (skin_file, skin_list[SKIN[2].d1].name);
      update_skin ();
   }

   strcpy (skin_file, skin_list[SKIN[2].d1].name);
   update_skin ();

   return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_About() {
  DIALOG ABOUT[] = {
    /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
    /*
    { box,            0,            0, 170, 120,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { icon,           5,            5,   0,   0,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { label,         50,           50,   0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,       "(c) &M&A&Zsoft 2000-2001",          NULL,  NULL  },
    { button,       105,           85,  50,  20,   0,   0,    'd',     D_EXIT,  0,           0,         "&Done",             NULL,          NULL },
    //{ label,          0,            0, 170, 100,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { NULL,           0,            0,   0,NULL,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
    */
    { box,            0,            0, 122, 154,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { icon,           1,            1,   0,   0,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { label,          4,          140,   0,   0,white,makecol(200,200,200),     0,     0,       0,TEXT_OUTLINE_4,       "(c) &M&A&Zsoft 2000-2002",          NULL,  NULL  },
    { button,        68,          120,  50,  20,   0,   0,    'd',     D_EXIT,  0,           0,         "&Done",             NULL,          NULL },
    /*
    { box,            0,            0, 184, 173,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { icon,           1,            1,   0,   0,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { label,          5,          155,   0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,       "(c) &M&A&Zsoft 2000-2001",          NULL,  NULL  },
    { button,       128,            5,  50,  20,   0,   0,    'd',     D_EXIT,  0,           0,         "&Done",             NULL,          NULL },
    */
    //{ label,          0,            0, 170, 100,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { NULL,           0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
  };
  ABOUT[1].dp = ateddata[LOGO].dat;
  centre_dialog(ABOUT);
  do_ated_dialog(ABOUT, -1);
  //alert(APP_FULL_NAME" by Mike Pichagin", __DATE__, "(C)MAZsoft 2000", "OK", NULL, NULL, NULL);
  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_SelectVideoMode() {
  int c;

  if(ated_gfx_mode_select_ex(&c, &screen_w, &screen_h, &screen_bpp)) {
    set_color_depth(screen_bpp);

    if(set_gfx_mode(c, screen_w, screen_h, 0, 0))
      fat_error(allegro_error);
  }
  show_mouse(screen);
  //SEND_MESSAGE_TO_ALL(&ATED_MAIN, MSG_SCREEN_RESIZE, 0, 2);

  init_colors();

  broadcast_dialog_message(MSG_SCREEN_RESIZE, 0);
  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_Launch() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_ProjectReSelect() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_VisitDOS() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_ChangeIconRows() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_Huffman() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_Pass() {

  return D_O_K;
}
/*----------------------------------------------------------------------------------------*/
int Item_Quit()
{
   if(ated_alert(NULL,"Quit - Are you sure?",NULL,"&Yes","&No", 'y', 'n') == 1)
   {
      if(DirtyFlag)
      {
         switch(ated_alert3("The map has been modified.","Do you want to SAVE it","before exiting "APP_NAME"?","Yes","No","Cancel", 0, 0, 0))
         {
            case 1:
               Item_SaveATEDtilesetFile();
               break;
               
            case 3:
               return D_REDRAW;
         }
      }
      
      return D_CLOSE;
   }
   return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_LastMap() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_EditTinfoValues() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_LAUNCHname() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_PARMstring() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/


TILEINFO tinfo_back[MAX_TINFO];
TILEINFO tinfo_fore[MAX_TINFO];
TILEINFO tinfo_info[MAX_TINFO];

#define MAX_PLANES      3

int tinfo_current_plane;

typedef struct CHAR
{
   char *str;
} CHAR;

static char *tinfo_planes_getter(int index, int *list_size)
{
   if (index < 0)
   {
      *list_size = !(!MapBkgnd) + !(!MapFrgnd) + !(!MapInfoPl);

      return NULL;
   }

   if (MapBkgnd  && index == 0)
      return "Background";
   if (MapFrgnd  && index <= 1)
      return "Foreground";
   if (MapInfoPl && index <= 2)
      return "Infoplane";

   return "Oops, its just bug";
}

void update_tinfo_current_plane();

char *tinfo_info_getter(int index, int *list_size)
{
   int  i, j;

   //update_tinfo_current_plane();

   if (index < 0)
   {
      for(j = 0, i = 0; i < MAX_TINFO; i++)
      {
         switch(tinfo_current_plane)
         {
            case TINFO_BACK:
               if (strlen(tinfo_back[i].name))
                  j++;
               break;

            case TINFO_FORE:
               if (strlen(tinfo_fore[i].name))
                  j++;
               break;

            case TINFO_INFO:
               if (strlen(tinfo_info[i].name))
                  j++;
               break;
         }
      }

      *list_size = j;

      return NULL;
   }

   for(j = 0, i = 0; i < MAX_TINFO; i++)
   {
      switch (tinfo_current_plane)
      {
         case TINFO_BACK:
            if(strlen(tinfo_back[i].name))
            {
               if (j++ == index)
                  return tinfo_back[i].name;
            }
            break;

         case TINFO_FORE:
            if(strlen(tinfo_fore[i].name))
            {
               if (j++ == index)
                  return tinfo_fore[i].name;
            }
            break;

         case TINFO_INFO:
            if(strlen(tinfo_info[i].name))
            {
               if (j++ == index)
                  return tinfo_info[i].name;
            }
            break;
      }
   }

   return NULL;
}

#include "adddeltinfo_d.h"

#define ADDDELTINFO_DONE               1
#define ADDDELTINFO_ADD                2
#define ADDDELTINFO_REMOVE             3
#define ADDDELTINFO_RENAME             4
#define ADDDELTINFO_PLANES             5
#define ADDDELTINFO_INFO               6

#define ADDTINFO_EDIT                   1

char tinfo_name[32];

static DIALOG ADD_TINFO[] =
{
    /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
    { box,            0,            0, 100,  70,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { edit_text,      5,           25,  90,  20,   0,   0,     0,D_EXIT,      32,           0,    tinfo_name,                NULL,  NULL  },
    { button,        25,           45,  50,  20,   0,   0,    'o',D_EXIT,      0,           0,        "&OK",                 NULL,  NULL  },
    { label,         50,            1,   0,   0,   0,   0,     0,     0,     TRUE,TEXT_OUTLINE_2,     "",                    NULL,  NULL  },
    { label,         50,           10,   0,   0,   0,   0,     0,     0,     TRUE,TEXT_OUTLINE_2,     "Name the TINFO:",     NULL,  NULL  },
    { NULL,           0,            0,   0,   0,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};

void add_tinfo()
{
   int i;

   update_tinfo_current_plane();

   if (tinfo_current_plane == -1)
   {
      ated_alert (NULL, "Doubleclick on the planes listbox first!", NULL, "I'll remember to do that", NULL, 0, 0);
      return;
   }

   strcpy (tinfo_name, "");

   centre_dialog(ADD_TINFO);

   if (do_dialog(ADD_TINFO, -1) == -1)
      return;

   for (i = 0; i < MAX_TINFO; i++)
   {
      switch (tinfo_current_plane)
      {
         case TINFO_BACK:
            if(strlen(tinfo_back[i].name))
               continue;

            strcpy(tinfo_back[i].name, ADD_TINFO[ADDTINFO_EDIT].dp);
            tinfo_back[i].num  = gfx_t_count;
            tinfo_back[i].info = calloc(gfx_t_count, sizeof(unsigned));
            return;

         case TINFO_FORE:
            if(strlen(tinfo_fore[i].name))
               continue;

            strcpy(tinfo_fore[i].name, ADD_TINFO[ADDTINFO_EDIT].dp);
            tinfo_fore[i].num  = gfx_tm_count;
            tinfo_fore[i].info = calloc(gfx_tm_count, sizeof(unsigned));
            return;

         case TINFO_INFO:
            if(strlen(tinfo_info[i].name))
               continue;

            strcpy(tinfo_info[i].name, ADD_TINFO[ADDTINFO_EDIT].dp);
            tinfo_info[i].num  = gfx_i_count;
            tinfo_info[i].info = calloc(gfx_i_count, sizeof(unsigned));
            return;
      }
   }
}

void remove_tinfo()
{
   int i, counter, index;

   index = ADDDEL_TINFO[ADDDELTINFO_INFO].d1;

   update_tinfo_current_plane();

   if (tinfo_current_plane > -1 && index > -1)
   {
      if (ated_alert ("Are you sure, you want", "delete that TILEINFO?", NULL, "Sure", "Oops!", 0, 0) == 1)
      {
         for (counter = 0, i = 0; i < MAX_TINFO; i++)
         {
            switch (tinfo_current_plane)
            {
               case TINFO_BACK:
                  if (!strlen(tinfo_back[i].name))
                     continue;
                  if (counter++ != index)
                     continue;

                  strcpy(tinfo_back[i].name, "");
                  if(tinfo_back[i].info)
                  {
                     free(tinfo_back[i].info);
                     tinfo_back[i].info = NULL;
                  }
                  return;

               case TINFO_FORE:
                  if (!strlen(tinfo_fore[i].name))
                     continue;
                  if (counter++ != index)
                     continue;

                  strcpy(tinfo_fore[i].name, "");
                  if(tinfo_fore[i].info)
                  {
                     free(tinfo_fore[i].info);
                     tinfo_fore[i].info = NULL;
                  }
                  return;

               case TINFO_INFO:
                  if (!strlen(tinfo_info[i].name))
                     continue;
                  if (counter++ != index)
                     continue;

                  strcpy(tinfo_info[i].name, "");
                  if(tinfo_info[i].info)
                  {
                     free(tinfo_info[i].info);
                     tinfo_info[i].info = NULL;
                  }
                  return;
            }
         }
      }
   }
   else if (tinfo_current_plane == -1)
   {
      ated_alert (NULL, "Doubleclick on the planes listbox first!", NULL, "I will remember that", 0, 0, 0);
   }
}

void update_tinfo_current_plane()
{
   tinfo_current_plane = ADDDEL_TINFO[ADDDELTINFO_PLANES].d1 + 1;

   if (!MapBkgnd)
      tinfo_current_plane++;

   if (!MapFrgnd)
      tinfo_current_plane++;

   if (!MapInfoPl)
      tinfo_current_plane++;
}

void rename_tinfo()
{
   int i, counter, index;

   index = ADDDEL_TINFO[ADDDELTINFO_INFO].d1;

   update_tinfo_current_plane();

   if (!(tinfo_current_plane > -1 && index > -1))
      return;

   for (counter = 0, i = 0; i < MAX_TINFO; i++)
   {
      switch (tinfo_current_plane)
      {
         case TINFO_BACK:
            if (!strlen(tinfo_back[i].name))
               continue;
            if (counter++ != index)
               continue;

            strcpy(tinfo_name, tinfo_back[i].name);
            break;

         case TINFO_FORE:
            if (!strlen(tinfo_fore[i].name))
               continue;
            if (counter++ != index)
               continue;

            strcpy(tinfo_name, tinfo_fore[i].name);
            break;

         case TINFO_INFO:
            if (!strlen(tinfo_info[i].name))
               continue;
            if (counter++ != index)
               continue;

            strcpy(tinfo_name, tinfo_info[i].name);
            break;
      }
   }
   centre_dialog(ADD_TINFO);

   if (do_dialog(ADD_TINFO, -1) == -1)
      return;

   if (!strlen(tinfo_name))
      return;

   for (counter = 0, i = 0; i < MAX_TINFO; i++)
   {
      switch (tinfo_current_plane)
      {
         case TINFO_BACK:
            if (!strlen(tinfo_back[i].name))
               continue;
            if (counter++ != index)
               continue;

            strcpy(tinfo_back[i].name, tinfo_name);
            return;

         case TINFO_FORE:
            if (!strlen(tinfo_fore[i].name))
               continue;
            if (counter++ != index)
               continue;

            strcpy(tinfo_fore[i].name, tinfo_name);
            return;

         case TINFO_INFO:
            if (!strlen(tinfo_info[i].name))
               continue;
            if (counter++ != index)
               continue;

            strcpy(tinfo_info[i].name, tinfo_name);
            return;
      }
   }
}

int Item_AddDelTinfo()
{
   int ret;

   tinfo_current_plane = -1;

   update_tinfo_current_plane();
   
   centre_dialog(ADDDEL_TINFO);

   while ((ret = do_dialog(ADDDEL_TINFO, -1)) != ADDDELTINFO_DONE)
   {
      switch (ret)
      {
         case ADDDELTINFO_ADD:
            add_tinfo();
            break;

         case ADDDELTINFO_REMOVE:
            remove_tinfo();
            break;

         case ADDDELTINFO_RENAME:
         case ADDDELTINFO_INFO:
            rename_tinfo();
            break;

         case ADDDELTINFO_PLANES:
            update_tinfo_current_plane();
            break;
      }
   }

   return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_EditMapNames() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
char val_size[10];

static DIALOG CHANGE_MAP_EDGES[] =
{
    /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
    { box,            0,            0, 100, 130,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { button,        40,           30,  20,  18,   0,   0,    'u',    D_EXIT,  0,           0,        "&U",                  NULL,  NULL  },
    { button,        40,           50,  20,  18,   0,   0,    'd',    D_EXIT,  0,           0,        "&D",                  NULL,  NULL  },
    { button,        17,           40,  20,  18,   0,   0,    'l',    D_EXIT,  0,           0,        "&L",                  NULL,  NULL  },
    { button,        63,           40,  20,  18,   0,   0,    'r',    D_EXIT,  0,           0,        "&R",                  NULL,  NULL  },
    { button,        25,          105,  50,  20,   0,   0,    'x',    D_EXIT,  0,           0,        "E&xit",               NULL,  NULL  },
    { label,         50,           75,   0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,    map_size_x,              NULL,  NULL  },
    { label,         50,           85,   0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,    map_size_y,              NULL,  NULL  },
    { label,          5,           75,   0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,      "Width:",              NULL,  NULL  },
    { label,          5,           85,   0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,      "Height:",             NULL,  NULL  },
    { label,         50,            1,   0,   0,   0,   0,     0,     0,   TRUE,TEXT_OUTLINE_2,       "Pick map edge",       NULL,  NULL  },
    { label,         50,           10,   0,   0,   0,   0,     0,     0,   TRUE,TEXT_OUTLINE_2,       "to change",           NULL,  NULL  },
    { NULL,           0,            0,   0,   0,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};

static DIALOG EDIT_MAP_EDGES[] =
{
    /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
    { box,            0,            0, 100,  60,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { edit_text,      5,           15,  90,  20,   0,   0,     0,     0,       9,           0,   val_size,                 NULL,  NULL  },
    { button,        25,           35,  50,  20,   0,   0,    'o',D_EXIT,      0,           0,        "&OK",                 NULL,  NULL  },
    { label,         50,            1,   0,   0,   0,   0,     0,     0,     TRUE,TEXT_OUTLINE_2,     "+ or - value",        NULL,  NULL  },
    { NULL,           0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};
#define EDGE_UP         1
#define EDGE_DOWN       2
#define EDGE_LEFT       3
#define EDGE_RIGHT      4

#define BUTT_EXIT       5

#define MENU_SELECT_EDGE        1
#define MENU_CHANGE_EDGE        2

int Item_EditMapEdges()
{
   int ret = -1, menu = -1, action = -1;
   int val, newwidth, newheight;//, tempwidth, tempheight;
   unsigned *tempB, *tempF, *tempI;
   unsigned x, y;//, dx, dy, obx, oby;
   int b, f, i;
   char *bp;
   DIALOG *d;

   centre_dialog(CHANGE_MAP_EDGES);
   centre_dialog(EDIT_MAP_EDGES);

   b = (MapBkgnd  != NULL);
   f = (MapFrgnd  != NULL);
   i = (MapInfoPl != NULL);

   while(ret)
   {
      if(menu == MENU_CHANGE_EDGE)
         val = strtoul(val_size, &bp, 0);

      if(action)
      {
         set_mouse_sprite(busy_pointer);
         busy_mouse = TRUE;
      }

      switch(action)
      {
         case EDGE_UP:
            newheight = mapheight + val;

            if((newheight) <= 0)
            {
               set_mouse_sprite(mouse_pointer);
               busy_mouse = FALSE;
               ated_alert("","Incorrect map height!","","I promise to change it",NULL,0,0);
               return D_REDRAW;
            }

            if(b)
               if(!(tempB = calloc(mapwidth * newheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
            if(f)
               if(!(tempF = calloc(mapwidth * newheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
            if(i)
               if(!(tempI = calloc(mapwidth * newheight, sizeof(unsigned))))
                  fat_error("Out of memory!");

            if(val < 0)
            {
               //
               // Copy maps into temp memory
               //
               for(y = abs(val)/*0*/; y < (mapheight) /*+ val)*/; y++)
               {
                  for(x = 0; x < mapwidth; x++)
                  {
                     if(b)
                        tempB[((y+val)*mapwidth) + x] = MapBkgnd[((y)/* - val)*/*mapwidth)  + x];
                     if(f)
                        tempF[((y+val)*mapwidth) + x] = MapFrgnd[((y)/* - val)*/*mapwidth)  + x];
                     if(i)
                        tempI[((y+val)*mapwidth) + x] = MapInfoPl[((y)/* - val)*/*mapwidth) + x];
                  }
               }
            }
            else
            {
               for(y = 0; y < (mapheight + val); y++)
               {
                  for(x = 0; x < mapwidth; x++)
                  {
                     if(y < val)
                     {
                        if(b)
                           tempB[(y*mapwidth) + x] = which_t;
                        if(f)
                           tempF[(y*mapwidth) + x] = which_tm;
                        if(i)
                           tempI[(y*mapwidth) + x] = which_i;
                     }
                     else
                     {
                        if(b)
                           tempB[(y*mapwidth) + x] = MapBkgnd[((y - val)*mapwidth)  + x];
                        if(f)
                           tempF[(y*mapwidth) + x] = MapFrgnd[((y - val)*mapwidth)  + x];
                        if(i)
                           tempI[(y*mapwidth) + x] = MapInfoPl[((y - val)*mapwidth) + x];
                     }
                  }
               }
            }
            //
            // Free & reallocate memory
            //
            mapheight = newheight;
            create_planes(FALSE, FALSE, FALSE);
            create_planes(b,     f,     i);
            //
            // Copy maps back
            //
            for(y = 0; y < mapheight; y++)
            {
               for(x = 0; x < mapwidth; x++)
               {
                  if(b)
                     MapBkgnd[(y*mapwidth)  + x] = tempB[(y*mapwidth) + x];
                  if(f)
                     MapFrgnd[(y*mapwidth)  + x] = tempF[(y*mapwidth) + x];
                  if(i)
                     MapInfoPl[(y*mapwidth) + x] = tempI[(y*mapwidth) + x];
               }
            }
            //
            // Free temp memory
            //
            if(b)
               free(tempB);
            if(f)
               free(tempF);
            if(i)
               free(tempI);

            DirtyFlag    = TRUE;
            UndoRegion.x = -1;
            SaveUndo(0, 0, mapwidth, mapheight);
            break;

         case EDGE_DOWN:
            newheight = mapheight + val;

            if((newheight) <= 0)
            {
               set_mouse_sprite(mouse_pointer);
               busy_mouse = FALSE;
               ated_alert("","Incorrect map height!","","I promise to change it",NULL,0,0);
               return D_REDRAW;
            }

            if(b)
               if(!(tempB = calloc(mapwidth * newheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
            if(f)
               if(!(tempF = calloc(mapwidth * newheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
            if(i)
               if(!(tempI = calloc(mapwidth * newheight, sizeof(unsigned))))
                  fat_error("Out of memory!");

            if(val < 0)
            {
               //
               // Copy maps into temp memory
               //
               for(y = 0; y < (mapheight + val); y++)
               {
                  for(x = 0; x < mapwidth; x++)
                  {
                     if(b)
                        tempB[(y*mapwidth) + x] = MapBkgnd[(y*mapwidth)  + x];
                     if(f)
                        tempF[(y*mapwidth) + x] = MapFrgnd[(y*mapwidth)  + x];
                     if(i)
                        tempI[(y*mapwidth) + x] = MapInfoPl[(y*mapwidth) + x];
                  }
               }
            }
            else
            {
               for(y = 0; y < (mapheight + val); y++)
               {
                  for(x = 0; x < mapwidth; x++)
                  {
                     if(y >= mapheight)
                     {
                        if(b)
                           tempB[(y*mapwidth) + x] = which_t;
                        if(f)
                           tempF[(y*mapwidth) + x] = which_tm;
                        if(i)
                           tempI[(y*mapwidth) + x] = which_i;
                     }
                     else
                     {
                        if(b)
                           tempB[(y*mapwidth) + x] = MapBkgnd[(y*mapwidth)  + x];
                        if(f)
                           tempF[(y*mapwidth) + x] = MapFrgnd[(y*mapwidth)  + x];
                        if(i)
                           tempI[(y*mapwidth) + x] = MapInfoPl[(y*mapwidth) + x];
                     }
                  }
               }
            }
            //
            // Free & reallocate memory
            //
            mapheight = newheight;
            create_planes(FALSE, FALSE, FALSE);
            create_planes(b,     f,     i);
            //
            // Copy maps back
            //
            for(y = 0; y < mapheight; y++)
            {
               for(x = 0; x < mapwidth; x++)
               {
                  if(b)
                     MapBkgnd[(y*mapwidth) + x] = tempB[(y*mapwidth)  + x];
                  if(f)
                     MapFrgnd[(y*mapwidth) + x] = tempF[(y*mapwidth)  + x];
                  if(i)
                     MapInfoPl[(y*mapwidth) + x] = tempI[(y*mapwidth) + x];
               }
            }
            //
            // Free temp memory
            //
            if(b)
               free(tempB);
            if(f)
               free(tempF);
            if(i)
               free(tempI);

            DirtyFlag    = TRUE;
            UndoRegion.x = -1;
            SaveUndo(0, 0, mapwidth, mapheight);
            break;

         case EDGE_LEFT:
            newwidth = mapwidth + val;

            if((newwidth) <= 0) 
            {
               set_mouse_sprite(mouse_pointer);
               busy_mouse = FALSE;
               ated_alert("","Incorrect map width!","","I promise to change it",NULL,0,0);
               return D_REDRAW;
            }

            if(b)
               if(!(tempB = calloc(newwidth * mapheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
            if(f)
               if(!(tempF = calloc(newwidth * mapheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
            if(i)
               if(!(tempI = calloc(newwidth * mapheight, sizeof(unsigned))))
                  fat_error("Out of memory!");

            if(val < 0) 
            {
               //
               // Copy maps into temp memory
               //
               for(y = 0; y < mapheight; y++) 
               {
                  for(x = abs(val); x < mapwidth; x++) 
                  {
                     if(b)
                        tempB[(y*newwidth) + x + val] = MapBkgnd[(y*mapwidth)  + x];
                     if(f)
                        tempF[(y*newwidth) + x + val] = MapFrgnd[(y*mapwidth)  + x];
                     if(i)
                        tempI[(y*newwidth) + x + val] = MapInfoPl[(y*mapwidth) + x];
                  }
               }
            } 
            else 
            {
               for(y = 0; y < mapheight; y++) 
               {
                  for(x = 0; x < (mapwidth + val); x++) 
                  {
                     if(x < abs(val)) 
                     {
                        if(b)
                           tempB[(y*newwidth) + x] = which_t;
                        if(f)
                           tempF[(y*newwidth) + x] = which_tm;
                        if(i)
                           tempI[(y*newwidth) + x] = which_i;
                     } 
                     else 
                     {
                        if(b)
                           tempB[(y*newwidth) + x] = MapBkgnd[(y*mapwidth)  + x - val];
                        if(f)
                           tempF[(y*newwidth) + x] = MapFrgnd[(y*mapwidth)  + x - val];
                        if(i)
                           tempI[(y*newwidth) + x] = MapInfoPl[(y*mapwidth) + x - val];
                     }
                  }
               }
            }
            //
            // Free & reallocate memory
            //
            mapwidth = newwidth;
            create_planes(FALSE, FALSE, FALSE);
            create_planes(b,     f,     i);
            //
            // Copy maps back
            //
            for(y = 0; y < mapheight; y++) 
            {
               for(x = 0; x < mapwidth; x++) 
               {
                  if(b)
                     MapBkgnd[(y*mapwidth) + x] = tempB[(y*mapwidth)  + x];
                  if(f)
                     MapFrgnd[(y*mapwidth) + x] = tempF[(y*mapwidth)  + x];
                  if(i)
                     MapInfoPl[(y*mapwidth) + x] = tempI[(y*mapwidth) + x];
               }
            }
            //
            // Free temp memory
            //
            if(b)
               free(tempB);
            if(f)
               free(tempF);
            if(i)
               free(tempI);

            DirtyFlag    = TRUE;
            UndoRegion.x = -1;
            SaveUndo(0, 0, mapwidth, mapheight);
            break;

         case EDGE_RIGHT:
            newwidth = mapwidth + val;
   
            if((newwidth) <= 0) 
            {
               set_mouse_sprite(mouse_pointer);
               busy_mouse = FALSE;
               ated_alert("","Incorrect map width!","","I promise to change it",NULL,0,0);
               return D_REDRAW;
            }

            if(b)
               if(!(tempB = calloc(newwidth * mapheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
            if(f)
               if(!(tempF = calloc(newwidth * mapheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
            if(i)
               if(!(tempI = calloc(newwidth * mapheight, sizeof(unsigned))))
                  fat_error("Out of memory!");
   
            if(val < 0) 
            {
               //
               // Copy maps into temp memory
               //
               for(y = 0; y < mapheight; y++) 
               {
                  for(x = 0; x < (mapwidth + val); x++) 
                  {
                     if(b)
                        tempB[(y*newwidth) + x] = MapBkgnd[(y*mapwidth)  + x];
                     if(f)
                        tempF[(y*newwidth) + x] = MapFrgnd[(y*mapwidth)  + x];
                     if(i)
                        tempI[(y*newwidth) + x] = MapInfoPl[(y*mapwidth) + x];
                  }
               }
            }
            else 
            {
               for(y = 0; y < mapheight; y++) 
               {
                  for(x = 0; x < (mapwidth + val); x++) 
                  {
                     if(x >= mapwidth) 
                     {
                        if(b)
                           tempB[(y*newwidth) + x] = which_t;
                        if(f)   
                           tempF[(y*newwidth) + x] = which_tm;
                        if(i)
                           tempI[(y*newwidth) + x] = which_i;
                     }
                     else 
                     {
                        if(b)
                           tempB[(y*newwidth) + x] = MapBkgnd[(y*mapwidth)  + x];
                        if(f)
                           tempF[(y*newwidth) + x] = MapFrgnd[(y*mapwidth)  + x];
                        if(i)
                           tempI[(y*newwidth) + x] = MapInfoPl[(y*mapwidth) + x];
                     }
                  }
               }
            }
            //
            // Free & reallocate memory
            //
            mapwidth = newwidth;
            create_planes(FALSE, FALSE, FALSE);
            create_planes(b,     f,     i);
            //
            // Copy maps back
            //
            for(y = 0; y < mapheight; y++) 
            {
               for(x = 0; x < mapwidth; x++) 
               {
                  if(b)
                     MapBkgnd[(y*mapwidth) + x] = tempB[(y*mapwidth)  + x];
                  if(f)
                     MapFrgnd[(y*mapwidth) + x] = tempF[(y*mapwidth)  + x];
                  if(i)
                     MapInfoPl[(y*mapwidth) + x] = tempI[(y*mapwidth) + x];
               }
            }
            //
            // Free temp memory
            //
            if(b)
               free(tempB);
            if(f)
               free(tempF);
            if(i)
               free(tempI);

            DirtyFlag    = TRUE;
            UndoRegion.x = -1;
            SaveUndo(0, 0, mapwidth, mapheight);
            break;
      }

      if(action) 
      {
         xbase = ybase = 0;

         set_mouse_sprite(mouse_pointer);
         busy_mouse = FALSE;
      }

      switch(menu) 
      {
         case MENU_CHANGE_EDGE:
            action = -1;
            menu = MENU_SELECT_EDGE;
            ret = 0;
            break;

         case MENU_SELECT_EDGE:
            switch(ret) 
            {
               case EDGE_LEFT:
                  menu = MENU_CHANGE_EDGE;
                  action = ret;
                  ret = 0;
                  break;

               case EDGE_RIGHT:
                  menu = MENU_CHANGE_EDGE;
                  action = ret;
                  ret = 0;
                  break;

               case EDGE_UP:
                  menu = MENU_CHANGE_EDGE;
                  action = ret;
                  ret = 0;
                  break;

               case EDGE_DOWN:
                  menu = MENU_CHANGE_EDGE;
                  action = ret;
                  ret = 0;
                  break;

               case BUTT_EXIT:
               case -1:
                  menu = 0;
                  ret = 0;
            }
         break;
      }

      switch(menu) 
      {
         case MENU_CHANGE_EDGE:
            d = EDIT_MAP_EDGES;
            strcpy(val_size, "");
            break;

         case NULL:
            d = NULL;
            break;

         case MENU_SELECT_EDGE:
         default:
            //strcpy(map_size_x, mapwidth_string);
            //strcpy(map_size_y, mapheight_string);
            itoa (mapwidth,  map_size_x, 10);
            itoa (mapheight, map_size_y, 10);
            menu = MENU_SELECT_EDGE;
            d = CHANGE_MAP_EDGES;
            break;
      }

      if(d)
         ret = do_ated_dialog(d, -1);
   }
   return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_Copy() {
  ResetModes();

  SelectMode = TRUE;
  SelX1 = SelY1 = SelX2 = SelY2 = -1;
  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_Paste() {
  ResetModes();

  PasteMode = TRUE;

  if(SnapMode) {
    snapx = -1;
    snapy = -1;
    snapxsize = TileCopy.w;
    snapysize = TileCopy.h;
  }

  px = -1;
  py = -1;

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_BlockFill() {
  ResetModes();

  BfillMode = TRUE;
  SelX1 = SelY1 = SelX2 = SelY2 = -1;
  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_FloodFill() {
  ResetModes();

  FillMode = TRUE;
  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_Undo() {
  if(UndoRegion.x == -1) {
    ated_alert("","You don't have anything to UNDO!","", "Oh, yeah", 0,0,0);
    return D_O_K;
  }

  RestoreUndo();
  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_TileSearch() {
  ResetModes();
  /*
  if((abs(planeton) + abs(planemon) + abs(planeion)) > 1) {
    ated_alert("TILE SEARCH will only work with ONE ",
               "active plane! Make sure that the tile",
               "you're searching for is selected!", "OK", NULL,NULL,NULL);

    return D_O_K;
  }
  */
  TsearchMode = TRUE;
  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_GridMode() {
  GridMode ^= 1;
  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_SnapTog() {
  if(!PasteMode)
    return D_O_K;

  SnapMode ^= 1;

  snapx = -1;
  snapy = -1;
  snapxsize = TileCopy.w;
  snapysize = TileCopy.h;

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_POtog() {
  F3_flag ^= 1;

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
static char str1[10], str2[10], str3[10], str4[10], str5[10], str6[10];

extern DIALOG tile_sel[];

int tile_select_updater(int msg, DIALOG *d, int c) 
{
   //char hexstr[16]="0123456789ABCDEF",str[2]="";
   //char temp[10];

   if((msg == MSG_DRAW) || (msg == MSG_UPDATE_INFO)) 
   {
      vsync();

      drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);
      rectfill(screen, tile_sel[4].x,  tile_sel[4].y-1, tile_sel[4].x + tile_sel[4].w-1, tile_sel[4].y + (tile_sel[5].h * 2), gray);
      rectfill(screen, tile_sel[7].x,  tile_sel[7].y-1, tile_sel[7].x + tile_sel[7].w-1, tile_sel[7].y + (tile_sel[7].h * 2), gray);
      rectfill(screen, tile_sel[10].x, tile_sel[10].y-1, tile_sel[10].x + tile_sel[10].w-1, tile_sel[10].y + (tile_sel[10].h * 2), gray);
      drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

      if((which_t < gfx_t_count) && (which_t > 0)) 
      {
         tile_sel[3].dp = gfx_t[which_t].dat;

         strcpy(tile_sel[4].dp, "");
         strcpy(tile_sel[5].dp, "");

         printhex(screen, which_t, tile_sel[4].x, tile_sel[4].y, black);
         printint(screen, which_t, tile_sel[5].x, tile_sel[5].y, black);
      }
      else 
      {
         tile_sel[3].dp = gfx_t[0].dat;

         strcpy(tile_sel[4].dp, " No");
         strcpy(tile_sel[5].dp, "Tile");
      }

      if((which_tm < gfx_tm_count) && (which_tm > 0)) 
      {
         tile_sel[6].dp = gfx_tm[which_tm].dat;

         strcpy(tile_sel[7].dp, "");
         strcpy(tile_sel[8].dp, "");

         printhex(screen, which_tm, tile_sel[7].x, tile_sel[7].y, black);
         printint(screen, which_tm, tile_sel[8].x, tile_sel[8].y, black);
      }
      else 
      {
         tile_sel[6].dp = gfx_tm[0].dat;

         strcpy(tile_sel[7].dp, " No");
         strcpy(tile_sel[8].dp, "Tile");
      }

      if((which_i < gfx_i_count) && (which_i > 0)) 
      {
         tile_sel[9].dp = gfx_i[which_i].dat;

         strcpy(tile_sel[10].dp, "");
         strcpy(tile_sel[11].dp, "");

         printhex(screen, which_i, tile_sel[10].x, tile_sel[10].y, black);
         printint(screen, which_i, tile_sel[11].x, tile_sel[11].y, black);
      }
      else if((which_i >= gfx_i_count) && (which_i > 0)) 
      {
         tile_sel[9].dp = gfx_i[0].dat;

         strcpy(tile_sel[10].dp, "");
         strcpy(tile_sel[11].dp, "");

         if (which_i <= 0xffff)
         {
            printhex (screen, which_i, tile_sel[10].x, tile_sel[10].y, black);
            printint (screen, which_i, tile_sel[11].x, tile_sel[11].y, black);
         }
         else
         {
            printhex (screen, which_i >> 16,        tile_sel[10].x, tile_sel[10].y, black);
            printhex (screen, which_i & 0x0000ffff, tile_sel[11].x, tile_sel[11].y, black);
         }
      }
      else 
      {
         tile_sel[9].dp = gfx_i[0].dat;

         strcpy(tile_sel[10].dp, " No");
         strcpy(tile_sel[11].dp, "Icon");
      }
   }

   return D_O_K;
}

DIALOG tile_sel[] =
{
   // (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key)    (flags)     (d1)     (d2)     (dp)     (dp2) (dp3)
   { box,               0,    0,    0,    0,    0,    0,    0,       0,          0,       0,       NULL,    NULL, NULL  },
   { tile_select_updater,0,   0,    0,    0,    0,    0,    0,       0,          0,       0,       NULL,    NULL, NULL  },
   { tile_select,       0,    0,    0,    0,    0,    0,    0,       0,          0,       0,       NULL,    NULL, NULL  },
   { rle_icon,          0,    0,    0,    0,    0,    0,    0,       0,       TRUE,       0,       NULL,    NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,       0,          0,TEXT_OUTLINE_4, str1,    NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,       0,          0,TEXT_OUTLINE_4, str2,    NULL, NULL  },
   { rle_icon,          0,    0,    0,    0,    0,    0,    0,       0,       TRUE,       0,       NULL,    NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,       0,          0,TEXT_OUTLINE_4, str3,    NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,       0,          0,TEXT_OUTLINE_4, str4,    NULL, NULL  },
   { rle_icon,          0,    0,    0,    0,    0,    0,    0,       0,       TRUE,       0,       NULL,    NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,       0,          0,TEXT_OUTLINE_4, str5,    NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,       0,          0,TEXT_OUTLINE_4, str6,    NULL, NULL  },
   { button,            0,    0,   50,   15,    0,    0,  't',D_NOFOCUS,      TRUE,       1,   "&Tiles",    NULL, NULL  },
   { button,            0,    0,   50,   15,    0,    0,  'm',D_NOFOCUS,      TRUE,       1,  "&Masked",    NULL, NULL  },
   { button,            0,    0,   50,   15,    0,    0,  'i',D_NOFOCUS,      TRUE,       1,   "&Icons",    NULL, NULL  },
   { button,            0,    0,   50,   15,    0,    0,  'e',D_NOFOCUS,         0,       0,    "&Exit",    NULL, NULL  },
   { d_keyboard_proc,   0,    0,    0,    0,    0,    0,  'g',       0,          0,       0,Item_GridMode,  NULL, NULL  },
   { NULL,              0,    0,    0,    0,    0,    0,    0,       0,          0,       0,       NULL,    NULL, NULL  }
};

int tile_select_w = 0;
int current_tile_select_w = 0;

int Item_SelectTile()
{
   if(!tsize || !gfx_t)
      return D_O_K;

   strcpy(str1, "NoName");
   strcpy(str2, "NoName");
   strcpy(str3, "NoName");
   strcpy(str4, "NoName");
   strcpy(str5, "NoName");
   strcpy(str6, "NoName");

   //
   // box
   //
   tile_sel[0].x = 0;
   tile_sel[0].y = 0;
   tile_sel[0].w = SCREEN_W;
   tile_sel[0].h = SCREEN_H;
   //
   // tile select box
   //
   tile_sel[2].w = 0;//SCREEN_W;
   tile_sel[2].h = 0;//SCREEN_H;

   //while(tile_sel[2].w > (SCREEN_W - 20))
   //  tile_sel[2].w -= tsize;

   while(tile_sel[2].w < (SCREEN_W - 20))
      tile_sel[2].w += tsize;
   tile_sel[2].w -= tsize;

   current_tile_select_w = tile_sel[2].w;

   if (!tile_select_w)
      tile_select_w = tile_sel[2].w;
   else if (tile_select_w < tile_sel[2].w)
      tile_sel[2].w = tile_select_w;

   //while(tile_sel[2].h > (SCREEN_H - (25 + tile_sel[12].h + ((tsize >= 16) ? tsize : 16))))
   //  tile_sel[2].h -= tsize;

   while(tile_sel[2].h < (SCREEN_H - (25 + tile_sel[12].h + ((tsize >= 16) ? tsize : 16))))
      tile_sel[2].h += tsize;
   tile_sel[2].h -= tsize;

   tile_sel[2].d1 = tile_sel[2].w / tsize;
   tile_sel[2].d2 = tile_sel[2].h / tsize;

   tile_sel[2].x = (SCREEN_W - tile_sel[2].w) / 2;
   tile_sel[2].y = ((SCREEN_H - (/*15*/6 + tile_sel[12].h + ((tsize >= 16) ? tsize : 16))) - tile_sel[2].h) / 2;

   //
   // buttons
   //
   tile_sel[12].y = tile_sel[13].y = tile_sel[14].y = tile_sel[15].y = SCREEN_H - (10 + tile_sel[12].h);

   tile_sel[12].x = (SCREEN_W / 2) - (15 + (tile_sel[12].w * 2));
   tile_sel[13].x = (SCREEN_W / 2) - (5  + (tile_sel[12].w));
   tile_sel[14].x = (SCREEN_W / 2) + (5);
   tile_sel[15].x = (SCREEN_W / 2) + (15 + (tile_sel[12].w));

   if ((!(tile_sel[12].flags & D_SELECTED)) &&
       (!(tile_sel[13].flags & D_SELECTED)) &&
       (!(tile_sel[14].flags & D_SELECTED)))
   {
      tile_sel[12].flags ^= D_SELECTED;
   }

   if(!(tile_sel[15].flags & D_EXIT))
   {
      tile_sel[15].flags ^= D_EXIT;
   }

   //
   // icons
   //
   tile_sel[3].w = tile_sel[6].w = tile_sel[9].w = tsize + 48;
   tile_sel[3].d2 = tile_sel[6].d2 = tile_sel[9].d2 = tsize-1;
   tile_sel[3].h = tile_sel[6].h = tile_sel[9].h = (tsize >= 16) ? tsize : 16;
   tile_sel[3].y = tile_sel[6].y = tile_sel[9].y = tile_sel[12].y - (5 + tile_sel[3].h);

   tile_sel[6].x = (SCREEN_W - tile_sel[3].w) / 2;
   tile_sel[3].x = tile_sel[6].x - tile_sel[3].w;
   tile_sel[9].x = tile_sel[6].x + tile_sel[3].w;
   //
   // labels
   //
   tile_sel[4].dp2 = tile_sel[5].dp2 = tile_sel[7].dp2 = tile_sel[8].dp2 =
                     tile_sel[10].dp2 = tile_sel[11].dp2 = f8x8;

   tile_sel[4].h = tile_sel[5].h = tile_sel[7].h = tile_sel[8].h =
                   tile_sel[10].h = tile_sel[11].h = text_height(f8x8);

   tile_sel[4].w = tile_sel[5].w = tile_sel[7].w = tile_sel[8].w =
                   tile_sel[10].w = tile_sel[11].w = 8 * 6;

   tile_sel[4].fg = tile_sel[5].fg = tile_sel[7].fg = tile_sel[8].fg =
                    tile_sel[10].fg = tile_sel[11].fg = white;

   tile_sel[4].y = tile_sel[7].y = tile_sel[10].y = tile_sel[3].y;
   tile_sel[5].y = tile_sel[8].y = tile_sel[11].y = tile_sel[3].y + 8;

   tile_sel[4].x  = tile_sel[5].x  = tile_sel[3].x + tsize;
   tile_sel[7].x  = tile_sel[8].x  = tile_sel[6].x + tsize;
   tile_sel[10].x = tile_sel[11].x = tile_sel[9].x + tsize;
   /*sys_dlg[2].fg = white;
   sys_dlg[2].x = 0;
   sys_dlg[2].y = 0;
   sys_dlg[2].w = sys_dlg[0].w-2;
   sys_dlg[2].h = sys_dlg[0].h-32;

   sys_dlg[3].x = (sys_dlg[0].w-80)/2;
   sys_dlg[3].y = sys_dlg[0].h-24;
   sys_dlg[3].w = 80;
   sys_dlg[3].h = 16;

   sys_dlg[1].d1 = 0;
   sys_dlg[1].d2 = 0;
   sys_dlg[1].dp = stattext;

   sys_dlg[2].d1 = 0;
   sys_dlg[2].d2 = 0;
   sys_dlg[2].dp = stattext;*/

   //centre_dialog(tile_sel);
   //set_dialog_color(sys_dlg, gui_fg_color, gui_bg_color);

   while(key[KEY_SPACE]);

   if(SelectMode)
   {
      SelectMode = FALSE;

      SelX1 = SelY1 = SelX2 = SelY2 = -1;
   }

   do_ated_dialog(tile_sel, -1);

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/

#include "mapstats_d.h"

int Item_MapStats()
{
   unsigned long size;
   unsigned      x, y, b = 0, f = 0, i = 0, j, loc;
   char          *back, *fore;
   char          temp[255];

   strcpy (map_name_string,   map_name);
   itoa   (mapwidth,          map_size_x, 10);
   itoa   (mapheight,         map_size_y, 10);

   sprintf(temp, "%s%s%s", !MapBkgnd ? "" : "BACK ", !MapFrgnd ? "" : "FORE ", !MapInfoPl ? "" : "INFO");
   strcpy (map_planes,        temp);

   back = calloc(gfx_t_count,  sizeof(char));
   fore = calloc(gfx_tm_count, sizeof(char));

   for (x = 0; x < mapwidth; x++)
   {
      for (y = 0; y < mapheight; y++)
      {
         loc = (y * mapheight) + x;

         if(MapBkgnd && MapBkgnd[loc])
         {
            back[MapBkgnd[loc]] = TRUE;
         }

         if(MapFrgnd && MapFrgnd[loc])
         {
            fore[MapFrgnd[loc]] = TRUE;
         }

         if(MapInfoPl && MapInfoPl[loc])
         {
            i++;
         }
      }
   }

   if (MapBkgnd)
      for (j = 0; j < gfx_t_count; j++)
         if (back[j])
            b++;

   if (MapFrgnd)
      for (j = 0; j < gfx_tm_count; j++)
         if (fore[j])
            f++;

   free (back);
   free (fore);

   itoa   (b,                 num_back,  10);
   itoa   (f,                 num_fore,  10);
   itoa   (i,                 num_icons, 10);

   size = 0;

   if (MapBkgnd)
      size += mapwidth * mapheight * sizeof(unsigned);
   if (MapFrgnd)
      size += mapwidth * mapheight * sizeof(unsigned);
   if (MapInfoPl)
      size += mapwidth * mapheight * sizeof(unsigned);

   size /= 1024;

   sprintf(temp, "%ldKB", size);
   strcpy(map_size,          temp);

   centre_dialog(MAP_STATS);

   do_ated_dialog(MAP_STATS, -1);

   return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_ToggleInfo()
{
   infobaron ^= 1;
   broadcast_dialog_message(MSG_SCREEN_RESIZE, 0);

   return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
static char info_val[32];

static DIALOG INPUT_INFOPLANE[] =
{
    /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
    { box,            0,            0, 100,  70,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
    { edit_text,      5,           25,  90,  20,   0,   0,     0,D_EXIT,      30,           0,    info_val,                  NULL,  NULL  },
    { button,        25,           45,  50,  20,   0,   0,    'o',D_EXIT,      0,           0,        "&OK",                 NULL,  NULL  },
    { label,         50,            1,   0,   0,   0,   0,     0,     0,     TRUE,TEXT_OUTLINE_2,     "Enter a value for",   NULL,  NULL  },
    { label,         50,           10,   0,   0,   0,   0,     0,     0,     TRUE,TEXT_OUTLINE_2,     "the INFO plane:",     NULL,  NULL  },
    { NULL,           0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};

int Item_InputInfoplane()
{
   char *bp;
   BITMAP *bmp;

   strcpy(info_val, "");

   centre_dialog(INPUT_INFOPLANE);

   bmp = create_bitmap(INPUT_INFOPLANE[0].w+1, INPUT_INFOPLANE[0].h+1);
   blit(screen, bmp, INPUT_INFOPLANE[0].x, INPUT_INFOPLANE[0].y, 0, 0, INPUT_INFOPLANE[0].w+1, INPUT_INFOPLANE[0].h+1);

   INPUT_INFOPLANE[1].bg = INPUT_INFOPLANE[2].fg = INPUT_INFOPLANE[3].fg = INPUT_INFOPLANE[4].fg = white;
   INPUT_INFOPLANE[2].bg = INPUT_INFOPLANE[3].bg = INPUT_INFOPLANE[4].bg = gray;
   INPUT_INFOPLANE[1].fg = black;

   if(do_ated_dialog(INPUT_INFOPLANE, 1) != -1)
   {
      which_i = strtoul(info_val, &bp, 0);
   }

   blit(bmp, screen, 0, 0, INPUT_INFOPLANE[0].x, INPUT_INFOPLANE[0].y, INPUT_INFOPLANE[0].w+1, INPUT_INFOPLANE[0].h+1);
   destroy_bitmap(bmp);

   wait_for_keys();
   broadcast_dialog_message(MSG_UPDATE_INFO, 0);

   return D_O_K; //D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
typedef struct VIEWMAP
{
   BITMAP       *bmp;

   int          width, height;                  // in pixels

   float        tile_width, tile_height;        // in pixels per tile, if 0.5 then its
                                                // 2 tiles per pixel
} VIEWMAP;

VIEWMAP viewmap;

static void viewmap_helper (BITMAP *dest, int x1, int y1, int x2, int y2)
{
   int          x, y, w, h, mw, mh;
   unsigned     loc, tile_t, tile_m, tile_i;
   BITMAP       *bmp;

   w = x2 - x1;
   h = y2 - y1;

   if (w <= 0 || h <= 0)
   {
      char buf  [64];
      char buf2 [64];
      
      set_mouse_sprite(mouse_pointer);
      busy_mouse = FALSE;
      
      sprintf (buf,  "x1 = %d; y1 = %d; x2 = %d; y2 = %d", x1, y1, x2, y2);
      sprintf (buf2, "mapwidth = %d; mapheight = %d", mapwidth, mapheight);
      
      ated_alert("This is very strange, please send this values to developer:", buf, buf2, "Please, don't forget!", 0, 0, 0);
      ated_alert("INSUFFICIENT MEMORY", "Can't create temp buffer", "required for that operation", "I'm very distressed", 0, 0, 0);
      return;
   }

   bmp = create_bitmap (w * tsize, h * tsize);

   if (!bmp)
   {
      //char buf [64];
   
      mw = (x1 + x2) / 2;
      mh = (y1 + y2) / 2;

      //sprintf (buf, "x1 = %d; mw = %d; x2 = %d; y1 = %d; mh = %d; y2 = %d", x1, mw, x2, y1, mh, y2);
      //ated_alert("INSUFFICIENT MEMORY", "Trying to split map by 4", buf, "Process", NULL, 0, 0);
      
      viewmap_helper (dest, x1, y1, mw, mh);
      viewmap_helper (dest, mw, y1, x2, mh);
      viewmap_helper (dest, x1, mh, mw, y2);
      viewmap_helper (dest, mw, mh, x2, y2);

      return;
   }

   clear_to_color(bmp, BkgndColor);

   //
   // Draw the whole map on the temp buffer
   //
   for (x = x1; x < x2; x++)
   {
      for (y = y1; y < y2; y++)
      {
         loc = (mapwidth * y) + x;

         if (MapBkgnd)
         {
            tile_t = MapBkgnd[loc];
            if(tile_t < gfx_t_count)
            {
               draw_rle_sprite(bmp, gfx_t [tile_t].dat, (x - x1) * tsize, (y - y1) * tsize);
            }
         }

         if (MapFrgnd)
         {
            tile_m = MapFrgnd[loc];
            if(tile_m < gfx_tm_count)
            {
               draw_rle_sprite(bmp, gfx_tm[tile_m].dat, (x - x1) * tsize, (y - y1) * tsize);
            }
         }

         if (MapInfoPl)
         {
            tile_i = MapInfoPl[loc];
            if(tile_i < gfx_i_count)
            {
               draw_rle_sprite(bmp, gfx_i [tile_i].dat, (x - x1) * tsize, (y - y1) * tsize);
            }
            else
            {
               //
               // print hex
               //
               draw_info_val(bmp, tile_i, (x - x1) * tsize, (y - y1)  *tsize);
            }
         }
         
         if (key[KEY_ESC])
         {
            while (key[KEY_ESC]);

            goto go_away;
         }
      }
   }

go_away:

   //
   // Resize the bitmap
   //
   stretch_blit (bmp, dest, 0, 0, w * tsize, h * tsize, viewmap.tile_width * x1, viewmap.tile_height * y1, (float)(viewmap.tile_width * w), (float)(viewmap.tile_height * h));

   destroy_bitmap (bmp);
}

int      viewmap_needs_update;

void do_viewmap(int rebuild)
{
   float        t;
   int          x, y;

   
   set_mouse_sprite(busy_pointer);
   busy_mouse = TRUE;

   if (!rebuild)
   {
      if (!viewmap.bmp || (viewmap.width > SCREEN_W) || (viewmap.height > SCREEN_H) || viewmap_needs_update)
      {
         rebuild = TRUE;
      }
   }

   if (rebuild)
   {
      t = mapwidth  * tsize; viewmap.tile_width  = (screen_w / t) * tsize;
      t = mapheight * tsize; viewmap.tile_height = (screen_h / t) * tsize;

      viewmap.tile_width = viewmap.tile_height = MIN(viewmap.tile_width, viewmap.tile_height);

      viewmap.width  = viewmap.tile_width  * mapwidth;
      viewmap.height = viewmap.tile_height * mapheight;

      if (viewmap.bmp)
      {
         destroy_bitmap (viewmap.bmp);
         viewmap.bmp = NULL;
      }

      viewmap.bmp = create_bitmap (viewmap.width, viewmap.height);
      clear_to_color(viewmap.bmp, BkgndColor);
      
      viewmap_helper (viewmap.bmp, 0, 0, mapwidth, mapheight);
   }

   viewmap_needs_update = FALSE;
   
   //
   // Draw the VIEWMAP
   //

   scare_mouse();

   clear_bitmap (screen);

   blit (viewmap.bmp, screen, 0, 0, 0, 0, viewmap.width, viewmap.height);

   unscare_mouse();

   set_mouse_sprite(mouse_pointer);
   busy_mouse = FALSE;

   //
   // User Input
   //
   while(!mouse_b && !key[KEY_ESC])
      ;

   if(key[KEY_ESC])
   {
      // to avoid accidental quitting
      while(key[KEY_ESC])
         ;

      return;
   }
   
   x = mouse_x;
   y = mouse_y;

   xbase = (x / viewmap.tile_width)  - (screenw / 2);
   ybase = (y / viewmap.tile_height) - (screenh / 2);

   check_map_edges();

   // to avoid accidental tile-putting
   while(mouse_b)
      ;
}

/*----------------------------------------------------------------------------------------*/
int Item_ViewMap()
{
   do_viewmap(TRUE);

   return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_ReviewMap()
{
   do_viewmap(FALSE);

   return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_ChangeLaunchIcon() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
static void update_back_col() {
  BkgndColor = makecol(BkgndColor_R, BkgndColor_G, BkgndColor_B);

  scare_mouse();
  broadcast_dialog_message(MSG_UPDATE_INFO, 0);
  unscare_mouse();
}

static int back_col_r(void *dp3, int d2) {
  BkgndColor_R = d2;
  update_back_col();

  return D_O_K;
}

static int back_col_g(void *dp3, int d2) {
  BkgndColor_G = d2;
  update_back_col();

  return D_O_K;
}

static int back_col_b(void *dp3, int d2) {
  BkgndColor_B = d2;
  update_back_col();

  return D_O_K;
}

static int draw_color(int msg, DIALOG *d, int c) {
  if((msg == MSG_DRAW) || (msg == MSG_UPDATE_INFO)) {
    rectfill(screen, d->x, d->y, d->x + d->w, d->y + d->h, BkgndColor);
  }

  return D_O_K;
}

DIALOG BKGND_COL[] =
{
  /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
  { box,            0,            0,  200, 125,   0,   0,     0,     0,       0,           0,         NULL,                NULL,  NULL  },
  { slider,        10,           20,  180,  15,   0,   0,     0,     0,     255,           0,         NULL,          back_col_r,  NULL  },
  { slider,        10,           45,  180,  15,   0,   0,     0,     0,     255,           0,         NULL,          back_col_g,  NULL  },
  { slider,        10,           70,  180,  15,   0,   0,     0,     0,     255,           0,         NULL,          back_col_b,  NULL  },
  { label,          5,            5,    0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,       "Select R, G, B",    NULL,  NULL  },
  { button,        75,           95,   50,  20,   0,   0,    'd',     D_EXIT,  0,           0,         "&Done",            NULL,  NULL  },
  { draw_color,     5,           95,   20,  20,   0,   0,     0,     0,       0,            0,        NULL,                NULL,  NULL  },
  //{ label,          0,            0, 170, 100,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { NULL,           0,            0,   0,     0,  0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};

int Item_ChangeBkgndColor() {

  BKGND_COL[1].d2 = BkgndColor_R;
  BKGND_COL[2].d2 = BkgndColor_G;
  BKGND_COL[3].d2 = BkgndColor_B;

  centre_dialog(BKGND_COL);
  do_ated_dialog(BKGND_COL, -1);

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_TINFOcopy() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_GraphicDump() {

  return D_REDRAW;
}
/*----------------------------------------------------------------------------------------*/
int Item_ESC() {
  while(key[KEY_ESC]);

  if (SelectMode || TsearchMode || BfillMode || PasteMode || FillMode || TINFOMode) {
    ResetModes();
    return D_REDRAW;
  }else {
    if(Item_Quit() == D_EXIT)
      return D_EXIT;
  }

  return D_O_K;
}
/*----------------------------------------------------------------------------------------*/
int Item_TINFOMode(void)
{
   ResetModes ();
  
   TINFOMode = TRUE;
  
   return D_REDRAW;
}
