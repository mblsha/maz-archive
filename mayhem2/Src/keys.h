#ifndef __KEYS_H__
#define __KEYS_H__

#include "keydef.h"
/*
//
// mouse buttons generate virtual keys
//
#define KEY_MOUSE1                      KEY_MAX + 5
#define KEY_MOUSE2                      KEY_MAX + 6
#define KEY_MOUSE3                      KEY_MAX + 7
#define KEY_MOUSE_WHEEL_UP              KEY_MAX + 8
#define KEY_MOUSE_WHEEL_DOWN            KEY_MAX + 9

//
// joystick buttons
//
#define KEY_JOY1                        KEY_MAX + 10
#define KEY_JOY2                        KEY_MAX + 11
#define KEY_JOY3                        KEY_MAX + 12
#define KEY_JOY4                        KEY_MAX + 13

//
// aux keys are for multi-buttoned joysticks to generate so they can use
// the normal binding process
//
#define KEY_AUX1                        KEY_MAX + 15
#define KEY_AUX2                        KEY_MAX + 16
#define KEY_AUX3                        KEY_MAX + 17
#define KEY_AUX4                        KEY_MAX + 18
#define KEY_AUX5                        KEY_MAX + 19
#define KEY_AUX6                        KEY_MAX + 20
#define KEY_AUX7                        KEY_MAX + 21
#define KEY_AUX8                        KEY_MAX + 22
#define KEY_AUX9                        KEY_MAX + 23
#define KEY_AUX10                       KEY_MAX + 24
#define KEY_AUX11                       KEY_MAX + 25
#define KEY_AUX12                       KEY_MAX + 26
#define KEY_AUX13                       KEY_MAX + 27
#define KEY_AUX14                       KEY_MAX + 28
#define KEY_AUX15                       KEY_MAX + 29
#define KEY_AUX16                       KEY_MAX + 30
#define KEY_AUX17                       KEY_MAX + 31
#define KEY_AUX18                       KEY_MAX + 32
#define KEY_AUX19                       KEY_MAX + 33
#define KEY_AUX20                       KEY_MAX + 34
#define KEY_AUX21                       KEY_MAX + 35
#define KEY_AUX22                       KEY_MAX + 36
#define KEY_AUX23                       KEY_MAX + 37
#define KEY_AUX24                       KEY_MAX + 38
#define KEY_AUX25                       KEY_MAX + 39
#define KEY_AUX26                       KEY_MAX + 40
#define KEY_AUX27                       KEY_MAX + 41
#define KEY_AUX28                       KEY_MAX + 42
#define KEY_AUX29                       KEY_MAX + 43
#define KEY_AUX30                       KEY_MAX + 44
#define KEY_AUX31                       KEY_MAX + 45
#define KEY_AUX32                       KEY_MAX + 46
*/
typedef enum
{
   key_game,
   key_console,
   key_message,
   key_menu
} keydest_t;

extern  keydest_t key_dest;
extern  ustring   keybindings[KEY_NAMES_MAX];
extern  int       key_repeats [KEY_NAMES_MAX];
extern  int       key_count;                  // incremented every key event
extern  int       key_lastpress;

void key_bind_list_f();

void key_console_poll                   (int key);
void key_message_poll                   (int key);

void key_mouse_poll                     ();
void key_keyboard_poll                  ();
void key_joystick_poll                  ();

void key_event                          (int key, boolean down, unsigned time);
void key_init                           (void);
void key_write_bindings                 (char *file);
void key_read_bindings                  (void);
void key_set_binding                    (int keynum, char *binding);
void key_clear_states                   (void);

int  return_printable                   (int scan);
char *key_keynum_to_string              (int keynum);

//void console_poll       ();

#endif
