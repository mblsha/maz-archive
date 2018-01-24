#ifndef __GAME_H__
#define __GAME_H__

typedef enum
{
   sv_connected,
   sv_disconnected,
   sv_demo
} sv_state_t;

typedef enum
{
   game_playing,
   not_playing
} game_state_t;

extern sv_state_t    sv_state;
extern game_state_t  game_state;

#endif
