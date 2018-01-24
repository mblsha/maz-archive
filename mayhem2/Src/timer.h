#ifndef __TIMER_H__
#define __TIMER_H__

extern volatile unsigned long curtime;
extern volatile int game_time;
extern volatile int frame_count, fps;
extern volatile unsigned long frames;

extern float frame_time;
extern int   frame_msec;

extern boolean timer_initialized;

void timer_init         ();
void timer_uninit       ();

#endif
