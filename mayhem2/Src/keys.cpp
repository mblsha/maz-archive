#include "def.h"
#include "string.h"
#include "gui.h"

#include "codepage.cpp"

keydest_t   key_dest;

#define  CHAT_BUFFER_SIZE     64
#define  MAXCMDLINE           256
char     key_lines[32][MAXCMDLINE*2];
int      key_linepos;

int      edit_line    = 0;
int      history_line = 0;

//int      shift_down = false;
int      key_lastpress;

int      key_count;        // incremented every key event

ustring  keybindings [KEY_NAMES_MAX];
boolean  consolekeys [KEY_NAMES_MAX]; // if true, can't be rebound while in console
boolean  menubound   [KEY_NAMES_MAX]; // if true, can't be rebound while in menu
int      key_repeats [KEY_NAMES_MAX]; // if > 1, it is autorepeating
boolean  keydown     [KEY_NAMES_MAX];

typedef enum keytype_t
{
   key_keyboard,
   key_mouse,
   key_joystick,
   key_aux
} keytype_t;

typedef struct
{
   char        name[32];

   int         keynum;
   boolean     printable;
   keytype_t   keytype;

   int         key;
   int         key_shift;

   int         key2;
   int         key_shift2;
} keyname_t;

#include "keynames.h"

int con_key[KEY_NAMES_MAX];

boolean english_keyboard = true;

BEGIN_COMMAND ( togglekeyboard, "Toggle between different keyboard layouts" )
{
   if (english_keyboard)
      english_keyboard = false;
   else
      english_keyboard = true;
}
END_COMMAND ( togglekeyboard )


#define KEY_SHIFT       ((key_shifts & KB_SHIFT_FLAG)/* || ((key_shifts & KB_CAPSLOCK_FLAG) && !((key_shifts & KB_SHIFT_FLAG)))*/)

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, or a KEY_* name) for the
given keynum.
FIXME: handle quote special (general escape sequence?)
===================
*/
char *key_keynum_to_string (int keynum)
{
   keyname_t   *kn;

   if (keynum == -1)
      return "<UNKNOWN KEYNUM>";

   for (kn = keynames; kn->keynum; kn++)
      if (keynum == kn->keynum)
         return kn->name;

   return "<KEY NOT FOUND>";
}

int return_printable(int scan)
{
   if (keynames[scan].printable)
   {
      if (KEY_SHIFT)
      {
         if ( !english_keyboard && keynames[scan].key_shift2 )
            return keynames[scan].key_shift2;
         else
            return keynames[scan].key_shift;
      }
      else
      {
         if ( !english_keyboard && keynames[scan].key2 )
            return keynames[scan].key2;
         else
            return keynames[scan].key;
      }
   }
   
   return -1;
}

static void key_reset()
{
   int i;

   for(i = 0; i < KEY_NAMES_MAX; i++)
   {
      key_repeats[i] = 0;
      keydown[i] = false;
      key[i] = 0;
   }
}

static inline void key_helper (int num, int _pressed)
{
   boolean down;
   boolean pressed = _pressed ? true : false;

   if (keydown[num] != pressed)
   {
      if (keydown[num])
         down = false;
      else
         down = true;

      keydown[num] = pressed;

      key_event(num, down, curtime);
   }
   else if (key_repeats[num] && pressed)
   {
      keydown[num] = true;

      key_event(num, true, curtime);
   }
}

CVAR (mouse_filter,    "1", CVAR_ARCHIVE);
CVAR (mouse_sensivity, "3", CVAR_ARCHIVE);
int mx, my;
int mickey_x, mickey_y;
int old_mx, old_my;

void key_mouse_poll()
{
   int i, num;
   boolean m;

   if (key_dest != key_menu)
   {
      //
      // Update mouse buttons' status
      //
      for(i = 0; i <= 2; i++)
      {
         num = i + 1;

         if (num > 2)
            num = 4;

         m = (mouse_b & num) ? true : false;

         key_helper (KEY_MOUSE1+i, m);
      }

      if( abs(mouse_z) > 0 )
      {
         if(mouse_z > 0)
         {
            key_event(KEY_MOUSE_WHEEL_DOWN, true,  curtime);
            key_event(KEY_MOUSE_WHEEL_DOWN, false, curtime);
         }
         else
         {
            key_event(KEY_MOUSE_WHEEL_UP, true,  curtime);
            key_event(KEY_MOUSE_WHEEL_UP, false, curtime);
         }

         position_mouse_z (0);
      }
   }
   else
   {
      gui_animate ();
   }

   //
   // Update mouse position
   //
   get_mouse_mickeys (&mickey_x, &mickey_y);

   if (mouse_filter.value)
   {
      mx = (mickey_x + old_mx) * 0.5;
      my = (mickey_y + old_my) * 0.5;
   }
   else
   {
      mx = mickey_x;
      my = mickey_y;
   }

   old_mx = mickey_x;
   old_my = mickey_y;

   mx *= mouse_sensivity.value;
   my *= mouse_sensivity.value;

   //
   // TODO: Put mouse pos handling code here
   //

   position_mouse (mouse_x + mx, mouse_y + my);
}

void key_keyboard_callback (int scan)
{
   scan &= ~0x80;    // clear the down state

   key_helper (scan, key[scan]);
}
END_OF_FUNCTION (key_keyboard_callback);


// obsolete
void key_keyboard_poll()
{
   int     i;
   //boolean down;

   for(i = 0; i < KEY_NAMES_MAX; i++)
   {
      if (keynames[i].keytype != key_keyboard)
         continue;

      key_helper (i, key[i]);
   }
}

CVAR (use_joystick, "1", CVAR_ARCHIVE);

void key_joystick_poll()
{
   int     i, j;

   if (!use_joystick.value)
      return;

   poll_joystick ();

   for (i = 0; i < num_joysticks; i++)
   {
      for (j = 0; j < joy[i].num_buttons; j++)
         key_helper(KEY_JOYSTICK_START + (KEY_JOYSTICK_1 + j) + (i * KEY_JOYSTICK_SIZE), joy[i].button[j].b);

      key_helper(KEY_JOYSTICK_START + (KEY_JOYSTICK_LEFT)  + (i * KEY_JOYSTICK_SIZE), joy[i].stick[0].axis[0].d1);
      key_helper(KEY_JOYSTICK_START + (KEY_JOYSTICK_RIGHT) + (i * KEY_JOYSTICK_SIZE), joy[i].stick[0].axis[0].d2);
      key_helper(KEY_JOYSTICK_START + (KEY_JOYSTICK_UP)    + (i * KEY_JOYSTICK_SIZE), joy[i].stick[0].axis[1].d1);
      key_helper(KEY_JOYSTICK_START + (KEY_JOYSTICK_DOWN)  + (i * KEY_JOYSTICK_SIZE), joy[i].stick[0].axis[1].d2);
   }
}

extern int action;

/*
==============================================================================

         LINE TYPING INTO THE CONSOLE

==============================================================================
*/


/*
====================
Key_Console

Interactive line editing and console scrollback
====================
*/
extern int con_char_h;

void key_console_poll (int key)
{
   char    cmd[128];
   int     k;

   if (key == KEY_ENTER || key == KEY_ENTER_PAD)
   {
      cbuf_add_text  (key_lines[edit_line]);
      cbuf_add_text  ("\n");

      console.printf (con_command, "]%s\n", key_lines[edit_line]);

      edit_line = (edit_line + 1) & 31;
      history_line = edit_line;
      usetat (key_lines[edit_line], 0, 0);
      key_linepos = 0;
      
      return;
   }

   if (key == KEY_TAB)
   {  //
      // command completion
      //
      ustrcpy (cmd, cmd_complete_command (key_lines[edit_line]));
      if ( !ustrlen (cmd) )
         ustrcpy (cmd, cvar_complete_variable (key_lines[edit_line]));

      if ( ustrlen (cmd) )
      {
         ustrcpy (key_lines[edit_line], cmd);
         key_linepos = ustrlen (cmd);

         usetat ((char *)key_lines[edit_line], key_linepos, ' ');
         key_linepos++;
         usetat ((char *)key_lines[edit_line], key_linepos, 0);
      }
      
      return;
   }

   if (key == KEY_BACKSPACE || key == KEY_LEFT)
   {
      if (key_linepos > 0)
      {
         key_linepos--;

         usetat ((char *)key_lines[edit_line], key_linepos, 0);
      }

      return;
   }

   if (key == KEY_UP)
   {
      do
      {
         history_line = (history_line - 1) & 31;
      } while (history_line != edit_line && !key_lines[history_line]);

      if (history_line == edit_line)
         history_line = (edit_line+1)&31;
         
      ustrcpy (key_lines[edit_line], key_lines[history_line]);
      key_linepos = strlen(key_lines[edit_line]);
      return;
   }

   if (key == KEY_DOWN)
   {
      if (history_line == edit_line) return;
      do
      {
         history_line = (history_line + 1) & 31;
      }
      while (history_line != edit_line && !key_lines[history_line][1]);

      if (history_line == edit_line)
      {
         //key_lines[edit_line][0] = ']';
         key_linepos = 0;
      }
      else
      {
         ustrcpy (key_lines[edit_line], key_lines[history_line]);
         key_linepos = strlen(key_lines[edit_line]);
      }
      return;
   }

   // scroll up
   if (key == KEY_PGUP || key == KEY_MOUSE_WHEEL_UP)
   {
      console.backscroll += 1;
      if (console.backscroll > console.totallines - (SCREEN_H/console.char_h) - 2)
         console.backscroll = console.totallines - (SCREEN_H/console.char_h) - 2;
      return;
   }

   // scroll down
   if (key == KEY_PGDN || key == KEY_MOUSE_WHEEL_DOWN)
   {
      console.backscroll -= 1;
      if (console.backscroll < 0)
         console.backscroll = 0;
      return;
   }

   // scroll to top
   if (key == KEY_HOME)
   {
      console.backscroll = console.totallines - (SCREEN_H/console.char_h) - 2;
      return;
   }

   // scroll to bottom
   if (key == KEY_END)
   {
      console.backscroll = 0;
      return;
   }

   if( (k = return_printable (key) ) < 0)
   {
      return;
   }

   if (key_linepos < MAXCMDLINE-1)
   {
      usetat ((char *)key_lines[edit_line], key_linepos, k);
      key_linepos++;
      usetat ((char *)key_lines[edit_line], key_linepos, 0);
   }
}

//============================================================================

char chat_buffer[CHAT_BUFFER_SIZE*2];
boolean team_message = false;
static int chat_bufferlen = 0;

void key_message_poll (int key)
{
   if (key == KEY_ENTER || key == KEY_ENTER_PAD)
   {
      if (team_message)
         cbuf_add_text ("say_team \"");
      else
         cbuf_add_text ("say \"");

      cbuf_add_text (chat_buffer);
      cbuf_add_text ("\"\n");

      key_dest = key_game;
      chat_bufferlen = 0;
      usetat (chat_buffer, 0, 0);
      return;
   }

   if (key == KEY_ESC)
   {
      key_dest = key_game;
      chat_bufferlen = 0;
      usetat (chat_buffer, 0, 0);
      return;
   }

   if (key == KEY_BACKSPACE)
   {
      if (chat_bufferlen)
      {
         chat_bufferlen--;
         usetat (chat_buffer, chat_bufferlen, 0);
      }
      return;
   }

   if( (key = return_printable(key) ) < 0)
      return;

   if (chat_bufferlen >= CHAT_BUFFER_SIZE - 1)
      return; // all full

   usetat (chat_buffer, chat_bufferlen++, key);
   usetat (chat_buffer, chat_bufferlen,   0);
}

//============================================================================


/*
===================
Key_StringToKeynum

Returns a key number to be used to index keybindings[] by looking at
the given string.  Single ascii characters return themselves, while
the KEY_* names are matched up.
===================
*/
int key_string_to_keynum (char *str)
{
   keyname_t   *kn;

   for (kn = keynames; kn->keynum; kn++)
   {
      if ( !ustricmp (kn->name, str) )
      {
         return kn->keynum;
      }
      else if ( kn->printable && (ustrlen (str) == 1) )
      {
         int c = ugetc (str);

         if ( c == kn->key       ||
              c == kn->key_shift ||
              c == kn->key2      ||
              c == kn->key_shift2 )
         {
            return kn->keynum;
         }
      }
   }

   return -1;
}


/*
===================
Key_SetBinding
===================
*/
void key_set_binding (int keynum, char *binding)
{
   if (keynum == -1)
      return;

   // free old bindings
   /*if ( keybindings[keynum].length () )
   {
      keybindings[keynum] = empty_string;
   }*/

   // new binding
   keybindings[keynum] = binding;
}

/*
===================
Key_Unbind_f
===================
*/
BEGIN_COMMAND (unbind, "Remove command from a key")
{
   int b;

   if (argc != 2)
   {
      console.printf (con_description, "unbind <key> : remove command from a key\n");
      return;
   }

   b = key_string_to_keynum ( argv[1].get_s () );

   if (b == -1)
   {
      console.printf (con_warning, "\"%s\" isn't a valid key\n", argv[1].get_s () );
      return;
   }

   key_set_binding (b, "\0");
}
END_COMMAND (unbind)


BEGIN_COMMAND (unbindall, "Removes all commands from all keys")
{
   int   i;

   for (i = 0; i < KEY_NAMES_MAX; i++)
      key_set_binding (i, empty_string);
}
END_COMMAND (unbindall)

/*
===================
Key_Bind_f
===================
*/
BEGIN_COMMAND (bind, "Attaches command to a key")
{
   int   i, c, b;
   char  cmd[1024];

   c = argc;

   if (c < 2)
   {
      console.printf (con_description, "bind <key> [command] : attach a command to a key\n");
      return;
   }

   b = key_string_to_keynum (argv[1].get_s ());

   if (b == -1)
   {
      console.printf (con_warning, "\"%s\" isn't a valid key\n", argv[1].get_s ());
      return;
   }

   if (c == 2)
   {
      if ( keybindings[b].length () )
         console.printf ("\"%s\" = \"%s\"\n", argv[1].get_s (), keybindings[b].get_s () );
      else
         console.printf ("\"%s\" is not bound\n", argv[1].get_s () );

      return;
   }

   // copy the rest of the command line
   cmd[0] = 0;      // start out with a null string
   for (i = 2; i < c; i++)
   {
      if (i > 2)
         strcat (cmd, " ");

      strcat (cmd, argv[i].get_s ());
   }

   key_set_binding (b, cmd);
}
END_COMMAND (bind)

/*
============
Key_WriteBindings

Writes lines containing "bind key value"
============
*/
void key_write_bindings (char *file)
{
   FILE  *fd;

   fd = fopen(file, "a");

   int   i;

   for (i = 0; i < KEY_NAMES_MAX; i++)
      if ( keybindings[i].length () )
         fprintf (fd, "bind \"%s\" \"%s\"\n", key_keynum_to_string(i), keybindings[i].get_s () );
            
   fclose(fd);
}

// obsolete
void key_read_bindings ()
{
   int   i;

   for (i = 0; i <= KEY_NAMES_MAX; i++)
      key_set_binding (i, copy_string (get_config_string("key_bindings", key_keynum_to_string (i), "\0")));
}


/*
===================
Key_Init
===================
*/
void key_init (void)
{
   int  i;
   char buf [64];

   i = 0;
   while ( keynames[i].keynum )
   {
      //console.debug_log ("debug.log", "strcpy ( %d, \"\")\n", i);
      //
      //strcpy(keybindings[i], "");

      if( !keynames[i].keynum )
      {
         ustrcpy (keynames[i].name, empty_string);
         keynames[i].keynum    = -1;
         keynames[i].printable = false;
         keynames[i].key       = 0;
         keynames[i].key_shift = 0;
      }

      i++;
   }

   for (i = 0; i < 32; i++)
   {
      ustrcpy (key_lines[i], empty_string);
   }
   key_linepos = 0;

   //
   // init ascii characters in console mode
   //
   i = 0;
   while ( keynames[i].keynum )
   {
      sprintf (buf, "key%d", i);
      keynames[i].key2       = get_config_int("key_ascii", buf, 0);
      keynames[i].key_shift2 = get_config_int("key_shift", buf, 0);

      if(keynames[i].printable && (keynames[i].keynum > -1))
      {
         consolekeys[keynames[i].keynum] = true;
      }

      i++;
   }

   consolekeys[KEY_HOME]                 = true;
   consolekeys[KEY_END]                  = true;
   consolekeys[KEY_SPACE]                = true;
   consolekeys[KEY_ENTER]                = true;
   consolekeys[KEY_ENTER_PAD]            = true;
   consolekeys[KEY_TAB]                  = true;
   consolekeys[KEY_LEFT]                 = true;
   consolekeys[KEY_RIGHT]                = true;
   consolekeys[KEY_UP]                   = true;
   consolekeys[KEY_DOWN]                 = true;
   consolekeys[KEY_BACKSPACE]            = true;
   consolekeys[KEY_PGUP]                 = true;
   consolekeys[KEY_PGDN]                 = true;
   consolekeys[KEY_SHIFT]                = true;
   consolekeys[KEY_MOUSE_WHEEL_UP]       = true;
   consolekeys[KEY_MOUSE_WHEEL_DOWN]     = true;
   consolekeys[KEY_TILDE]                = false;

   menubound[KEY_ESC] = true;
   for (i = 0; i < 12; i++)
      menubound[KEY_F1 + i] = true;
   
   // mouse
   for (i = KEY_MOUSE_START; i <= KEY_MOUSE_MAX; i++)
      menubound[i] = true;

   key_reset();

   //
   // install our keyboard handler
   //
   keyboard_lowlevel_callback = key_keyboard_callback;

   //
   // set the Russian codepage
   //
   #ifdef ALLEGRO_DOS
      set_ucodepage ((const unsigned short *)&codepage_ru866, NULL);
   #else
      set_ucodepage ((const unsigned short *)&codepage_ru1251, NULL);
   #endif
   //
   // don't give another chance to quit the game
   //
   #ifndef DEBUG
           three_finger_flag = false;
   #endif
}

/*
===================
Key_Event

Called by the system between frames for both key up and key down events
Should NOT be called during an interrupt!
===================
*/
void key_event (int key, boolean down, unsigned time)
{
   char  *kb;
   char  cmd[1024];

   keydown[key] = down;

   if (!down)
      key_repeats[key] = 0;

   key_lastpress = key;
   key_count++;
   if (key_count <= 0)
   {
      return;     // just catching keys for Con_NotifyBox
   }

   // update auto-repeat status
   if (down)
   {
      key_repeats[key]++;

      if ( key_dest != key_game )
      {
         // allow key repeats
      }
      else if (key_repeats[key] > 1)
      {
         return;
      }

      //if (key >= KEY_NAMES_MAX && !keybindings[key])
      //   console_printf ("%s is unbound, hit F4 to set.\n", key_keynum_to_string (key) );
   }

   //
   // handle escape specialy, so the user can never unbind it
   //
   if (key == KEY_ESC)
   {
      if (!down)
         return;

      switch (key_dest)
      {
         case key_message:
            key_message_poll (key);
            break;

         case key_menu:
            gui_key_event(key);
            break;

         case key_game:
         case key_console:
            cbuf_add_text ("menu reinit\n");
            break;

         default:
            sys_error ("Bad key_dest\n");
      }

      return;
   }

   //
   // key up events only generate commands if the game key binding is
   // a button command (leading + sign).  These will occur even in console mode,
   // to keep the character from continuing an action started before a console
   // switch.  Button commands include the keynum as a parameter, so multiple
   // downs can be matched with ups
   //
   if (!down)
   {
      kb = keybindings[key];
      if (kb && ugetat (kb, 0) == '+')
      {
         usprintf (cmd, "-%s %i %u\n", kb + ucwidth ('+'), key, time);
         cbuf_add_text (cmd);
      }

      return;
   }

   //
   // if not a consolekey, send to the interpreter no matter what mode is
   //
   if ( (key_dest == key_menu    && menubound[key])    ||
        (key_dest == key_console && !consolekeys[key]) ||
        (key_dest == key_game ) )
   {
      kb = keybindings[key];
      if (kb)
      {
         if (ugetat (kb, 0) == '+')
         {  // button commands add keynum as a parm
            usprintf (cmd, "%s %i %u\n", kb, key, time);
            cbuf_add_text (cmd);
         }
         else
         {
            cbuf_add_text (kb);
            cbuf_add_text ("\n");
         }
      }

      return;
   }

   if (!down)
      return;  // other systems only care about key down events

   switch (key_dest)
   {
      case key_game:
      case key_console:
         key_console_poll (key);
         break;

      case key_menu:
         gui_key_event(key);
         break;

      case key_message:
         key_message_poll (key);
         break;

      default:
         sys_error ("Bad key_dest");
  }
}


/*
===================
Key_ClearStates
===================
*/
void key_clear_states (void)
{
   int   i;

   for (i = 0; i <= KEY_NAMES_MAX; i++)
   {
      keydown[i] = false;
      key_repeats[i] = 0;
   }
}

BEGIN_COMMAND (bindlist, "Lists all binded keys")
{
   int            i;

   console.printf ("Binded keys:\n");

   for(i = 0; i < KEY_NAMES_MAX; i++)
      if( keybindings[i].length () && ustrlen (keynames[i].name) )
         console.printf (" %s = %s\n", key_keynum_to_string(i), keybindings[i].get_s () );
}
END_COMMAND (bindlist)

