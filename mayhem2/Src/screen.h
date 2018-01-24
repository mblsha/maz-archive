#ifndef __SCREEN_H__
#define __SCREEN_H__

#define ANIM_DOUBLE_BUFFER              0
#define ANIM_PAGE_FLIP                  1
#define ANIM_TRIPLE_BUFFER              2

extern BITMAP   *mouse_pointer;
extern BITMAP   *busy_pointer;

extern boolean  accel_system2screen_blits;
extern boolean  accel_system2screen_masked_blits;
extern boolean  accel_vram2vram_blits;
extern boolean  accel_vram2vram_masked_blits;

extern cvar     wait_vsync;
extern cvar     show_fps;

void    scr_init                        (void);
void    scr_draw                        (void);
void    scr_init_buffers                (void);
void    scr_destroy_buffers             (void);
void    scr_set_anim_type               (int new_anim_type);

BITMAP *make_bitmap                     (int w, int h, int priority, boolean is_masked);

#endif
