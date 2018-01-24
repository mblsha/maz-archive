#include "def.h"

volatile unsigned long curtime;
volatile int game_time;
volatile int frame_count, fps;
volatile unsigned long frames;

boolean timer_initialized = false;

float frame_time;
int   frame_msec;

// timer callback for measuring the frames per second

void fps_proc(void)
{
   fps = frame_count;
   frame_count = 0;
}
END_OF_FUNCTION(fps_proc);

void time_proc(void)
{
   curtime++;
}
END_OF_FUNCTION(timer_time);

void game_time_proc()
{
   game_time++;
}
END_OF_FUNCTION(game_time)

extern cvar max_fps;

void timer_init ()
{
   timer_initialized = true;

   install_int (fps_proc,             1000);
   install_int (time_proc,            1);
   install_int (game_time_proc, (int)(1000/max_fps.value));

   frame_time = 1/max_fps.value;
   frame_msec = 1000/max_fps.value;
}

void timer_uninit ()
{
   remove_int (fps_proc);
   remove_int (time_proc);
   remove_int (game_time_proc);
}
