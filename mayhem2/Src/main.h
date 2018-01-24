#ifndef __MAIN_H__
#define __MAIN_H__

#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#include <allegro.h>

#include "console.h"
#include "c_entity.h"
#include "c_list.h"
#include "c_expl.h"
#include "gui.h"
#include "init.h"

extern DATAFILE *explnsdata;
extern DATAFILE *debrisdata;
extern DATAFILE *smokedata;
extern DATAFILE *flaredata;
extern DATAFILE *tmpdata;
extern BITMAP   *aux_page;
extern COLOR_MAP col_map;

extern LIST ship_list;
extern LIST efx_list;
extern LIST bullet_list;

extern int  brightness;

extern bool mouse;
extern bool joystick;

extern cvar debug;
//extern bool debug;
//extern bool dev_mode;

#endif
