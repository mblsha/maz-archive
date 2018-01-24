/*  ______   ___    ___
 * /\  _  \ /\_ \  /\_ \
 * \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *  \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *   \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *    \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *     \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                    /\____/
 *                                    \_/__/
 *           ______     ___               ______      __        __
 *          /\__  _\__ /\_ \             /\  ___\    /\ \  __  /\ \__
 *          \/_/\ \/\_\\//\ \      __    \ \ \__/    \_\ \/\_\/\__  _\  ___   _ __
 *             \ \ \/_/_ \ \ \   /'__'\   \ \  __\   / __ \/_/\/_/\ \/ / __'\/\''__\
 *              \ \ \/\ \ \_\ \_/\  __/    \ \ \_/__/\ \L\ \/\ \ \ \ \/\ \L\ \ \ \/
 *               \ \_\ \_\/\____\ \____\    \ \_____\ \_____\ \_\ \ \_\ \____/\ \ \
 *                \/_/\/_/\/____/\/____/     \/_____/\/_____/\/_/  \/_/\/___/  \/_/
 *
 *                                     by MAZsoft
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * DESCRIPTION:
 *
 *   Language defined file (TODO: i need to sligtly modify it)
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 */

#ifndef __LANG_H__
#define __LANG_H__

#ifdef LANGUAGE_ENGLISH

#define ERR_READ_FILE   "Can't read datafile "
#define ERR_LOAD_MAP    "Error when loading map..."
#define ERR_UNLOAD      "Unloading error!"
#define ERR_MOUS_NF     "Mouse not found!"
#define ERR_SEL_GMODE   "Select graphics mode"
#define ERR_FATAL       "FATAL ERROR"
#define ERR_ERR         "ERROR"

#define INIT_ALLEG      "Allegro initialization..."
#define INIT_LOAD       "Loading application variables..."
#define INIT_DATAFILE   "Loading datafile into memory..."
#define INIT_SOUND      "Install sound card..."
#define INIT_MOUSE      "Install mouse driver..."
#define INIT_TIMER      "Install timer..."
#define INIT_KEYB_H     "Install keyboard handler..."
#define INIT_GFX        "Install graphics display color mode..."
#define INIT_DONE       "OK"  //"Done!"

#define MAIN_MENU_NUM_ROWS      4

#define L_MAIN_MENU_1   "1 Player"
#define L_MAIN_MENU_2   "2 Players"
#define L_MAIN_MENU_3   "Options"
//#define L_MAIN_MENU_4   ""//#define L_MAIN_MENU_4   "Help"
#define L_MAIN_MENU_4   "Quit"

#define L_OPT_MENU_1    "Back to Main"
#define L_OPT_MENU_2    "Customize Controls"
#define L_OPT_MENU_3    "Video Options"
#define L_OPT_MENU_4    "Reset to Defaults"
#define L_OPT_MENU_5    "Gamma Correction"
#define L_OPT_MENU_6    "Music Volume"
#define L_OPT_MENU_7    "Sound Volume"

#define L_OK            "OK"
#define L_CANCEL        "Cancel"

#else

#endif

#endif