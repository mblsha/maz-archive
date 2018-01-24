#ifndef __PLANES_H__
#define __PLANES_H__

#include "allegro.h"

#ifndef BYTE_DEFINED
    typedef unsigned char     byte;
    #define BYTE_DEFINED
#endif

#ifdef __cplusplus
        typedef enum {False = FALSE, True = TRUE}   boolean;
        #define true            True
        #define false           False
#else
        typedef enum {false = FALSE, true = TRUE}   boolean;
#endif

void sys_error (char *text);
extern boolean dirty_flag;


class tile_gfx
{
private:
   DATAFILE *data;
   int      count;

public:
   ~tilegfx ();

   void        init              (DATAFILE *dat);
   RLE_SPRITE  *get_sprite       (int num);
   int         get_count         ();
}

extern tile_gfx *tilegfx;

typedef enum plane_type
{
   plane_back = 0,
   plane_fore = 1,
   plane_info = 2,
   
   plane_type_max
} plane_type;

typedef enum edge_type
{
   edge_left,
   edge_right,
   edge_up,
   edge_down
} edge_type;

class plane
{
private:
   char           name[32];            // plane's name: "Back", "Fore", "Info", etc.
   int            letter;              // used in INFOBAR. Can be 'B', 'b', 'F', etc.
   plane_type     type;                // to determine, which tiles to use
   
   boolean        view_on;
   boolean        edit_on;

   unsigned int   tile;                // selected tile
   unsigned int   tileselect;

   int            w, h;                // width & heigth
   unsigned int   *tilemap;            // the most important thing out there
   unsigned int   *undomap;
   unsigned int   *copymap;

protected:
   unsigned int *resize_helper   (int w, int h);
   void     update_state_helper  ();

public:
   ~plane();

   void     clear                ();
   void     resize               (int width, int heigth);
   void     change_edge          (edge_type edge, int val);

   boolean  edit_state           ();
   boolean  view_state           ();   
   void     set_edit_state       (boolean state);
   void     set_view_state       (boolean state);

   void     save_undo            ();
   void     save_undo            (int xx, int yy, int w, int h);
   void     restore_undo         ();

   void     draw_tile            (BITMAP *bmp, unsigned num,   int x, int y);
   void     draw_tile            (BITMAP *bmp, int tx, int ty, int x, int y);

   unsigned get_tile             (int x, int y);
   void     set_tile             (int x, int y, unsigned val);

   unsigned get_copy_tile        (int x, int y);

   void     flood_fill           (int mx, int my);
};

extern plane *planes;

#endif // __PLANES_H__
