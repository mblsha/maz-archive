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

#ifndef __ATEDP_H__
#define __ATEDP_H__

#include <allegro.h>
#include <string.h>

extern char        *ated_error;

//#define DAT_MAP_HEAD    DAT_ID('M','H','E','D')
//#define DAT_MAP_INFO    DAT_ID('M','I','N','F')
//#define DAT_PL_BACK     DAT_ID('P','B','C','K')
//#define DAT_PL_FORE     DAT_ID('P','F','R','E')
//#define DAT_PL_INFO     DAT_ID('P','I','N','F')
//#define DAT_TILE_INFO   DAT_ID('T','I','N','F')
//#define DAT_TILE_INFOM  DAT_ID('T','I','N','M')

typedef struct TILE_SET
{
   char     name[128];
   char     author[128];

   int      num_maps;
} TILE_SET;

typedef struct TILE_MAP
{
   char     name[128];
   int      tile_size;
   int      w, h;

   int      back;
   int      fore;
   int      info;

   unsigned *back_plane;
   unsigned *fore_plane;
   unsigned *info_plane;
} TILE_MAP;

typedef struct ATED_MAP_HEADER
{
   char     id[5];                                       // ATED
   //int  v;                                             // Header version
   char     name[128];                                   // Name of map
   unsigned w, h;                                        // width & height
} ATED_MAP_HEADER;

typedef struct VIEWPORT
{
   int    x, y;                                          // top-left coord. of viewport window
   int    w, h;                                          // width & heigth

   int    zoom;                                          // Zoom level: 1000 -- original size
                                                         // 100 -- 10 times smaller
                                                         // must be between 100 and 1000

   int    x1, y1, x2, y2;                                // FOR INTERNAL USE: clipping rectangle

   int    dx, dy;                                        // offset from top-left corner of tilemap in pixels
   int    dx_old, dy_old;                                // old offset

   float  vx, vy;

   int    back;
   int    fore;
   int    info;

   BITMAP *back_bmp, *fore_bmp, *info_bmp;               // bitmaps to draw on
   BITMAP *back_tmp, *fore_tmp, *info_tmp;               // temp buffers
} VIEWPORT;

//
// Tile map data
//
extern TILE_MAP         tile_map;
extern TILE_SET         tile_set;

//
// GFX pointers
//
extern DATAFILE	*tile_gfx;
extern DATAFILE *back_tile_gfx;
extern DATAFILE *fore_tile_gfx;
extern DATAFILE *info_tile_gfx;

extern BITMAP **back_tile_bmp;
extern BITMAP **fore_tile_bmp;
extern BITMAP **info_tile_bmp;
//extern RLE_SPRITE **back_tile_rle;
//extern RLE_SPRITE **fore_tile_rle;
//extern RLE_SPRITE **info_tile_rle;

extern unsigned int back_tile_gfx_count;
extern unsigned int fore_tile_gfx_count;
extern unsigned int info_tile_gfx_count;

typedef enum
{
   tiletype_bitmap,
   tiletype_rle_sprite
} tiletype_t;

extern tiletype_t tiletype;

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

//////////////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////////////

//
// DRAW.C
//
void            draw_background (VIEWPORT *vp, BITMAP *bmp);
void            draw_foreground (VIEWPORT *vp, BITMAP *bmp);
void            draw_planes     (VIEWPORT *vp, BITMAP *bmp);
void            draw_info_plane (VIEWPORT *vp, BITMAP *bmp);
void            update_viewport (VIEWPORT *vp);
void            update_anim     (void);
//void draw_bkgnd (BITMAP *dest, int x1, int y1, int x2, int y2, int dx, int dy);
//void draw_frgnd (BITMAP *dest, int x1, int y1, int x2, int y2, int dx, int dy);
//void draw_iconpl(BITMAP *dest, int x1, int y1, int x2, int y2, int dx, int dy);

VIEWPORT        *create_viewport     (int x, int y, int w, int h, int back, int fore, int info, BITMAP *bmp);
void            destroy_viewport     (VIEWPORT *vp);
void            move_viewport        (VIEWPORT *vp, int x, int y, int is_absolute);
void            check_viewport_edges (VIEWPORT *vp);
void            viewport2bmp         (VIEWPORT *vp, BITMAP *bmp, int back, int fore, int info);

//
// LOAD.C
//
//int  load_tile_set          (char *filename);
//void unload_tile_set        (void);
//int  load_map_from_tile_set (int num);
int             load_tile_map          (char *filename);
int             load_tile_gfx          (char *filename);
void            unload_tile_map        (void);
void            unload_tile_gfx        (void);
int             init_viewport          (VIEWPORT *vp);

BITMAP          *convert_bitmap                         (BITMAP     *bitmap);
void            convert_datafile_bitmap                 (DATAFILE   *dat);
void            convert_bitmaps_in_datafile             (DATAFILE   *dat);
RLE_SPRITE      *convert_rle_sprite                     (RLE_SPRITE *sprite);
void            convert_datafile_rle_sprite             (DATAFILE   *dat);
void            convert_rle_sprites_in_datafile         (DATAFILE   *dat);
void            convert_images_in_datafile              (DATAFILE   *dat);
BITMAP          *make_bitmap                            (int w, int h, int priority, int is_masked);

//
// MAP.C
//
int             create_planes       (int back, int fore, int info);
void            create_tile_map     (void);
void            destroy_tile_map    (void);
void            check_tile_map      (void);
//void            load_tileinfo       (void);

void            destroy_tinfo       (void);

int             get_background_tile (int x, int y);
int             get_foreground_tile (int x, int y);
unsigned        get_info_plane_tile (int x, int y);
void            set_background_tile (int x, int y, int      val);
void            set_foreground_tile (int x, int y, int      val);
void            set_info_plane_tile (int x, int y, unsigned val);
//int             get_tileinfo (int tile, int num);
//int             get_tileinfom(int tile, int num);
//void            get_animinfo (int animinfo, int speedinfo);
//void            get_animinfom(int animinfo, int speedinfo);

#endif
