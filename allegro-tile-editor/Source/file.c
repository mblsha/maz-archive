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
 *   All file writing and reading routines are here
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 *
 *   30.05.01 - v0.1.0
 *     Writing load_ated_temp_file()
 *
 *   29.10.01 - v0.2.0
 *     Doing TILEINFO saving and loading
 */

#include "ated.h"

unsigned gfx_t_count, gfx_tm_count, gfx_i_count;

char tilegfx_path[256];

char info_msg[] = "DO NOT DELETE!";
char ated_id[] = "ATED";

DATAFILE ated_header = { info_msg, DAT_ATED_HEADER,     sizeof(info_msg), NULL };
//DATAFILE map_header  = { info_msg, DAT_TILE_MAP_HEADER, sizeof(info_msg), NULL };
//DATAFILE datedit_info = { grabber_info_msg, DAT_INFO, sizeof(grabber_info_msg), NULL };
DATAFILE current_map = { info_msg, DAT_TILE_MAP, sizeof(info_msg), NULL };

char map_name[128]        = "";
char map_name_string[128] = "";
char map_author[128]      = "";

char tileset_name[128] = "No   Name";
char author_name[128]  = "Anonymous";

char tiledata_file[256];
char tilegfx_file[256];
char password[256];

static void load_tilegfx ();


int tile_maps_count;
INDEX_LIST tile_maps[1000];

char *map_list(int index, int *list_size) {
  if(index < 0) {
    *list_size = tile_maps_count;
    return NULL;
  }

  return (char *)tile_maps[index].name;
}

#define MAP_LIST                2
#define EDIT_BUTTON             3
#define CREATE_BUTTON           4
#define REMOVE_BUTTON           5
#define RENAME_BUTTON           6
#define DONE_BUTTON             7
#define TILESET_NAME            8
DIALOG MANAGE_MAPS[] = {
  /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
  { box,            0,            0, 320, 200,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { label,          5,            1,   0,   0,   0,   0,     0,     0,       0,TEXT_OUTLINE_2,       "Map List:",          NULL,  NULL  },
  { listbox,        5,           15, 150, 180,   0,   0,     0,     0,       0,           0,         map_list,             NULL,  NULL  },
  { button,       165,           15, 145,  20,   0,   0,  'e',D_EXIT,       0,           0,         "&Edit Map",          NULL,  NULL  },
  { button,       165,           45, 145,  20,   0,   0,  'c',D_EXIT,       0,           0,         "&Create Map",        NULL,  NULL  },
  { button,       165,           75, 145,  20,   0,   0,  'r',D_EXIT,       0,           0,         "&Remove Map",        NULL,  NULL  },
  { button,       165,          105, 145,  20,   0,   0,  'n',D_EXIT,       0,           0,         "Re&name Map",        NULL,  NULL  },
  { button,       165,          145, 145,  20,   0,   0,  'd',D_EXIT,       0,           0,         "&Done",              NULL,  NULL  },
  //{ d_button_proc,165,           15, 145,  20,   0,   15,  'e',D_EXIT,       0,           0,         "&Edit Map",          NULL,  NULL  },
  //{ d_button_proc,165,           45, 145,  20,   0,   15,  'c',D_EXIT,       0,           0,         "&Create Map",        NULL,  NULL  },
  //{ d_button_proc,165,           75, 145,  20,   0,   15,  'r',D_EXIT,       0,           0,         "&Remove Map",        NULL,  NULL  },
  //{ d_button_proc,165,          105, 145,  20,   0,   15,  'n',D_EXIT,       0,           0,         "Re&name Map",        NULL,  NULL  },
  //{ d_button_proc,165,          145, 145,  20,   0,   15,  'd',D_EXIT,       0,           0,         "&Done",              NULL,  NULL  },
  //{ d_text_proc,    5,            5,      0,   0,   0,   15,    0,     0,       0,           0,         "Map List:",          NULL,  NULL  },
  { edit_text,    165,          170,  145,  0,   0,   0,    0,     0,      32,           0,         tileset_name,         NULL,  NULL  },
  { NULL,           0,            0,    0,  0,   0,   0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};

int Item_ManageMaps()
{
   int index, ret;

   if(DirtyFlag)
   {
      //switch(ated_alert3("Your current work may be lost.","Do you want to SAVE it?","","Yes","No","Cancel", 0, 0, 0)) {
      switch(ated_alert3("The map has been modified.","Do you want to SAVE it?","","Yes","No","Cancel", 0, 0, 0))
      {
         case 1:
            Item_SaveATEDtilesetFile();
            break;
         case 3:
            return D_REDRAW;
      }
   }

   centre_dialog(MANAGE_MAPS);
   ret = do_ated_dialog(MANAGE_MAPS,-1);

   index = tile_maps[MANAGE_MAPS[MAP_LIST].d1].index;
   switch(ret)
   {
      case EDIT_BUTTON:
         edit_map(index);
         break;

      case CREATE_BUTTON:
         create_map();
         break;

      case REMOVE_BUTTON:
         delete_map(index);
         break;

      case RENAME_BUTTON:
         rename_map(index);
         break;
   }
   return D_REDRAW;
}

static void init_map()
{
   //
   // Base variables
   //
   xbase = ybase = 0;
   planeton = planemon = planeion = FALSE;
   viewton  = viewmon  = viewion  = FALSE;
   //
   // Undo Buffer
   //
   UndoRegion.x = UndoRegion.y = 0;
   UndoRegion.w = mapwidth;
   UndoRegion.h = mapheight;
   CopyUndoRegion();
   //
   // Copy Buffer
   //
   TileCopy.x = TileCopy.y = TileCopy.w = TileCopy.h = 0;
   TileCopy.back = TileCopy.fore = TileCopy.info = FALSE;
   TileCopy.MapOrTileSelect = FALSE;
   TileCopy.tile_select_w = TileCopy.tile_select_w = 0;
   //
   // Reset Modes
   //
   SelectMode  = FALSE;
   TsearchMode = FALSE;
   BfillMode   = FALSE;
   PasteMode   = FALSE;
   FillMode    = FALSE;
   SelX1 = SelY1 = SelX2 = SelY2 = -1;

   DirtyFlag = FALSE;
}

void load(char *filename, int flush)
{
   DATAFILE *new_datafile;
   int obj_count;
   int new_obj_count;
   //int items_num;

   set_mouse_sprite(busy_pointer);
   busy_mouse = TRUE;

   if ((tiledata) && (flush)) 
   {
      unload_datafile(tiledata);
      tiledata = NULL;
   }

   if (filename) 
   {
      fix_filename_path(tiledata_file, filename, sizeof(tiledata_file));
      //strcpy(tiledata_file, datedit_pretty_name(tiledata_file, "atm", FALSE));
   }
   else
   {
      tiledata_file[0] = 0;
   }

   //main_dlg[DLG_FILENAME].d2 = strlen(data_file);

   //new_datafile = datedit_load_datafile(filename, FALSE, password);
   //if (!new_datafile)
   //   new_datafile = datedit_load_datafile(NULL, FALSE, NULL);

   if (!flush) 
   {
      obj_count = 0;
      while (tiledata[obj_count].type != DAT_END)
	      obj_count++;

      new_obj_count = 0;
      while (new_datafile[new_obj_count].type != DAT_END)
	      new_obj_count++;

      tiledata = realloc(tiledata, (obj_count+new_obj_count+1) * sizeof(DATAFILE));
      memmove(tiledata+obj_count, new_datafile, (new_obj_count+1) * sizeof(DATAFILE));
      free(new_datafile);

      //datedit_sort_datafile(tiledata);
   }
   else
   {
      tiledata = new_datafile;
   }
   //SELECTED_ITEM = 0;

   //retrieve_property(DLG_HEADERNAME, DAT_HNAM, "");
   //retrieve_property(DLG_PREFIXSTRING, DAT_HPRE, "");
   //retrieve_property(DLG_XGRIDSTRING, DAT_XGRD, "16");
   //retrieve_property(DLG_YGRIDSTRING, DAT_YGRD, "16");

   /*if (utolower(*get_datafile_property(&datedit_info, DAT_BACK)) == 'y')
      main_dlg[DLG_BACKUPCHECK].flags |= D_SELECTED;
   else
      main_dlg[DLG_BACKUPCHECK].flags &= ~D_SELECTED;

   if (utolower(*get_datafile_property(&datedit_info, DAT_DITH)) == 'y')
      main_dlg[DLG_DITHERCHECK].flags |= D_SELECTED;
   else
      main_dlg[DLG_DITHERCHECK].flags &= ~D_SELECTED;

   main_dlg[DLG_PACKLIST].d1 = atoi(get_datafile_property(&datedit_info, DAT_PACK));
   pack_getter(-1, &items_num);
   if (main_dlg[DLG_PACKLIST].d1 >= items_num)
      main_dlg[DLG_PACKLIST].d1 = items_num-1;
   else if (main_dlg[DLG_PACKLIST].d1 < 0)
      main_dlg[DLG_PACKLIST].d1 = 0;
   */
   //rebuild_list(NULL, TRUE);
   create_planes(FALSE, FALSE, FALSE);
   init_map();
   DirtyFlag = TRUE;

   set_mouse_sprite(mouse_pointer);
   busy_mouse = FALSE;
}

int save(int strip)
{
   char buf[256], buf2[256];
   int err = FALSE;

   strcpy(buf, tiledata_file);

   if (ated_file_select("Save ATED file", buf, "ats")) 
   {
      if ((stricmp(tiledata_file, buf) != 0) && (exists(buf))) 
      {
	      sprintf(buf2, "%s already exists, overwrite?", buf);
	      if (ated_alert(buf2, NULL, NULL, "Yes", "Cancel", 'y', 27) != 1)
	         return D_REDRAW;
      }

      box_start();

      set_mouse_sprite(busy_pointer);
      busy_mouse = TRUE;

      fix_filename_case(buf);
      strcpy(tiledata_file, buf);
      //main_dlg[DLG_FILENAME].d2 = strlen(data_file);

      update_info();

      //if (!datedit_save_datafile(tiledata, tiledata_file, strip, -1, TRUE, FALSE, TRUE, password))
	   //   err = TRUE;
      /*
      if ((header_file[0]) && (!err)) {
	 box_eol();

	 if ((!strchr(header_file, OTHER_PATH_SEPARATOR)) && (!strchr(header_file, '/'))) {
	    strcpy(buf, data_file);
	    strcpy(get_filename(buf), header_file);
	 }
	 else
	    strcpy(buf, header_file);

	 if (!datedit_save_header(datafile, data_file, buf, "grabber", prefix_string, FALSE))
	    err = TRUE;
      }*/

      set_mouse_sprite(mouse_pointer);
      busy_mouse = FALSE;

      box_end(!err);
   }

   return D_REDRAW;
}

#define BACK_PLANE      1
#define FORE_PLANE      2
#define INFO_PLANE      3
DIALOG MANAGE_PLANES[] = {
  /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
  { box,            0,            0, 125,  90,   0,   0,     0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { checkbox,      10,           10,   0,   0,   0,   0,   'b',     0,       0,           0,         "&Background Plane",  NULL,  NULL  },
  { checkbox,      10,           25,   0,   0,   0,   0,   'f',     0,       0,           0,         "&Foreground Plane",  NULL,  NULL  },
  { checkbox,      10,           40,   0,   0,   0,   0,   'i',     0,       0,           0,         "&Info Plane",  NULL,  NULL  },
  { button,        20,           60,  85,  20,   0,   0,  'd',D_EXIT,       0,           0,         "&Done",              NULL,  NULL  },
  //{ edit_text,    165,          170,  145,  0,NULL, NULL,    0,     0,       0,           0,         tileset_name,         NULL,  NULL  },
  { NULL,           0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};

int Item_ManagePlanes() 
{
   int ret;
   int back, fore, info;

  /*if(DirtyFlag) {
    switch(ated_alert3("Your current work may be lost.","Do you want to SAVE it?","","Yes","No","Cancel", NULL, NULL, NULL)) {
    //switch(ated_alert3("The map has been modified.","Do you want to SAVE it","before exiting "APP_NAME"?","Yes","No","Cancel", NULL, NULL, NULL)) {
      case 1:
        Item_SaveFile();
        break;
      case 3:
        return D_REDRAW;
    }
  }*/

   MANAGE_PLANES[BACK_PLANE].flags = MapBkgnd  ? D_SELECTED : 0;
   MANAGE_PLANES[FORE_PLANE].flags = MapFrgnd  ? D_SELECTED : 0;
   MANAGE_PLANES[INFO_PLANE].flags = MapInfoPl ? D_SELECTED : 0;

   back = (MANAGE_PLANES[BACK_PLANE].flags & D_SELECTED);
   fore = (MANAGE_PLANES[FORE_PLANE].flags & D_SELECTED);
   info = (MANAGE_PLANES[INFO_PLANE].flags & D_SELECTED);

   centre_dialog(MANAGE_PLANES);
   ret = do_ated_dialog(MANAGE_PLANES,-1);
   /*
   index = tile_maps[MANAGE_MAPS[MAP_LIST].d1].index;
  switch(ret) {
    case EDIT_BUTTON:
      edit_map(index);
      break;

    case CREATE_BUTTON:
      create_map();
      break;

    case REMOVE_BUTTON:
      delete_map(index);
      break;

    case RENAME_BUTTON:
      rename_map(index);
      break;
  }*/
   if((back != (MANAGE_PLANES[BACK_PLANE].flags & D_SELECTED)) ||
      (fore != (MANAGE_PLANES[FORE_PLANE].flags & D_SELECTED)) ||
      (info != (MANAGE_PLANES[INFO_PLANE].flags & D_SELECTED))) 
   {
      if(ated_alert("","Modify the tilemap, master?","","&OK","&Cancel",'o','c') == 1) 
      {
         create_planes ((MANAGE_PLANES[BACK_PLANE].flags & D_SELECTED),
                        (MANAGE_PLANES[FORE_PLANE].flags & D_SELECTED),
                        (MANAGE_PLANES[INFO_PLANE].flags & D_SELECTED));
      }
      else 
      {
         if(back != (MANAGE_PLANES[BACK_PLANE].flags & D_SELECTED))
            MANAGE_PLANES[BACK_PLANE].flags ^= D_SELECTED;
         if(fore != (MANAGE_PLANES[FORE_PLANE].flags & D_SELECTED))
            MANAGE_PLANES[FORE_PLANE].flags ^= D_SELECTED;
         if(info != (MANAGE_PLANES[INFO_PLANE].flags & D_SELECTED))
            MANAGE_PLANES[INFO_PLANE].flags ^= D_SELECTED;
      }
   }
   return D_REDRAW;
}

int Item_NewFile() 
{
   ated_alert ("Work In Progress: You'd better use the",
               "\"File->Manage Maps->Create\" option.",
               "", "Thanks for the tip", "OK", 0, 0);
   /*
   if(ated_alert("","New File - Are you sure?","","Yes","No", 0, 0) == 1) 
   {
      if(DirtyFlag) 
      {
         switch (ated_alert3("The map has been modified.","Do you want to SAVE it","before exiting "APP_NAME"?","Yes","No","Cancel", 0, 0, 0)) 
         {
            case 1:
               Item_SaveATEDtilesetFile();
               break;
            case 3:
               return D_REDRAW;
         }
      }

      load(NULL, TRUE);
   }
   */
   return D_REDRAW;
}

int Item_LoadATEDtilesetFile() {
  //char path[256] = "";

  if(ated_alert("","Load File - Are you sure?","","Yes","No", 0, 0) == 1) {
    if(DirtyFlag) {
      switch(ated_alert3("The map has been modified.","Do you want to SAVE it","before exiting "APP_NAME"?","Yes","No","Cancel", 0, 0, 0)) {
        case 1:
          Item_SaveATEDtilesetFile();
          break;
        case 3:
          return D_REDRAW;
      }
    }

    if(ated_file_select("Select ATED file to LOAD", tiledata_file, "ATS")) {
      //tiledata = load_datafile(path);
      //tiledata_file = path;
      load(tiledata_file, FALSE);
      //load(path, TRUE);
      rebuild_map_list();
    }
  }
  return D_REDRAW;
}

int Item_SaveATEDtilesetFile() {
  //char path[256] = "";
  //
  //if(ated_file_select("Select ATED file to SAVE", path, "ATM"))
  //  load(path, TRUE);
  save(FALSE);
  return D_REDRAW;
}

/*
 ATED Singlemap file format

 [Header]
 long     - 'ATED'
 long     - Format version (higher byte - major version, lower byte - minor version)

 [Chunks]

 [Main]
 char[128]- Name
 char[128]- Author
 char[128]- TileGFX File
 long     - Width
 long     - Height

 [Planes]
 long     - 'plns'
 long     - chunk id ('back', 'fore', 'info')
 long     - tile_size -- size of one tile
 data     - plane data (size = uint * width * heigth)
 ...
 long     - 'end '

 [TInfo]
 long     - 'tinf'
 long     - chunk id ('back', 'fore', 'info')
 char[32] - Name
 long     - num_tiles
 long     - tinfo_size -- size of one tinfo item
 data     - tinfo data (size = tinfo_size * num_tiles)
 ...
 long     - 'end '
*/
#define ID(a,b,c,d)              (a | (b << 8) | (c << 16) | (d << 24))

#define ATED_HEADER              ID ('A','T','E','D')
#define ATED_HEADER_VERSION      0x0101 // 1.01

#define ATED_CHUNK_PLANES        ID ('p','l','n','s')
#define ATED_CHUNK_TINFO         ID ('t','i','n','f')
#define ATED_CHUNK_MAIN          ID ('m','a','i','n')
#define ATED_END                 ID ('e','n','d',' ')

#define ATED_BACK                ID ('b','a','c','k')
#define ATED_FORE                ID ('f','o','r','e')
#define ATED_INFO                ID ('i','n','f','o')

#define ATED_CHUNK_TEMP          ID ('t','e','m','p')
#define ATED_BACK_UNDO           ID ('b','u','n','d')
#define ATED_FORE_UNDO           ID ('f','u','n','d')
#define ATED_INFO_UNDO           ID ('i','u','n','d')
#define ATED_BACK_CUT            ID ('b','c','u','t')
#define ATED_FORE_CUT            ID ('f','c','u','t')
#define ATED_INFO_CUT            ID ('i','c','u','t')

typedef struct ATED_CHUNK
{
   long id;
   void (*handler) (PACKFILE *f);
} ATED_CHUNK;

#define __FILE_C__
#include "file_save.c"
#include "file_load.c"
#undef __FILE_C__

int Item_LoadATEDsinglemapFile()
{
   int                  b, f, i;
   PACKFILE             *df;

   if (ated_alert("","Load File - Are you sure?","","Yes","No", 0, 0) == 1)
   {
      if (DirtyFlag)
      {
         switch (ated_alert3("The map has been modified.","Do you want to SAVE it","before exiting "APP_NAME"?","Yes","No","Cancel", 0, 0, 0))
         {
            case 1:
               Item_SaveATEDsinglemapFile();
               break;

            case 3:
               return D_REDRAW;
         }
      }

      if(ated_file_select("Select ATED file to LOAD", tiledata_file, "ATM"))
      {
         if(!(df = pack_fopen(tiledata_file, "rp")))
         {
            if(!(df = pack_fopen(tiledata_file, "r")))
            {
               ated_alert("","Can't open specified file","","OK",0,0,0);

               return D_O_K;
            }
         }
         
         set_mouse_sprite (busy_pointer);
         busy_mouse = TRUE;
      
         load_singlemap (tiledata_file, NULL);

         if (MapBkgnd)
            b = TRUE;
         if (MapFrgnd)
            f = TRUE;
         if (MapInfoPl)
            i = TRUE;

         init_map();
         viewton = b;
         viewmon = f;
         viewion = i;

         load_tinfo_file ();
         load_tilegfx ();

         set_mouse_sprite (mouse_pointer);
         busy_mouse = FALSE;
      
         viewmap_needs_update = TRUE;
      }
   }

   return D_REDRAW;
}

int Item_SaveATEDsinglemapFile()
{
   char buf[256], buf2[256];

   strcpy(buf, tiledata_file);

   if (ated_file_select("Save ATED file", buf, "atm"))
   {
      if ((stricmp(tiledata_file, buf) != 0) && (exists(buf)))
      {
         sprintf(buf2, "%s already exists, overwrite?", buf);
         if (ated_alert(buf2, NULL, NULL, "Yes", "Cancel", 'y', 27) != 1)
            return D_REDRAW;
      }

      fix_filename_case(buf);
      strcpy(tiledata_file, buf);
/*
      if(!(df = pack_fopen(tiledata_file, "wp")))
      {
         ated_alert("","Can't open specified file","","OK",0,0,0);

         return D_O_K;
      }
*/
      set_mouse_sprite(busy_pointer);
      busy_mouse = TRUE;

      save_singlemap (tiledata_file);
/*
      //
      // Saving header//width & height
      //
      strcpy(head.id, ated_id);
      strcpy(head.name, map_name);
      head.w = mapwidth;
      head.h = mapheight;
      strcpy (head.gfx_name, get_filename (tilegfx_file));
      pack_fwrite(&(head), sizeof(head), df);
      //pack_fwrite(&(fname), sizeof(fname), df);
      //pack_fwrite(&(mapwidth),  sizeof(mapwidth),  df);
      //pack_fwrite(&(mapheight), sizeof(mapheight), df);
      //
      // Saving planes
      //
      tmp = (MapBkgnd != NULL);
      pack_fwrite(&(tmp), sizeof(tmp), df);
      tmp = (MapFrgnd != NULL);
      pack_fwrite(&(tmp), sizeof(tmp), df);
      tmp = (MapInfoPl != NULL);
      pack_fwrite(&(tmp), sizeof(tmp), df);

      if(MapBkgnd)
         pack_fwrite(MapBkgnd, sizeof(unsigned)*mapwidth*mapheight, df);
      if(MapFrgnd)
         pack_fwrite(MapFrgnd, sizeof(unsigned)*mapwidth*mapheight, df);
      if(MapInfoPl)
         pack_fwrite(MapInfoPl, sizeof(unsigned)*mapwidth*mapheight, df);

      //
      // That's all
      //
      pack_fclose(df);
*/
      save_tinfo_file ();

      set_mouse_sprite(mouse_pointer);
      busy_mouse = FALSE;
   }

   return D_REDRAW;
}

/* callback for the datedit functions to display a message */
void datedit_msg(AL_CONST char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   show_mouse(NULL);
   acquire_screen();

   box_out(buf);
   box_eol();

   release_screen();
   show_mouse(screen);
}



/* callback for the datedit functions to start a multi-part message */
void datedit_startmsg(AL_CONST char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   box_out(buf);
}



/* callback for the datedit functions to end a multi-part message */
void datedit_endmsg(AL_CONST char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   show_mouse(NULL);
   acquire_screen();

   box_out(buf);
   box_eol();

   release_screen();
   show_mouse(screen);
}



/* callback for the datedit functions to report an error */
void datedit_error(AL_CONST char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   set_mouse_sprite(mouse_pointer);

   ated_alert(buf, NULL, NULL, "Oh dear", NULL, 13, 0);

   if (busy_mouse)
      set_mouse_sprite(busy_pointer);
}



/* callback for the datedit functions to ask a question */
int datedit_ask(AL_CONST char *fmt, ...)
{
   va_list args;
   char buf[1024];
   int ret;

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   strcat(buf, "?");

   set_mouse_sprite(mouse_pointer);

   ret = alert(buf, NULL, NULL, "Yes", "Cancel", 'y', 27);

   if (busy_mouse)
      set_mouse_sprite(busy_pointer);

   if (ret == 1)
      return 'y';
   else
      return 'n';
}

/* updates the info chunk with the current settings */
void update_info()
{
  char buf[8];
  //
  // Update tileset properties
  //
  //datedit_set_property(&ated_header, DAT_ATED_TILESET_NAME,   tileset_name);
  //datedit_set_property(&ated_header, DAT_ATED_TILESET_AUTHOR, author_name);
  //datedit_set_property(&ated_header, DAT_ATED_TILEGFX_FILE,   tilegfx_file);
  //
  // Update map properties
  //
  //if(current_map.dat != NULL) {
  //  datedit_set_property(&current_map, DAT_TILE_MAP_NAME,     map_name);
  //  datedit_set_property(&current_map, DAT_TILE_MAP_WIDTH,    mapwidth_string);
  //  datedit_set_property(&current_map, DAT_TILE_MAP_HEIGHT,   mapheight_string);
  //}
  //
  // Force global compression
  //
  sprintf(buf, "%d", 2);
  //datedit_set_property(&datedit_info, DAT_PACK, buf);
}

#define MAP_NAME                        2
#define MAP_WIDTH_STRING                4
#define MAP_HEIGHT_STRING               6
DIALOG CREATE_MAP[] = {
  /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
  { box,            0,            0, 150, 120,   0,   0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { label,          5,            5,   0,   0,   0,   0,    0,     0,       0,TEXT_OUTLINE_2,       "Map Name:",          NULL,  NULL  },
  { edit_text,      5,           20, 140,   0,   0,   0,    0,     0,     128,           0,         map_name_string,      NULL,  NULL  },
  { label,          5,           35,   0,   0,   0,   0,    0,     0,       0,TEXT_OUTLINE_2,       "Map Width:",         NULL,  NULL  },
  { edit_text,      5,           50, 140,   0,   0,   0,    0,     0,      16,           0,         mapwidth_string,      NULL,  NULL  },
  { label,          5,           65,   0,   0,   0,   0,    0,     0,       0,TEXT_OUTLINE_2,       "Map Height:",        NULL,  NULL  },
  { edit_text,      5,           80, 140,   0,   0,   0,    0,     0,      16,           0,         mapheight_string,     NULL,  NULL  },
  { button,         37,          95,  76,  20,   0,   0,  'd',D_EXIT,       0,           0,         "&Done",              NULL,  NULL  },
  { NULL,           0,            0,   0,   0,   0,   0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};

void create_map() {
  //DATAFILE *df;
  void *v = NULL;
  char *bp;
  long size = 0;

  strcpy(map_name_string,   "No Name");
  strcpy(mapwidth_string,   "0");
  strcpy(mapheight_string,  "0");

  centre_dialog(CREATE_MAP);
  if(do_ated_dialog(CREATE_MAP, -1) == -1)
    return;

  strcpy(map_name,   map_name_string);
  mapwidth  = strtoul(mapwidth_string,  &bp, 0);
  mapheight = strtoul(mapheight_string, &bp, 0);

  create_planes(FALSE, FALSE, FALSE);
  init_map();
  DirtyFlag = TRUE;

  v = makenew_data(&size);
  //df = tiledata;
  //tiledata = datedit_insert(tiledata, NULL, CREATE_MAP[MAP_NAME].dp, DAT_TILE_MAP, v, size);
  //datedit_sort_datafile(tiledata);
  rebuild_map_list();
}

void edit_map(int index) {

}

void delete_map(int index) {
  if(ated_alert("Are you sure", "you want to", "delete map?", "&Yes", "&No", 'y', 'n') == 1) {
    //tiledata = datedit_delete(tiledata, index);

    rebuild_map_list();
  }
}

void rename_map(int index) {

}

/*int Item_SaveMap() {

  return D_REDRAW;
}*/

/*int Item_SwitchMap() {
  return D_REDRAW;
}

int Item_Amputate() {
  return D_REDRAW;
}

int Item_ImportMaps() {
  return D_REDRAW;
}*/
/* creates a new binary data object */
void *makenew_data(long *size)
{
   static char msg[] = "Binary Data";

   void *v = _al_malloc(sizeof(msg));

   strcpy(v, msg);
   *size = sizeof(msg);

   return v;
}
/*
static void convert_rle_sprite(RLE_SPRITE *sprite) {//DATAFILE *dat) {
  //signed long *p;
  //int x, y;
  //int c;
  //int source_color, dest_color, type1, type2;
  int depth;
  RLE_SPRITE *spr;
  BITMAP *bmp, *bmp2;
  RGB tmprgb = ated_current_palette[0];

  depth = screen_bpp;

  spr = sprite;//(RLE_SPRITE *)dat->dat;
  if (spr->color_depth == depth)
    return;

  bmp = create_bitmap_ex(spr->color_depth, spr->w, spr->h);
  clear_to_color(bmp, bmp->vtable->mask_color);
  draw_rle_sprite(bmp, spr, 0, 0);
  bmp2 = create_bitmap_ex(depth, bmp->w, bmp->h);

  ated_current_palette[0].r = 63;
  ated_current_palette[0].g = 0;
  ated_current_palette[0].b = 63;
  select_palette(ated_current_palette);

  blit(bmp, bmp2, 0, 0, 0, 0, bmp->w, bmp->h);

  unselect_palette();
  ated_current_palette[0] = tmprgb;

  //dat->dat = get_rle_sprite(bmp2);
  sprite = get_rle_sprite(bmp2);

  destroy_bitmap(bmp);
  destroy_bitmap(bmp2);
  destroy_rle_sprite(spr);

  //ated_alert("","Just converted RLE sprite!!!","","&WoW!!!",NULL,'w',NULL);
}*/

static void load_tilegfx()
{
   RLE_SPRITE *tile;

   if (!exists (tilegfx_file))
      return;

   if(!(tilegfx = load_datafile(tilegfx_file)))
      fat_error("Wrong Tile GFX file!");

   replace_filename (tilegfx_path, tilegfx_file, "", sizeof(tilegfx_path));

   //
   // Load palette
   //
   if(tilegfx[3].type == DAT_PALETTE)
   {
      sel_palette(tilegfx[3].dat);
      init_colors();

      //
      // Reload datafile
      //
      unload_datafile(tilegfx);
      tilegfx = load_datafile(tilegfx_file);
   }

   //
   // Convert transparent color
   //
   convert_images_in_datafile(tilegfx);

   //
   // Load tiles
   //
   gfx_t  = tilegfx[0].dat;
   gfx_tm = tilegfx[1].dat;
   gfx_i  = tilegfx[2].dat;
   //
   // Count tiles
   //
   gfx_t_count = gfx_tm_count = gfx_i_count = 0;
   while(gfx_t[gfx_t_count].type   != DAT_END)
      gfx_t_count++;
   while(gfx_tm[gfx_tm_count].type != DAT_END)
      gfx_tm_count++;
   while(gfx_i[gfx_i_count].type   != DAT_END)
      gfx_i_count++;
   //
   // Set TileSize
   //
   tile = gfx_t[0].dat;
   if(!(tsize = tile->w))
   {
      tile = gfx_tm[0].dat;
      if(!(tsize = tile->w))
      {
         tile = gfx_i[0].dat;
         if(!(tsize = tile->w))
         {
            ated_alert("Can't determine","what tile size that","tileset uses!","OK",0,0,0);
            tsize = 16;
         }
      }
   }
}

int Item_LoadTileGFX()
{
   if(ated_file_select("Select Tile GFX file", tilegfx_file, "dat"))
   {
      set_mouse_sprite(busy_pointer);
      busy_mouse = TRUE;

      load_tilegfx();

      set_mouse_sprite(mouse_pointer);
      busy_mouse = FALSE;
   }

   return D_REDRAW;
}

void rebuild_map_list()
{
   DATAFILE *d;
   int i;
   /*
   tile_maps_count = 0;
   for(i = 0; tiledata[i].type != DAT_END; i++)
   {
      d = tiledata + i;

      if(d->type == DAT_TILE_MAP)
      {
         tile_maps_count++;
      }
   }

   if(tile_maps)
      free(tile_maps);
   tile_maps = malloc(sizeof(INDEX_LIST) * tile_maps_count);
   */
   tile_maps_count = 0;
   for(i = 0; tiledata[i].type != DAT_END; i++)
   {
      d = tiledata + i;

      if(d->type == DAT_TILE_MAP)
      {
         tile_maps[tile_maps_count].index = i;
         //strncat(tile_maps[tile_maps_count]->name, get_datafile_property(d, DAT_NAME), 128);
         strncpy(tile_maps[tile_maps_count].name, get_datafile_property(d, DAT_TILE_MAP_NAME), 128);
         tile_maps_count++;
      }
   }
}

void create_planes(int back, int fore, int info)
{
   //unsigned i;
   //long size;
   //char *bp;

   //size = sizeof(unsigned) * (mapwidth * mapheight);

   set_mouse_sprite(busy_pointer);
   busy_mouse = TRUE;

   if(back)
   {
      //
      // Allocate memory...
      //
      if(!MapBkgnd)
      {
         MapBkgnd = calloc(mapwidth * mapheight, sizeof(unsigned));
         CutBkgnd = calloc(mapwidth * mapheight, sizeof(unsigned));
         undoB    = calloc(mapwidth * mapheight, sizeof(unsigned));
         if((MapBkgnd == NULL) || (CutBkgnd == NULL) || (undoB == NULL))
         {
            MapBkgnd = CutBkgnd = NULL;
            ated_alert("ERROR!","Not enough memory for back layer!","ERROR!","Oh, my god!",0,0,0);
            return;
         }
      }
   }
   else
   {
      //
      // free..
      //
      if(MapBkgnd)
      {
         free(MapBkgnd);
         free(CutBkgnd);
         free(undoB);
         MapBkgnd = CutBkgnd = undoB =  NULL;
      }
   }

   if(fore)
   {
      //
      // so on...
      //
      if(!MapFrgnd)
      {
         MapFrgnd = calloc(mapwidth * mapheight, sizeof(unsigned));
         CutFrgnd = calloc(mapwidth * mapheight, sizeof(unsigned));
         undoF    = calloc(mapwidth * mapheight, sizeof(unsigned));
         if((MapFrgnd == NULL) || (CutFrgnd == NULL) || (undoF == NULL))
         {
            MapFrgnd = CutFrgnd = NULL;
            ated_alert("ERROR!","Not enough memory for fore layer!","ERROR!","Oh, my god!",0,0,0);
            return;
         }
      }
   }
   else
   {
      if(MapFrgnd)
      {
         free(MapFrgnd);
         free(CutFrgnd);
         free(undoF);
         MapFrgnd = CutFrgnd = undoF = NULL;
      }
   }

   if(info)
   {
      if(!MapInfoPl)
      {
         MapInfoPl = calloc(mapwidth * mapheight, sizeof(unsigned));
         CutInfoPl = calloc(mapwidth * mapheight, sizeof(unsigned));
         undoI     = calloc(mapwidth * mapheight, sizeof(unsigned));
         if((MapInfoPl == NULL) || (CutInfoPl == NULL) || (undoI == NULL))
         {
            MapInfoPl = CutInfoPl = NULL;
            ated_alert("ERROR!","Not enough memory for info layer!","ERROR!","Oh, my god!",0,0,0);
            return;
         }
      }
   }
   else
   {
      if(MapInfoPl)
      {
         free(MapInfoPl);
         free(CutInfoPl);
         free(undoI);
         MapInfoPl = CutInfoPl = undoI = NULL;
      }
   }

   set_mouse_sprite(mouse_pointer);
   busy_mouse = FALSE;
}

//
// Saving
//
static void save_bool(int var, PACKFILE *file)
{
   short tmp = (var != FALSE);

   pack_fwrite(&(tmp), sizeof(tmp), file);
}

static void save_uint(void *var, PACKFILE *file)
{
   pack_fwrite(var, sizeof(var), file);
}

static void save_char(const char *var, PACKFILE *file)
{
   pack_fputs (var,  file);
   pack_fputs ("\n", file);
}

void save_tinfo_file()
{
   PACKFILE             *df;
   int                  b, f, i;
   int                  tmp;
   unsigned             size;
   char                 name[32];
   char                 buf[255];

   if (save_tinfofile (replace_extension(buf, tilegfx_file, "tnf", 255)))
   {
      ated_alert ("ERROR", "Can't save TileInfo!", "", "Again?!", 0,0,0);
   }

   return;
   
   if(!(df = pack_fopen(replace_extension(buf, tilegfx_file, "tnf", 255), "wp")))

   for (i = 0; i < MAX_TINFO; i++)
   {
      if(strlen(tinfo_back[i].name))
      {
         pack_fwrite(&tinfo_back[i].name, sizeof(tinfo_back[i].name), df);
         tmp = TINFO_BACK;
         pack_fwrite(&(tmp), sizeof(tmp), df);
         size = tinfo_back[i].num;
         pack_fwrite(&(size), sizeof(size), df);
         pack_fwrite(tinfo_back[i].info, sizeof(unsigned) * size, df);
      }
      if(strlen(tinfo_fore[i].name))
      {
         pack_fwrite(&tinfo_fore[i].name, sizeof(tinfo_fore[i].name), df);
         tmp = TINFO_FORE;
         pack_fwrite(&(tmp), sizeof(tmp), df);
         size = tinfo_fore[i].num;
         pack_fwrite(&(size), sizeof(size), df);
         pack_fwrite(tinfo_fore[i].info, sizeof(unsigned) * size, df);
      }
      if(strlen(tinfo_info[i].name))
      {
         pack_fwrite(&tinfo_info[i].name, sizeof(tinfo_info[i].name), df);
         tmp = TINFO_INFO;
         pack_fwrite(&(tmp), sizeof(tmp), df);
         size = tinfo_info[i].num;
         pack_fwrite(&(size), sizeof(size), df);
         pack_fwrite(tinfo_info[i].info, sizeof(unsigned) * size, df);
      }
   }

   pack_fclose (df);
}

void load_tinfo_file()
{
   char buf[255];

   if( load_singlemap(replace_extension(buf, tilegfx_file, "tnf", 255), NULL) )
   {
      ated_alert ("ERROR", "Can't open TileInfo File!", NULL, "Ohhh...", 0, 0, 0);
   }
   
   return;
}

void save_ated_temp_file()
{
   save_tempfile ("ated.tmp");
}


//
// Loading
//
static int load_bool(PACKFILE *file)
{
  short tmp; //= (var != NULL);

  pack_fread(&tmp, sizeof(tmp), file);

  //var = tmp;
  return ABS(tmp);
}

static void load_uint(void *var, PACKFILE *file)
{
  pack_fread(var, sizeof(var), file);
}
/*
static void load_char(void *var, PACKFILE *file)
{
  pack_fread(var, sizeof(var), file);
}
*/
void load_ated_temp_file()
{
   set_mouse_sprite(busy_pointer);
   busy_mouse = TRUE;
   
   if (!load_singlemap ("ated.tmp", NULL))
      load_tilegfx ();

   set_mouse_sprite(mouse_pointer);
   busy_mouse = FALSE;
}

/*
==================
=
= CarmackCompress
=
= Compress a string of words
=
==================
*/

#define NEARTAG	0xa7
#define FARTAG	0xa8
/*
long carmack_compress (unsigned far *source, long length, unsigned *dest)
{
	unsigned char ch, chhigh;
	unsigned char *instart, *inptr, *inscan, *stopscan, *outptr;
	unsigned char *bestscan, beststring;
	unsigned char dist, maxstring, string, outlength;
	unsigned char nearrepeats, farrepeats;
	unsigned char dups, count;
	unsigned char newwords;

        //
        // this compression method produces a stream of words
        // If the words high byte is NEARTAG or FARTAG, the low byte is a word
        // copy count from the a position specified by either the next byte
        // or the next word, respectively.  A copy count of 0 means to insert the
        // next byte as the low byte of the tag into the output
        //


        //
        // set up
        //
	instart = inptr = source;
	outptr = dest;

	outlength = 0;
	length = (length+1)/2;

	nearrepeats = farrepeats = dups = 0;
	//count = 10;
	newwords = 0;
        //
        // compress
        //
	do
	{
	  ch = *inptr;

          //
          // scan from start for patterns that match current data
          //
          beststring = 0;
          for (inscan = instart; inscan < inptr; inscan++)
          {
            if (*inscan != ch)
              continue;

            maxstring = inptr - inscan;

            if (maxstring > length)
              maxstring = length;

            if (maxstring > 255)
              maxstring = 255;

            string = 1;
            while (*(inscan + string) == *(inptr + string) && (string < maxstring))
              string++;

            if (string >= beststring)
            {
              beststring = string;
              bestscan = inscan;
            }
          }

          if ((beststring > 1) && (inptr-bestscan <= 255))
          {
            // near string
            *outptr++ = beststring + (NEARTAG * 256);
            *outptr++ = inptr - bestscan;
            // *((unsigned char *)outptr)++ = inptr - bestscan;
            outlength += 3;
            nearrepeats++;
            inptr += beststring;
            length -= beststring;
          }
          else if (beststring > 2)
          {
            // far string
            *outptr++ = beststring + (FARTAG * 256);
            *outptr++ = bestscan - instart;
            outlength += 4;
            farrepeats++;
            inptr += beststring;
            length -= beststring;
          }
          else
          {
            // no compression
            chhigh = ch >> 8;
            if (chhigh == NEARTAG || chhigh == FARTAG)
            {
              // special case of encountering a
              // tag word in the data stream
              // send a length of 0, and follow it with the real low byte
              *outptr++ = ch & 0xff00;
              *((unsigned char *)outptr)++ = ch&0xff;
              outlength += 3;
              dups++;
            }
            else
            {
              *outptr++ = ch;
              outlength += 2;
            }

            inptr++;
            length--;
            newwords++;
          }*/
          /*
          if (!--count)
          {
            static char cc[2] = "-";
            cc[0] ^= '+' ^ '-';
            print(cc);
            sx--;

            count = 10;

          }
          *//*
          if (key[KEY_ESC])
          {
            while(key[KEY_ESC]);
            return 0;
          }

          if (length < 0)
            fat_error("Length < 0!");
	} while (length);

	#if DEBUG
	printf ("%u near patterns\n", nearrepeats);
	printf ("%u far patterns\n",  farrepeats);
	printf ("%u new words\n",     newwords);
	printf ("%u dups\n\n",        dups);
	#endif

	return outlength;
}
*/

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

