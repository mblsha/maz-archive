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
 *   Initialization code, main DIALOGs and some other things are here
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 *
 *   29.10.01 - v0.2.0
 *     Added centre_printf()
 */

#include "ated.h"

char mouse_pointer_data[256] =
{
   1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   3,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
   3,4,5,1,0,0,0,0,0,0,0,0,0,0,0,0,
   3,2,4,5,1,0,0,0,0,0,0,0,0,0,0,0,
   3,2,4,4,5,1,0,0,0,0,0,0,0,0,0,0,
   3,2,2,4,4,5,1,0,0,0,0,0,0,0,0,0,
   3,2,2,4,4,4,5,1,0,0,0,0,0,0,0,0,
   3,2,2,2,4,4,4,5,1,0,0,0,0,0,0,0,
   3,2,2,2,4,4,5,5,5,1,0,0,0,0,0,0,
   3,2,2,4,4,3,1,1,1,0,0,0,0,0,0,0,
   3,2,4,1,2,4,1,0,0,0,0,0,0,0,0,0,
   3,4,1,0,1,2,4,1,0,0,0,0,0,0,0,0,
   0,1,0,0,1,2,4,1,0,0,0,0,0,0,0,0,
   0,0,0,0,0,1,2,4,1,0,0,0,0,0,0,0,
   0,0,0,0,0,1,4,4,1,0,0,0,0,0,0,0,
   0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0
};

char busy_pointer_data[256] =
{
   0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
   0,0,0,0,0,1,1,2,2,1,1,0,0,0,0,0,
   0,0,0,0,1,2,2,4,4,4,4,1,0,0,0,0,
   0,0,0,1,2,4,4,3,3,4,4,4,1,0,0,0,
   0,0,1,2,4,3,3,0,0,3,6,4,4,1,0,0,
   0,1,2,4,3,0,0,0,0,1,1,6,4,4,1,0,
   0,1,2,4,3,0,0,0,1,1,0,3,4,4,1,0,
   1,2,4,3,0,0,0,1,1,0,0,0,3,4,5,1,
   1,2,4,3,0,0,0,1,1,0,0,0,3,4,5,1,
   0,1,4,4,3,0,0,0,1,1,0,3,4,5,1,0,
   0,1,4,4,3,0,0,0,0,1,1,6,4,5,1,0,
   0,0,1,4,4,3,3,0,0,3,6,4,5,1,0,0,
   0,0,0,1,4,4,4,3,3,4,4,5,1,0,0,0,
   0,0,0,0,1,4,4,4,4,5,5,1,0,0,0,0,
   0,0,0,0,0,1,1,5,5,1,1,0,0,0,0,0,
   0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0
};
int busy_mouse = FALSE;

char mapwidth_string[16] = "";
char mapheight_string[16] = "";
/******************************************************************************************/
//extern unsigned _stklen=0x2000;

////////////////////////////////////////////////////
//
// Variables
//
////////////////////////////////////////////////////
//extern char far TEDCHAR,far VGAPAL,tdata;
extern unsigned doubled[256];

//
// CONFIGURATION
//
int      screen_h;
int      screen_w;
int      screen_bpp;

char     skin_file[256];
int      skin_num;

int      use_ated_temp_file;

//
// SKIN & COLORS
//
PALETTE  ated_current_palette;

BITMAP   *skin;
int      red;
int      green;
int      blue;
int      black;
int      white;
int      gray;

int      gui_col1;
int      gui_col2;
int      gui_col3;
int      gui_col4;
int      gui_col5;
int      gui_col6;
int      gui_col7;
int      gui_col8;
int      gui_col9;

int      BkgndColor;
int      BkgndColor_R, BkgndColor_G, BkgndColor_B;

UndoStr  UndoRegion;
CopyStr  TileCopy;
MapFileHeaderStr  *MapFileHeader;
char  *Tinfo[10], *TMinfo[10], *GraphHeader;
long  *XMSlookup, *EgaXMSlookup, *CgaXMSlookup, *VgaXMSlookup;

unsigned *MapBkgnd,  *MapFrgnd, *MapInfoPl;
unsigned *CutBkgnd,  *CutFrgnd, *CutInfoPl;

MapHeaderStr MapHeader;

//TempStruct LaunchInfo;
InfoStruct  *TEDInfo;
GfxStruct   *GFXInfo;
//video lastvideo,videomode;
//screentype whichscreen=TILES;
VMapStr    VMapData;

char launchname[64],ext[4],format[2],projname[64],mapname[64],planes,
     infoname[64],mapheadname[64],MapNames[100][16],parmstring[64];
char SM_name[64],SM_loadname[64],GfxToUse;

unsigned temp,whichmap,numtplanes,tilenum,tilemnum,numtmplanes,left,
	 DirtyFlag,tilelen,tilemlen,which_t,which_tm,which_i,
	 tsize,infoy,infomaxw,mapwidth,mapheight,screenw,usingbat,
	 screenh,planeton,planemon,planeion,maxiconrows,lasticon,firsticon,
	 viewton,viewmon,viewion,launched,
	 XMSmaps,EgaXMS,CgaXMS,VgaXMS,xmshandle,GridMode,TINFOMode,SnapMode,snapx,
	 snapy,snapxsize,snapysize,writeH,NoAbout,F3_flag;
unsigned *undoB,*undoF,*undoI;
int      tilebase=0,tilembase=0,tileibase=0,infobaron=1,xbase,ybase,scrnbot,scrnrgt,
	 FillMode=0,PasteMode=0,SelectMode=0,SelX1=-1,SelY1=-1,PasteOK=0,SelX2=-1,
	 SelY2=-1,pixelx,pixely,selectcols,px,py,lastmap=-1,TIybase,TIymbase,TIxbase,
	 TIxmbase,BfillMode,Plotting,TsearchMode,NoXMSFlag;
long     CgaXMSsize,EgaXMSsize,VgaXMSsize;
long     tics, tictime=1092L*2L;

/******************************************************************************************/
//#define USE_ALT_KEY

#ifdef  USE_ALT_KEY
     #define A(x)          0
     #define FNK           "ALT-"
#else
     #define A(x)          (x - 'a' + 1)
     #define FNK           "CTRL-"
#endif

#define DD      D_DISABLED
MENU AboutMenu[] = {
  /* (text)                       (proc)                     (child)         (flags)(dp)*/
  { "&About...",                  Item_About,                NULL,           0,     NULL },
  { "&Select Video Mode\tF9",     Item_SelectVideoMode,      NULL,           0,     NULL },
  //{ "Launch Project\t"FNK"L",     Item_Launch,               NULL,           0,     NULL },
  //{ "",                           NULL,                      NULL,           0,     NULL },
  //{ "Project Re-Select",          Item_ProjectReSelect,      NULL,           0,     NULL },
  //{ "Visit DOS",                  Item_VisitDOS,             NULL,           0,     NULL },
  { NULL,                         NULL,                      NULL,           0,     NULL }
};
MENU LoadMenu[] = {
  /* (text)                       (proc)                     (child)         (flags)(dp)*/
  { "ATED &TileSet",              Item_LoadATEDtilesetFile,  NULL,           DD,    NULL },
  { "ATED &SingleMap",            Item_LoadATEDsinglemapFile,NULL,           0,     NULL },
  { NULL,                         NULL,                      NULL,           0,     NULL }
};
MENU SaveMenu[] = {
  /* (text)                       (proc)                     (child)         (flags)(dp)*/
  { "ATED &TileSet",              Item_SaveATEDtilesetFile,  NULL,           DD,    NULL },
  { "ATED &SingleMap",            Item_SaveATEDsinglemapFile,NULL,           0,     NULL },
  { NULL,                         NULL,                      NULL,           0,     NULL }
};
MENU FileMenu[] = {
  /* (text)                       (proc)                     (child)         (flags)(dp)*/
  { "&New file",                  Item_NewFile,              NULL,           0,     NULL },
  { "&Load file",                 NULL,                      LoadMenu,       0,     NULL },
  { "&Save file",                 NULL,                      SaveMenu,       0,     NULL },
  { "Load tile &gfx file",        Item_LoadTileGFX,          NULL,           0,     NULL },
  { "",                           NULL,                      NULL,           0,     NULL },
  //{ "Edit New Map\t"FNK"O",       Item_EditMap,              NULL,           0,     NULL },
  //{ "Save Map\t"FNK"S",           Item_SaveMap,              NULL,           0,     NULL },
  //{ "Create Map\t"FNK"C",         Item_CreateMap,            NULL,           0,     NULL },
  //{ "Delete Map\t"FNK"D",         Item_DeleteMap,            NULL,           0,     NULL },
  //{ "Switch Map\t"FNK"W",         Item_SwitchMap,            NULL,           0,     NULL },
  //{ "Amputate Map",               Item_Amputate,             NULL,           0,     NULL },
  { "&Manage Maps",               Item_ManageMaps,           NULL,           0,     NULL },
  { "Manage &Planes",             Item_ManagePlanes,         NULL,           0,     NULL },
  { "",                           NULL,                      NULL,           0,     NULL },
  { "&Change Skin",               Item_Skin,                 NULL,           0,     NULL },
  { "&Assign Password\t"FNK"A",   Item_Pass,                 NULL,           DD,    NULL },
  //{ "",                           NULL,                      NULL,           0,     NULL },
  //{ "Import Maps",                Item_ImportMaps,           NULL,           0,     NULL },
  //{ "Change ICON Rows",           Item_ChangeIconRows,       NULL,           0,     NULL },
  //{ "Carmacize Maps",             Item_Huffman,              NULL,           0,     NULL },
  { "",                           NULL,                      NULL,           0,     NULL },
  { "&Quit "APP_NAME"\t"FNK"X",   Item_Quit,                 NULL,           0,     NULL },
  { NULL,                         NULL,                      NULL,           0,     NULL }
};
MENU EditMenu[] = {
  /* (text)                       (proc)                     (child)         (flags)(dp)*/
  //{ "&Switch to last Map\t"FNK"M",Item_LastMap,              NULL,           DD,    NULL },
  { "Edit TILEINFO &Values\t"FNK"T",Item_EditTinfoValues,    NULL,           0,     NULL },
  //{ "Change &LAUNCH name",        Item_LAUNCHname,           NULL,           DD,    NULL },
  //{ "Change &PARM string",        Item_PARMstring,           NULL,           DD,    NULL },
  { "Edit TILEINFO &Planes",      Item_AddDelTinfo,          NULL,           0,     NULL },
  //{ "Edit MAP Names",             Item_EditMapNames,         NULL,           0,     NULL },
  { "&Change MAP Edges",          Item_EditMapEdges,         NULL,           0,     NULL },
  { NULL,                         NULL,                      NULL,           0,     NULL }
};
MENU ModesMenu[] = {
  /* (text)                       (proc)                     (child)         (flags)(dp)*/
  { "&Copy Mode\tC",              Item_Copy,                 NULL,           0,     NULL },
  { "&Paste Mode\tP",             Item_Paste,                NULL,           0,     NULL },
  { "&Block Fill\tB",             Item_BlockFill,            NULL,           0,     NULL },
  { "&Flood Fill\tF",             Item_FloodFill,            NULL,           0,     NULL },
  { "&Undo last action\tU",       Item_Undo,                 NULL,           0,     NULL },
  { "&Tile Search\tT",            Item_TileSearch,           NULL,           0,     NULL },
  { "&TINFO Edit Mode\tN",        Item_TINFOMode,            NULL,           0,     NULL },
  { "&GridMode toggle\tG",        Item_GridMode,             NULL,           0,     NULL },
  { "&Snap-Paste toggle\tS",      Item_SnapTog,              NULL,           0,     NULL },
  { "Paste &Overlay toggle\tF3",  Item_POtog,                NULL,           0,     NULL },
  { NULL,                         NULL,                      NULL,           0,     NULL }
};
MENU MiscMenu[] = {
  /* (text)                       (proc)                     (child)         (flags)(dp)*/
  { "&Select Tile\tSPACE",        Item_SelectTile,           NULL,           0,     NULL },
  { "Map &Stats\tI",              Item_MapStats,             NULL,           0,     NULL },
  { "Toggle &INFOBAR\tF8",        Item_ToggleInfo,           NULL,           0,     NULL },
  { "&New INFOPLANE value\tENTER",Item_InputInfoplane,       NULL,           0,     NULL },
  { "&View Map && Goto\t"FNK"V",  Item_ViewMap,              NULL,           0,     NULL },
  { "&ReView Map && Goto\tBKSP",  Item_ReviewMap,            NULL,           0,     NULL },
  //{ "Change LAUNCH icon",         Item_ChangeLaunchIcon,     NULL,           0,     NULL },
  { "&Change bkgnd color",        Item_ChangeBkgndColor,     NULL,           0,     NULL },
  { "&TILEINFO/M Copy",           Item_TINFOcopy,            NULL,           DD,    NULL },
  //{ "Graphics Map Dump",          Item_GraphicDump,          NULL,           0,     NULL },
  { NULL,                         NULL,                      NULL,           0,     NULL }
};
MENU MenuBar[] = {
  /* (text)                       (proc)                     (child)         (flags)(dp)*/
  { "?",                          NULL,                      AboutMenu,      0,     NULL },
  { "&File",                      NULL,                      FileMenu,       0,     NULL },
  { "&Edit",                      NULL,                      EditMenu,       0,     NULL },
  { "&Modes",                     NULL,                      ModesMenu,      0,     NULL },
  { "M&isc",                      NULL,                      MiscMenu,       0,     NULL },
  { NULL,                         NULL,                      NULL,           0,     NULL }
};
#undef DD

#define C(x)      (x - 'a' + 1)
#define A(x)      (x - 'a' + 1) //‘(x)
int dummy()
{
   return D_O_K;
}

int screenshot()
{
   int     i;
   char    pcxname[80];
   BITMAP  *scr;

   //
   // find a file name to save it to
   //
   strcpy(pcxname, "ated_00.pcx");
		
   for (i = 0; i <= 99; i++)
   {
      pcxname[5] = i/10 + '0';
      pcxname[6] = i%10 + '0';

      if (!exists(pcxname))
         break;	// file doesn't exist
   }

   if (i == 100)
   {
      ated_alert("Couldn't create a PCX file!", "You have alredy too many", "screenshots!",
                 "I know...", 0, 0, 0);
      return D_O_K;
   }

   //
   // Save screen
   //
   scr = create_bitmap(SCREEN_W, SCREEN_H);
   blit(screen, scr, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   save_pcx(pcxname, scr, ated_current_palette);
   destroy_bitmap(scr);
   return D_O_K;
}

DIALOG ATED_MAIN[] =
{
  /* (dialog proc)  (x)           (y)  (w)  (h)  (fg)  (bg)  (key)  (flags)  (d1)         (d2)       (dp)                  (dp2)  (dp3) */
  //{ d_clear_proc,   0,            0,   0,   0,   15,   8,    0,     0,       0,           0,         NULL,          NULL,  NULL  },
  //{ d_clear_proc,   0,            0,   0,   0,   15,   8,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  //{ draw_rect_proc, 0,            0,SCREEN_W,11, 15,   15,   0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  //{ d_clear_proc,   0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { draw_infobar_proc,0,          0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { draw_map_proc,  0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { menu,           0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         MenuBar,              NULL,  NULL  },

  //{ mouse_b_proc,   0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { scroll_map_proc,0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { toggle_layers_proc,0,         0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },
  { check_for_keys_proc,0,        0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  },

  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_F9,      0,         Item_SelectVideoMode, NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('l'),0,       0,           0,         Item_Launch,          NULL,  NULL  },
  //{ d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('o'),0,       0,           0,         Item_EditMap,         NULL,  NULL  },
  //{ d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('s'),0,       0,           0,         Item_SaveMap,         NULL,  NULL  },
  //{ d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('c'),0,       0,           0,         Item_CreateMap,       NULL,  NULL  },
  //{ d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('d'),0,       0,           0,         Item_DeleteMap,       NULL,  NULL  },
  //{ d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('w'),0,       0,           0,         Item_SwitchMap,       NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('a'),0,       0,           0,         Item_Pass,            NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('x'),0,       0,           0,         Item_Quit,            NULL,  NULL  },

  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    'c',   0,       0,           0,         Item_Copy,            NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    'p',   0,       0,           0,         Item_Paste,           NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    'b',   0,       0,           0,         Item_BlockFill,       NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    'f',   0,       0,           0,         Item_FloodFill,       NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    'u',   0,       0,           0,         Item_Undo,            NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    't',   0,       0,           0,         Item_TileSearch,      NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    'g',   0,       0,           0,         Item_GridMode,        NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    's',   0,       0,           0,         Item_SnapTog,         NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_F3,      0,         Item_POtog,           NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    'i',   0,       0,           0,         Item_MapStats,        NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    'n',   0,       0,           0,         Item_TINFOMode,       NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_F8,      0,         Item_ToggleInfo,      NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    A('v'),0,       0,           0,         Item_ViewMap,         NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_BACKSPACE,0,        Item_ReviewMap,       NULL,  NULL  },

  //{ d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_SPACE,   0,         Item_SelectTile,      NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_ESC,     0,         dummy,                NULL,  NULL  },

  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_F12,     0,         screenshot,           NULL,  NULL  },
  { d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_PRTSCR,  0,         screenshot,           NULL,  NULL  },
  //{ d_keyboard_proc,0,            0,   0,   0,   0,    0,    0,     0,       KEY_ENTER,   0,         Item_InputInfoplane,  NULL,  NULL  },

  { NULL,           0,            0,   0,   0,   0,    0,    0,     0,       0,           0,         NULL,                 NULL,  NULL  }
};

/*----------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
   ParseCmdLine(argc, argv);

   InitATED();

   main_menu();

   if(shut_down())
   {  // Error!
      error(ERR_UNLOAD);
      return 666;
   }

   printf("Thanks for using " APP_NAME "\n");
   return 0;
}
END_OF_MAIN();
/*----------------------------------------------------------------------------------------*/
void ParseCmdLine(int _argc, char *_argv[])
{
   int i;

   for (i=1;i<_argc;i++)
   {
      _argv[i]=strupr(_argv[i]);

      if (_argv[i][0]=='-' || _argv[i][0]=='/')
        _argv[i]++;

      if (!strcmp(_argv[i],"?"))
      {
         printf("Allegro Tile EDitor v"APP_VER" (C) 2000-2002 MAZsoft\n\n");
         printf("Command Line parameters:\n");
         printf("/?              : gets this stuff\n");
         printf("/NOABOUT        : don't display about on startup\n");

         exit(0);
      }
      else if(!strcmp(_argv[i],"NOABOUT"))
      {
  	      NoAbout=1;
      } 
   }
}
/*----------------------------------------------------------------------------------------*/
void main_menu(void)
{
   clear(screen);

   do_ated_dialog(ATED_MAIN,-1);
}
/*----------------------------------------------------------------------------------------*/
void InitATED(void)
{
   //int i;
   DATAFILE *tmp;
   BITMAP   *bmp;
   PALETTE  pal;
   int      count;
   char buf[64];

   init();

   get_palette(pal);
   set_palette(pal);
   init_colors();
   clear_to_color(screen, makecol(128,128,128));
   textout_centre(screen, font, "Initializing. One moment.", SCREEN_W/2, (SCREEN_H-text_height(font))/2, makecol(255,255,255));

   show_mouse(screen);
   set_mouse_sprite(busy_pointer);
   busy_mouse = TRUE;

   convert_images_in_datafile(ateddata);

   skin_init ();
   update_skin ();

   if(use_ated_temp_file)
   {
      load_ated_temp_file();
   }

   set_mouse_sprite(mouse_pointer);
   busy_mouse = FALSE;

   tsize = 16;

   clear_to_color(screen, makecol(128,128,128));

   if (!NoAbout)
      Item_About();
}

#define TEXT_CONSOLE_WIDTH      80
void centre_printf(const char *fmt, ...)
{
   va_list   argptr;
   char      msg[TEXT_CONSOLE_WIDTH+1];
   int       length;
   int       i, l;

   va_start (argptr,fmt);
   vsprintf (msg,fmt,argptr);
   va_end   (argptr);

   length = strlen(msg);

   l = (TEXT_CONSOLE_WIDTH - length) / 2;

   for(i = 0; i < l; i++)
      printf(" ");

   printf(msg);

   printf("\n");
}
/*----------------------------------------------------------------------------------------*/
void init(void)
{
   int i;
   
   for(i = 0; i < TEXT_CONSOLE_WIDTH; i++)
      printf("Ä");

   //
   // Print some info...
   //
   centre_printf (APP_FULL_NAME" v"APP_VER);
   centre_printf ("%s", __DATE__", "__TIME__", "ALLEGRO_PLATFORM_STR);
   centre_printf ("(c) MAZsoft 2000-2002");
   
   for(i = 0; i < TEXT_CONSOLE_WIDTH; i++)
      printf("Ä");

   set_uformat (U_ASCII);

   /*-----------------------------Initialization Process-----------------------------------*/
   //datedit_init();
   printf (INIT_ALLEG);
   if     (allegro_init())
      fatal_error(allegro_error);
   printf (INIT_DONE"\n");

   printf (INIT_LOAD);
   set_config_file("ated.cfg");
   load_cfg();
   printf (INIT_DONE"\n");

   //printf (INIT_DATAFILE);
   //if(!(ateddata = load_datafile("ated.dat")))
   //  fat_error(ERR_READ_FILE "\"ated.dat\"");
   //printf (INIT_DONE"\n");
   /*
   printf (INIT_SOUND);
   if     (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL))
      error(allegro_error);
   else
      printf (INIT_DONE"\n");
   */
   printf (INIT_MOUSE);
   if     (install_mouse() < 0)
      error(ERR_MOUS_NF);
   else
      printf(INIT_DONE"\n");

   printf (INIT_TIMER);
   install_timer();
   printf (INIT_DONE"\n");

   printf (INIT_KEYB_H);
   install_keyboard();
   printf (INIT_DONE"\n");

   printf (INIT_GFX);

   rest   (1000);                                         /* Wait 1 second */

   set_color_conversion (COLORCONV_TOTAL | COLORCONV_DITHER | COLORCONV_KEEP_TRANS);

   set_color_depth(screen_bpp);

   if ( set_gfx_mode (GFX_AUTODETECT, screen_w, screen_h, 0, 0) )
      fatal_error (allegro_error);
      
   if ( !(ateddata = load_datafile ("ated.dat")) )
      fat_error (ERR_READ_FILE "\"ated.dat\"");

   text_mode(-1);                                         // Set transparent textmode

   font = ateddata[ARIAL8B].dat;
   f8x8 = ateddata[FONT8X8].dat;
   smallfont = ateddata[FONT6X6].dat;
}
/*----------------------------------------------------------------------------------------*/
extern int tile_select_w;
void load_cfg(void)
{
   screen_w      = get_config_int("ated", "screen_width",                640);
   screen_h      = get_config_int("ated", "screen_height",               480);
   screen_bpp    = get_config_int("ated", "screen_bpp",                  8);

   strcpy (skin_file, get_config_string    ( "ated", "skin_file",    "" ));
   
   GridMode      = get_config_int("ated", "grid_on",                     0);
   SnapMode      = get_config_int("ated", "snap_on",                     0);
   
   tile_select_w = get_config_int("ated", "tile_select_w",               0);
   
   strcpy (tilegfx_path, get_config_string ( "ated", "tilegfx_path", "" ));
   
   BkgndColor_R  = get_config_int("ated", "back_col_r",                  255);
   BkgndColor_G  = get_config_int("ated", "back_col_g",                  0);
   BkgndColor_B  = get_config_int("ated", "back_col_b",                  255);

   use_ated_temp_file = get_config_int("ated", "use_ated_temp_file",     0);

   //tilegfx_file  = get_config_string("ated", "tile_gfx_file",    NULL);
   //tiledata_file = get_config_string("ated", "tile_map_file",    NULL);
}
/*----------------------------------------------------------------------------------------*/
void save_cfg(void)
{
   set_config_int("ated", "screen_width",                        screen_w);
   set_config_int("ated", "screen_height",                       screen_h);
   set_config_int("ated", "screen_bpp",                          screen_bpp);

   set_config_string ( "ated", "skin_file", skin_file );
   
   set_config_int("ated", "grid_on",                             GridMode);
   set_config_int("ated", "snap_on",                             SnapMode);

   set_config_int("ated", "tile_select_w",                       tile_select_w);
   
   set_config_string ( "ated", "tilegfx_path", tilegfx_path );
   
   set_config_int("ated", "back_col_r",                          BkgndColor_R);
   set_config_int("ated", "back_col_g",                          BkgndColor_G);
   set_config_int("ated", "back_col_b",                          BkgndColor_B);

   set_config_int("ated", "use_ated_temp_file",                  use_ated_temp_file);
   //set_config_string("ated", "tile_gfx_file",                    tilegfx_file);
   //set_config_string("ated", "tile_map_file",                    tiledata_file);
}
/*----------------------------------------------------------------------------------------*/
int shut_down(void)
{
   if(use_ated_temp_file)
   {
      save_ated_temp_file();
   }

   set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);

   //destroy_bitmap(coll_page);
   //destroy_bitmap(aux_page);

   //
   // Unload Map Planes
   //
   create_planes(FALSE, FALSE, FALSE);
   //
   // Unload DataFile
   //
   if(tiledata)
      unload_datafile(tiledata);
   if(tilegfx)
      unload_datafile(tilegfx);
   //
   // Unload Bitmaps
   //
   if(skin)
      destroy_bitmap(skin);

   save_cfg();

   allegro_exit();

   return 0;
}
/*----------------------------------------------------------------------------------------*/
void fatal_error(char msg[])                //Close program while in text mode
{
  //set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
  printf (ERR_FATAL":\n%s\n", msg);
  exit         (1);
}
/*----------------------------------------------------------------------------------------*/
void fat_error(char msg[])                  //Close program & shut down all
{
   shut_down();
   allegro_exit();

   allegro_message (ERR_FATAL": %s\n", msg);

   exit         (1);
}
/*----------------------------------------------------------------------------------------*/
void error(char msg[])                      //Error message
{
   printf       (ERR_ERR":\n%s\n", msg);
}
/*----------------------------------------------------------------------------------------*/
void clear_kbuf(void)
{
   while(keypressed())
      readkey();
}
/*----------------------------------------------------------------------------------------*/
void wait_for_keys()
{
   vsync();

   while(key[KEY_ESC] || key[KEY_ENTER] || key[KEY_SPACE]);
}
/*----------------------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////
//
// print hex byte
//
////////////////////////////////////////////////////////////////////
void printhexb(BITMAP *bmp, unsigned char value, int x, int y, int col)
{
   int loop;
   char hexstr[16]="0123456789ABCDEF", str[2];

   for (loop = 0; loop < 2; loop++)
   {
      str[0] = hexstr[((value >> ((1 - loop) * 4))) & 15];
      str[1] = 0;
      print(bmp, str, x, y);
      x += 8;
   }
}

////////////////////////////////////////////////////////////////////
//
// print hex
//
////////////////////////////////////////////////////////////////////
void printhex(BITMAP *bmp, unsigned value, int x, int y, int col)
{
   print     (bmp, "$", x, y);

   printhexb (bmp, (unsigned char)value >> 8,   x + 8,  y, col);
   printhexb (bmp, (unsigned char)value & 0xff, x + 24, y, col);
}


////////////////////////////////////////////////////////////////////
//
// print int
//
////////////////////////////////////////////////////////////////////
void printint(BITMAP *bmp, unsigned value, int x, int y, int col)
{
   char temp[10];
   
   //ltoa ((unsigned long)value, temp, 10);
   sprintf (temp, "%d", value);
   print (bmp, temp, x, y);
}


////////////////////////////////////////////////////////////////////
//
// print bin
//
////////////////////////////////////////////////////////////////////
void printbin(BITMAP *bmp, unsigned value, int x, int y, int col)
{
   int loop;

   textout(bmp, f8x8, "%", x, y, col);
   for (loop=0;loop<16;loop++)
   {
      x += 8;
      print(bmp, (((value>>15)-loop)&1) ? "1" : "0", x, y);
      //textout(bmp, f8x8, (((value>>15)-loop)&1) ? "1" : "0", x, y, col);
   }
}

/*void SEND_MESSAGE_TO_ALL(DIALOG *d[], int msg, int c, int num) {
  int i;

  while((i++ < num) && (d[i]->proc != NULL)) {
    SEND_MESSAGE(d[i], msg, c);
  }
}*/

void sel_palette(RGB *pal)
{
   int c, x, y;

   if(pal) 
   {
      memcpy(ated_current_palette, pal, sizeof(PALETTE));
      set_palette(ated_current_palette);
   }

   gui_fg_color = makecol(0, 0, 0);
   gui_mg_color = makecol(0x80, 0x80, 0x80);
   gui_bg_color = makecol(0xFF, 0xFF, 0xFF);

   if(mouse_pointer)
      destroy_bitmap(mouse_pointer);
   if(busy_pointer)
      destroy_bitmap(busy_pointer);

   mouse_pointer = create_bitmap(16, 16);
   busy_pointer  = create_bitmap(16, 16);

   for (y=0; y<16; y++) 
   {
      for (x=0; x<16; x++) 
      {
	      switch (mouse_pointer_data[x+y*16]) 
         {
	         case 1:  c = black; break;
	         case 2:  c = white; break;
	         case 3:  c = gui_col3; break;
	         case 4:  c = gui_col1; break;
	         case 5:  c = gui_col6; break;
	         case 6:  c = gui_col5; break;
	         default: c = screen->vtable->mask_color; break;
	      }
	      putpixel(mouse_pointer, x, y, c);

	      switch (busy_pointer_data[x+y*16]) 
         {
	         case 1:  c = black; break;
	         case 2:  c = white; break;
	         case 3:  c = gui_col3; break;
	         case 4:  c = gui_col1; break;
	         case 5:  c = gui_col6; break;
	         case 6:  c = gui_col5; break;
	         default: c = screen->vtable->mask_color; break;
	      }
	      putpixel(busy_pointer, x, y, c);
      }
   }

   set_mouse_sprite(mouse_pointer);
   busy_mouse = FALSE;
}

void init_colors()
{
   BkgndColor = makecol(BkgndColor_R, BkgndColor_G, BkgndColor_B);//13;
   red        = makecol(255,  0,  0);
   green      = makecol(  0,255,  0);
   blue       = makecol(  0,  0,255);

   white      = makecol(255,255,255);
   black      = makecol(  0,  0,  0);
   gray       = makecol(128,128,128);

   gui_col1   = makecol(196,196,196);
   gui_col2   = makecol(128,128,128);
   gui_col3   = makecol( 64, 64, 64);

   gui_col4   = makecol(196,128, 96);

   gui_col5   = makecol( 32, 32, 32);
   gui_col6   = makecol(150,150,150);
/*
   gc_titlebar_n1  = makecol(  0, 72,186);
   gc_titlebar_n2  = makecol(  0, 44,117);
   gc_titlebar_d1  = makecol(150,150,150);
   gc_titlebar_d2  = makecol( 50, 50, 50);
   gc_titlecol_n1  = makecol(255,255,255);
   gc_titlecol_n2  = makecol(  0,  0,  0);
   gc_titlecol_d1  = makecol(150,150,150);
   gc_titlecol_d2  = makecol(  0,  0,  0);
   gc_widget_n1    = makecol(196,196,196);
   gc_widget_n2    = makecol(128,128,128);  
   gc_widget_n3    = makecol( 64, 64, 64);
   gc_widget_d1    = makecol(196,196,196);
   gc_widget_d2    = makecol(128,128,128);  
   gc_widget_d3    = makecol( 64, 64, 64);
   gc_textcol_n1   = makecol(255,255,255);
   gc_textcol_n2   = makecol(  0,  0,  0);
   gc_textcol_d1   = makecol( 64, 64, 64);
   gc_textcol_d2   = makecol(196,196,196);
   gc_textcol_h1   = makecol(255,255,255);
   gc_textcol_h2   = makecol(  0,  0,  0);
   gc_textcol_s1   = makecol(255,255,255);
   gc_textcol_s2   = makecol(  0,  0,  0);  
   gc_textcol_c1   = makecol(255,255,255);
   gc_textcol_c2   = makecol(  0,  0,  0);
   gc_iconcol_n1   = makecol(196,128, 96);  
   gc_clientcol_n1 = makecol(196,128, 96);
   gc_clientcol_d1 = makecol(128,128,128);
   gc_hovercol_n1  = makecol(  0, 44,117);
   gc_selectcol_n1 = makecol( 64, 64, 64);
   gc_glasscol_n1  = makecol(  0,  0,  0);
   gc_black        = makecol(  0,  0,  0);
   gc_white        = makecol(255,255,255);
*/
   gui_fg_color = black; gui_bg_color = white;

   sel_palette (NULL);
}
