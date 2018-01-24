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
 *   NONE used in ated (I will remove that file later)
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 */

#include "ated.h"

int box_x = 0;
int box_y = 0;
int box_active = FALSE;

void LoadMapHeader(void) {

  strcpy(mapheadname, "MAPHEAD.");
  strcat(mapheadname, ext);
  strcpy(mapname,     "MAPTEMP.");
  strcat(mapname,     ext);
  strcpy(SM_name,     "MAPTEMP1.");
  strcat(SM_name,     ext);
  strcpy(SM_loadname, "MAPTEMP.");
  strcat(SM_loadname, ext);

  if (access(mapheadname, 0)) {
    //
    // GOING TO CREATE A NEW MAP FILE!
    //
    //MapFileHeader = malloc(MapFileHeaderStr); //1316:1 ted5.c
  }
  else {
    //
    // LOAD THE MAP!
    //
  }
}
#ifdef LOAD_MAP_HEADER
void LoadMapHeader(void) {
  unsigned size, i, j, pflag;
  char     types = 0;

  strcpy(mapheadname, "MAPHEAD.");
  strcat(mapheadname, ext);
  strcpy(mapname,     "MAPTEMP.");
  strcat(mapname,     ext);
  strcpy(SM_name,     "MAPTEMP1.");
  strcat(SM_name,     ext);
  strcpy(SM_loadname, "MAPTEMP.");
  strcat(SM_loadname, ext);

  if (access(mapheadname, 0)) {
    int i;
    //
    // Going to create map file!
    //
    //...
    for (i = 0; i < sizeof(MapFileHeaderStr); i++)
      *((char *)MapFileHeader + i) = 0;

    MapFileHeader->RLEWtag=0xabcd;
    for (i = 0; i < 100; i++) {
      MapFileHeader->dataoffsets[i] = -1;
      MapFileHeader->datalengths[i] = 0;
      memset(MapNames[i], 0, 16);
    }

    if (!GFXInfo->num8   &&
        !GFXInfo->num8m  &&
        !GFXInfo->num16  &&
        !GFXInfo->num16m &&
        !GFXInfo->num32  &&
        !GFXInfo->num32m) {
      alert("Uhh...you 'neglected' to grab tiles to use.","Running "APP_NAME" is quite useless at this point.","I'm afraid","Duh!",NULL,NULL,NULL);
      fat_error("Get some tiles ... quick! Me hungry!");
    }

    if (!GFXInfo->num8  &&
        !GFXInfo->num16 &&
        !GFXInfo->num32) {
      alert("You may have grabbed some MASKED","tiles, but I require NON-MASKED","tiles as a minimum requirement!","Geez...",NULL,NULL,NULL);
      fat_error("Please grab some normal tiles!");
    }

    types += (GFXInfo->num8 > 0) + (GFXInfo->num16 > 0) + (GFXInfo->num32 > 0);

    redo:

    if (types > 1) {
      int which;

      //do_ated_dialog(select_tile_size,-1);
      switch(which) {
        case 0:
          fat_error("");
        case 1:
          if (!GFXInfo->num8)
            which = 0;
          break;
        case 2:
          if (!GFXInfo->num16)
            which = 0;
          break;
        case 3:
          if (!GFXInfo->num32)
            which = 0;
          break;
      }

      MapFileHeader->tsize = TEDInfo->tsize = tsize = which;
    }else{
      if (GFXInfo->num8)
        TEDInfo->tsize=1;
      else
      if (GFXInfo->num16)
        TEDInfo->tsize=2;
      else
      if (GFXInfo->num32)
        TEDInfo->tsize=3;

      MapFileHeader->tsize = tsize = TEDInfo->tsize;
    }

    //
    // pick the planes that all maps will use
    //
    if (PickMorePlanes())
      goto redo;

    //
    // initialize TILEINFO/TILEINFOM
    //
    switch(tsize) {
      case 1:
        tilenum  = GFXInfo->num8;
        tilemnum = GFXInfo->num8m;
        break;
      case 2:
        tilenum  = GFXInfo->num16;
        tilemnum = GFXInfo->num16m;
        break;
      case 3:
        tilenum  = GFXInfo->num32;
        tilemnum = GFXInfo->num32m;
        break;
    }
    InitTileInfo();
    if (numtplanes || numtmplanes)      // only input if a[[licable
      Item_EditTinfoNames();            // void where prohibited
    //
    // now create a map!
    //
    CreateMap(0);
    FigureScreenEdges();
    MapFileHeader->NumIconRows = maxiconrows = InputIconAmount();
  }
  //
  // MAP FILE ALREDY IN PLACE, LOAD STUFF IN...
  //
  else {
    int/*memptr*/ block, tempblock;

    //LoadIn(mapheadname, (memptr *)&MapFileHeader)
    LoadIn(mapheadname, (int *)&MapFileHeader);

    if (MapFileHeader->NumIconRows > 50)
      MapFileHeader->NumIconRows = 4;

      //
      // has the TEDINFO?.ext file been changed?
      // if so, reconstruct pertinent data...
      //
      if (!TEDInfo->tsize) {
        tsize = TEDInfo->tsize = MapFileHeader->tsize;
        switch(tsize) {
          case 1:
            tilenum  = GFXInfo->num8;
            tilemnum = GFXInfo->num8m;
            break;
          case 2:
            tilenum  = GFXInfo->num16;
            tilemnum = GFXInfo->num16m;
            break;
          case 3:
            tilenum  = GFXInfo->num32;
            tilemnum = GFXInfo->num32m;
            break;
        }
      }

      maxiconrows = MapFileHeader->NumIconRows;

      //
      // Read-in all the Map Names
      //
      for (i = 0; i < 100; i++) {
        if (MapFileHeader->dataoffsets[i] != -1) {
          MapHeaderStr TempHead;

          LoadFile(mapname, (char *)&TempHead, MapFileHeader->dataoffsets[i],
                  sizeof(MapHeaderStr));
          strcpy(MapNames[i], TempHead.name);
        }
      }

      FigureScreenEdges();

      if (!TEDInfo->level) {
        for (i = 0; i < 100; i++) {
          if (MapFileHeader->dataoffsets[i] != -1) {
            whichmap = TEDInfo->level = i;
            break;
          }
        }
      }else
        whichmap = TEDInfo->level;

      LoadMap(TEDInfo->level);
      //*/ // \/ -- Was Commented!
      /*//
      // IF WE WERE LAUNCHED AND CHARACTER POSITION WES CHANGED,
      // PUT IT BACK!
      //
      if (launched && (TEDInfo->lastx || TEDInfo->lasty)) {
        int i;

        for (i = 0; i < mapwidth*mapheigth; i++) {
          if (MapInfoPl[i] == TEDInfo->permicon) {
            MapInfoPl[i] = 0;
            MapInfoPl[TEDInfo->lasty*mapwidth+TEDInfo->lastx] = TEDInfo->permicon;
            TEDInfo->lastx = TEDInfo->lasty = 0;
            DirtyFlag = 1;
            break;
          }
        }
      }*/

      //
      // POSITION SCREEN
      //
      xbase = TEDInfo->oscrx;
      ybase = TEDInfo->oscry;
      if (xbase + screenw > mapwidth)
        xbase = mapwidth - screenw;
      if (ybase + screenh > mapheigth)
        ybase = mapheigth - screenh;

      //if (launched)
      //  _fmemcpy((void *)

      //
      // LOAD TILEINFO/M AND ADJUST IF IT CHANGED
      //
      numtplanes  = MapFileHeader->numtplanes;
      numtmplanes = MapFileHeader->numtmplanes;

      pflag = 0;
      for (i = 0; i < numtplanes; i++) {
        //
        // SPACE FOR OLD TILEINFO TO DECOMPRESS INTO
        //
        tempblock = malloc(MapFileHeader->oldtilenum);
        //MMAllocate(&tempblock,MapFileHeader->oldtilenum);
        //
        // SPACE FOR OLD TILEINFO TO LOAD INTO
        //
        block = malloc(MapFileHeader->tileinfolen[i]);
        //MMAllocate(&block,MapFileHeader->tileinfolen[i]);
        LoadFile(mapheadname,MK_FP(block,0),MapFileHeader->tileinfooff[i],MapFileHeader->tileinfolen[i]);
        //
        // DECOMPRESS FROM "BLOCK" TO "TEMPBLOCK"
        //
        RLEBExpand(MK_FP(block,0),MK_FP(tempblock,0),
                  MapFileHeader->oldtilenum,MapFileHeader->RLEWtag);
        free(block);
        //MMFreePtr(&block);
        //
        // ALLOCATE TINFO ARRAY
        //
        Tinfo[i] = malloc(tilenum);
        //MMAllocate((memptr *)&Tinfo[i],tilenum);
        //
        // MOVE FROM "TEMPBLOCK" TO "TINFO[I]" ARRAY
        //
 	   if (MapFileHeader->oldtilenum<tilenum) {
   	     movedata((unsigned)tempblock,0,(unsigned)Tinfo[i],0,MapFileHeader->oldtilenum);
             //
 	     // IF NEW TILEINFO IS MORE, FILL END WITH 0s
 	     //
 	     for (j=MapFileHeader->oldtilenum;j<tilenum;j++)
	       *(Tinfo[i]+j)=0;
	     DirtyFlag=pflag=1;
	   }
           else {
	     movedata((unsigned)tempblock,0,(unsigned)Tinfo[i],0,tilenum);
	     if (MapFileHeader->oldtilenum>tilenum)
	       DirtyFlag=pflag=2;
	   }

        MMFreePtr(&tempblock);
      }//1509:11 ted5.c
}
#endif
/*----------------------------------------------------------------------------------------*/
#ifdef IS_IM_NEED_IT
//TODO:
//!Write_end_of_this_file!;
/*----------------------------------------------------------------------------------------*/
//UnNeeded!
//void FindGraphFile(void) {
/*char pname[15] = "TILES.*", *tempstr, tiname[13] = "TEDINFO.TMP";
  int  i, which;

  if (ext[0]) {
    strcpy(pname, "TILES.");
    strcat(pname, ext);
  }
  if (findfirst(pname, &ffblk, FA_ARCH)) {
    alert("","I can't find a graphics", "file! (ex:TILES.ext)","Alright", NULL, NULL, NULL);
    fat_error("Can't work without graphics ya know!");
  }
  if (GfxToUse)
    format[0] = GfxToUse;
  else {
    //setup dialog
    strcpy(bstrings[0], ffblk.ff_name);
    //...
    for (i = 1; i < 10; i++) {
      if (findnext(&ffblk))
        break;
      strcpy(bstrings[i],ffblk.ff_name);
      //...
    }
    //...
    which = 1;*/
    /*if (i > 1)
      do {
        //which = dodialog;
      }while(!which);
    which--;
    */
    /*tempstr = strpbrk(bstrings[which], ".") + 1;
    strcpy(ext, tempstr);
    format[0] = bstrings[which][0];
  }*/
//}
/*----------------------------------------------------------------------------------------*/
/*void LoadInfoFile(void) {
  char pname[14] = "TEDINFO.", gfxname[14] = "GFXINFO";

  //Load TEDINFO.exe file!
  strcat(pname,    ext);
  strcpy(infoname, pname);

  if (access(pname, 0)) {
    //...
    TEDInfo->pflags = 0x27;
  }else
    LoadIn(pname, (memptr *)&TEDInfo);

  tsize = TEDInfo->tsize;
  //...//if (launchname[0])//...

  //
  // LOAD GFXINFO?.ext FILE
  //
  strcat(gfxname, format);
  strcat(gfxname, ".");
  strcat(gfxname, ext);
  LoadIn(gfxname, (memptr *)&GFXInfo);

  switch(tsize) {
    case 1:
      tilenum  = GFXInfo->num8;
      tilemnum = GFXInfo->num8m;
      break;
    case 2:
      tilenum  = GFXInfo->num16;
      tilemnum = GFXInfo->num16m;
      break;
    case 3:
      tilenum  = GFXInfo->num32;
      tilemnum = GFXInfo->num32m;
      break;
  }

  _fstrcpy(launchname, TEDInfo->launchname);

  //if (launched)
  //  TEDInfo->lastvid = LaunchInfo.lastmode;

  //
  // SET PLANES FLAGS TO NORMAL
  //
  planeton = (TEDInfo->pflags>>6)&1;
  planemon = (TEDInfo->pflags>>5)&1;
  planeion = (TEDInfo->pflags>>4)&1;
  viewton  = (TEDInfo->pflags>>2)&1;
  viewmon  = (TEDInfo->pflags>>1)&1;
  viewion  = (TEDInfo->pflags)&1;

  //
  // SET BACKGROUND COLOR
  //
  BkgndColor = TEDInfo->BackgndColor;
  if (BkgndColor > 26)
    TEDInfo->BackgndColor = BkgndColor = O_FGNDBACK;
}*/
/*----------------------------------------------------------------------------------------*/
#endif

/* starts outputting a progress message */
void box_start()
{
   show_mouse(NULL);

   rectfill(screen, BOX_L, BOX_T, BOX_R, BOX_B, gui_bg_color);
   rect(screen, BOX_L-1, BOX_T-1, BOX_R+1, BOX_B+1, gui_fg_color);
   hline(screen, BOX_L, BOX_B+2, BOX_R+1, gui_fg_color);
   vline(screen, BOX_R+2, BOX_T, BOX_B+2, gui_fg_color);

   show_mouse(screen);

   box_x = box_y = 0;
   box_active = TRUE;
}

/* outputs text to the progress message */
void box_out(char *msg)
{
   BITMAP *mouse_screen = _mouse_screen;

   if (box_active) {
      if (mouse_screen)
	 show_mouse(NULL);

      set_clip(screen, BOX_L+8, BOX_T+1, BOX_R-8, BOX_B-1);

      text_mode(gui_bg_color);
      textout(screen, font, msg, BOX_L+(box_x+1)*8, BOX_T+(box_y+1)*8, gui_fg_color);

      set_clip(screen, 0, 0, SCREEN_W-1, SCREEN_H-1);

      if (mouse_screen)
	 show_mouse(mouse_screen);

      box_x += strlen(msg);
   }
}

/* outputs text to the progress message */
void box_eol()
{
   BITMAP *mouse_screen = _mouse_screen;

   if (box_active) {
      box_x = 0;
      box_y++;

      if ((box_y+2)*8 >= BOX_H) {
	 if (mouse_screen)
	    show_mouse(NULL);

	 blit(screen, screen, BOX_L+8, BOX_T+16, BOX_L+8, BOX_T+8, BOX_W-16, BOX_H-24);
	 rectfill(screen, BOX_L+8, BOX_T+BOX_H-16, BOX_L+BOX_W-8, BOX_T+BOX_H-8, gui_bg_color);

	 if (mouse_screen)
	    show_mouse(mouse_screen);

	 box_y--;
      }
   }
}

/* ends output of a progress message */
void box_end(int pause)
{
   if (box_active) {
      if (pause) {
	 box_eol();
	 box_out("-- press a key --");

	 do {
	    poll_mouse();
	 } while (mouse_b);

	 do {
	    poll_mouse();
	 } while ((!keypressed()) && (!mouse_b));

	 do {
	    poll_mouse();
	 } while (mouse_b);

	 clear_keybuf();
      }

      box_active = FALSE;
   }
}

