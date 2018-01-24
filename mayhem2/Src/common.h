#ifndef __COMMON_H__
#define __COMMON_H__

#include "allegro.h"
#include "stdio.h"

#ifndef BYTE_DEFINED
    typedef unsigned char     byte;
    #define BYTE_DEFINED
#endif

#ifdef __cplusplus
   typedef enum 
   {
      False = FALSE, 
      True  = TRUE
   } boolean;

   #define true  True
   #define false False
#else
   typedef enum 
   {
      false = FALSE,
      true  = TRUE
   } boolean;
#endif

extern jmp_buf abortframe;     // an ERR_DROP occured, exit the entire frame

typedef float Vec;
typedef Vec Vec2[2];


#define ANGLE2SHORT(x)  ((int)((x)*65536/360) & 65535)
#define SHORT2ANGLE(x)  ((x)*(360.0/65536))


#ifdef _MSC_VER
       #define STACK_ARGS __cdecl
#else
       #define STACK_ARGS
#endif


typedef struct
{
   boolean allowoverflow;  // if false, do a sys_error
   boolean overflowed;     // set to true if the buffer size failed
   
   byte    *data;
   
   int     maxsize;
   int     cursize;
   int     readcount;
} Sizebuf;


//============================================================================

void msg_write_char           (Sizebuf *sb, int c);
void msg_write_byte           (Sizebuf *sb, int c);
void msg_write_short          (Sizebuf *sb, int c);
void msg_write_long           (Sizebuf *sb, int c);
void msg_write_float          (Sizebuf *sb, float f);
void msg_write_string         (Sizebuf *sb, char *s);
void msg_write_coord          (Sizebuf *sb, float f);
void msg_write_pos            (Sizebuf *sb, Vec2 pos);
void msg_write_angle          (Sizebuf *sb, float f);
void msg_write_angle16        (Sizebuf *sb, float f);
//void msg_write_delta_usercmd  (Sizebuf *sb, struct usercmd_s *from, struct usercmd_s *cmd);
//void msg_write_delta_entity   (struct entity_state_s *from, struct entity_state_s *to, Sizebuf *msg, qboolean force, qboolean newentity);
void msg_write_dir            (Sizebuf *sb, Vec2 vector);


void  msg_begin_reading       (Sizebuf *sb);

int   msg_read_char           (Sizebuf *sb);
int   msg_read_byte           (Sizebuf *sb);
int   msg_read_short          (Sizebuf *sb);
int   msg_read_long           (Sizebuf *sb);
float msg_read_float          (Sizebuf *sb);
char  *msg_read_string        (Sizebuf *sb);
char  *msg_read_string_line   (Sizebuf *sb);

float msg_read_coord          (Sizebuf *sb);
void  msg_read_pos            (Sizebuf *sb, Vec2 pos);
float msg_read_angle          (Sizebuf *sb);
float msg_read_angle16        (Sizebuf *sb);
//void  msg_read_delta_usercmd  (Sizebuf *sb, struct usercmd_s *from, struct usercmd_s *cmd);

void  msg_read_dir            (Sizebuf *sb, Vec2 vector);

void  msg_read_data           (Sizebuf *sb, void *buffer, int size);

//============================================================================

extern   boolean  bigendien;

extern   short    big_short      (short l);
extern   short    little_short   (short l);
extern   int      big_long       (int l);
extern   int      little_long    (int l);
extern   float    big_float      (float l);
extern   float    little_float   (float l);

//============================================================================

extern   char     com_token[1024];
extern   boolean  com_eof;

extern   int      com_argc;
extern   char     **com_argv;

char *com_parse                 (char *data);

int  com_check_parm             (char *parm);

char *com_load_file             (char *path);

void com_init                   (void);

void sz_init                    (Sizebuf *buf, byte *data, int length);
void sz_alloc                   (Sizebuf *buf, int startsize);
void sz_free                    (Sizebuf *buf);
void sz_clear                   (Sizebuf *buf);
void *sz_get_space              (Sizebuf *buf, int length);
void sz_write                   (Sizebuf *buf, void *data, int length);
void sz_print                   (Sizebuf *buf, char *data);

typedef enum
{
   err_drop,
   err_disconnect,
   err_fatal
} err_code_t;

void com_error                  (err_code_t code, char *fmt, ...);
void sys_error                  (char *fmt, ...);


char *copy_string               (const char *s);
void replace_string             (char **ptr, char *str);

#endif
