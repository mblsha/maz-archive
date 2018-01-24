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
 *   Main header file, all definitions, macroses, etc. are here
 *
 * CHANGES:
 *
 *   01.06.00
 *     0.1.0 beta: Initial release
 *
 *   15.07.01
 *     0.1.0 beta: Added tiles' color depth conversion routines
 *
 *   09.06.01
 *     0.1.0 beta: Added "char *skin_file", that should contain skin file name
 *
 *   10.06.01
 *     0.1.0 beta: Adding optional saving to "ated.tmp"
 *
 *   29.10.01
 *     0.2.0 beta: Added "View Map & Goto", "Review Map & Goto" and "Map Info"
 *                 do_viewmap(), Item_ViewMap(), Item_ReviewMap(), Item_MapStats()
 *
 *                 Added centre_printf()
 *
 *   12.12.01
 *     0.2.1 beta: MinGW32 Port + Some bug fixes
 *
 *   04.01.02
 *     0.2.2 beta: Rewritten ViewMap code, TINFO Edit Mode (no good yet) + Another bugfixes
 *
 *                 This is the last "pure C" version
 *                 I will rewrite the planes' and tinfo code to use advantages of C++
 *
 *   13.02.02
 *     0.3.0 beta: Rewritten singlemap file format for a more flexibility.
 *                 Maps are now stored in optimized form, taking approx. three time
 *                 less disk space
 *                 P.S. This isn't the last pure "C" release :-)
 *                      Rewritten version will come later (if i'll finish it)
 *
 *   23.02.02
 *     0.3.1 beta: Rewritten skin loading process:
 *                   * No more crashes on Windows ;-)
 *                   * Flexible skin support -- can handle any amount of image files
 *                     in "Skins/" directory
 *
 */
 
#ifndef __ATED_MAIN_HEADER__
#define __ATED_MAIN_HEADER__

////////////////////////////////////////////////////////////////////////////////////////////

#define APP_NAME        "ATED"
#define APP_FULL_NAME   "Allegro Tile EDitor"
#define APP_VER         "0.3.1"

#define ATED_TILE_EDITOR

////////////////////////////////////////////////////////////////////////////////////////////

#include <allegro.h>
#include <stdio.h>
#include <string.h>

#include <allegro/internal/aintern.h>
//#include <datedit.h>

#include "data.h"
#include "menu.h"

#define  LANGUAGE_ENGLISH
#include "lang.h"

#define DEBUG

#ifdef DEBUG
     #define D(x)     (x)
#else
     #define D(x)     /* empty */
#endif

//
// CONFIGURATION
//
extern int      screen_h;
extern int      screen_w;
extern int      screen_bpp;

// Background "skin"
extern BITMAP   *skin;
extern char     skin_file[256];
extern int      skin_num;

extern int      use_ated_temp_file;

extern char     tilegfx_path[256];

extern int      viewmap_needs_update;

//
// GUI
//
#define TEXT_OUTLINE_4          1
#define TEXT_OUTLINE_2          2
#define print(bmp,s,x,y)        gui_textout_highlight((bmp), (s), (x), (y), gray, white, black, FALSE, TEXT_OUTLINE_4, f8x8)
//#define alert                   ated_alert

// My custom messages
#define MSG_SCREEN_RESIZE                       25
#define MSG_UPDATE_INFO                         26
#define MSG_UPDATE                              27

// This is to say button, that its always unfocusable
#define D_NOFOCUS                               512


//
// DATA
//
//int DirtyFlag;
DATAFILE *ateddata;
DATAFILE *tiledata;
DATAFILE *tilegfx;
DATAFILE *gfx_t;
DATAFILE *gfx_tm;
DATAFILE *gfx_i;
FONT     *f8x8;
FONT     *smallfont;

//
// MISC
//
extern PALETTE ated_current_palette;

// How many tiles in tilegfx file
extern unsigned gfx_t_count, gfx_tm_count, gfx_i_count;

// Mouse pointer data
extern char mouse_pointer_data[256];
extern char busy_pointer_data[256];
BITMAP     *busy_pointer;
BITMAP     *mouse_pointer;
extern int  busy_mouse;

//struct ffblk ffblk;

typedef struct TILE
{
   unsigned int tile;
} TILE;

#define TILESET_NONE                    0
#define TILESET_BACK                    1
#define TILESET_FORE                    2
#define TILESET_INFO                    3
#define TILESET_PARALLAX                4

#define TYPE_NONE                       0
#define TYPE_BACK                       1
#define TYPE_FORE                       2
#define TYPE_INFO                       3
#define TYPE_PARALLAX                   4

typedef struct LAYER
{
   TILE  *layer;
   char  name[16];
   short tileset;
   short type;
} LAYER;

LAYER *layers;

//
// Thats for datafile-writing
//
typedef struct DATAITEM
{
   DATAFILE *dat;
   DATAFILE **parent;
   int i;
   char name[128];
} DATAITEM;

#define DAT_XSIZE                               DAT_ID('X','S','I','Z')
#define DAT_YSIZE                               DAT_ID('Y','S','I','Z')

#define DAT_ATED_HEADER                         DAT_ID('A','T','E','D')
#define DAT_ATED_TILESET_NAME                   DAT_ID('N','A','M','E')
#define DAT_ATED_TILESET_AUTHOR                 DAT_ID('A','U','T','H')
#define DAT_ATED_TILEGFX_FILE                   DAT_ID('T','N','A','M')

//#define DAT_TILE_MAP                            DAT_ID('T','M','A','P')
//#define DAT_TILE_MAP_HEADER                     DAT_ID('M','H','E','D')
#define DAT_TILE_MAP                            DAT_ID('F','I','L','E')
#define DAT_TILE_MAP_NAME                       DAT_ID('N','A','M','E')
#define DAT_TILE_MAP_WIDTH                      DAT_ID('X','S','I','Z')
#define DAT_TILE_MAP_HEIGHT                     DAT_ID('Y','S','I','Z')

#define DAT_BKGND_PLANE                         DAT_ID('P','B','G','D')
#define DAT_FRGND_PLANE                         DAT_ID('P','F','G','D')
#define DAT_INFO_PLANE                          DAT_ID('P','I','N','F')

#define DAT_TILEINFO                            DAT_ID('T','I','N','F')
#define DAT_TILEINFOM                           DAT_ID('T','I','N','M')
#define DAT_TILEINFOI                           DAT_ID('T','I','N','M')

//static char info_msg[] = "DO NOT DELETE!";//"For internal use by the ATED";
//static char grabber_info_msg[] = "For internal use by the grabber";
extern DATAFILE datedit_info;
extern DATAFILE ated_header;
//extern DATAFILE map_header;
extern DATAFILE current_map;

//
// COLORS
//
#ifdef  WHITE
#undef  WHITE
#define WHITE makecol(255,255,255)
#endif
#ifdef  GRAY
#undef  GRAY
#definw GRAY makecol(255,255,255)
#endif
#ifdef  BLACK
#undef  BLACK
#define BLACK makecol(0,0,0)
#endif
extern int      red;
extern int      green;
extern int      blue;
extern int      black;
extern int      white;
extern int      gray;
extern int      gui_col1;
extern int      gui_col2;
extern int      gui_col3;
extern int      gui_col4;
extern int      gui_col5;
extern int      gui_col6;
extern int      gui_col7;
extern int      gui_col8;
extern int      gui_col9;

// Background color
extern int      BkgndColor;
extern int      BkgndColor_R, BkgndColor_G, BkgndColor_B;

#define BOX_W     MIN(512, SCREEN_W-16)
#define BOX_H     MIN(256, (SCREEN_H-64)&0xFFF0)

#define BOX_L     ((SCREEN_W - BOX_W) / 2)
#define BOX_R     ((SCREEN_W + BOX_W) / 2)
#define BOX_T     ((SCREEN_H - BOX_H) / 2)
#define BOX_B     ((SCREEN_H + BOX_H) / 2)
extern int box_x;
extern int box_y;
extern int box_active;


// Thats for listbox gui proc
typedef struct INDEX_LIST
{
   char name[128];
   int  index;
} INDEX_LIST;

typedef struct TILEINFO
{
   char         name[32];               // Name of TileInfo
   //int          type;                   // Type of TileInfo
   int          num;                    // Number of INFOs

   unsigned     *info;                  // Array of numbers
} TILEINFO;

#define MAX_TINFO       100

#define TINFO_BACK              1
#define TINFO_FORE              2
#define TINFO_INFO              3

extern TILEINFO tinfo_back[MAX_TINFO];
extern TILEINFO tinfo_fore[MAX_TINFO];
extern TILEINFO tinfo_info[MAX_TINFO];

//
// STRINGS
//
extern char tileset_name[128];
extern char author_name[128];

extern char tiledata_file[256];
extern char tilegfx_file[256];
extern char password[256];

extern char mapwidth_string[16];
extern char mapheight_string[16];

extern char map_name[128];
extern char map_name_string[128];

//
// Defs for TED5
//

typedef enum {TILES,MASKED,ICONS} screentype;
typedef enum {DATA,CODE,FARDATA} segtype;	// FOR MAKEOBJ ONLY

#define SCindex		0x3C4
#define SCmapmask	2
#define GCindex		0x3CE
#define GCreadmap	4
#define GCmode		5
#define crtcaddr	0x3d4

//
// STRUCTURE OF THE "TEDINFO.EXT" FILE THAT TED5 CREATES
//
typedef struct {
		int  level,          lastvid,       lastx,         lasty, tsize;
		int  OldCgaXMS,      OldEgaXMS,     OldVgaXMS;
		long OldCgaXMSsize,  OldEgaXMSsize, OldVgaXMSsize;
		int  CgaXMSlook,     EgaXMSlook,    VgaXMSlook;
		char permicon,       pflags;
		int  oscrx,          oscry;
		char parmstring[64], launchname[14];
		char BackgndColor;
		char ImportPath[64];
	       } InfoStruct;

//
// STRUCTURE OF THE "GFXINFO?.EXT" FILE THAT IGRAB CREATES
//
typedef struct {
		 int num8,       num8m,      num16,      num16m, num32, num32m;
		 int off8,       off8m,      off16,      off16m, off32, off32m;
		 int numpics,    numpicm,    numsprites;
		 int offpic,     offpicm,    offsprites;
		 int offpicstr,  offpicmstr, offsprstr;
		 int numexterns, offexterns;
	       } GfxStruct;

//
// TED5 LOADS & SAVES THIS HEADER FOR INTERNAL MAPFILE USAGE
//
typedef struct { unsigned maptype;	//bit 0=bkgnd/1=frgnd/2=info
		 unsigned tsize;	//1=8/2=16/3=32

		 unsigned numtplanes,           oldtilenum;
		 long     tileinfooff[10];
		 unsigned tileinfolen[10];
		 char     tnames[10][8];

		 unsigned numtmplanes,          oldtilemnum;
		 long     tileinfomoff[10];
		 unsigned tileinfomlen[10];
		 char     tmnames[10][8];

		 unsigned RLEWtag;
		 long     dataoffsets[100];
		 long     datalengths[100];

		 int NumIconRows;
	       } MapFileHeaderStr;

//
// TED5 SAVES THIS MAPFILE HEADER FOR THE GAME
//
typedef struct {
		 unsigned RLEWtag;
		 long     dataoffsets[100];

	       } OutputHeadStr;

//
// EACH AND EVERY MAP HAS THIS HEADER (IF THE MAP EXISTS)
//
typedef struct { long            mapbkgndpl;
		 long            mapfrgndpl;
		 long            mapinfopl;
		 unsigned        mapbkgndlen;
		 unsigned        mapfrgndlen;
		 unsigned        mapinfolen;
		 unsigned        width,         height;
		 char            name[16];
	       } MapHeaderStr;

//
// SPECIFY WHAT, WHEREFROM, AND HOW TO COPY A REGION
//
typedef struct
{
   //char PlanesCopied;	// use BPLANE,FPLANE,IPLANE to mask
   int  back;
   int  fore;
   int  info;
   int  MapOrTileSelect;	// 0:map,1:tileselect
   int  tile_select_w;
   int  tile_select_h;
   int  x,y,w,h;		// from map or tileselect
} CopyStr;

//
// UNDO REGION
//
typedef struct
{
   int x, y, w, h;
} UndoStr;

//
// LAST-BUILT "VIEWMAP"
//
typedef struct {
		unsigned step,built_flag,EGAseg,maxpack;
	       } VMapStr;


//
// HEADER FOR APPLE-PREFERRED FILES
//
typedef struct { long length;
		 char Kind[5];
		 int  MasterMode;
		 int  PixelsPerLine;
		 int  NumColorTables;
		 int  ColorTable[16];
		 int  NumScanLines;
	       } ApPrefStr;

#define CREATED		1
#define NOTCREATED 	2
#define ANYLIST		3

#define BPLANE		1
#define FPLANE		2
#define IPLANE		4

#define O_FGNDBACK	4
#define ICONBACK	3

#define TINFOWIDTH	3
#define TINFOHEIGHT	7
#define TINFOHEIGHTEGA2	25

//extern MBarDef TED5MenuBar[];
//extern DialogDef DoCreated;

extern UndoStr UndoRegion;
extern CopyStr TileCopy;

extern MapFileHeaderStr  *MapFileHeader;
extern char  *Tinfo[10], *TMinfo[10], *GraphHeader;
extern long  *XMSlookup, *CgaXMSlookup, *EgaXMSlookup, *VgaXMSlookup;

// Map data
extern unsigned *MapBkgnd, *MapFrgnd, *MapInfoPl;
// Copy data
extern unsigned *CutBkgnd, *CutFrgnd, *CutInfoPl;
// Undo data
extern unsigned *undoB,*undoF,*undoI;

extern MapHeaderStr MapHeader;

//extern TempStruct LaunchInfo;
extern InfoStruct  *TEDInfo;
extern GfxStruct   *GFXInfo;
//extern GfxStruct   *GfxInfo;
//extern video lastvideo,videomode;
extern screentype whichscreen;
extern VMapStr VMapData;

extern char launchname[64],ext[4],format[2],projname[64],mapname[64],planes,
	infoname[64],mapheadname[64],tdata,MapNames[100][16],parmstring[64];
extern char SM_name[64],SM_loadname[64];

extern unsigned temp,whichmap,numtplanes,tilenum,tilemnum,numtmplanes,left,
	DirtyFlag,tilelen,tilemlen,which_t,which_tm,which_i,
	tsize,infoy,infomaxw,mapwidth,mapheight,screenw,usingbat,
	screenh,planeton,planemon,planeion,maxiconrows,lasticon,firsticon,
	viewton,viewmon,viewion,XMSmaps,
	EgaXMS,CgaXMS,VgaXMS,xmshandle,GridMode,TINFOMode,SnapMode,snapx,snapy,
	snapxsize,snapysize,writeH,F3_flag,NoAbout;
extern int tilebase,tilembase,tileibase,infobaron,xbase,ybase,scrnbot,scrnrgt,
	FillMode,PasteMode,SelectMode,SelX1,SelY1,PasteOK,SelX2,SelY2,pixelx,pixely,
	selectcols,px,py,lastmap,TIybase,TIymbase,TIxbase,TIxmbase,BfillMode,
	Plotting,TsearchMode;
extern long CgaXMSsize,EgaXMSsize,VgaXMSsize;

//extern void far *XMSdriver;


//
// FUNCTION PROTOTYPES
//

//
// ATED.C
//
void init_colors(void);
void InitATED(void);
void init(void);
void ParseCmdLine(int _argc, char *_argv[]);
void load_cfg(void);
void save_cfg(void);
int  shut_down(void);

void fatal_error(char msg[]);
void error(char msg[]);
void fat_error(char msg[]);

void main_menu();

void clear_kbuf(void);
void wait_for_keys();

void printhexb(BITMAP *bmp, unsigned char value, int x, int y, int col);
void printhex(BITMAP *bmp,  unsigned value,      int x, int y, int col);
void printint(BITMAP *bmp,  unsigned value,      int x, int y, int col);
void printbin(BITMAP *bmp,  unsigned value,      int x, int y, int col);
//void SEND_MESSAGE_TO_ALL(DIALOG *d[], int msg, int c, int num);
void sel_palette(RGB *pal);

//
// DRAW.C
//
int  draw_infobar_proc(int msg, DIALOG *d, int c);
int  draw_rect_proc(int msg, DIALOG *d, int c);
int  draw_map_proc(int msg, DIALOG *d, int c);
void draw_info_val(BITMAP *bmp, unsigned val, int x, int y);

//
// DIALOG.C
//
void check_map_edges();
DIALOG_PLAYER *init_ated_dialog(DIALOG *dialog, int focus_obj);
int update_ated_dialog(DIALOG_PLAYER *player);

int mouse_b_proc(int msg, DIALOG *d, int c);
int scroll_map_proc(int msg, DIALOG *d, int c);
int toggle_layers_proc(int msg, DIALOG *d, int c);
int check_for_keys_proc(int msg, DIALOG *d, int c);

//
// FILE.C
//
int Item_LoadTileGFX();
int datedit_ask(AL_CONST char *fmt, ...);
void datedit_msg(AL_CONST char *fmt, ...);
void datedit_startmsg(AL_CONST char *fmt, ...);
void datedit_endmsg(AL_CONST char *fmt, ...);
void datedit_error(AL_CONST char *fmt, ...);
void load(char *filename, int flush);
int save(int strip);
void update_info();
void *makenew_data(long *size);
void create_map();
void edit_map(int index);
void delete_map(int index);
void rename_map(int index);
void rebuild_map_list();
void create_planes(int back, int fore, int info);
void load_ated_temp_file();
void save_ated_temp_file();
void save_tinfo_file();
void load_tinfo_file();

BITMAP     *convert_bitmap                              (BITMAP     *bitmap);
void        convert_datafile_bitmap                     (DATAFILE   *dat);
void        convert_bitmaps_in_datafile                 (DATAFILE   *dat);
RLE_SPRITE *convert_rle_sprite                          (RLE_SPRITE *sprite);
void        convert_datafile_rle_sprite                 (DATAFILE   *dat);
void        convert_rle_sprites_in_datafile             (DATAFILE   *dat);
void        convert_images_in_datafile                  (DATAFILE   *dat);

/*int Item_EditMap(void);
int Item_SaveMap(void);
int Item_CreateMap(void);
int Item_DeleteMap(void);
int Item_SwitchMap(void);
int Item_Amputate(void);
int Item_ImportMaps(void);
*/

//
// MISC.C
//
void box_start();
void box_out(char *msg);
void box_eol();
void box_end(int pause);

//
// GUI.C
//
int background (int msg, DIALOG *d, int c);
int do_ated_dialog(DIALOG *dialog, int focus_obj);
void gui_textout_highlight(BITMAP *bmp, char *txt, int x, int y, int col1, int col2, int col3, int centre, int outline, FONT *fnt);
int label(int msg, DIALOG *d, int c);
int button(int msg, DIALOG *d, int c);
int box(int msg, DIALOG *d, int c);
int edit_text(int msg, DIALOG *d, int c);
int menu(int msg, DIALOG *d, int c);
int icon(int msg, DIALOG *d, int c);
int rle_icon(int msg, DIALOG *d, int c);
int ated_alert3(AL_CONST char *s1, AL_CONST char *s2, AL_CONST char *s3, AL_CONST char *b1, AL_CONST char *b2, AL_CONST char *b3, int c1, int c2, int c3);
int ated_alert(AL_CONST char *s1, AL_CONST char *s2, AL_CONST char *s3, AL_CONST char *b1, AL_CONST char *b2, int c1, int c2);
int listbox(int msg, DIALOG *d, int c);
int checkbox(int msg, DIALOG *d, int c);
int tile_select(int msg, DIALOG *d, int c);
int ated_gfx_mode_select(int *card, int *w, int *h);
int ated_gfx_mode_select_ex(int *card, int *w, int *h, int *color_depth);
void dotted_rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int fg, int bg);
int slider(int msg, DIALOG *d, int c);

//
// FSEL.C
//
int ated_file_select(AL_CONST char *message, char *path, AL_CONST char *ext);
int ated_file_select_ex(AL_CONST char *message, char *path, AL_CONST char *ext, int width, int height);


//
// MAP.C
//
void do_tinfo_edit_tile (unsigned tile);
void do_tinfo_edit_map (int x, int y);

void DoFloodFill(int mx, int my);
void RestoreUndo(void);
void CopyUndoRegion();
void SaveUndo(int x, int y, int w, int h);

//
// SKIN.C
//
int skin_init ();
BITMAP *get_bmp_file (char *file_name);
int skin_build_list (INDEX_LIST *l, int *current);
void update_skin ();

#endif
