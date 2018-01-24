#include "def.h"
//#include "allegro/aintern.h"
#include "gui.h"
#include "screen.h"
#include "string.h"

boolean gui_initialized = false;

BEGIN_CUSTOM_CVAR (gui_trans, "1", CVAR_ARCHIVE)
{
   if (gui_initialized)
      send_dialog_message (MSG_START, 0);
}
END_CUSTOM_CVAR (gui_trans)

//BITMAP *gui_check;
//BITMAP *gui_cross;

BITMAP *gui_bmp;

//
// Gui colors
//
int     green;
int     green_dark;
int     green_very_dark;
int     red;
int     red_dark;
int     red_very_dark;

int     white;
int     black;

int     gray;
int     gray_bright;
int     gray_dark;
int     gray_very_dark;

int     mask_color;
/*
DIALOG test[] =
                {       /// (proc)        (x)  (y)  (w)  (h)  (fg)   (bg)   (key)  (flags) (d1) (d2) (dp)                                       (dp2)                   (dp3) ////////////
                        { NULL,           0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   (void *)"test"            },
                        { NULL,           0,   0,   0,   0,   0,     0,     0,     0,      0,   0,   NULL,                                      NULL,                   NULL              }
};*/

// all game DIALOGs are in "dialogs.h"
#define __GUI_C__
        #include "dialogs.h"
#undef  __GUI_C__

void gui_intro_f()
{
  int i;
  BITMAP *bmp;

  console.printf ("Intro\n");

  //d   = (DATAFILE *)may_data[BMP].dat;
  //convert_datafile_bitmap(d + BMP_MAZ);
  bmp = (BITMAP *)store.dat ("/mayhem/bmp/maz");//d[BMP_MAZ].dat;

  clear_to_color(screen, white);
  draw_sprite(screen, bmp, (SCREEN_W-bmp->w)/2,(SCREEN_H-bmp->h)/2);

  //textout(screen, font, "presents", 0, 0, makecol(0,0,0));
  gui_textout_highlight(screen, "presents", SCREEN_W/2, SCREEN_H/2+75, white, gray_bright, TRUE, TRUE);

  while(keypressed())
    readkey();

  for(i = 0; i < 300; i++)
  {
    if(keypressed())
      break;

    rest(10);
  }

  clear(screen);

  gui_textout_highlight(screen, "MAYHEM 2", SCREEN_W/2, SCREEN_H/2, white, gray_bright, TRUE, TRUE);

  for(i = 0; i < 300; i++)
  {
    if(keypressed())
      break;

    rest(10);
  }
}

void gui_trans_f()
{
}

static keydest_t old_key_dest;
static char cur_dlg[64];
BEGIN_COMMAND (menu, "Displays menus")
{
   int    i = 0;
   char   tmp[64];
   DIALOG *d;

   if (argc < 2)
   {
      console.printf (con_description, "menu <menu_name>: displays a menu_name menu\n");
      return;
   }

   // search for menu
   while(dialogs[i].name)
   {
      if( !argv[1].case_compare ( dialogs[i].name ) )
      {
         gui_initialized = true;

         strcpy(cur_dlg, dialogs[i].name);

         d = dialogs[i].d;
         gui_open_dialog(dialogs[i].d);

         if(key_dest != key_menu)
            old_key_dest = key_dest;

         if(d)
            key_dest = key_menu;
         else
            key_dest = key_game;

         return;
      }
      i++;
   }

   if( !argv[1].case_compare ( "reinit" ) )
   {
      if(!strlen(cur_dlg))
         strcpy(cur_dlg, "main_menu");

      //console_dprintf("ReIniting %s menu\n", cur_dlg);

      //tmp = (char *)malloc(64);
      sprintf(tmp, "menu %s\n", cur_dlg);
      cbuf_add_text(tmp);
      //free(tmp);

      return;
   }

   if( !argv[1].case_compare ( "close") )
   {
      if(key_dest == key_menu)
         key_dest = old_key_dest;

      return;
   }

   if( !argv[1].case_compare ( "console" ) )
   {
      if(key_dest == key_menu)
         key_dest = old_key_dest;
      //key_dest = key_console;

      if ( !console.is_active () )
         console.toggle ();

      return;
   }

   if(developer.value)
   {
      console.dprintf ("Valid menu_names are:\n");

      i = 0;
      while(dialogs[i].name)
      {
         console.dprintf ("%s\n", dialogs[i].name);
         i++;
      }
   }
   else
      console.printf(con_warning, "\"%s\" menu doesn't exist!\n", argv[1].get_s ());
}
END_COMMAND (menu)

void gui_init()
{
/*  cvar_register_variable(&gui_trans);

  cmd_add_command       ("gui_trans", gui_trans_f);
  cmd_add_command       ("menu",      gui_menu_f);
  cmd_add_command       ("intro",     gui_intro_f);
  */
}

void gui_init_colors()
{
   green           = makecol(  0,255,  0);
   green_dark      = makecol(  0,200,  0);
   green_very_dark = makecol(  0,150,  0);

   red             = makecol(255,  0,  0);
   red_dark        = makecol(200,  0,  0);
   red_very_dark   = makecol(150,  0,  0);

   white           = makecol(255,255,255);
   black           = makecol(  0,  0,  0);

   gray            = makecol(128,128,128);
   gray_bright     = makecol(200,200,200);
   gray_dark       = makecol( 50, 50, 50);
   gray_very_dark  = makecol( 15, 15, 15);

   mask_color      = screen->vtable->mask_color;
}
/*
int main()
{
  allegro_init();
  install_keyboard();
  install_mouse();
  install_timer();

  //set_color_depth(16);

  if(set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0))
    exit(1);

  install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);


  backbmp = guidata[BMP].dat;

  gui_init_colors();

  menu();

  return 0;
}

void menu(void)
{
  //
  //set_trans_blender(128,128,128,128);
  //
  text_mode(-1);

  intro();
  while(ret)
  {
    #define BUTT_PLAY                     3
    #define BUTT_OPT                      4
    #define BUTT_EXIT                     5

    #define MENU_MAIN                     1
    #define MENU_OPT                      2
    #define MENU_GAME                     3
    switch(menu)
    {
      case MENU_MAIN:
        switch(ret)
        {
          case BUTT_PLAY:                     // Play Game
            menu = MENU_GAME;
            break;

          case BUTT_OPT:                      // Options
            menu = MENU_OPT;
            break;

          case BUTT_EXIT:                     // Exit
            menu = NULL;
            ret = NULL;
            break;

          default:                            // Main Menu
            menu = MENU_MAIN;
            break;
        }
        break;

      case MENU_GAME:
        menu = MENU_MAIN;
        break;

      case MENU_OPT:
        menu = MENU_MAIN;
        break;
    }

    switch(menu)
    {
      case MENU_GAME:
        d = start_game;
        break;

      case MENU_OPT:
        d = options;
        break;

      case NULL:
        d = NULL;
        break;

      default:
        menu = MENU_MAIN;
        d = main_menu;
        break;
    }

    if(d)
      ret = do_dialog(d, -1);
  }
}
*/
void intro(void)
{/*
  int i = 0;
  DATAFILE *d;
  BITMAP *bmp;

  d = may_data[BMP].dat;
  bmp = d[BMP_MAZ].dat;

  clear_to_color(screen, 15);
  draw_sprite(screen, bmp, (SCREEN_W-bmp->w)/2,(SCREEN_H-bmp->h)/2);

  while((i++ < 1000) && !keypressed())
  {
    //delay(1);
  }*/
}

////////////////////////////////////////////////////////////////////////////////////////
//
//                      Bitmap color depth convertion routines
//
////////////////////////////////////////////////////////////////////////////////////////

BITMAP *convert_bitmap(BITMAP *bitmap)
{
   int     source_depth;
   int     target_depth;
   BITMAP  *source_bmp, *target_bmp;
   PALETTE pal;

   get_palette(pal);

   source_depth = bitmap->vtable->color_depth;
   target_depth = screen->vtable->color_depth;

   if(source_depth == target_depth)
   {
      target_bmp = create_bitmap (bitmap->w,  bitmap->h);
      draw_sprite                (target_bmp, bitmap, 0, 0);
      return target_bmp;
   }

   source_bmp = create_bitmap_ex (source_depth, bitmap->w, bitmap->h);
   clear_to_color                (source_bmp, bitmap->vtable->mask_color);
   draw_sprite                   (source_bmp, bitmap, 0, 0);

   target_bmp = create_bitmap_ex (target_depth, bitmap->w, bitmap->h);

   pal[0].r = 63;
   pal[0].g = 0;
   pal[0].b = 63;

   select_palette(pal);
   blit(source_bmp, target_bmp, 0, 0, 0, 0, bitmap->w, bitmap->h);
   unselect_palette();

   destroy_bitmap (source_bmp);

   return target_bmp;//bitmap;
}

void convert_datafile_bitmap(DATAFILE *dat)
{
   int     source_depth;
   int     target_depth;
   BITMAP  *source_bmp, *target_bmp;
   BITMAP  *bitmap;
   PALETTE pal;

   bitmap = (BITMAP *)dat->dat;

   get_palette(pal);

   source_depth = bitmap->vtable->color_depth;
   target_depth = screen->vtable->color_depth;

   if(source_depth == target_depth)
   {
      return;
   }

   source_bmp = create_bitmap_ex (source_depth, bitmap->w, bitmap->h);
   clear_to_color                (source_bmp, bitmap->vtable->mask_color);
   draw_sprite                   (source_bmp, bitmap, 0, 0);

   target_bmp = create_bitmap_ex (target_depth, bitmap->w, bitmap->h);

   pal[0].r = 63;
   pal[0].g = 0;
   pal[0].b = 63;

   select_palette(pal);
   blit(source_bmp, target_bmp, 0, 0, 0, 0, bitmap->w, bitmap->h);
   unselect_palette();

   source_bmp = bitmap;

   dat->dat = target_bmp;

   destroy_bitmap (source_bmp);
}

void convert_bitmaps_in_datafile(DATAFILE *dat)
{
   int i;
   DATAFILE *d;
   DATAFILE *nested;

   for(i = 0; dat[i].type != DAT_END; i++)
   {
      d = dat + i;

      if(dat[i].type == DAT_BITMAP)
      {
         convert_datafile_bitmap(d);
      }
      else if(dat[i].type == DAT_FILE)
      {
         nested = (DATAFILE *)dat[i].dat;
         convert_bitmaps_in_datafile(nested);
      }
   }
}

RLE_SPRITE *convert_rle_sprite(RLE_SPRITE *sprite)
{
   int     source_depth;
   int     target_depth;
   BITMAP  *source_bmp, *target_bmp;
   PALETTE pal;
   RLE_SPRITE *temp;

   get_palette(pal);

   source_depth = sprite->color_depth;
   target_depth = screen->vtable->color_depth;

   if(source_depth == target_depth)
   {
      target_bmp = create_bitmap (sprite->w,  sprite->h);
      draw_rle_sprite            (target_bmp, sprite, 0, 0);
      return sprite;
   }

   source_bmp = create_bitmap_ex (source_depth, sprite->w, sprite->h);
   clear_to_color                (source_bmp, source_bmp->vtable->mask_color);
   draw_rle_sprite               (source_bmp, sprite, 0, 0);

   target_bmp = create_bitmap_ex (target_depth, sprite->w, sprite->h);

   pal[0].r = 63;
   pal[0].g = 0;
   pal[0].b = 63;

   select_palette(pal);
   blit(source_bmp, target_bmp, 0, 0, 0, 0, sprite->w, sprite->h);
   unselect_palette();

   temp = get_rle_sprite(target_bmp);

   destroy_bitmap (source_bmp);
   destroy_bitmap (target_bmp);

   return temp;
}

void convert_datafile_rle_sprite(DATAFILE *dat)
{
   int     source_depth;
   int     target_depth;
   BITMAP  *source_bmp, *target_bmp;
   PALETTE pal;
   RLE_SPRITE *sprite;// *temp;

   get_palette(pal);

   sprite = (RLE_SPRITE *)dat->dat;

   source_depth = sprite->color_depth;
   target_depth = screen->vtable->color_depth;

   if(source_depth == target_depth)
   {
      return;
   }

   source_bmp = create_bitmap_ex (source_depth, sprite->w, sprite->h);
   clear_to_color                (source_bmp, source_bmp->vtable->mask_color);
   draw_rle_sprite               (source_bmp, sprite, 0, 0);

   target_bmp = create_bitmap_ex (target_depth, sprite->w, sprite->h);

   pal[0].r = 63;
   pal[0].g = 0;
   pal[0].b = 63;

   select_palette(pal);
   blit(source_bmp, target_bmp, 0, 0, 0, 0, sprite->w, sprite->h);
   unselect_palette();

   dat->dat = get_rle_sprite(target_bmp);

   destroy_rle_sprite (sprite);
   destroy_bitmap     (source_bmp);
   destroy_bitmap     (target_bmp);
}

void convert_rle_sprites_in_datafile(DATAFILE *dat)
{
   int i;
   DATAFILE *d;
   DATAFILE *nested;

   for(i = 0; dat[i].type != DAT_END; i++)
   {
      d = dat + i;

      if(dat[i].type == DAT_RLE_SPRITE)
      {
         convert_datafile_rle_sprite(d);
      }
      else if(dat[i].type == DAT_FILE)
      {
         nested = (DATAFILE *)dat[i].dat;
         convert_rle_sprites_in_datafile(nested);
      }
   }
}

void convert_images_in_datafile(DATAFILE *dat)
{
   convert_bitmaps_in_datafile     (dat);
   convert_rle_sprites_in_datafile (dat);
}

