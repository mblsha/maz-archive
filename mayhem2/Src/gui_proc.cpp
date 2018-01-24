#include "def.h"
#include <allegro/internal/aintern.h>

//#include "string.h"

#include "gui.h"
#include "sound.h"

#define DEF_LISTBOX_RAD         10

#ifdef USE_SCREEN
   #define gui_bmp  screen
#else
   #define DONT_UPDATE
#endif

extern BITMAP *gui_bmp;
extern cvar   gui_trans;

int button(int msg, DIALOG *d, int c)
{
   int col1, col2, col3, rad;
   int txt1, txt2;
   int state1 = false, state2 = false, swap;

   switch(msg)
   {
      case MSG_DRAW:
         if((d->flags & D_GOTFOCUS) || (d->flags & D_SELECTED))
         {
            col1 = GUI_SELECT1;
            col2 = GUI_SELECT2;
            col3 = GUI_SELECT3;

            txt1 = GUI_SELECT_TEXT1;
            txt2 = GUI_SELECT_TEXT2;
         }
         else
         {
            col1 = GUI_NORMAL1;
            col2 = GUI_NORMAL2;
            col3 = GUI_NORMAL3;

            txt1 = GUI_NORMAL_TEXT1;
            txt2 = GUI_NORMAL_TEXT2;
         }

         if(d->d1 > 0)
         {
            rad = d->d1;
         }
         else
         {
            rad = d->h/2;
         }

         r_rect     (gui_bmp, d->x,   d->y,   d->x+d->w,   d->y+d->h,   col2, rad  );
         r_rect     (gui_bmp, d->x+1, d->y+1, d->x+d->w-1, d->y+d->h-1, col1, rad-2);
         r_rectfill (gui_bmp, d->x+3, d->y+3, d->x+d->w-3, d->y+d->h-3, col3, rad-4);
         r_rect     (gui_bmp, d->x+2, d->y+2, d->x+d->w-2, d->y+d->h-2, col2, rad-3);

         text_mode  (-1);
         gui_textout_highlight(gui_bmp, (char *)d->dp, d->x+d->w/2, d->y+d->h/2-text_height(font)/2-1, txt1, txt2, TRUE, TRUE);
         break;

      case MSG_WANTFOCUS:
         PLAY_SOUND_SELECT;
         return D_WANTFOCUS;

      case MSG_JUST_CLICK:
         PLAY_SOUND_CLICK;

         //d->flags ^= D_SELECTED;

         if(d->dp3)
         {
            cbuf_add_text((char *)d->dp3);
            cbuf_add_text("\n");
         }

         if(d->flags & D_EXIT) {
            return D_CLOSE;
         }

#ifndef DONT_UPDATE
         scare_mouse();
         SEND_MESSAGE(d, MSG_DRAW, 0);
         unscare_mouse();
#endif
         break;

      case MSG_KEY:
         if(!(d->flags & D_EXIT))
            d->flags ^= D_SELECTED;
         SEND_MESSAGE(d, MSG_JUST_CLICK, 0);
         break;

      case MSG_CLICK:
         // what state was the button originally in?
         state1 = d->flags & D_SELECTED;
         
         if (d->flags & D_EXIT)
            swap = FALSE;
         else
            swap = state1;

         // track the mouse until it is released
         while (gui_mouse_b())
         {
            state2 = ((gui_mouse_x() >= d->x) && (gui_mouse_y() >= d->y) &&
                      (gui_mouse_x() < d->x + d->w) && (gui_mouse_y() < d->y + d->h));
            if (swap)
               state2 = !state2;

            // redraw?
            if (((state1) && (!state2)) || ((!state1) && (state2)))
            {
               d->flags ^= D_SELECTED;
               state1 = d->flags & D_SELECTED;
               
#ifndef DONT_UPDATE
               scare_mouse();
               SEND_MESSAGE(d, MSG_DRAW, 0);
               unscare_mouse();
#endif
            }

            // let other objects continue to animate
            broadcast_dialog_message(MSG_UPDATE, 0);
            //broadcast_dialog_message(MSG_IDLE, 0);
         }

         if((state1 && state2 && !swap) || (!state1 && !state2 && swap))
            SEND_MESSAGE(d, MSG_JUST_CLICK, 0);
         //play_sample(snddata[CLICK].dat, 255, 128, 1000, FALSE);

         //if (!(d->flags & D_EXIT))
         //  d->flags ^= D_SELECTED;

         // should we close the dialog?
         if ((d->flags & D_SELECTED) && (d->flags & D_EXIT))
         {
            d->flags ^= D_SELECTED;
            return D_CLOSE;
         }
         break;
   }

   return D_O_K;
}

int checkbox(int msg, DIALOG *d, int c)
{
   int b, col1, col2, col3, rad;
   int    txt1, txt2;
   int    ret = D_O_K;
   char   *cmd;
   BITMAP *bmp, *icon;

   switch(msg)
   {
      case MSG_START:
         if(!(d->w))
         {
            d->w = (int)(1.5*d->h + text_length(font, (char *)d->dp));
         }

         if(d->dp3)
         {
            //
            // Get the value
            //
            d->flags &= ~D_SELECTED;

            if( cvar_variable_value((char *)d->dp3) )
               d->flags |= D_SELECTED;
         }
         break;

      case MSG_END:
      case MSG_JUST_CLICK:
         PLAY_SOUND_CLICK;

         if(d->dp3)
         {
            //
            // Set the value
            //
            cmd = (char *)malloc(256);
            sprintf(cmd, "%s %s\n", (char *)d->dp3, (d->flags & D_SELECTED) ? "1" : "0");
            cbuf_add_text(cmd);
            free(cmd);
         }

         if(d->flags & D_EXIT) {
            return D_CLOSE;
         }
      
#ifndef DONT_UPDATE
         scare_mouse();
         SEND_MESSAGE(d, MSG_DRAW, 0);
         unscare_mouse();
#endif
         break;

      case MSG_DRAW:
         bmp = create_bitmap(d->h, d->h);
         blit(gui_bmp, bmp, d->x, d->y, 0, 0, d->h, d->h);

         if(d->flags & D_GOTFOCUS)
         {
            col1 = GUI_SELECT1;
            col2 = GUI_SELECT2;
            col3 = GUI_SELECT3;

            txt1 = GUI_SELECT_TEXT1;
            txt2 = GUI_SELECT_TEXT2;
         } else {
            col1 = GUI_NORMAL1;
            col2 = GUI_NORMAL2;
            col3 = GUI_NORMAL3;

            txt1 = GUI_NORMAL_TEXT1;
            txt2 = GUI_NORMAL_TEXT2;
         }

         if(d->flags & D_SELECTED)
         {
            icon = (BITMAP *)store.dat ("/mayhem/gui/check");
            //icon = gui_check;
            b = CHECK;
         } else {
            icon = (BITMAP *)store.dat ("/mayhem/gui/cross");
            //icon = gui_cross;
            b = CROSS;
         }
         rad = (d->d1 > 0) ? d->d1 : d->h/2;

         //r_rect      (gui_bmp, d->x,   d->y,   d->x+d->h,   d->y+d->h,   col2, rad);
         //r_rect      (gui_bmp, d->x+1, d->y+1, d->x+d->h-1, d->y+d->h-1, col1, rad-2);
         //draw_sprite (gui_bmp, guidata[bmp].dat, d->x+(d->h-8)/2, d->y+(d->h-8)/2);
         r_rect     (bmp, 0, 0, d->h,   d->h,   col2, rad);
         r_rect     (bmp, 1, 1, d->h-1, d->h-1, col1, rad-2);
         r_rect     (bmp, 2, 2, d->h-2, d->h-2, col2, rad-3);
         r_rectfill (bmp, 3, 3, d->h-3, d->h-3, col3, rad-4);

         draw_sprite(bmp, icon, (d->h-10)/2, (d->h-10)/2);

         draw_sprite(gui_bmp, bmp, d->x, d->y);
         gui_textout_highlight(gui_bmp, (char *)d->dp, (int)(d->x+1.5*d->h), d->y+d->h/2-text_height(font)/2-1, txt1, txt2, FALSE, TRUE);

         destroy_bitmap(bmp);
         return D_O_K;

      default:
         ret = button(msg, d, 0);
   }

   return ret;
}
/*
int background(int msg, DIALOG *d, int c)
{
   int x,y;
   BITMAP *s;

   if(msg == MSG_DRAW)
   {
      s = (BITMAP *)back_bmp[d->d1].dat;

      for(x = 0; x < SCREEN_W; x += s->w)
      {
         for(y = 0; y < SCREEN_H; y += s->h)
         {
            blit(s, gui_bmp, 0, 0, x, y, s->w, s->h);
         }
      }
   }

   return D_O_K;
}
*/
int frame(int msg, DIALOG *d, int c)
{
   int rad; //x, y, t;
   BITMAP *bmp;
   BITMAP *pattern;

   switch(msg) 
   {
      case MSG_START:
         if(d->dp)
         {
            destroy_bitmap((BITMAP *)d->dp);
         }

         bmp = make_bitmap(d->w, d->h, 2, true);
         clear_to_color(bmp, mask_color);

         // creating pattern
         pattern = create_bitmap(2, 2);
         line(pattern, 0, 0, 1, 0, GUI_FILL4);
         line(pattern, 0, 1, 1, 1, GUI_FILL3);

         rad = (d->d1 > 0) ? d->d1 : d->h/2;
         d->d1 = rad;

         r_rect    (bmp, 0,   0,   d->w, d->h, GUI_FILL1, rad);

         if(!gui_trans.value)
         {
            line(pattern, 0, 0, 1, 1, mask_color);
        
            drawing_mode (DRAW_MODE_COPY_PATTERN, pattern, 0, 0);
            floodfill (bmp, rad, rad,             GUI_FILL3);
            drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);

            r_rect    (bmp, 0, 0, d->w,   d->h,   GUI_FILL2, d->d1);
            r_rect    (bmp, 1, 1, d->w-1, d->h-1, GUI_FILL1, d->d1-2);
            r_rect    (bmp, 2, 2, d->w-2, d->h-2, GUI_FILL2, d->d1-3);
         }
         else
         {
            drawing_mode (DRAW_MODE_COPY_PATTERN, pattern, 0, 0);
            floodfill (bmp, rad, rad,             GUI_FILL3);
            drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
         }

         d->dp = bmp;
         break;

      case MSG_END:
         destroy_bitmap((BITMAP *)d->dp);
         d->dp = NULL;
         break;

      case MSG_DRAW:
#ifdef DLG_EDIT
         bmp = create_bitmap(d->w, d->h);

         clear_to_color(bmp, mask_color);

         // creating pattern
         pattern = create_bitmap(2, 2);
         clear_to_color(pattern, mask_color);
         line(pattern, 0, 0, 1, 1, GUI_FILL3);

         rad = (d->d1 > 0) ? d->d1 : d->h/2;
         d->d1 = rad;

         r_rect    (bmp, 0,   0,   d->w, d->h, GUI_FILL1, rad);

         if(!gui_trans.value)
         {
            drawing_mode(DRAW_MODE_COPY_PATTERN, pattern, 0, 0);
            floodfill (bmp, rad, rad,             GUI_FILL3);
            drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

            r_rect    (bmp, 0, 0, d->w,   d->h,   GUI_FILL2, d->d1);
            r_rect    (bmp, 1, 1, d->w-1, d->h-1, GUI_FILL1, d->d1-2);
            r_rect    (bmp, 2, 2, d->w-2, d->h-2, GUI_FILL2, d->d1-3);
         }
         else
         {
            floodfill (bmp, rad, rad,             GUI_FILL3);
         }

         if(!gui_trans.value)
         {
            draw_sprite(gui_bmp, bmp, d->x, d->y);
         }
         else
         {
            draw_trans_sprite(gui_bmp, bmp, d->x, d->y);

            r_rect(gui_bmp, d->x,   d->y,   d->x+d->w,   d->y+d->h,   GUI_FILL2, d->d1);
            r_rect(gui_bmp, d->x+1, d->y+1, d->x+d->w-1, d->y+d->h-1, GUI_FILL1, d->d1-2);
            r_rect(gui_bmp, d->x+2, d->y+2, d->x+d->w-2, d->y+d->h-2, GUI_FILL2, d->d1-3);
         }

         destroy_bitmap(bmp);
#else
         if(!gui_trans.value)
         {
            draw_sprite(gui_bmp, (BITMAP *)d->dp, d->x, d->y);
         }
         else
         {
            draw_trans_sprite(gui_bmp, (BITMAP *)d->dp, d->x, d->y);

            r_rect(gui_bmp, d->x,   d->y,   d->x+d->w,   d->y+d->h,   GUI_FILL2, d->d1);
            r_rect(gui_bmp, d->x+1, d->y+1, d->x+d->w-1, d->y+d->h-1, GUI_FILL1, d->d1-2);
            r_rect(gui_bmp, d->x+2, d->y+2, d->x+d->w-2, d->y+d->h-2, GUI_FILL2, d->d1-3);
         }
#endif
         break;
   }

   return D_O_K;
}

int hor_line(int msg, DIALOG *d, int c)
{
   if(msg == MSG_DRAW)
   {
      hline(gui_bmp, d->x, d->y-1, d->x + d->w, GUI_FILL2);
      hline(gui_bmp, d->x, d->y, d->x + d->w,   GUI_FILL1);
      hline(gui_bmp, d->x, d->y+1, d->x + d->w, GUI_FILL2);
   }

   return D_O_K;
}

int label(int msg, DIALOG *d, int c)
{
   if(msg == MSG_DRAW)
   {
      gui_textout_highlight(gui_bmp, (char *)d->dp, d->x, d->y, d->fg, d->bg, d->d1, d->d2);
   }
   else if(msg == MSG_START)
   {
      d->fg = d->fg == -1  ? GUI_NORMAL1 : d->fg == -2  ? GUI_NORMAL2 : d->fg == -3  ? GUI_NORMAL3 : d->fg == -4 ? GUI_NORMAL_TEXT1 : d->fg == -5  ? GUI_NORMAL_TEXT2 :
              d->fg == -6  ? GUI_SELECT1 : d->fg == -7  ? GUI_SELECT2 : d->fg == -8  ? GUI_SELECT3 : d->fg == -9 ? GUI_SELECT_TEXT1 : d->fg == -10 ? GUI_SELECT_TEXT2 :
              d->fg == -11 ? GUI_FILL1   : d->fg == -12 ? GUI_FILL2   : d->fg == -13 ? GUI_FILL3   : d->fg;

      d->bg = d->bg == -1  ? GUI_NORMAL1 : d->bg == -2  ? GUI_NORMAL2 : d->bg == -3  ? GUI_NORMAL3 : d->bg == -4 ? GUI_NORMAL_TEXT1 : d->bg == -5  ? GUI_NORMAL_TEXT2 :
              d->bg == -6  ? GUI_SELECT1 : d->bg == -7  ? GUI_SELECT2 : d->bg == -8  ? GUI_SELECT3 : d->bg == -9 ? GUI_SELECT_TEXT1 : d->bg == -10 ? GUI_SELECT_TEXT2 :
              d->bg == -11 ? GUI_FILL1   : d->bg == -12 ? GUI_FILL2   : d->bg == -13 ? GUI_FILL3   : d->bg;

      if(d->h)
      {
         d->y = d->y + (d->h - text_height(font))/2;
         d->h = 0;
      }
   }

   return D_O_K;
}

void r_rect(BITMAP *buf, int x1, int y1, int x2, int y2, int color, int rad)
{
   hline(buf, x1+rad, y1,   x2-rad-1, color);
   hline(buf, x1+rad, y2-1, x2-rad-1, color);

   vline(buf, x1,   y1+rad, y2-rad-1, color);
   vline(buf, x2-1, y1+rad, y2-rad-1, color);

   arc(buf, x1+rad,   y1+rad,   itofix(64),  itofix(128), rad, color);
   arc(buf, x2-rad-1, y1+rad,   itofix(0),   itofix(64),  rad, color);
   arc(buf, x1+rad,   y2-rad-1, itofix(128), itofix(192), rad, color);
   arc(buf, x2-rad-1, y2-rad-1, itofix(192), itofix(256), rad, color);
}

void r_rectfill(BITMAP *buf, int x1, int y1, int x2, int y2, int color, int rad)
{
   int w = x2 - x1, h = y2 - y1;

   BITMAP *tmp = create_bitmap(w, h);

   clear_to_color (tmp, mask_color);

   r_rect    (tmp, 0, 0, w, h, color, rad);
   floodfill (tmp, rad, rad, color);

   draw_sprite (buf, tmp, x1, y1);

   destroy_bitmap(tmp);
}

void gui_textout_highlight(BITMAP *bmp, AL_CONST char *s, int x, int y, int col1, int col2, int centre, int outline)
{
   int col = col2;
   int c, pix_len = 0;
   int in_pos = 0;
   FONT_GLYPH *g = NULL;

   if (centre)
   {
      while ( (c = ugetc(s + in_pos)) != 0)
      {
         if (c == '&')
         {
            in_pos += ucwidth (c);
            c = ugetc (s + in_pos);
         }
         pix_len += font->vtable->char_length(font, c);
         in_pos += ucwidth (c);
      }

      x = x - pix_len / 2;
      in_pos = 0;
   }


   while( (c = ugetc (s + in_pos)) != 0 )
   {
      if(c == '&')
      {
         in_pos += ucwidth (c);
         c = ugetc (s + in_pos);

         if(c != '&')
         {
            col = col1;
         } 
      }

      // Draw the char
      g = _mono_find_glyph(font, c);

      if(outline)
      {
	      bmp->vtable->draw_glyph(bmp, g, x,   y-1, black);
	      bmp->vtable->draw_glyph(bmp, g, x-1, y,   black);
	      bmp->vtable->draw_glyph(bmp, g, x,   y+1, black);
	      bmp->vtable->draw_glyph(bmp, g, x+1, y,   black);
      }
      bmp->vtable->draw_glyph(bmp, g, x, y, col);
      x += g->w;

      col = col2;
      in_pos += ucwidth (c);
   }
}

/*
============================================================================================
slider

Draws a slider on the screen. All parameters as usual, except:

  o d1  - Doesn't matter
  o d2  - Current value
  o bg  - MAX value
============================================================================================
*/
typedef int (*slider_proc)(void *cbpointer, int d2value);
int slider(int msg, DIALOG *d, int c)
{
   int col1, col2, col3;
   int offset;
   int retval = D_O_K;
   int mx, my;
   char *cmd;
   BITMAP *bmp;
   slider_proc proc;

   switch(msg)
   {
      case MSG_START:
         //if(d->d1)
         //  d->h = d->d1;
         d->d1 = d->h/2;

         //d->dp = NULL;
         if(d->dp3)
         {
            //
            // Get the value
            //
            d->d2 = (int)cvar_variable_value((char *)d->dp3);
            if(d->d2 < 0)
               d->d2 = 0;
            else if(d->d2 > d->bg)
               d->d2 = d->bg;
         }
         break;

      case MSG_END:
         destroy_bitmap((BITMAP *)d->dp);
         d->dp = NULL;
         break;

      case MSG_WANTFOCUS:
         PLAY_SOUND_SELECT;
         return D_WANTFOCUS;

      case MSG_DRAW:
         //if(!d->dp) {
         bmp = create_bitmap(d->w + 2 * d->w, 2 * d->d1);
         blit(gui_bmp, bmp, d->x - d->d1, d->y, 0, 0, d->w + 2 * d->d1, 2 * d->d1);
         /*  d->dp = bmp;

         SEND_MESSAGE(d, MSG_DRAW, 0);

         destroy_bitmap(d->dp);
         d->dp = NULL;
         return D_O_K;
         } else {
         bmp = create_bitmap(d->w + 2 * d->w, 2 * d->d1);
         blit(d->dp, bmp, 0, 0, 0, 0, d->w + 2 * d->d1, 2 * d->d1);
         }*/

         if(d->flags & D_GOTFOCUS)
         {
            col1 = GUI_SELECT1;
            col2 = GUI_SELECT2;
            col3 = GUI_SELECT3;
         }
         else
         {
            col1 = GUI_NORMAL1;
            col2 = GUI_NORMAL2;
            col3 = GUI_NORMAL3;
         }

         mx = (d->w * d->d2 / d->bg) + d->d1;
         my = 0;//(d->h >> 1) - d->d1;
         //
         // Draw bar
         //
         r_rect(bmp, d->d1-3, d->d1-3, d->d1+d->w+3, d->d1+3, col2, 3);
         r_rect(bmp, d->d1-2, d->d1-2, d->d1+d->w+2, d->d1+2, col1, 1);
         r_rect(bmp, d->d1-1, d->d1-1, d->d1+d->w+1, d->d1+1, col2, 0);
         //
         // Draw slider
         //
         circlefill(bmp, mx, d->d1, d->d1-2, col3);
         r_rect(bmp, mx-d->d1,   0, mx+d->d1,   2*d->d1,   col2, d->d1);
         r_rect(bmp, mx-d->d1+1, 1, mx+d->d1-1, 2*d->d1-1, col1, d->d1-2);
         r_rect(bmp, mx-d->d1+2, 2, mx+d->d1-2, 2*d->d1-2, col2, d->d1-3);

         scare_mouse();
         blit(bmp, gui_bmp, 0, 0, d->x - d->d1, d->y, d->w + 2 * d->d1, 2 * d->d1);
         unscare_mouse();

         destroy_bitmap(bmp);
         break;

      case MSG_JUST_CLICK:
         PLAY_SOUND_CLICK;

         if(d->dp != NULL)
         {
            proc = (slider_proc)d->dp;
            retval |= (*proc)((void *)d->dp3, (int)d->d2);
         }

         if(d->dp3)
         {
            cmd = (char *)malloc(256);
            sprintf(cmd, "%s %i\n", (char *)d->dp3, (int)d->d2);
            cbuf_add_text(cmd);
            free(cmd);
         }

#ifndef DONT_UPDATE
         scare_mouse();
         SEND_MESSAGE(d, MSG_DRAW, 0);
         unscare_mouse();
#endif
         break;

      case MSG_SCAN:
         switch(c)
         {
            case KEY_LEFT:
               d->d2 = (--d->d2 < 0) ? 0 : d->d2;
               retval = D_USED_CHAR;
               break;

            case KEY_RIGHT:
               d->d2 = (++d->d2 > d->bg) ? d->bg : d->d2;
               retval = D_USED_CHAR;
               break;
         }

         if(retval == D_USED_CHAR)
         {
            SEND_MESSAGE(d, MSG_JUST_CLICK, 0);
         }

         return retval;
         break;
    /*
      case MSG_CHAR:
         switch(c >> 8)
         {
            case KEY_LEFT:
               d->d2 = (--d->d2 < 0) ? 0 : d->d2;
               retval = D_USED_CHAR;
               break;

            case KEY_RIGHT:
               d->d2 = (++d->d2 > d->bg) ? d->bg : d->d2;
               retval = D_USED_CHAR;
               break;
         }

         if(retval == D_USED_CHAR)
         {
            SEND_MESSAGE(d, MSG_JUST_CLICK, 0);
         }

         return retval;
         break;
    */
      case MSG_CLICK:
         mx = -1;//mouse_x;
         offset = d->d2;

         while(mouse_b)
         {
            if(mx != mouse_x)
            {
               mx = mouse_x;

               d->d2 = (mouse_x - d->x) * d->bg / (d->w - 2);
               d->d2 = MIN(d->d2, d->bg);

               if(d->d2 < 0)
                  d->d2 = 0;

               if(offset != d->d2)
               {
                  offset = d->d2;

                  SEND_MESSAGE(d, MSG_JUST_CLICK, 0);
               }
            }

            // let other objects continue to animate
            broadcast_dialog_message(MSG_UPDATE, 0);
         }
         break;
   }

   return retval;
}

void gui_set_bmp(BITMAP *bmp)
{
   gui_bmp = bmp;
}

void handle_scrollable_scroll_click(DIALOG *d, int listsize, int *offset, int height)
{
   int xx, yy;
   int hh = d->h - 5;

   while (gui_mouse_b())
   {
      int i = (hh * height + listsize/2) / listsize;
      int len = (hh * (*offset) + listsize/2) / listsize + 2;

      if ((gui_mouse_y() >= d->y+len) && (gui_mouse_y() <= d->y+len+i))
      {
	      xx = gui_mouse_y() - len + 2;
	      while (gui_mouse_b())
	      {
	         yy = (listsize * (gui_mouse_y() - xx) + hh/2) / hh;
	         if (yy > listsize-height) 
	            yy = listsize-height;

	         if (yy < 0) 
	            yy = 0;

	         if (yy != *offset)
	         {
	            *offset = yy;
#ifndef DONT_UPDATE
	            scare_mouse();
	            SEND_MESSAGE(d, MSG_DRAW, 0);
	            unscare_mouse();
#endif
	         }

	         // let other objects continue to animate
	         broadcast_dialog_message(MSG_UPDATE, 0);
	      }
      }
      else
      {
	      if (gui_mouse_y() <= d->y+len) 
	         yy = *offset - height;
	      else 
	         yy = *offset + height;

	      if (yy > listsize-height) 
	         yy = listsize-height;

	      if (yy < 0) 
	         yy = 0;

	      if (yy != *offset)
	      {
	         *offset = yy;
#ifndef DONT_UPDATE
	         scare_mouse();
	         SEND_MESSAGE(d, MSG_DRAW, 0);
	         unscare_mouse();
#endif
	      }
      }

      // let other objects continue to animate
      broadcast_dialog_message(MSG_UPDATE, 0);
   }
}

#define MSG_LIST_SELECT         1354123

typedef char *(*getfuncptr)(int, int *);
int listbox(int msg, DIALOG *d, int c)
{
   int listsize = 0, i, bottom, height, bar, orig;
   char *sel = (char *)d->dp2;
   char *buf[256];
   int redraw = FALSE;

   switch (msg)
   {
      case MSG_START:
         if(d->dp3)
            d->d1 = (int)cvar_variable_value((char *)d->dp3);

	         (*(getfuncptr)d->dp)(-1, &listsize);
	         _handle_scrollable_scroll(d, listsize, &d->d1, &d->d2);
	         break;

      case MSG_END:
         break;

      case MSG_DRAW:
         draw_listbox(d);
	      break;

      case MSG_JUST_CLICK:
         //console_dprintf("just_click!\n");
         //_handle_scrollable_scroll(d, listsize, &d->d1, &d->d2);
         if(d->dp3)
         {
            sprintf((char *)buf, "%s %d\n", (char *)d->dp3, d->d1);
            cbuf_add_text((char *)buf);
         }
         break;

      case MSG_CLICK:
	      (*(getfuncptr)d->dp)(-1, &listsize);
	      height = (d->h-4) / text_height(font);
	      bar = (listsize > height);
	      if ((!bar) || (gui_mouse_x() < d->x+d->w-2*DEF_LISTBOX_RAD))
	      {
	         if ((sel) && (!(key_shifts & KB_CTRL_FLAG)))
	         {
	            for (i=0; i<listsize; i++)
	            {
		            if (sel[i])
		            {
		               redraw = TRUE;
		               sel[i] = FALSE;
		            }
	            }

	            if (redraw)
	            {
#ifndef DONT_UPDATE
		            scare_mouse();
		            SEND_MESSAGE(d, MSG_DRAW, 0);
		            unscare_mouse();
#endif
	            }
	         }
	         _handle_listbox_click(d);
	         while (gui_mouse_b())
	         {
	            broadcast_dialog_message(MSG_UPDATE, 0);
	            //broadcast_dialog_message(MSG_IDLE, 0);
	            d->flags |= D_INTERNAL;
	            _handle_listbox_click(d);
	            d->flags &= ~D_INTERNAL;
	         }
         }
	      else
	      {
	         handle_scrollable_scroll_click(d, listsize, &d->d2, height);
	      }
         SEND_MESSAGE(d, MSG_JUST_CLICK, 0);
	      break;

      case MSG_LIST_SELECT:
	      (*(getfuncptr)d->dp)(-1, &listsize);
	      /*
	      if (d->flags & D_EXIT)
	      {
	         if (listsize)
	         {
	            i = d->d1;
	            SEND_MESSAGE(d, MSG_CLICK, 0);
	            if (i == d->d1)
	            return D_CLOSE;
	         }
	      }
	      */
         if (listsize)
         {
  	         i = d->d1;
	         if (i == d->d1)
            {
               if(d->flags & LISTBOX_EXEC_DP3)
               {
                  if(d->dp3)
                  {
                     //sprintf(buf, "%s %d\n", d->dp2, i);
                     sprintf((char *)buf, "%s %d\n", (char *)d->dp3, d->d1);
                     cbuf_add_text((char *)buf);

                     //console_dprintf((char *)buf);
                  }
               }

               if(d->flags & LISTBOX_CALL_DP)
               {
                  (*(getfuncptr)d->dp)(-(i+2), &listsize);
                  break;
               }
            }
         }
         break;

      case MSG_DCLICK:
	      (*(getfuncptr)d->dp)(-1, &listsize);
	      height = (d->h-4) / text_height(font);
	      bar = (listsize > height);
	      if ((!bar) || (gui_mouse_x() < d->x+d->w-2*DEF_LISTBOX_RAD))
	      {
	         SEND_MESSAGE(d, MSG_CLICK, 0);
            SEND_MESSAGE(d, MSG_LIST_SELECT, 0);
  	      }
	      break;

      case MSG_WHEEL:
	      (*(getfuncptr)d->dp)(-1, &listsize);
	      height = (d->h-4) / text_height(font);
	      if (height < listsize)
	      {
	         int delta = (height > 3) ? 3 : 1;

	         if (c > 0) 
	            i = MAX(0, d->d2-delta);
	         else
	            i = MIN(listsize-height, d->d2+delta);
	         if (i != d->d2)
	         {
	            d->d2 = i;
#ifndef DONT_UPDATE
	            scare_mouse();
	            SEND_MESSAGE(d, MSG_DRAW, 0);
	            unscare_mouse();
#endif
	         }
	      }
         SEND_MESSAGE(d, MSG_JUST_CLICK, 0);
	      break;

      case MSG_KEY:
         SEND_MESSAGE(d, MSG_LIST_SELECT, 0);
	      //(*(getfuncptr)d->dp)(-1, &listsize);
	      //if ((listsize) && (d->flags & D_EXIT))
	      //   return D_CLOSE;
	      break;

      case MSG_WANTFOCUS:
	      return D_WANTFOCUS;

      case MSG_CHAR:
	      (*(getfuncptr)d->dp)(-1, &listsize);

	      if (listsize)
	      {
	         c >>= 8;
            if(SEND_MESSAGE(d, MSG_SCAN, c) != D_O_K)
              return D_USED_CHAR;
         }
         break;

      case MSG_SCAN:
	      (*(getfuncptr)d->dp)(-1, &listsize);

	      if (listsize)
	      {
	         //c >>= 8;

	         bottom = d->d2 + (d->h-4)/text_height(font) - 1;
	         if (bottom >= listsize-1)
	            bottom = listsize-1;

	         orig = d->d1;

	         if (c == KEY_UP)
	            d->d1--;
	         else if (c == KEY_DOWN)
	            d->d1++;
	         else if (c == KEY_HOME)
	            d->d1 = 0;
	         else if (c == KEY_END)
	            d->d1 = listsize-1;
	         else if (c == KEY_PGUP)
	         {
	            if (d->d1 > d->d2)
		            d->d1 = d->d2;
	            else
		            d->d1 -= (bottom - d->d2) ? bottom - d->d2 : 1;
	         }
	         else if (c == KEY_PGDN)
	         {
	            if (d->d1 < bottom)
		            d->d1 = bottom;
	            else
		            d->d1 += (bottom - d->d2) ? bottom - d->d2 : 1;
	         } 
	         else 
	            return D_O_K;

	         if (sel)
	         {
	            if (!(key_shifts & (KB_SHIFT_FLAG | KB_CTRL_FLAG)))
	            {
		            for (i=0; i<listsize; i++)
		               sel[i] = FALSE;
	            }
	            else if (key_shifts & KB_SHIFT_FLAG)
	            {
		            for (i=MIN(orig, d->d1); i<=MAX(orig, d->d1); i++)
		            {
		               if (key_shifts & KB_CTRL_FLAG)
			               sel[i] = (i != d->d1);
		               else
			               sel[i] = TRUE;
		            }
	            }
	         }

	         // if we changed something, better redraw...
	         _handle_scrollable_scroll(d, listsize, &d->d1, &d->d2);
            SEND_MESSAGE(d, MSG_JUST_CLICK, 0);
	         d->flags |= D_DIRTY;
	         return D_USED_CHAR;
	      }
	      break;
   }

   return D_O_K;
}

void draw_listbox(DIALOG *d)
{
   int height, listsize = 0, i, len, bar, x, y, w;
   int fg_color = 0, fg, bg;
   char *sel = (char *)d->dp2;
   char s[1024];

   (*(getfuncptr)d->dp)(-1, &listsize);

   height = (d->h-4) / text_height(font);
   bar = (listsize > height);
   w = (bar ? d->w-15 : d->w-3);

   r_rectfill (gui_bmp, d->x+3, d->y+3, d->x+d->w-3, d->y+d->h-3, GUI_FILL3, DEF_LISTBOX_RAD-4);

   set_clip(gui_bmp, d->x, d->y, d->x+d->w, d->y+d->h);

   // draw box contents
   for (i = 0; i < height; i++)
   {
      if (d->d2+i < listsize)
      {
         if (d->d2+i == d->d1)
         {
            fg = GUI_SELECT_TEXT2;
            bg = GUI_SELECT_TEXT1;
         }
         else if ((sel) && (sel[d->d2+i]))
         {
            fg = GUI_SELECT_TEXT2;
            bg = GUI_SELECT_TEXT1;
         }
         else
         {
            fg = GUI_NORMAL_TEXT1;
            bg = GUI_NORMAL_TEXT2;
         }
         //usetc(s, 0);
         //ustrncat(s, (*(getfuncptr)d->dp)(i+d->d2, NULL), sizeof(s)-ucwidth(0));
         strcpy(s, "");
         strncat(s, (*(getfuncptr)d->dp)(i+d->d2, NULL), sizeof(s));
         //strncat(s, (*(getfuncptr)d->dp)(i+d->d2, NULL), 50);
         x = d->x + 2;
         y = d->y + 2 + i*text_height(font);
         //rtm = text_mode(bg);
         //rectfill(gui_bmp, x, y, x+7, y+text_height(font)-1, bg);
         x += 8;
         len = ustrlen(s);
         while (text_length(font, s) >= MAX(d->w - 1 - (bar ? 22 : 10), 1))
         {
            len--;
            usetat(s, len, 0);
         }
         //textout(gui_bmp, font, s, x, y, fg);
         //console_dprintf(".");
         gui_textout_highlight(gui_bmp, s, x, y, fg, bg, FALSE, TRUE);
         //text_mode(rtm);
         x += text_length(font, s);
         //if (x <= d->x+w)
         //   rectfill(gui_bmp, x, y, d->x+w, y+text_height(font)-1, bg);
      }
      else
      {
         //rectfill(gui_bmp, d->x+2,  d->y+2+i*text_height(font), d->x+w, d->y+1+(i+1)*text_height(font), d->bg);
      }
   }

   set_clip(gui_bmp, 0, 0, SCREEN_W, SCREEN_H);

   // draw frame, maybe with scrollbar
   draw_scrollable_frame(d, listsize, d->d2, height, fg_color, d->bg);
}

void draw_scrollable_frame(DIALOG *d, int listsize, int offset, int height, int fg_color, int bg)
{
   int   xx;
   float yy;
   int   col1, col2, col3, col4;
   //int rad = d->d1;

   float size     = d->h / text_height(font);
   float one_size = d->h / listsize;
   float h        = size * one_size;

   if(h < (2*DEF_LISTBOX_RAD))
   {
      h = (2*DEF_LISTBOX_RAD);
      one_size = (d->h - h) / listsize;
   }

   //textprintf(gui_bmp, font, 0,0,white, "size = %f, one_Size = %f, h = %f, listsize = %d", size, one_size, h, listsize);

   if(d->flags & D_GOTFOCUS)
   {
      col1 = GUI_SELECT1;
      col2 = GUI_SELECT2;
      col3 = GUI_SELECT3;
   }
   else
   {
      col1 = GUI_NORMAL1;
      col2 = GUI_NORMAL2;
      col3 = GUI_NORMAL3;
   }
   col4 = GUI_NORMAL3;

   r_rect     (gui_bmp, d->x,   d->y,   d->x+d->w,   d->y+d->h,   col2, DEF_LISTBOX_RAD);
   r_rect     (gui_bmp, d->x+1, d->y+1, d->x+d->w-1, d->y+d->h-1, col1, DEF_LISTBOX_RAD-2);
   //r_rectfill (gui_bmp, d->x+3, d->y+3, d->x+d->w-3, d->y+d->h-3, col3, DEF_LISTBOX_RAD-4);
   r_rect     (gui_bmp, d->x+2, d->y+2, d->x+d->w-2, d->y+d->h-2, col2, DEF_LISTBOX_RAD-3);

   // possibly draw scrollbar
   if (listsize > height)
   {
      // little scroller frame
      r_rect     (gui_bmp, d->x+d->w - (2*DEF_LISTBOX_RAD),   d->y,   d->x+d->w,   d->y+d->h,   col2, DEF_LISTBOX_RAD);
      r_rect     (gui_bmp, d->x+d->w - (2*DEF_LISTBOX_RAD)+1, d->y+1, d->x+d->w-1, d->y+d->h-1, col1, DEF_LISTBOX_RAD-2);
      r_rectfill (gui_bmp, d->x+d->w - (2*DEF_LISTBOX_RAD)+3, d->y+3, d->x+d->w-3, d->y+d->h-3, GUI_NORMAL3, DEF_LISTBOX_RAD-4);
      r_rect     (gui_bmp, d->x+d->w - (2*DEF_LISTBOX_RAD)+2, d->y+2, d->x+d->w-2, d->y+d->h-2, col2, DEF_LISTBOX_RAD-3);

      xx = d->x+d->w - (2*DEF_LISTBOX_RAD);
      yy = d->y;

      if (offset > 0)
      {
         yy += (float)(one_size * offset);
      }

      r_rect     (gui_bmp, xx,   (int)(yy),   d->x+d->w,   (int)(yy+h  ), col2, DEF_LISTBOX_RAD);
      r_rect     (gui_bmp, xx+1, (int)(yy+1), d->x+d->w-1, (int)(yy+h-1), col1, DEF_LISTBOX_RAD-2);
      r_rectfill (gui_bmp, xx+3, (int)(yy+3), d->x+d->w-3, (int)(yy+h-3), col3, DEF_LISTBOX_RAD-4);
      r_rect     (gui_bmp, xx+2, (int)(yy+2), d->x+d->w-2, (int)(yy+h-2), col2, DEF_LISTBOX_RAD-3);
   }
}

#ifdef USE_SCREEN
#define gui_bmp  gui_bmp
#endif

