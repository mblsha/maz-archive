#ifndef __GUI_H__
#define __GUI_H__

#include "allegro.h"

#include "gui_d.h"
#include "main_d.h"
#include "pal_d.h"
#include "sound_d.h"

#include "gui_colors.h"
#include "gui_dialog.h"

#define LISTBOX_EXEC_DP3        512
#define LISTBOX_CALL_DP         1024

//#define PLAY_SOUND_CLICK        play_sound((SAMPLE *)snd_data[CLICK].dat,  128, FALSE)
//#define PLAY_SOUND_SELECT       play_sound((SAMPLE *)snd_data[SELECT].dat, 128, FALSE)
#define PLAY_SOUND_CLICK        play_sound((SAMPLE *)store.dat ("/sound/click"),  128, FALSE)
#define PLAY_SOUND_SELECT       play_sound((SAMPLE *)store.dat ("/sound/select"), 128, FALSE)


// Base GUI colors
#define GUI_NORMAL1      gray_bright
#define GUI_NORMAL2      gray_dark
#define GUI_NORMAL3      gray_dark
#define GUI_NORMAL_TEXT1 gray_bright
#define GUI_NORMAL_TEXT2 gray

#define GUI_SELECT1      white
#define GUI_SELECT2      gray
#define GUI_SELECT3      gray_bright//green_very_dark
#define GUI_SELECT_TEXT1 white
#define GUI_SELECT_TEXT2 white

#define GUI_FILL1        gray_bright
#define GUI_FILL2        gray
#define GUI_FILL3        gray_dark
#define GUI_FILL4        gray_very_dark

//#define play_sample(x, y, z, a, b)          /* empty */
int gui_idle_logic(int msg, DIALOG *d, int c);


#define MSG_JUST_CLICK   1325
#define MSG_UPDATE       1326
#define MSG_SCAN         1327

extern BITMAP *gui_check;
extern BITMAP *gui_cross;

void    gui_intro_f                             (void);
//void    intro                                   (void);
void    menu                                    (void);

void    gui_init                                (void);
void    gui_init_colors                         (void);
void    gui_set_bmp                             (BITMAP *bmp);

int     hor_line                                (int msg, DIALOG *d, int c);
int     button                                  (int msg, DIALOG *d, int c);
int     checkbox                                (int msg, DIALOG *d, int c);
int     background                              (int msg, DIALOG *d, int c);
int     frame                                   (int msg, DIALOG *d, int c);
int     label                                   (int msg, DIALOG *d, int c);
int     slider                                  (int msg, DIALOG *d, int c);
int     listbox                                 (int msg, DIALOG *d, int c);
// Helpers for listbox
void    draw_listbox                            (DIALOG *d);
void    draw_scrollable_frame                   (DIALOG *d, int listsize, int offset, int height, int fg_color, int bg);

void    gui_textout_highlight                   (BITMAP *bmp, AL_CONST char *s, int x, int y, int col1, int col2, int centre, int outline);
void    r_rect                                  (BITMAP *buf, int x1, int y1, int x2, int y2, int color, int rad);
void    r_rectfill                              (BITMAP *buf, int x1, int y1, int x2, int y2, int color, int rad);

/*extern  DATAFILE                *gui_data;
extern  DATAFILE                *back_bmp;
extern  DATAFILE                *may_data;
extern  DATAFILE                *tmp_data;
extern  DATAFILE                *snd_data;
extern  DATAFILE                *font_data;*/
extern  COLOR_MAP               color_map_menu;

BITMAP     *convert_bitmap                      (BITMAP     *bmp);
void       convert_datafile_bitmap              (DATAFILE   *dat);
void       convert_bitmaps_in_datafile          (DATAFILE   *dat);
RLE_SPRITE *convert_rle_sprite                  (RLE_SPRITE *sprite);
void       convert_datafile_rle_sprite          (DATAFILE   *dat);
void       convert_rle_sprites_in_datafile      (DATAFILE   *dat);
void       convert_images_in_datafile           (DATAFILE   *dat);

#endif
