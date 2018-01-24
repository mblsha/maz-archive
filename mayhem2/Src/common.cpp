#include "def.h"
#include "gui.h"

#include <sys/stat.h>


jmp_buf abortframe;     // an ERR_DROP occured, exit the entire frame


char  com_token[1024];
int   com_argc;
char  **com_argv;

void replace_string (char **ptr, char *str)
{
   if (*ptr)
   {
      if (*ptr == str)
         return;

      delete[] *ptr;
   }
   *ptr = copy_string (str);
}

char *copy_string (const char *s)
{
   char *b;

   if (s)
   {
      b = new char[strlen(s)+1];
      strcpy (b, s);
   }
   else
   {
      b = new char[1];
      b[0] = '\0';
   }

   return b;
}

void com_error (err_code_t code, char *fmt, ...)
{
   va_list     argptr;
   static char msg[MAXPRINTMSG];
   static      boolean         recursive;

   if (recursive)
      sys_error ("recursive error after: %s", msg);
   recursive = true;

   va_start (argptr,fmt);
   vsprintf (msg,fmt,argptr);
   va_end (argptr);

   if (code == err_disconnect)
   {
//      cl_drop ();
      recursive = false;
      longjmp (abortframe, -1);
   }
   else if (code == err_drop)
   {
      console.printf ("********************\nERROR: %s\n********************\n", msg);
//      sv_shutdown (va("Server crashed: %s\n", msg), false);
//      cl_drop ();
      recursive = false;
      longjmp (abortframe, -1);
   }
   else
   {
//      sv_shutdown (va("Server fatal crashed: %s\n", msg), false);
//      cl_shutdown ();
   }
/*
   if (logfile)
   {
      fclose (logfile);
      logfile = NULL;
   }
*/
   sys_error ("%s", msg);
}

void sys_error (char *str, ...)
{
   console.debug_log ("debug.log", "sys_error: %s\n", str);
   console.printf    (con_error,   "sys_error: %s\n", str);

   set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
   allegro_exit ();

   allegro_message ("%s\n", str);

   exit (1);
}

/*
================
COM_CheckParm

Returns the position (1 to argc-1) in the program's argument list
where the given parameter apears, or 0 if not present
================
*/
int com_check_parm (char *parm)
{
   int             i;

   for (i = 1; i < com_argc; i++)
   {
      if (!com_argv[i])
         continue;               // NEXTSTEP sometimes clears appkit vars.

      if (!strcmp (parm, com_argv[i]))
         return i;
   }

   return 0;
}


char *com_load_file (char *path)
{
   char     *buf = NULL;
   int      len;
   PACKFILE *file;

   if(!(file = pack_fopen(path, "rp")))
   {
      if(!(file = pack_fopen(path, "r")))
         return NULL;
   }

   len = file_size(path);
   if (!len)
   {
      pack_fclose (file);
      return NULL;
   }
   
   buf = (char *)calloc ( sizeof (char *), len+1);

   if (!buf)
   {
      pack_fclose (file);
      sys_error ("com_load_file: not enough space for %s (need %d bytes)", path, len+1);
   }
   
   pack_fread  (buf, len, file);
   pack_fclose (file);

   return buf;
}


char *com_parse2 (char *data)
{
   char *tok = NULL;
   char *prev_data;
   int c;

   ustrcpy (com_token, empty_string);

skipwhite:

   if (!data)
      return NULL;

   //
   // skip whitespace
   //
   prev_data = data;
   c = ugetxc ((const char **)&data);
   
   if ( c <= ' ')
   {
      if (c == 0)
      {
         return NULL;         // end of string encountered
      }

      goto skipwhite;
   }

   //
   // skip // comments
   //
   if (c == '/' && ugetc (data) == '/')
   {
      while (1)
      {
         c = ugetxc ((const char **)&data);

         if (c == '\n' || c == 0)
            break;
      }

      goto skipwhite;
   }

   //
   // handle quoted strings specially
   //
   if (c == '\"')
   {
      tok = data;
      
      while (1)
      {
         prev_data = data;
         c = ugetxc ((const char **)&data);
         
         if (c == '\"' || c == 0)
         {
            if (c == 0)
               data = prev_data; // data--

            data += usetat (prev_data, 0, 0);

            ustrcpy (com_token, tok);

            return data;
         }
      }
   }

   tok = prev_data;

   //
   // parse a regular word
   //
   do
   {
      prev_data = data;
      c = ugetxc ((const char **)&data);
      
   } while (c > ' ');
   
   data += usetat (prev_data, 0, 0);
   ustrcpy (com_token, tok);
   
   return data;
}

char *com_parse (char *data)
{
   cvar     *var;
   ustring  str;

   if ( (data = com_parse2 (data)) )
   {
      //
      // Replace $cvar_name for cvar_name->string
      //
      if ( ugetc ((const char *)com_token) == '$')
      {
         str = &com_token[1];
         
         if ( (var = cvar_find_var (str)) )
         {
            ustrcpy (com_token, var->string);
         }
      }
   }

   return data;
}
/*
char *com_parse (char *data)
{
   int c;
   int len;

   len = 0;
   com_token[0] = 0;

   if (!data)
      return NULL;

   // skip whitespace
skipwhite:
   while ( (c = *data) <= ' ')
   {
      if (c == 0)
         return NULL;                    // end of file

      data++;
   }

   // skip // comments
   if (c == '/' && data[1] == '/')
   {
      while (*data && *data != '\n')
         data++;

      goto skipwhite;
   }


   // handle quoted strings specially
   if (c == '\"')
   {
      data++;

      while (1)
      {
         c = *data++;
         if (c=='\"' || !c)
         {
            com_token[len] = 0;
            return data;
         }
         com_token[len] = c;
         len++;
      }
   }

   // parse single characters
   if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c==':')
   {
      com_token[len] = c;
      len++;
      com_token[len] = 0;
      return data+1;
   }

   // parse a regular word
   do
   {
      com_token[len] = c;
      data++;
      len++;
      c = *data;

      if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c==':')
         break;
   } while (c>32);

   // terminating '\0'
   com_token[len] = 0;

   return data;
}
*/

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char    *va(char *format, ...)
{
   va_list      argptr;
   static char  string[1024];

   va_start (argptr, format);
   vsprintf (string, format, argptr);
   va_end   (argptr);

   return string;
}


/// just for debugging
int memsearch (byte *start, int count, int search)
{
   int             i;

   for (i=0 ; i<count ; i++)
      if (start[i] == search)
         return i;
         
   return -1;
}

//=============================================

void sz_init (Sizebuf *buf, byte *data, int length)
{
   memset (buf, 0, sizeof(*buf));
   buf->data    = data;
   buf->maxsize = length;
}

void sz_alloc (Sizebuf *buf, int startsize)
{
   if (startsize < 256)
      startsize = 256;

   buf->data = (byte *)calloc (sizeof (byte *), startsize);
   //buf->data = malloc (startsize, "sizebuf");
   buf->maxsize = startsize;
   buf->cursize = 0;
   buf->overflowed = false;
}


void sz_free (Sizebuf *buf)
{
   free (buf->data);
   buf->data = NULL;
   buf->maxsize = 0;
   buf->cursize = 0;
}

void sz_clear (Sizebuf *buf)
{
   buf->cursize = 0;
   buf->overflowed = false;
}

void *sz_get_space (Sizebuf *buf, int length)
{
   void    *data;

   if (buf->cursize + length > buf->maxsize)
   {
      if (!buf->allowoverflow)
         com_error (err_fatal, "sz_get_space: overflow without allowoverflow set");

      if (length > buf->maxsize)
         com_error (err_fatal, "sz_get_space: %i is > full buffer size", length);

      buf->overflowed = true;
      console.printf (con_warning, "sz_get_space: overflow (need %d, but has only %d)", buf->cursize + length, buf->maxsize);
      sz_clear (buf);
   }

   data = buf->data + buf->cursize;
   buf->cursize += length;

   return data;
}

void sz_write (Sizebuf *buf, void *data, int length)
{
   memcpy (sz_get_space(buf, length), data, length);
}

void sz_print (Sizebuf *buf, char *data)
{
   int len;

   len = ustrlen(data)+1;

   if (buf->cursize)
   {
      if (buf->data[buf->cursize - 1])
         memcpy ((byte *)sz_get_space(buf, len), data, len); // no trailing 0
      else
         memcpy ((byte *)sz_get_space(buf, len - 1) - 1, data, len); // write over trailing 0
   }
   else
   {
      memcpy ((byte *)sz_get_space(buf, len), data, len);
   }
}


/*
============================================================================

                     BYTE ORDER FUNCTIONS

============================================================================
*/

boolean  bigendien;

// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places

short   (*_big_short)      (short l);
short   (*_little_short)   (short l);
int     (*_big_long)       (int l);
int     (*_little_long)    (int l);
float   (*_big_float)      (float l);
float   (*_little_float)   (float l);

short big_short    (short l)  {return _big_short    (l);}
short little_short (short l)  {return _little_short (l);}
int   big_long     (int   l)  {return _big_long     (l);}
int   little_long  (int   l)  {return _little_long  (l);}
float big_float    (float l)  {return _big_float    (l);}
float little_float (float l)  {return _little_float (l);}

short short_swap (short l)
{
   byte    b1,b2;

   b1 = l&255;
   b2 = (l>>8)&255;

   return (b1<<8) + b2;
}

short short_no_swap (short l)
{
   return l;
}

int long_swap (int l)
{
   byte    b1,b2,b3,b4;

   b1 = l&255;
   b2 = (l>>8)&255;
   b3 = (l>>16)&255;
   b4 = (l>>24)&255;

   return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}

int long_no_swap (int l)
{
   return l;
}

float float_swap (float f)
{
   union
   {
      float f;
      byte  b[4];
   } dat1, dat2;


   dat1.f = f;
   dat2.b[0] = dat1.b[3];
   dat2.b[1] = dat1.b[2];
   dat2.b[2] = dat1.b[1];
   dat2.b[3] = dat1.b[0];
   return dat2.f;
}

float float_no_swap (float f)
{
   return f;
}

/*
================
Swap_Init
================
*/
void swap_init (void)
{
   byte  swaptest[2] = {1,0};

   // set the byte swapping variables in a portable manner
   if ( *(short *)swaptest == 1)
   {
      bigendien = false;
      _big_short    = short_swap;
      _little_short = short_no_swap;
      _big_long     = long_swap;
      _little_long  = long_no_swap;
      _big_float    = float_swap;
      _little_float = float_no_swap;
   }
   else
   {
      bigendien = true;
      _big_short    = short_no_swap;
      _little_short = short_swap;
      _big_long     = long_no_swap;
      _little_long  = long_swap;
      _big_float    = float_no_swap;
      _little_float = float_swap;
   }
}

/*
==============================================================================

                  MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

//
// writing functions
//

void msg_write_char (Sizebuf *sb, int c)
{
   byte  *buf;

#ifdef PARANOID
   if (c < -128 || c > 127)
      sys_error ("msg_write_char: range error");
#endif

   buf = (byte *)sz_get_space (sb, 1);
   buf[0] = c;
}

void msg_write_byte (Sizebuf *sb, int c)
{
   byte  *buf;

#ifdef PARANOID
   if (c < 0 || c > 255)
      sys_error ("msg_write_byte: range error");
#endif

   buf = (byte *)sz_get_space (sb, 1);
   buf[0] = c;
}

void msg_write_short (Sizebuf *sb, int c)
{
   byte  *buf;

#ifdef PARANOID
   if (c < ((short)0x8000) || c > (short)0x7fff)
      sys_error ("msg_write_short: range error");
#endif

   buf = (byte *)sz_get_space (sb, 2);
   buf[0] = c&0xff;
   buf[1] = c>>8;
}

void msg_write_long (Sizebuf *sb, int c)
{
   byte  *buf;

   buf = (byte *)sz_get_space (sb, 4);
   buf[0] = c&0xff;
   buf[1] = (c>>8)&0xff;
   buf[2] = (c>>16)&0xff;
   buf[3] = c>>24;
}

void msg_write_float (Sizebuf *sb, float f)
{
   union
   {
      float f;
      int   l;
   } dat;


   dat.f = f;
   dat.l = little_long (dat.l);

   sz_write (sb, &dat.l, 4);
}

void msg_write_string (Sizebuf *sb, char *s)
{
   if (!s)
      sz_write (sb, (char *)"", 1);
   else
      sz_write (sb, s, strlen(s)+1);
}

void msg_write_coord (Sizebuf *sb, float f)
{
   msg_write_short (sb, (int)(f*8));
}

void msg_write_pos (Sizebuf *sb, Vec2 pos)
{
   msg_write_short (sb, (int)(pos[0]*8));
   msg_write_short (sb, (int)(pos[1]*8));
   msg_write_short (sb, (int)(pos[2]*8));
}

void msg_write_angle (Sizebuf *sb, float f)
{
   msg_write_byte (sb, (int)(f*256/360) & 255);
}

void msg_write_angle16 (Sizebuf *sb, float f)
{
   msg_write_short (sb, ANGLE2SHORT(f));
}

/*
void msg_write_delta_usercmd (Sizebuf *buf, usercmd_t *from, usercmd_t *cmd)
{
   int   bits;

   //
   // send the movement message
   //
   bits = 0;
   if (cmd->angles[0] != from->angles[0])
      bits |= CM_ANGLE1;
   if (cmd->angles[1] != from->angles[1])
      bits |= CM_ANGLE2;
   if (cmd->angles[2] != from->angles[2])
      bits |= CM_ANGLE3;
   if (cmd->forwardmove != from->forwardmove)
      bits |= CM_FORWARD;
   if (cmd->sidemove != from->sidemove)
      bits |= CM_SIDE;
   if (cmd->upmove != from->upmove)
      bits |= CM_UP;
   if (cmd->buttons != from->buttons)
      bits |= CM_BUTTONS;
   if (cmd->impulse != from->impulse)
      bits |= CM_IMPULSE;

    msg_write_byte (buf, bits);

   if (bits & CM_ANGLE1)
      msg_write_short (buf, cmd->angles[0]);
   if (bits & CM_ANGLE2)
      msg_write_short (buf, cmd->angles[1]);
   if (bits & CM_ANGLE3)
      msg_write_short (buf, cmd->angles[2]);

   if (bits & CM_FORWARD)
      msg_write_short (buf, cmd->forwardmove);
   if (bits & CM_SIDE)
      msg_write_short (buf, cmd->sidemove);
   if (bits & CM_UP)
      msg_write_short (buf, cmd->upmove);

   if (bits & CM_BUTTONS)
      msg_write_byte (buf, cmd->buttons);
   if (bits & CM_IMPULSE)
      msg_write_byte (buf, cmd->impulse);

   msg_write_byte (buf, cmd->msec);
   msg_write_byte (buf, cmd->lightlevel);
}
*/
/*
void msg_write_dir (Sizebuf *sb, Vec2 dir)
{
   int   i, best;
   float d, bestd;

   if (!dir)
   {
      msg_write_byte (sb, 0);
      return;
   }

   bestd = 0;
   best  = 0;
   for (i = 0; i < NUMVERTEXNORMALS; i++)
   {
      d = dot_product (dir, bytedirs[i]);
      if (d > bestd)
      {
         bestd = d;
         best = i;
      }
   }
   msg_write_byte (sb, best);
}
*/

/*
==================
msg_write_delta_entity

Writes part of a packetentities message.
Can delta from either a baseline or a previous packet_entity
==================
*/
/*
void msg_write_delta_entity (entity_state_t *from, entity_state_t *to, Sizebuf *msg, qboolean force, boolean newentity)
{
	int		bits;

	if (!to->number)
		Com_Error (ERR_FATAL, "Unset entity number");
	if (to->number >= MAX_EDICTS)
		Com_Error (ERR_FATAL, "Entity number >= MAX_EDICTS");

// send an update
	bits = 0;

	if (to->number >= 256)
		bits |= U_NUMBER16;		// number8 is implicit otherwise

	if (to->origin[0] != from->origin[0])
		bits |= U_ORIGIN1;
	if (to->origin[1] != from->origin[1])
		bits |= U_ORIGIN2;
	if (to->origin[2] != from->origin[2])
		bits |= U_ORIGIN3;

	if ( to->angles[0] != from->angles[0] )
		bits |= U_ANGLE1;		
	if ( to->angles[1] != from->angles[1] )
		bits |= U_ANGLE2;
	if ( to->angles[2] != from->angles[2] )
		bits |= U_ANGLE3;
		
	if ( to->skinnum != from->skinnum )
	{
		if ((unsigned)to->skinnum < 256)
			bits |= U_SKIN8;
		else if ((unsigned)to->skinnum < 0x10000)
			bits |= U_SKIN16;
		else
			bits |= (U_SKIN8|U_SKIN16);
	}
		
	if ( to->frame != from->frame )
	{
		if (to->frame < 256)
			bits |= U_FRAME8;
		else
			bits |= U_FRAME16;
	}

	if ( to->effects != from->effects )
	{
		if (to->effects < 256)
			bits |= U_EFFECTS8;
		else if (to->effects < 0x8000)
			bits |= U_EFFECTS16;
		else
			bits |= U_EFFECTS8|U_EFFECTS16;
	}
	
	if ( to->renderfx != from->renderfx )
	{
		if (to->renderfx < 256)
			bits |= U_RENDERFX8;
		else if (to->renderfx < 0x8000)
			bits |= U_RENDERFX16;
		else
			bits |= U_RENDERFX8|U_RENDERFX16;
	}
	
	if ( to->solid != from->solid )
		bits |= U_SOLID;

	// event is not delta compressed, just 0 compressed
	if ( to->event  )
		bits |= U_EVENT;
	
	if ( to->modelindex != from->modelindex )
		bits |= U_MODEL;
	if ( to->modelindex2 != from->modelindex2 )
		bits |= U_MODEL2;
	if ( to->modelindex3 != from->modelindex3 )
		bits |= U_MODEL3;
	if ( to->modelindex4 != from->modelindex4 )
		bits |= U_MODEL4;

	if ( to->sound != from->sound )
		bits |= U_SOUND;

	if (newentity || (to->renderfx & RF_BEAM))
		bits |= U_OLDORIGIN;

	//
	// write the message
	//
	if (!bits && !force)
		return;		// nothing to send!

	//----------

	if (bits & 0xff000000)
		bits |= U_MOREBITS3 | U_MOREBITS2 | U_MOREBITS1;
	else if (bits & 0x00ff0000)
		bits |= U_MOREBITS2 | U_MOREBITS1;
	else if (bits & 0x0000ff00)
		bits |= U_MOREBITS1;

	msg_write_Byte (msg,	bits&255 );

	if (bits & 0xff000000)
	{
		msg_write_Byte (msg,	(bits>>8)&255 );
		msg_write_Byte (msg,	(bits>>16)&255 );
		msg_write_Byte (msg,	(bits>>24)&255 );
	}
	else if (bits & 0x00ff0000)
	{
		msg_write_Byte (msg,	(bits>>8)&255 );
		msg_write_Byte (msg,	(bits>>16)&255 );
	}
	else if (bits & 0x0000ff00)
	{
		msg_write_Byte (msg,	(bits>>8)&255 );
	}

	//----------

	if (bits & U_NUMBER16)
		msg_write_Short (msg, to->number);
	else
		msg_write_Byte (msg,	to->number);

	if (bits & U_MODEL)
		msg_write_Byte (msg,	to->modelindex);
	if (bits & U_MODEL2)
		msg_write_Byte (msg,	to->modelindex2);
	if (bits & U_MODEL3)
		msg_write_Byte (msg,	to->modelindex3);
	if (bits & U_MODEL4)
		msg_write_Byte (msg,	to->modelindex4);

	if (bits & U_FRAME8)
		msg_write_Byte (msg, to->frame);
	if (bits & U_FRAME16)
		msg_write_Short (msg, to->frame);

	if ((bits & U_SKIN8) && (bits & U_SKIN16))		//used for laser colors
		msg_write_Long (msg, to->skinnum);
	else if (bits & U_SKIN8)
		msg_write_Byte (msg, to->skinnum);
	else if (bits & U_SKIN16)
		msg_write_Short (msg, to->skinnum);


	if ( (bits & (U_EFFECTS8|U_EFFECTS16)) == (U_EFFECTS8|U_EFFECTS16) )
		msg_write_Long (msg, to->effects);
	else if (bits & U_EFFECTS8)
		msg_write_Byte (msg, to->effects);
	else if (bits & U_EFFECTS16)
		msg_write_Short (msg, to->effects);

	if ( (bits & (U_RENDERFX8|U_RENDERFX16)) == (U_RENDERFX8|U_RENDERFX16) )
		msg_write_Long (msg, to->renderfx);
	else if (bits & U_RENDERFX8)
		msg_write_Byte (msg, to->renderfx);
	else if (bits & U_RENDERFX16)
		msg_write_Short (msg, to->renderfx);

	if (bits & U_ORIGIN1)
		msg_write_Coord (msg, to->origin[0]);		
	if (bits & U_ORIGIN2)
		msg_write_Coord (msg, to->origin[1]);
	if (bits & U_ORIGIN3)
		msg_write_Coord (msg, to->origin[2]);

	if (bits & U_ANGLE1)
		msg_write_Angle(msg, to->angles[0]);
	if (bits & U_ANGLE2)
		msg_write_Angle(msg, to->angles[1]);
	if (bits & U_ANGLE3)
		msg_write_Angle(msg, to->angles[2]);

	if (bits & U_OLDORIGIN)
	{
		msg_write_Coord (msg, to->old_origin[0]);
		msg_write_Coord (msg, to->old_origin[1]);
		msg_write_Coord (msg, to->old_origin[2]);
	}

	if (bits & U_SOUND)
		msg_write_Byte (msg, to->sound);
	if (bits & U_EVENT)
		msg_write_Byte (msg, to->event);
	if (bits & U_SOLID)
		msg_write_Short (msg, to->solid);
}
*/

//
// reading functions
//

void msg_begin_reading (Sizebuf *msg)
{
   msg->readcount = 0;
}

// returns -1 if no more characters are available
int msg_read_char (Sizebuf *msg_read)
{
   int   c;

   if (msg_read->readcount+1 > msg_read->cursize)
   {
      c = -1;
   }
   else
   {
      c = (signed char)msg_read->data[msg_read->readcount];
   }
   
   msg_read->readcount++;

   return c;
}

int msg_read_byte (Sizebuf *msg_read)
{
   int   c;

   if (msg_read->readcount+1 > msg_read->cursize)
      c = -1;
   else
      c = (unsigned char)msg_read->data[msg_read->readcount];
   msg_read->readcount++;

   return c;
}

int msg_read_short (Sizebuf *msg_read)
{
   int   c;

   if (msg_read->readcount+2 > msg_read->cursize)
   {
      c = -1;
   }
   else
   {
      c = (short)(msg_read->data[msg_read->readcount]
               + (msg_read->data[msg_read->readcount+1]<<8));
   }
   
   msg_read->readcount += 2;

   return c;
}

int msg_read_long (Sizebuf *msg_read)
{
   int   c;

   if (msg_read->readcount+4 > msg_read->cursize)
   {
      c = -1;
   }
   else
   {
      c = msg_read->data[msg_read->readcount]
       + (msg_read->data[msg_read->readcount+1]<<8)
       + (msg_read->data[msg_read->readcount+2]<<16)
       + (msg_read->data[msg_read->readcount+3]<<24);
   }

   msg_read->readcount += 4;

   return c;
}

float msg_read_float (Sizebuf *msg_read)
{
   union
   {
      byte  b[4];
      float f;
      int   l;
   } dat;

   if (msg_read->readcount+4 > msg_read->cursize)
   {
      dat.f = -1;
   }
   else
   {
      dat.b[0] = msg_read->data[msg_read->readcount];
      dat.b[1] = msg_read->data[msg_read->readcount+1];
      dat.b[2] = msg_read->data[msg_read->readcount+2];
      dat.b[3] = msg_read->data[msg_read->readcount+3];
   }
   
   msg_read->readcount += 4;

   dat.l = little_long (dat.l);

   return dat.f;
}

char *msg_read_string (Sizebuf *msg_read)
{
   static char string[2048];
   int         l,c;

   l = 0;
   do
   {
      c = msg_read_char (msg_read);
      
      if (c == -1 || c == 0)
         break;
         
      string[l] = c;
      l++;
   } while (l < (int)sizeof(string)-1);

   string[l] = 0;

   return string;
}

char *msg_read_string_line (Sizebuf *msg_read)
{
   static char string[2048];
   int         l,c;

   l = 0;
   do
   {
      c = msg_read_char (msg_read);
      
      if (c == -1 || c == 0 || c == '\n')
         break;
         
      string[l] = c;
      l++;
   } while (l < (int)sizeof(string)-1);

   string[l] = 0;

   return string;
}

float msg_read_coord (Sizebuf *msg_read)
{
   return msg_read_short(msg_read) * (1.0/8);
}

void msg_read_pos (Sizebuf *msg_read, Vec2 pos)
{
   pos[0] = msg_read_short(msg_read) * (1.0/8);
   pos[1] = msg_read_short(msg_read) * (1.0/8);
}

float msg_read_angle (Sizebuf *msg_read)
{
   return msg_read_char(msg_read) * (360.0/256);
}

float msg_read_angle16 (Sizebuf *msg_read)
{
   return SHORT2ANGLE(msg_read_short(msg_read));
}
/*
void msg_read_dir (Sizebuf *sb, Vec2 dir)
{
   int   b;

   b = msg_read_byte (sb);
   if (b >= NUMVERTEXNORMALS)
      sys_error ("msg_read_dir: out of range");
   vector_copy (bytedirs[b], dir);
}
*/
/*
void msg_read_delta_usercmd (Sizebuf *msg_read, usercmd_t *from, usercmd_t *move)
{
   int bits;

   memcpy (move, from, sizeof(*move));

   bits = msg_read_byte (msg_read);

   // read current angles
   if (bits & CM_ANGLE1)
      move->angles[0] = msg_read_short (msg_read);
   if (bits & CM_ANGLE2)
      move->angles[1] = msg_read_short (msg_read);
   if (bits & CM_ANGLE3)
      move->angles[2] = msg_read_short (msg_read);

   // read movement
   if (bits & CM_FORWARD)
      move->forwardmove = msg_read_short (msg_read);
   if (bits & CM_SIDE)
      move->sidemove = msg_read_short (msg_read);
   if (bits & CM_UP)
      move->upmove = msg_read_short (msg_read);

   // read buttons
   if (bits & CM_BUTTONS)
      move->buttons = msg_read_byte (msg_read);

   if (bits & CM_IMPULSE)
      move->impulse = msg_read_byte (msg_read);

   // read time to run command
   move->msec = msg_read_byte (msg_read);

   // read the light level
   move->lightlevel = msg_read_byte (msg_read);
}
*/

void msg_read_data (Sizebuf *msg_read, void *data, int len)
{
   int   i;

   for (i = 0; i < len; i++)
      ((byte *)data)[i] = msg_read_byte (msg_read);
}



/*
================
COM_Init
================
*/
void com_init ()
{
   if ( setjmp (abortframe) )
      sys_error ("Error during initialization");

   // very important part of program
   swap_init ();
   cvar_init ();
   cbuf_init ();
   cmd_init  ();
   gui_init  ();
   scr_init  ();
   key_init  ();

}
