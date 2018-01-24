  ______   ___    ___
 /\  _  \ /\_ \  /\_ \
 \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
  \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
   \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
    \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
     \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
                                    /\____/
                                    \/___/
         ______     ___               ______      __        __
        /\__  _\__ /\_ \             /\  ___\    /\ \  __  /\ \__
        \/_/\ \/\_\\//\ \      __    \ \ \__/    \_\ \/\_\/\__  _\  ___   _ __
           \ \ \/_/_ \ \ \   /'__'\   \ \  __\   / __ \/_/\/_/\ \/ / __'\/\''__\
            \ \ \/\ \ \_\ \_/\  __/    \ \ \_/__/\ \L\ \/\ \ \ \ \/\ \L\ \ \ \/
             \ \_\ \_\/\____\ \____\    \ \_____\ \_____\ \_\ \ \_\ \____/\ \ \
              \/_/\/_/\/____/\/____/     \/_____/\/_____/\/_/  \/_/\/___/  \/_/

                                   by MAZsoft

********************************************************************************

  Contents
    þ Changes
    þ What is ATED ?
    þ Features
    þ Using
      þ Preparing Tiles
      þ Editing TileMap
      þ Controls
    þ Feedback
    þ Credits

  Changes
    0.1 beta		Initial release
    0.2.0		Added "View map & goto", "Review map & goto",
			Added TileInfo Management, fixed some bugs,
			Added some little neat features...

  What is ATED ?
    ATED is 2D tile map editor. For starters: tile map is a
    rectangular array of data. Data usually is a tile number.

  Features
    þ Very easy map editing
    þ Automatically saves editor state on exit, and load it on startup,
      very important feature :-)
    þ ATED tile map can contain up to 3 layers: back, fore and info
    þ Viewing of each and every layer can be turned on or off
    þ Separate viewing and editing of layers (for example: you see back, fore
      and info layers, but can view and edit any of them in all ever possible
      combinations)
    þ Put and grab tiles with mouse
    þ Undo action
    þ Copy and Paste
    þ Also Snap-Paste Mode
    þ FloodFilling, also with your just copied data
    þ BlockFilling
    þ TileSearch Mode
    þ Grid Mode
    þ Rewritten Allegro GUI
    þ Supports many resolutions
    þ Reads and writes LZSS compressed files
    þ Changeable background image, named skin :-)
 

  Using
    Preparing Tiles
      At the first time, I would like to say, that ATED accepts only SQUARE
      tiles! That means, you can freely use 8x8, 16x16, 32x32, 24x24, 3x3 and
      so on. Any square tiles shold work, but I recommend you to use only that
      sizes, that are powers of 2 (example: 2^3 = 8; 2^4 = 16; 2^5 = 32;
      2^6 = 64 ...).

      Second, tiles for different layers are unique, i.e. different. Tiles
      stored in TileGFX files, which are usual datafiles, but contain special
      structure. First 3 items in these files are DATAFILES, that contains tile
      data. First is background graphics, second is foreground, third and last
      is info. All tiles should be of one size and depth (example: 16x16 8bpp).
      Other members of the TileGFX file are optional. TileGFX file can also
      contain palette, TileInfo and TileInfoM.

      Third, easiest way for creating TileGFX files is to use GRABBER utility,
      which is part of ALLEGRO. Step-by-step example:
        1) Enter GRABBER
        2) Open "gfx.dat" file, you can found it into "ATED\TILES" folder
        3) Use "Read bitmap" menu command from GRABBER, and load 
           "back_tiles.pcx", which also can be found into "ATED\TILES"
        4) Make sure, you selected "BACKGROUND TILES" at the left!
        5) Use "Grab from grid". Set X-size and Y-size to 16x16, type to
           "RLE SPRITE" and "Cols" to "256". Click "OK"
        6) Select "FOREGROUND TILES" and repeat steps 4-5 with bitmap
           "fore_tiles.pcx"
        7) Select "INFO TILES" and repeat steps 4-5 with bitmap "info_tiles.pcx"

    Editing TileMap
      1) Create new tile map: from "File" menu, select "Manage Maps", click
         "Create", give it name and dimensions.
      2) It's time to allocate some memory onto layers:
         "File" -> "Manage Palnes". Select all 3 checkboxes, click "Done",
         confirm your action.
      3) You need to load tileGFX file now: "File" -> "Load TileGFX file", and
         select some file.
      4) Now you can select tiles by pressing SPACEBAR: Press it select some
         tiles for back, fore and info. Press SPACE
      5) Firstly, press 4, 5, 6 to turn on viewing layers. Now press 1, 2, 3 to
         turn on edit mode on them.
      6) Move mouse around and press buttons, you would see, what happens :-)

    Controls
      MOUSE
        LEFT			Put tile
	RIGHT			Grab tiles

      KEYBOARD
        CTRL-X			Quit
        F9			Select video mode
        C			Copy Mode
        P	 		Paste Mode
        B			Blockfill mode
        F			Floodfill mode
				NOTE: If you press RIGHT mouse button, or
                                CTRL+LEFT mouse button, it will cause filling
                                with that, you have in copy buffer
	U			Undo
        T			Tilesearch
	G			Toggle Grid mode
	S			Toggle Snap-Paste mode (only work in Paste mode)
	F3			Toggle Paste overlay
	I			Map statistics
   	F12			Take screenshot (ated_XX.pcx)
	ENTER			Normally, change value for info plane, but in
                                Copy mode it selects, in Blockfill fills :-)

  Feedback
    If you find some bugs in a program, have some interesting ideas,
    then please send me email. You can find it in the end.

+------------------------------------------------------------------------------+
| ?     File     Edit   Misc
+------------------------------------------------------------------------------+
| 
| 
|
|
|
|
|
|
|
|
|
|
|
|
|
|
+------------------------------------------------------------------------------+
|00   No    /\ $0024 00  No       Map Name               BFIbfi  X = ???  ?????|
|00  Tile   \/ 00158 00 Icon                             123456  Y = ???  ?????|
+------------------------------------------------------------------------------+
                                                         
                                                         
  Credits    
    Programming skills and some art - Michail Pichagin aka MAZer
				      If my name is too difficult
				      for you, call me Mike :-)
				      e-mail: mailfrom@mail.ru

    Original idea - John Romero (when he was in ID, he created TED5, father of
                    ATED), now he is working in ION Storm

    DJGPP Compiler - DJ Delorie
                     WWW: www.delorie.com

    Allegro graphics library - Shawn Hargreaves and contributors
	                       WWW: talula.demon.co.uk

--------------------------------------------------------------------------------
  Feedback e-mail adress:
    mailfrom@mail.ru
    Subject: [ATED] (Place Your subj here)