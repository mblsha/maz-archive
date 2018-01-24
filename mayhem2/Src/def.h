#ifndef __DEF_H__
#define __DEF_H__

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include <allegro.h>

#include "ustring.h"
#include "cvar.h"
#include "console.h"
#include "common.h"
#include "cmd.h"
#include "keys.h"
#include "timer.h"
#include "atedp.h"
#include "game.h"
#include "screen.h"
#include "net.h"
#include "client.h"
#include "store_cpp.h"

#define  VERSION        0.13
#define  VERSION_STR    "0.1.3"

//
// If you want to compile MAYHEM2 by yourself, and don't have required
// music librares, just comment following lines:
//
#ifdef ALLEGRO_DOS
   #define USE_MIDI
   #define USE_ALMP3
   #define USE_JGMOD
#else
   #define USE_MIDI
   //#define USE_ALMP3
   #define USE_FMOD
#endif

//
// Simple ball demo
//

#define BALL_DEMO

//
// Debugging stuff
//
#define DEBUG
#define PARANOID

#ifdef DEBUG
        #define D(x)            (x)
        #define debug(x)        allegro_message (x)
#else
        #define D(x)            /* debug info */
        #define debug(x)        /* debug info */
#endif


extern VIEWPORT *view[16];

char    *va(char *format, ...);

#define  MAXPRINTMSG       4096

#endif
