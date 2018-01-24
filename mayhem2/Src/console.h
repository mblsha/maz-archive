#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <allegro.h>

typedef enum
{
   con_active,
   con_closed,
   con_opening,
   con_closing,

   NUMCONACTION
} console_action;

EXTERN_CVAR ( console_instant );
EXTERN_CVAR ( console_speed   );
EXTERN_CVAR ( developer       );

extern boolean team_message;

//
// console
//
// Console colors
typedef enum
{
   con_default = 0,
   con_command,
   con_warning,
   con_error,
   con_system,
   con_description,
   con_outline,

   MAXCONCOLOR
} console_color;

typedef struct
{
   ustring       text;
   console_color color;
   unsigned long time;
} notify_item;

#define MAX_NOTIFY      64

class Console_notify
{
private:
       notify_item       notify [MAX_NOTIFY];

       int               items;

public:
       Console_notify  ();
       ~Console_notify ();

       void add          (console_color color, char *text);
       void draw         (BITMAP *bmp);
       void clear        ();
};

typedef struct
{
   ustring       text;
   console_color color;
} console_line;

class Console
{
private:
       console_line             *lines;

       boolean                  initialized;
       boolean                  debuglog;

       BITMAP                   *background;
       int                      back_col;

       int                      con_x;
       int                      vislines;
       int                      linewidth;
       int                      max_lines;

       console_action           action;

       int                      input_offset;
public:

       boolean                  outline;

       int                      totallines;
       float                    pos;
       int                      backscroll;

       FONT                     *font;

       int                      char_w;
       int                      char_h;

       Console  ();
       ~Console ();

       void draw                (BITMAP *bmp, int lines, boolean draw_input);
       void draw_background     (BITMAP *bmp, int lines);
       void draw_input          (BITMAP *bmp);
       void set_color           (console_color num, int value);
       void set_properties      (FONT *font, boolean outline);
       void set_background      (BITMAP *image, /*boolean tiled,*/ int color);
       void animate             ();

       void line_feed           (console_color color, char *text);
       char *anti_line_feed     ();
       void check_resize        ();

       void print               (console_color color, char *txt);
       void printf              (console_color color, char *fmt, ...);
       void printf              (char *fmt, ...);
       void dprintf             (console_color color, char *fmt, ...);
       void dprintf             (char *fmt, ...);
//       void safe_printf         (char *fmt, ...);
       void debug_log           (char *file, char *fmt, ...);

       void console_debug_log   (char *file, char *fmt, ...);

       boolean is_active        ();
       void set_action          (console_action what);
       void set_height          (int height);

       void toggle              ();
       void clear               ();
};

extern Console_notify console_notify;
extern Console        console;

#endif
