#include "def.h"
#include "console.h"
#include "screen.h"
#include "gui.h"

#ifdef BALL_DEMO
       #include "balls.h"
#endif

static BITMAP           *buf   = NULL;
static BITMAP           *page1 = NULL;
static BITMAP           *page2 = NULL;
static BITMAP           *page3 = NULL;

static int              anim_type    = ANIM_DOUBLE_BUFFER;
static int              current_page = 0;

CVAR (wait_vsync, "0", CVAR_ARCHIVE);
CVAR (show_fps,   "0", CVAR_ARCHIVE);

void scr_init()
{/*
   cvar_register_variable(&wait_vsync);
   cvar_register_variable(&show_fps);
   */
   //cmd_add_command ("anim_type",         scr_set_anim_type_f);


   ///scr_init_buffers();
}

BEGIN_COMMAND (gfx_features, NULL)
{
   int c;
   static char *s[] =
   {
      "(scroll)",
      "(triple buffer)",
      "(hardware cursor)",
      "solid hline:",
      "xor hline:",
      "solid/masked pattern hline:",
      "copy pattern hline:",
      "solid fill:",
      "xor fill:",
      "solid/masked pattern fill:",
      "copy pattern fill:",
      "solid line:",
      "xor line:",
      "solid triangle:",
      "xor triangle:",
      "mono text:",
      "vram->vram blit:",
      "masked vram->vram blit:",
      "mem->screen blit:",
      "masked mem->screen blit:",
      "system->screen blit:",
      "masked system->screen blit:",
      NULL
   };


   console.printf ("Hardware accelerated features:\n\n");

   for (c = 3; s[c]; c++)
   {
      console.printf ("%-28s %s\n", s[c], (gfx_capabilities & (1<<c)) ? "yes" : "no");
   }
}
END_COMMAND (gfx_features)

BITMAP *make_bitmap (int w, int h, int priority, boolean is_masked)
{
   BITMAP *bmp = NULL;

   if ( priority <= 1 )
   {
      if      ( is_masked  && accel_vram2vram_masked_blits )
      {
         bmp = create_video_bitmap ( w, h );
      }
      else if ( !is_masked && accel_vram2vram_blits )
      {
         bmp = create_video_bitmap ( w, h );
      }

      if ( !bmp )
      {
         priority = 2;
      }
   }
   
   if ( priority == 2 )
   {
      if      ( is_masked  && accel_system2screen_masked_blits )
      {
         bmp = create_system_bitmap ( w, h );
      }
      else if ( !is_masked && accel_system2screen_blits )
      {
         bmp = create_system_bitmap ( w, h );
      }

      if ( !bmp )
      {
         priority = 3;
      }
   }
   
   if ( priority < 1 || priority >= 3)
   {
      bmp = create_bitmap ( w, h );
   }

   return bmp;
}

void scr_destroy_buffers()
{
   if(buf)
   {
      destroy_bitmap(buf);
      buf = NULL;
   }
   if(page1)
   {
      destroy_bitmap(page1);
      page1 = NULL;
   }
   if(page2)
   {
      destroy_bitmap(page2);
      page2 = NULL;
   }
   if(page3)
   {
      destroy_bitmap(page3);
      page3 = NULL;
   }
}

void scr_init_buffers()
{
   scr_destroy_buffers();

   switch(anim_type)
   {
      case ANIM_DOUBLE_BUFFER:
         buf = create_bitmap(SCREEN_W, SCREEN_H);
         break;

      case ANIM_PAGE_FLIP:
         page1 = create_video_bitmap(SCREEN_W, SCREEN_H);
         page2 = create_video_bitmap(SCREEN_W, SCREEN_H);
         break;

      case ANIM_TRIPLE_BUFFER:
         page1 = create_video_bitmap(SCREEN_W, SCREEN_H);
         page2 = create_video_bitmap(SCREEN_W, SCREEN_H);
         page3 = create_video_bitmap(SCREEN_W, SCREEN_H);
         break;
   }
}

BEGIN_CUSTOM_CVAR (animation_type, "0", CVAR_ARCHIVE)
{
   int t;

   t = (int)var.value;

   if(t < 0)
      t = 0;
   else if(t > 3)
      t = 3;

   if (t != (int)var.value)
   {
      var.set ( (float)t );

      return;
   }
   
   if ( t == anim_type )
      return;

   scr_set_anim_type(t);

   console.dprintf ("Animation type changed to ");

   if(!t)
      console.dprintf ("double buffer\n");
   else if(t == 1)
      console.dprintf ("page flip\n");
   else if(t == 2)
      console.dprintf ("triple buffer\n");
   else
      console.dprintf ("experimental blits\n");
}
END_CUSTOM_CVAR (anim_type)

void scr_set_anim_type(int new_anim_type)
{
   if(anim_type == new_anim_type)
      return;

   anim_type = new_anim_type;

   scr_init_buffers();
}

void scr_draw_game(BITMAP *bmp)
{
   int i;

   if(tile_map.back_plane && tile_map.fore_plane)
   {
      for (i = 0; i < 4; i++)
      {
         if (view[i]->w || view[i]->h)
         {
            draw_background (view[i], bmp);
            draw_foreground (view[i], bmp);
            draw_info_plane (view[i], bmp);
            //draw_planes (view[i], bmp);
            //viewport2bmp (view[i], bmp, TRUE, TRUE, FALSE);
         }
      }
   }
   #ifdef BALL_DEMO
          draw_balls(bmp);
   #else
   #endif
}

void scr_draw()
{
   BITMAP *bmp;

   switch(anim_type)
   {
      case ANIM_DOUBLE_BUFFER:
         bmp = buf;
         break;

      case ANIM_PAGE_FLIP:
         if(current_page == 0)
         {
            bmp = page2;
            current_page = 1;
         }
         else
         {
            bmp = page1;
            current_page = 0;
         }
         break;

      case ANIM_TRIPLE_BUFFER:
         if(current_page == 0)
         {
            bmp = page2;
            current_page = 1;
         }
         else if(current_page == 1)
         {
            bmp = page3;
            current_page = 2;
         }
         else
         {
            bmp = page1;
            current_page = 0;
         }
         break;

      default:
         bmp = screen;
         if(wait_vsync.value)
            vsync ();
   }
   clear (bmp);

   acquire_bitmap(bmp);

   //
   // draw the game
   //
   scr_draw_game(bmp);


   if(show_fps.value)
   {
      char buf[64];
      usprintf (buf, "FPS: %d", fps);
      int len = text_length (font, buf) + 5;
      int x = SCREEN_W - len;
      int y = 1;

      textout (bmp, font, buf, x-1, y,   black);
      textout (bmp, font, buf, x  , y-1, black);
      textout (bmp, font, buf, x+1, y,   black);
      textout (bmp, font, buf, x,   y+1, black);
      textout (bmp, font, buf, x,   y,   white);
   }

   if(key_dest != key_menu)
   {
      if( console.is_active () )
         console.draw (bmp, console.pos, true);
      else
         console_notify.draw (bmp);
   }
   else
   {
      console_notify.draw (bmp);

      // draw gui menu
      gui_draw(bmp);
   }

   release_bitmap(bmp);

   // blit it all to the screen!
   if(wait_vsync.value)
      vsync ();

   switch(anim_type)
   {
      case ANIM_DOUBLE_BUFFER:
         blit  (bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
         break;

      case ANIM_PAGE_FLIP:
         show_video_bitmap(bmp);
         break;

      case ANIM_TRIPLE_BUFFER:
         do {
         } while (poll_scroll());

         request_video_bitmap(bmp);
         break;
   }
}
