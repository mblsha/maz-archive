#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <time.h>

#include "def.h"

keydest_t        old_dest;

Console_notify console_notify;
Console        console;

CVAR (console_notifytime, "3", CVAR_ARCHIVE);   //seconds

BEGIN_CUSTOM_CVAR (notify_display, "16", CVAR_ARCHIVE)
{
   if (var.value < 0)
      var.set ((float)0);
   else if (var.value > MAX_NOTIFY)
      var.set ((float)MAX_NOTIFY);
}
END_CUSTOM_CVAR (notify_display)

BEGIN_CUSTOM_CVAR (history, "100", CVAR_ARCHIVE)
{
   if (var.value < 5)
   {
      var.set ((float)5);
      return;
   }

   console.check_resize ();
}
END_CUSTOM_CVAR (history)


static int colors [MAXCONCOLOR];

CVAR (console_instant,      "0",   CVAR_ARCHIVE);
CVAR (console_speed,        "300", CVAR_ARCHIVE);
CVAR (console_cursor_speed, "4",   CVAR_ARCHIVE);
CVAR (developer,            "0",   CVAR_ARCHIVE);

extern boolean  team_message;

////////////////////////////////////////////////////////////////////////////////////////

Console_notify::Console_notify ()
{
   //console.debug_log ("debug.log", "Console_notify::Console_notify() ");
   
   clear ();

   //console.debug_log ("debug.log", "ok\n");
}

Console_notify::~Console_notify ()
{
   clear ();
}

void Console_notify::add (console_color color, char *text)
{
   int i;

   notify_item temp;

   temp = notify [MAX_NOTIFY - 1];

   for ( i = MAX_NOTIFY - 1; i > 0; i--)
   {
      notify [i] = notify [i - 1];
   }

   notify [0] = temp;

   notify [0].text  = text;
   notify [0].color = color;
   notify [0].time  = curtime;
}

extern char chat_buffer[];
void Console_notify::draw (BITMAP *bmp)
{
   if (key_dest == key_message)
   {
      textprintf (bmp, console.font, 0, 0, makecol (255,255,255), "say \"%s\"", chat_buffer);
   }

   int i;
   int v;
   unsigned long t;
   int col;
   char *text;

   col = colors [ con_outline ];

   v = 1;
   for ( i = (int)notify_display.value; i >= 0 ; i--)
   {
      t = notify[i].time;

      if (t == 0)
         continue;

      t = curtime - t;

      if ((float)(t/1000) > console_notifytime.value)
         continue;

      text = notify[i].text.get_s ();

      if(console.outline)
      {
         textout (bmp, console.font, text, console.char_w-1, v,   col);
         textout (bmp, console.font, text, console.char_w,   v-1, col);
         textout (bmp, console.font, text, console.char_w+1, v,   col);
         textout (bmp, console.font, text, console.char_w,   v+1, col);
      }
      textout (bmp, console.font, text, console.char_w, v, colors[ notify[ i ].color ]);

      v += console.char_h;
   }
}

void Console_notify::clear ()
{
   int i;

   for ( i = MAX_NOTIFY - 1; i >= 0; i--)
   {
      notify [i].text  = "";
      notify [i].color = con_default;
      notify [i].time  = 0;
   }
}

////////////////////////////////////////////////////////////////////////////////////////


#define              MAXCMDLINE  256
extern char          key_lines[32][MAXCMDLINE*2];
extern int           edit_line;
extern int           key_linepos;


Console::Console ()
{
   //console.debug_log ("debug.log", "Console::Console() ");

#ifdef DEBUG
   debuglog = true;
#else
   debuglog = com_check_parm("-condebug");
#endif

   if (debuglog)
   {
      time_t now;
      time(&now);

      unlink    ( "console.log" );
      debug_log ("console.log", "Log started: %s\n", asctime(localtime(&now)));
   }

   totallines = 0;
   lines = NULL;

   //check_resize ();
   
   //console.debug_log ("debug.log", "ok\n");
}

Console::~Console ()
{
   if (lines)
   {
      delete[] lines;
      lines = NULL;
   }

   // Hmmm... Very Strange... I have many crashes on these lines... So they are commented.
/*   if (background)
   {
      destroy_bitmap (background);
      background = NULL;
   }*/
}

void Console::check_resize ()
{
   console_line *tmp;
   int i;
   long size;

   size = totallines;

   totallines = (int)history.value;

   if (!totallines)
   {
      //console.debug_log ("debug.log", "history.set ");
      history.set ((float)100);
      return;
   }

   if (lines)
   {
      if ( size != totallines )
      {
         console.debug_log ("debug.log", "console history resizing ");
         
         tmp = new console_line[totallines+1];

         if (!tmp)
            sys_error ("Insufficient memory for console temp buffer!\n");

         // copy from console to temp buffer
         for (i = 0; i <= MIN (totallines, size); i++)
         {
            tmp[i] = lines[i];
         }

         // reallocate console lines
         delete[] lines;

         lines = new console_line[totallines+1];

         if (!lines)
            sys_error ("Insufficient memory for console history!\n");

         clear ();

         // copy console history back
         for (i = 0; i <= MIN (totallines, size); i++)
         {
            lines[i] = tmp[i];
         }

         // destroy temp buffer
         delete[] tmp;
      }
   }
   else
   {
      lines = new console_line[totallines+1];

      if (!lines)
         sys_error ("Insufficient memory for console!\n");

      clear ();
   }

   if (char_w && char_h)
   {
      linewidth = (SCREEN_W / char_w) - 2;
      float temp = char_h * 1.5;
      input_offset = (int)temp;

      if (!initialized)
      {
         initialized = true;
         printf ("Console initialized.\n");
      }
   }
}

void Console::draw_background (BITMAP *bmp, int _h)
{
   draw_sprite(bmp, background, 0, _h-SCREEN_H);
}

void Console::draw_input (BITMAP *bmp)
{
   int      y;
   char     *text;
   char     temp[1024];
   float    t;
   int      col;

   if ( key_dest != key_console ) //&& !con_forcedup)
      return;    // don't draw anything

   ustrcpy (temp, "]");
   ustrcat (temp, key_lines [edit_line]);

   // add the cursor frame
   if(console_cursor_speed.value)
   {
      t = ((curtime)/(1000*console_cursor_speed.value));
      ustrcat (temp, (((int) t) & 1) ? "_" : " ");
   }
   else
   {
      ustrcat (temp, "_");
   }

   ustrcat (temp, empty_string);

   // prestep if horizontally scrolling
   int   i = 0, c;
   while ( ustrlen (temp + i) > linewidth )
   {
      c = ugetc (temp + i);
      i += ucwidth (c);
   }
   text = temp + i;

   // draw it
   y = vislines - input_offset;

   if(outline)
   {
      col = colors [con_outline];

      textout(bmp, font, text, char_w-1, y,   col);
      textout(bmp, font, text, char_w,   y-1, col);
      textout(bmp, font, text, char_w+1, y,   col);
      textout(bmp, font, text, char_w,   y+1, col);
   }
   textout(bmp, font, text, char_w, y, colors[con_command]);
}

void Console::draw (BITMAP *bmp, int offs, boolean drawinput)
{
   int      i, y;
   int      rows;
   char     *text;
   int      col = colors [ con_outline ];
   int      max;

   //if (offs <= 0)
   //   return;

   // draw the background
   //draw_background (bmp, lines);
   draw_sprite (bmp, background, 0, offs-SCREEN_H);

   // draw the text
   vislines = offs;

   rows = (offs - (2 * char_h)) / char_h;    // rows of text to draw (visible area height)
   //y    =  offs - (2 * char_h) - (rows * char_h); // start y coord

   y = offs - (char_h + input_offset) - (2);

   max = MIN(rows + backscroll, totallines);

   for ((i = backscroll); i <= max; i++, y -= char_h + 1)
   {
      text = lines [i].text.get_s ();

      if ( strlen (text) )
      {
         if ( outline )
         {
            textout (bmp, font, text, char_w-1, y,   col);
            textout (bmp, font, text, char_w,   y-1, col);
            textout (bmp, font, text, char_w+1, y,   col);
            textout (bmp, font, text, char_w,   y+1, col);
         }

         textout (bmp, font, text, char_w, y, colors [ lines[i].color ]);
      }
   }

   // draw the input prompt, user text, and cursor if desired
   if (drawinput)
      draw_input (bmp);
}

void Console::set_color (console_color color, int value)
{
   colors [color] = value;
}

void Console::set_properties (FONT *_font, boolean _outline)
{
   font = _font;
   outline = _outline;

   char_w = text_length ( font, "#" );
   char_h = text_height ( font );

   check_resize ();
}

void Console::set_background (BITMAP *image, /*boolean tiled,*/ int color)
{
   int w, h;

   if(background)
   {
      destroy_bitmap(background);
      background = NULL;
   }

   background = make_bitmap(SCREEN_W, SCREEN_H, 2, true);
   clear_bitmap (background);

   if(image)
   {
      w = image->w;
      h = image->h;

      blit(image, background, 0, 0, 0, 0, w, h);
   }
   else
   {
      back_col = color;
      clear_to_color(background, back_col);
   }
}

void Console::animate ()
{
   float t;

   if(action == con_closed || action == con_active)
      return;

   // handle console moving
   if(console_instant.value)
   {
      switch(action)
      {
         case con_opening:
            action = con_active;
            pos    = max_lines;
            break;

         case con_closing:
            action = con_closed;
            pos    = 0;
            break;

         default:
            break;
      }
   }
   else
   {
      if(console_speed.value <= 0)
         console_speed.set ("300");

      t = console_speed.value * frame_time;

      switch(action)
      {
         case con_opening:
            pos += t;
            break;

         case con_closing:
            pos -= t;
            break;

         default:
            break;
      }

      if (pos >= max_lines)
      {
         pos    = max_lines;
         action = con_active;
      }
      else if (pos <= 0)
      {
         pos    = 0;
         action = con_closed;
      }
   }
}

void Console::line_feed (console_color color, char *text)
{
   int i;

   console_line temp = lines[totallines - 1];

   for ( i = totallines; i > 0; i--)
   {
      lines [i] = lines [i - 1];
   }

   lines [0] = temp;

   lines [0].color = color;
   lines [0].text  = text;
}

char *Console::anti_line_feed ()
{
   int i;

   console_line temp = lines[0];

   for ( i = 0; i < totallines; i++)
   {
      lines [i] = lines [i + 1];
   }

   lines [totallines] = temp;
   lines [totallines].text = "";

   return temp.text.get_s ();
}

void Console::print (console_color color, char *txt)
{
   int               c, len;
   static boolean    cr;
   static char       string[1024];

   //if (!totallines)
   //   check_resize ();

   while ( (c = ugetc (txt)) )
   {
      // count word length for word wrap
      for (len = 0; len < linewidth ; len++)
         if (ugetat ((const char *)txt, len) <= ' ')
            break;

      // word wrap
      if (len != linewidth && (con_x + len > linewidth) )
         con_x = 0;

      //txt++;

      switch (c)
      {
         case '\n':
            usetat (string, ++con_x, 0);
            con_x = 0;
            break;

         case '\r':
            con_x = 0;
            cr = true;
            break;

         default:
            usetat (string, con_x, c);

            con_x++;
            if (con_x >= linewidth)
               con_x = 0;
            break;
      }

      if (cr)
      {
         ustrcpy ( string, anti_line_feed () );

         cr = false;
      }

      if ( !con_x )
      {
         console_notify.add ( color, string );
         line_feed (color, string);

         memset (string, '\0', linewidth);
      }

      txt += uwidth (txt);
   }
}

void Console::printf (console_color color, char *fmt, ...)
{
   va_list  argptr;
   char     msg[MAXPRINTMSG];
   BITMAP         *bmp;

   va_start  (argptr, fmt);
   uvsprintf (msg, fmt, argptr);
   va_end    (argptr);

   // log all messages to file
   if (debuglog)
      console.debug_log("console.log", "%s%s", initialized ? "" : "<CANNOT PRINTF>", msg);

   if (!initialized)
      return;

   //if (cls.state == ca_dedicated)
   //  return;    // no graphics mode

   // write it to the scrollable buffer
   print (color, msg);

   if (sv_state == sv_disconnected)
   {
      bmp = create_bitmap (SCREEN_W, SCREEN_H);
      clear_bitmap (bmp);
      console.draw (bmp, SCREEN_H, false);

      blit (bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   }
}

void Console::printf (char *fmt, ...)
{
   va_list   argptr;
   char      msg[MAXPRINTMSG];

   va_start  (argptr, fmt);
   uvsprintf (msg, fmt, argptr);
   va_end    (argptr);

   printf (con_default, msg);
}

void Console::dprintf (char *fmt, ...)
{
   va_list  argptr;
   char     msg[MAXPRINTMSG];

   if ( !developer.value )
      return;

   va_start  (argptr, fmt);
   uvsprintf (msg, fmt, argptr);
   va_end    (argptr);

   printf (con_default, msg);
}

void Console::dprintf (console_color color, char *fmt, ...)
{
   va_list  argptr;
   char     msg[MAXPRINTMSG];

   if ( !developer.value )
      return;

   va_start  (argptr, fmt);
   uvsprintf (msg, fmt, argptr);
   va_end    (argptr);

   printf (color, msg);
}

void Console::debug_log (char *file, char *fmt, ...)
{
   va_list argptr;
   static char data[8192];
   int fd;
    
   va_start  (argptr, fmt);
   uvsprintf (data, fmt, argptr);
   va_end    (argptr);

   fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
   write(fd, data, strlen(data));
   close(fd);
}

boolean Console::is_active ()
{
   return (boolean)(action != con_closed);
}

void Console::set_action (console_action what)
{
   action = what;
}

void Console::set_height (int height)
{
   int oldlines = max_lines;

   if(height < 2*char_h)
      height = 2*char_h;

   if(height > SCREEN_H)
      height = SCREEN_H;

   max_lines = height;

   if (action == con_active)
   {
      if(oldlines < max_lines)
         action = con_opening;
      else
         action = con_closing;
   }
}

void Console::toggle ()
{
   switch(action)
   {
      case con_active:
      case con_opening:
         action = con_closing;

         key_dest = old_dest;
         break;

      case con_closed:
      case con_closing:
         action = con_opening;

         old_dest = key_dest;
         key_dest = key_console;

         if(old_dest == key_menu || old_dest == key_console)
            old_dest = key_game;
         break;

      default:
         break;
   }

   console_notify.clear ();
}

void Console::clear ()
{
   int i;

   for (i = totallines; i >= 0; i--)
   {
      lines [i].text  = "";
      lines [i].color = con_default;
   }
}

////////////////////////////////////////////////////////////////////////////////////////

BEGIN_CUSTOM_CVAR (console_height, "500", CVAR_ARCHIVE)
{
   float lines;

   lines = var.value;

   if (lines > 1000)
      lines = SCREEN_H;
   else if (lines < 10)
      lines = 10;

   if (lines != var.value)
      var.set (lines);

   console.set_height (SCREEN_H * (lines / 1000));
}
END_CUSTOM_CVAR (console_height)

BEGIN_COMMAND (con_color, "Sets console colors")
{
   if ( argc < 5 )
   {
      console.printf (con_description, "%s <color_num> <r> <g> <b>\n", argv[0].get_s ());
   }
//   else if ( argc == 2 )
//   {
//      console.printf ("color #%d (%d, %d, %d)\n", argv[1]);
//   }
   else
   {
      console.set_color ((console_color) argv[1].get_int (), makecol( argv[2].get_int (), argv[3].get_int (), argv[4].get_int ()) );
   }
}
END_COMMAND (con_color)

BEGIN_COMMAND (toggleconsole, "This is usually binded to tilde key")
{
   console.toggle ();
}
END_COMMAND (toggleconsole)

BEGIN_COMMAND (clear, "Clears console history")
{
   console.clear ();
}
END_COMMAND (clear)

BEGIN_COMMAND (clear_notify, "Clears console notify")
{
   console_notify.clear ();
}
END_COMMAND (clear_notify)

extern boolean team_message;

BEGIN_COMMAND (messagemode, "Chat Mode")
{
   if (key_dest == key_console)
      return; 

   key_dest = key_message;
   team_message = false;
}
END_COMMAND (messagemode)

BEGIN_COMMAND (messagemode2, "Chat with teammates Mode")
{
   if (key_dest == key_console)
      return; 

  key_dest = key_message;
  team_message = true;
}
END_COMMAND (messagemode2)


