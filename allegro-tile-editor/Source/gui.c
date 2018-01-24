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
 *   Here is my modified gui code, and some my own inventions
 *
 * CHANGES:
 *
 *   01.06.00 - v0.1.0
 *     Initial release
 *
 *   09.06.01 - v0.1.0
 *     Trying to rewrite gui_textout_highlight
 */

#include "ated.h"

#ifdef DLG_EDIT
       #define fat_error(x)     exit(1)
#endif

typedef struct MENU_INFO            /* information about a popup menu */
{
   MENU *menu;                      /* the menu itself */
   struct MENU_INFO *parent;        /* the parent menu, or NULL for root */
   int bar;                         /* set if it is a top level menu bar */
   int size;                        /* number of items in the menu */
   int sel;                         /* selected item */
   int x, y, w, h;                  /* screen position of the menu */
   int (*proc)();                   /* callback function */
   BITMAP *saved;                   /* saved what was underneath it */
} MENU_INFO;

static void draw_menu(MENU_INFO *m);
static void fill_menu_info(MENU_INFO *m, MENU *menu, MENU_INFO *parent, int bar, int x, int y, int minw, int minh);
static void draw_menu_item(MENU_INFO *m, int c);
static int _do_menu(MENU *menu, MENU_INFO *parent, int bar, int x, int y, int repos, int *dret, int minw, int minh);
static int menu_alt_key(int k, MENU *m);
static int menu_key_shortcut(int c, AL_CONST char *s);
static int mouse_in_parent_menu(MENU_INFO *m);
static int menu_mouse_object(MENU_INFO *m);
static void get_menu_pos(MENU_INFO *m, int c, int *x, int *y, int *w);
void draw_ated_listbox(DIALOG *d);
void draw_scrollable_ated_frame(DIALOG *d, int listsize, int offset, int height, int fg_color, int bg);

int do_ated_dialog(DIALOG *dialog, int focus_obj)
{
   int ret;

   ret = do_dialog(dialog, focus_obj);

   while(key[KEY_ESC] || key[KEY_ENTER] || key[KEY_SPACE]);

   return ret;
}

void dotted_rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int fg, int bg)
{
   int x = ((x1+y1) & 1) ? 1 : 0;
   int c;

   for (c=x1; c<=x2; c++) 
   {
      putpixel(bmp, c, y1, (((c+y1) & 1) == x) ? fg : bg);
      putpixel(bmp, c, y2, (((c+y2) & 1) == x) ? fg : bg);
   }

   for (c=y1+1; c<y2; c++) 
   {
      putpixel(bmp, x1, c, (((c+x1) & 1) == x) ? fg : bg);
      putpixel(bmp, x2, c, (((c+x2) & 1) == x) ? fg : bg);
   }
}

int background (int msg, DIALOG *d, int c)
{
   if (msg == MSG_DRAW)
   {
      drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);
      //rectfill (screen, d->x, d->y, d->x + d->w, d->y + d->h, gray);
      rectfill (screen, 0, 0, SCREEN_W, SCREEN_H, gray);
      drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   }

   return D_O_K;
}

int box(int msg, DIALOG *d, int c)
{
   if (msg == MSG_DRAW) 
   {
      BITMAP *bmp;

      bmp = create_bitmap (d->w, d->h);
      drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);
      rectfill (bmp, 0, 0, bmp->w, bmp->h, gray);
      drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
      hline (bmp, 0,          bmp->h,     bmp->w,     black);// pure black
      vline (bmp, bmp->w,     0,          bmp->h,     black);// pure black      
      hline (bmp, 1,          bmp->h - 1, bmp->w - 1, gui_col3);
      vline (bmp, bmp->w - 1, 1,          bmp->h - 1, gui_col3);
      hline (bmp, 0,          0,          bmp->w - 1, gui_col1);
      vline (bmp, 0,          0,          bmp->h - 1, gui_col1);

      blit (bmp, screen, 0, 0, d->x, d->y, bmp->w, bmp->h);
      destroy_bitmap (bmp);
   }

   return D_O_K;
}

int icon(int msg, DIALOG *d, int c) 
{
   if(msg == MSG_DRAW || msg == MSG_UPDATE_INFO) 
   {
      if(d->d1)
         rectfill(screen, d->x, d->y, d->x + d->d2, d->y + d->d2, BkgndColor);

      draw_sprite(screen, d->dp, d->x, d->y);
   }

   return D_O_K;
}

int rle_icon(int msg, DIALOG *d, int c)
{
   if(msg == MSG_DRAW || msg == MSG_UPDATE_INFO)
   {
      if(d->d1)
         rectfill(screen, d->x, d->y, d->x + d->d2, d->y + d->d2, BkgndColor);
         
      draw_rle_sprite(screen, d->dp, d->x, d->y);
   }

   return D_O_K;
}

int button(int msg, DIALOG *d, int c) 
{
   int col1, col2;
   int state1, state2, swap;

   switch(msg) 
   {
      case MSG_START:
         if(!d->fg)
            d->fg = white;
         if(!d->bg)
            d->bg = gray;
         break;

      case MSG_DRAW:
         if((d->flags & D_GOTFOCUS)/* || (d->flags & D_SELECTED)*/) 
         {
            col1 = d->fg;
            col2 = d->fg;
         }
         else 
         {
            col1 = d->fg;
            col2 = d->bg;
         }

         drawing_mode(DRAW_MODE_COPY_PATTERN, skin, ((d->flags & D_SELECTED) ? 1 : 0), ((d->flags & D_SELECTED) ? 1 : 0));
         rectfill(screen, d->x, d->y, d->x + d->w, d->y + d->h, gray);
         drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

         if (d->flags & D_SELECTED) 
         {
            hline(screen, d->x,        d->y + d->h, d->x + d->w,     gui_col1);
            vline(screen, d->x + d->w, d->y,        d->y + d->h,     gui_col1);
            hline(screen, d->x + 1,    d->y + 1,    d->x + d->w - 1, gui_col3);
            vline(screen, d->x + 1,    d->y + 1,    d->y + d->h - 1, gui_col3);
            hline(screen, d->x,        d->y,        d->x + d->w - 1, black);// pure black
            vline(screen, d->x,        d->y,        d->y + d->h - 1, black);// pure black
         }
         else 
         {
            hline(screen, d->x,            d->y + d->h,     d->x + d->w,     black);// pure black
            vline(screen, d->x + d->w,     d->y,            d->y + d->h,     black);// pure black
            hline(screen, d->x + 1,        d->y + d->h - 1, d->x + d->w - 1, gui_col3);
            vline(screen, d->x + d->w - 1, d->y+1,          d->y + d->h - 1, gui_col3);
            hline(screen, d->x,            d->y,            d->x + d->w - 1, gui_col1);
            vline(screen, d->x,            d->y,            d->y + d->h - 1, gui_col1);
         }
         text_mode(-1);
         gui_textout_highlight(screen, d->dp, ((d->x+d->w/2) + ((d->flags & D_SELECTED) ? 1 : 0)),
                                              ((d->y+d->h/2-text_height(font)/2-1) + ((d->flags & D_SELECTED) ? 1 : 0)),
                                              col1, col2, black, TRUE, TEXT_OUTLINE_2, font);
         break;

      case MSG_WANTFOCUS:
         if(!(d->flags & D_NOFOCUS))
            return D_WANTFOCUS;
         break;

      case MSG_KEY:
         if(d->flags & D_EXIT) 
         {
            return D_CLOSE;
         }

         d->flags ^= D_SELECTED;

         if((d->d1) && (!(d->flags & D_SELECTED)))
            d->flags ^= D_SELECTED;

         scare_mouse();
         SEND_MESSAGE(d, MSG_DRAW, 0);
         unscare_mouse();
         break;

      case MSG_CLICK:
         /* what state was the button originally in? */
         state1 = d->flags & D_SELECTED;
         if (d->flags & D_EXIT)
            swap = FALSE;
         else
            swap = state1;

         /* track the mouse until it is released */
         while (gui_mouse_b()) 
         {
            state2 = ((gui_mouse_x() >= d->x) && (gui_mouse_y() >= d->y) &&
                      (gui_mouse_x() < d->x + d->w) && (gui_mouse_y() < d->y + d->h));
            if (swap)
               state2 = !state2;

            /* redraw? */
            if (((state1) && (!state2)) || ((!state1) && (state2)))
            {
               d->flags ^= D_SELECTED;
               state1 = d->flags & D_SELECTED;
               scare_mouse();
               SEND_MESSAGE(d, MSG_DRAW, 0);
               unscare_mouse();
            }

            /* let other objects continue to animate */
            broadcast_dialog_message(MSG_IDLE, 0);
         }

         if((d->d1 && (!(d->flags & D_SELECTED))) && ((state1 && state2 && !swap) || (!state1 && !state2 && swap)))
            d->flags ^= D_SELECTED;

         /* should we close the dialog? */
	      if ((d->flags & D_SELECTED) && (d->flags & D_EXIT)) 
         {
   	      d->flags ^= D_SELECTED;
	         return D_CLOSE;
         }
         break;

      case MSG_RADIO:
         if ((d->d1) && (c == d->d2) && (d->flags & D_SELECTED)) 
         {
	         d->flags &= ~D_SELECTED;
	         scare_mouse();
	         SEND_MESSAGE(d, MSG_DRAW, 0);
	         unscare_mouse();
         }
         break;
   }

   if((d->d1) && ((msg == MSG_KEY) || (msg == MSG_CLICK)) &&
      (d->flags & D_SELECTED) && (!(d->flags & D_EXIT))) 
   {
      d->flags &= ~D_SELECTED;
      broadcast_dialog_message (MSG_RADIO, d->d2);
      d->flags |= D_SELECTED;
      broadcast_dialog_message (MSG_UPDATE_INFO, 0);
      broadcast_dialog_message (MSG_DRAW, 0);
   }

   return D_O_K;
}

int label(int msg, DIALOG *d, int c) 
{
   FONT *fnt;

   switch(msg) 
   {
      case MSG_START:
         if(d->fg == 0)
            d->fg = white;
         if(d->bg == 0)
            d->bg = gray;//gui_col1;
         break;

      case MSG_DRAW:
      case MSG_UPDATE_INFO:
         if(!d->dp2) 
         {
            fnt = font;
         } 
         else 
         {
            fnt = d->dp2;
         }

         gui_textout_highlight(screen, d->dp, d->x, d->y, d->fg, d->bg, black, d->d1, d->d2, fnt);
         break;
   }

   return D_O_K;
}

void gui_textout_highlight(BITMAP *bmp, char *s, int xx, int y, int col1, int col2, int col3, int centre, int outline, FONT *fnt)
{
   int col = col2;
   int c, pix_len = 0;
   int in_pos = 0;
   int x = xx;
   FONT_GLYPH *g = NULL;

   if (!fnt)
      fnt = font;

   if (centre)
   {
      while ( (c = ugetc(s + in_pos)) != 0)
      {
         if (c == '&')
         {
            in_pos += ucwidth (c);
            c = ugetc (s + in_pos);
         }
         pix_len += fnt->vtable->char_length(fnt, c);
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
      else if (c == '\n')
      {
         x = xx;
         y += text_height (fnt);
      }

      // Draw the char
      g = _mono_find_glyph(fnt, c);

      if(outline == TEXT_OUTLINE_4)
      {
	      bmp->vtable->draw_glyph(bmp, g, x,   y-1, black);
	      bmp->vtable->draw_glyph(bmp, g, x-1, y,   black);
      }
      if(outline == TEXT_OUTLINE_4 || outline == TEXT_OUTLINE_2)
      {
	      bmp->vtable->draw_glyph(bmp, g, x,   y+1, black);
	      bmp->vtable->draw_glyph(bmp, g, x+1, y,   black);
      }
      bmp->vtable->draw_glyph(bmp, g, x, y, col);
      x += g->w;

      col = col2;
      in_pos += ucwidth (c);
   }
}

int edit_text(int msg, DIALOG *d, int c)
{
   static int ignore_next_uchar = FALSE;
   int f, l, p, w, x, fg, b, scroll;
   char buf[16];
   char *s;
   int rtm;

   s = d->dp;
   l = ustrlen(s);
   if (d->d2 > l) 
      d->d2 = l;

   /* calculate maximal number of displayable characters */
   if (d->d2 == l)  
   {
      usetc(buf+usetc(buf, ' '), 0);
      x = text_length(font, buf);
   }
   else
   {
      x = 0;
   }

   b = 0;

   for (p=d->d2; p>=0; p--) 
   {
      usetc(buf+usetc(buf, ugetat(s, p)), 0);
      x += text_length(font, buf);
      b++;
      if (x > d->w) 
	      break;
   }

   if (x <= d->w) 
   {
      b = l; 
      scroll = FALSE;
   }
   else 
   {
      b--; 
      scroll = TRUE;
   }

   switch (msg) 
   {
      case MSG_START:
         if(d->bg == 0)
           d->bg = white;

         if(d->fg == 0)
           d->fg = black;

         d->h = text_height(font);
         d->w -= 2;
         d->x += 2;

         d->d2 = l;
	      break;

      case MSG_END:
         d->w += 2;
         d->x -= 2;
         break;

      case MSG_DRAW:
	      fg = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
	      x = 0;

         hline(screen, d->x - 2,        d->y - 2,        d->x + d->w, black);
         vline(screen, d->x - 2,        d->y - 2,        d->y + d->h, black);
         hline(screen, d->x - 1,        d->y - 1,        d->x + d->w, gui_col3);
         vline(screen, d->x - 1,        d->y - 1,        d->y + d->h, gui_col3);
         hline(screen, d->x - 2,        d->y + d->h,     d->x + d->w, gui_col1);
         vline(screen, d->x + d->w,     d->y - 2,        d->y + d->h, gui_col1);

	      if (scroll) {
	         p = d->d2-b+1; 
	         b = d->d2; 
	      }
	      else 
	         p = 0; 

	      for (; p <= b; p++) 
         {
	         f = ugetat(s, p);
      	   usetc(buf+usetc(buf, (f) ? f : ' '), 0);
	         w = text_length(font, buf);
	         if (x+w > d->w) 
	            break;
	         f = ((p == d->d2) && (d->flags & D_GOTFOCUS));
	         rtm = text_mode((f) ? fg : d->bg);
	         textout(screen, font, buf, d->x+x, d->y, (f) ? d->bg : fg);
	         text_mode(rtm);
	         x += w;
	      }
	      if (x < d->w)
	         rectfill(screen, d->x+x, d->y, d->x+d->w-1, d->y+text_height(font)-1, d->bg);
	      break;

      case MSG_CLICK:
	      x = d->x;

	      if (scroll) 
         {
	         p = d->d2-b+1; 
	         b = d->d2; 
	      }
	      else
	         p = 0; 

	      for (; p<b; p++) 
         {
	         usetc(buf+usetc(buf, ugetat(s, p)), 0);
	         x += text_length(font, buf);
	         if (x > gui_mouse_x()) 
	            break;
	      }
	      d->d2 = MID(0, p, l);
	      scare_mouse();
	      SEND_MESSAGE(d, MSG_DRAW, 0);
	      unscare_mouse();
	      break;

      case MSG_WANTFOCUS:
      case MSG_LOSTFOCUS:
	      return D_WANTFOCUS;

      case MSG_KEY:
         if(d->flags & D_EXIT)
           return D_EXIT;
         break;

      case MSG_CHAR:
	      ignore_next_uchar = FALSE;
   
	      if ((c >> 8) == KEY_LEFT) {
	         if (d->d2 > 0) d->d2--;
	      }
	      else if ((c >> 8) == KEY_RIGHT) {
	         if (d->d2 < l) d->d2++;
	      }
	      else if ((c >> 8) == KEY_HOME) {
	         d->d2 = 0;
	      }
	      else if ((c >> 8) == KEY_END) {
	         d->d2 = l;
	      }
	      else if ((c >> 8) == KEY_DEL) {
	         if (d->d2 < l)
	            uremove(s, d->d2);
	      }
	      else if ((c >> 8) == KEY_BACKSPACE) {
	         if (d->d2 > 0) 
            {
	            d->d2--;
	            uremove(s, d->d2);
	         }
	      }
	      else if ((c >> 8) == KEY_ENTER) {
	         if (d->flags & D_EXIT) {
	            scare_mouse();
	            SEND_MESSAGE(d, MSG_DRAW, 0);
	            unscare_mouse();
	            return D_CLOSE;
	         }
	         else
	            return D_O_K;
	      }
	      else if ((c >> 8) == KEY_TAB) {
	         ignore_next_uchar = TRUE;
	         return D_O_K;
	      }
	      else {
	         /* don't process regular keys here: MSG_UCHAR will do that */
	         break;
	      }
	      scare_mouse();
	      SEND_MESSAGE(d, MSG_DRAW, 0);
	      unscare_mouse();
	      return D_USED_CHAR;

      case MSG_UCHAR:
	      if ((c >= ' ') && (uisok(c)) && (!ignore_next_uchar)) {
	         if (l < d->d1) {
	            uinsert(s, d->d2, c);
	            d->d2++;

	            scare_mouse();
	            SEND_MESSAGE(d, MSG_DRAW, 0);
	            unscare_mouse();
	         }
	         return D_USED_CHAR;
	      }
	      break;
   }

   return D_O_K;
}


int menu(int msg, DIALOG *d, int c)
{ 
   MENU_INFO m;
   int ret = D_O_K;
   int x;
   FONT *fnt;

   switch (msg) 
   {
      case MSG_START:
         fnt = font;
         font = smallfont;
	      fill_menu_info(&m, d->dp, NULL, TRUE, d->x-1, d->y-1, d->w+2, d->h+2);
         font = fnt;
	      d->w = m.w-2;
	      d->h = m.h-2;
	      break;

      case MSG_DRAW:
	      fill_menu_info(&m, d->dp, NULL, TRUE, d->x-1, d->y-1, d->w+2, d->h+2);
	      draw_menu(&m);
	      break;

      case MSG_XCHAR:
	      x = menu_alt_key(c, d->dp);
	      if (!x)
	         break;

	      ret |= D_USED_CHAR;
	      simulate_keypress(x);
	      /* fall through */

      case MSG_GOTMOUSE:
      case MSG_CLICK:
	      _do_menu(d->dp, NULL, TRUE, d->x-1, d->y-1, FALSE, &x, d->w+2, d->h+2);
      	ret |= x;
	      do {
	      } while (gui_mouse_b());
	      break;
   }

   return ret;
}

static void draw_menu(MENU_INFO *m)
{
   int c, x, y, w, h;

   drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);

   if(m->bar) 
   {
      rectfill(screen, 0, 0, SCREEN_W, 7, gray);
   } 
   else 
   {
      get_menu_pos(m, 0, &x, &y, &w);
      h = (text_height(font)+4) * m->size;
      rectfill(screen, x, y, x+w, y + h, gui_col3);
   }

   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

   if(!m->bar) 
   {
      hline(screen, x - 1,     y - 1,     x + w + 1, gui_col1);
      vline(screen, x - 1,     y - 1,     y + h + 1, gui_col1);
      hline(screen, x,         y + h + 1, x + w + 1, gui_col3);
      vline(screen, x + w + 1, y,         y + h + 1, gui_col3);
      hline(screen, x - 1,     y + h + 2, x + w + 2, black);// pure black
      vline(screen, x + w + 2, y - 1,     y + h + 2, black);// pure black
   }

   for (c=0; m->menu[c].text; c++)
      draw_menu_item(m, c);
}

static void fill_menu_info(MENU_INFO *m, MENU *menu, MENU_INFO *parent, int bar, int x, int y, int minw, int minh)
{
   char buf[80], *tok;
   int extra = 0;
   int c, i, j;
   int child = FALSE;
   FONT *fnt;


   m->menu = menu;
   m->parent = parent;
   m->bar = bar;
   m->x = x;
   m->y = y;
   m->w = 2;
   m->h = (m->bar) ? (text_height(font)+6) : 2;
   m->proc = NULL;
   m->sel = -1;

   /* calculate size of the menu */
   for (m->size=0; m->menu[m->size].text; m->size++) {

      if ((m->menu[m->size].child) && (!m->bar)) child = TRUE;

      i = 0;
      j = ugetc(m->menu[m->size].text);

      while ((j) && (j != '\t')) 
      {
	      i += usetc(buf+i, j);
	      j = ugetc(m->menu[m->size].text+i);
      }

      usetc(buf+i, 0);

      if(m->bar) {
        fnt = font;
        font = smallfont;
      }

      c = gui_strlen(buf);

      if (m->bar) 
      {
	      m->w += c+16;
         m->h = text_height(font)+4;//7;
      }
      else 
      {
	      m->h += text_height(font)+4;
	      m->w = MAX(m->w, c+16);
      }

      if (j == '\t') 
      {
	      tok = m->menu[m->size].text+i + uwidth(m->menu[m->size].text+i);
	      c = gui_strlen(tok);
	      extra = MAX(extra, c);
      }

      if(m->bar) {
        font = fnt;
      }
   }

   if (extra)
      m->w += extra+16;

   if (child)
      m->w += 22;

   m->w = MAX(m->w, minw);
   m->h = MAX(m->h, minh);
}

static int _do_menu(MENU *menu, MENU_INFO *parent, int bar, int x, int y, int repos, int *dret, int minw, int minh)
{
   MENU_INFO m;
   MENU_INFO *i;
   int c, c2;
   int ret = -1;
   int mouse_on = gui_mouse_b();
   int old_sel;
   int mouse_sel;
   int _x, _y;
   int redraw = TRUE;

   scare_mouse();

   fill_menu_info(&m, menu, parent, bar, x, y, minw, minh);

   if (repos) {
      m.x = MID(0, m.x, SCREEN_W-m.w-1);
      m.y = MID(0, m.y, SCREEN_H-m.h-1);
   }

   /* save screen under the menu */
   if(!(m.saved = create_bitmap(m.w+2, m.h+2)))
      fat_error("Out of memory!");

   if (m.saved)
      blit(screen, m.saved, m.x, m.y, 0, 0, m.w+2, m.h+2);
   else
      *allegro_errno = ENOMEM;

   m.sel = mouse_sel = menu_mouse_object(&m);
   if ((m.sel < 0) && (!mouse_on) && (!bar))
      m.sel = 0;

   unscare_mouse();

   do 
   {
      yield_timeslice();

      old_sel = m.sel;

      c = menu_mouse_object(&m);
      if ((gui_mouse_b()) || (c != mouse_sel))
	      m.sel = mouse_sel = c;

      if (gui_mouse_b())                             /* if button pressed */
      {
	      if ((gui_mouse_x() < m.x) || (gui_mouse_x() > m.x+m.w) ||
	          (gui_mouse_y() < m.y) || (gui_mouse_y() > m.y+m.h)) 
         {
	         if (!mouse_on)                            /* dismiss menu? */
	            break;

	         if (mouse_in_parent_menu(m.parent))       /* back to parent? */
	            break;
	      }

	      if ((m.sel >= 0) && (m.menu[m.sel].child))   /* bring up child? */
	         ret = m.sel;

	      mouse_on = TRUE;
	      clear_keybuf();
      }
      else                                            /* button not pressed */
      {
	      if (mouse_on)                                /* selected an item? */
	         ret = m.sel;

	      mouse_on = FALSE;
   
	      if (keypressed())                           /* keyboard input */
         {
	         c = readkey();

	         if ((c & 0xFF) == 27) 
            {
	            ret = -1;
	            goto getout;
	         }

	         switch (c >> 8) 
            {
	            case KEY_LEFT:
		            if (m.parent) {
		               if (m.parent->bar) {
			               simulate_keypress(KEY_LEFT<<8);
			               simulate_keypress(KEY_DOWN<<8);
		               }
		               ret = -1;
		               goto getout;
		            }
		            /* fall through */

   	         case KEY_UP:
		            if ((((c >> 8) == KEY_LEFT) && (m.bar)) ||
		                (((c >> 8) == KEY_UP) && (!m.bar))) 
                  {
		               c = m.sel;
		               do 
                     {
			               c--;
			               if (c < 0)
			               c = m.size - 1;
		               } while ((!ugetc(m.menu[c].text)) && (c != m.sel));
		               m.sel = c;
		            }
		            break;

	            case KEY_RIGHT:
		            if (((m.sel < 0) || (!m.menu[m.sel].child)) &&
		                (m.parent) && (m.parent->bar)) 
                  {
		               simulate_keypress(KEY_RIGHT<<8);
		               simulate_keypress(KEY_DOWN<<8);
		               ret = -1;
		               goto getout;
		            }
		            /* fall through */

	            case KEY_DOWN:
		            if ((m.sel >= 0) && (m.menu[m.sel].child) &&
		                ((((c >> 8) == KEY_RIGHT) && (!m.bar)) ||
		                 (((c >> 8) == KEY_DOWN) && (m.bar)))) 
                  {
		               ret = m.sel;
		            }
		            else if ((((c >> 8) == KEY_RIGHT) && (m.bar)) ||
			                  (((c >> 8) == KEY_DOWN) && (!m.bar))) 
                  {
		               c = m.sel;
		               do
                     {
			               c++;
			               if (c >= m.size)
			                  c = 0;
		               } while ((!ugetc(m.menu[c].text)) && (c != m.sel));
		               m.sel = c;
		            }
		            break;

	            case KEY_SPACE:
	            case KEY_ENTER:
		            if (m.sel >= 0)
		               ret = m.sel;
		            break;

	            default:
		            if ((!m.parent) && ((c & 0xFF) == 0))
		               c = menu_alt_key(c, m.menu);
		            for (c2=0; m.menu[c2].text; c2++) 
                  {
		               if (menu_key_shortcut(c, m.menu[c2].text)) 
                     {
			               ret = m.sel = c2;
			               break;
		               }
		            }
		            if (m.parent) 
                  {
		               i = m.parent;
		               for (c2=0; i->parent; c2++)
			            i = i->parent;
		               c = menu_alt_key(c, i->menu);
		               if (c) 
                     {
			               while (c2-- > 0)
			                  simulate_keypress(27);
			               simulate_keypress(c);
			               ret = -1;
			               goto getout;
		               }
		            }
		            break;
	         }
	      }
      }

      if ((redraw) || (m.sel != old_sel))            /* selection changed? */
      {
	      scare_mouse();
	      acquire_screen();
   
	      if (redraw) {
	         draw_menu(&m);
	         redraw = FALSE;
	      }
	      else 
         {
	         if (old_sel >= 0)
	            draw_menu_item(&m, old_sel);

	         if (m.sel >= 0)
	            draw_menu_item(&m, m.sel);
	      }

	      release_screen();
	      unscare_mouse();
      }

      if ((ret >= 0) && (m.menu[ret].flags & D_DISABLED))
	      ret = -1;

      if (ret >= 0)                                   /* child menu? */
      {
	      if (m.menu[ret].child) 
         {
	         if (m.bar) 
            {
	            get_menu_pos(&m, ret, &_x, &_y, &c);
	            _x += 6;
	            _y += text_height(font)+7;
	         }
	         else 
            {
	            _x = m.x+m.w-3;
      	      _y = m.y + (text_height(font)+4)*ret + text_height(font)/4+1;
	         }
	         c = _do_menu(m.menu[ret].child, &m, FALSE, _x, _y, TRUE, NULL, 0, 0);
	         if (c < 0) 
            {
	            ret = -1;
	            mouse_on = FALSE;
	            mouse_sel = menu_mouse_object(&m);
	         }
	      }
      }

      if ((m.bar) && (!gui_mouse_b()) && (!keypressed()) &&
	       ((gui_mouse_x() < m.x) || (gui_mouse_x() > m.x+m.w) ||
	        (gui_mouse_y() < m.y) || (gui_mouse_y() > m.y+m.h)))
      {
	      break;
      }
   } while (ret < 0);

getout:

   if (dret)
      *dret = 0;

   /* callback function? */
   if ((!m.proc) && (ret >= 0)) 
   {
      active_menu = &m.menu[ret];
      m.proc = active_menu->proc;
   }

   if (ret >= 0) 
   {
      if (parent)
	      parent->proc = m.proc;
      else  
      {
	      if (m.proc) 
         {
	         c = m.proc();
	         if (dret)
	            *dret = c;
	      }
      }
   }

   /* restore screen */
   if (m.saved) 
   {
      scare_mouse();
      blit(m.saved, screen, 0, 0, m.x, m.y, m.w+2, m.h+2);
      unscare_mouse();
      destroy_bitmap(m.saved);
   }

   return ret;
}

static void draw_menu_item(MENU_INFO *m, int c)
{
   int fg, bg;
   int i, j, x, y, w;
   char buf[256], *tok;
   int my;
   //int rtm;
   FONT *fnt;

   if (m->menu[c].flags & D_DISABLED) 
   {
      if (c == m->sel) 
      {
	      fg = gray;//gui_mg_color;
	      bg = gray;//gui_fg_color;
      }
      else 
      {
	      fg = gray;//gui_mg_color;
	      bg = gray;//gui_bg_color;
      } 
   }
   else 
   {
      if (c == m->sel) 
      {
	      fg = white;//gui_bg_color;
	      bg = white;//gui_fg_color;
      }
      else 
      {
	      fg = white;//gui_fg_color;
	      bg = gray;//gui_bg_color;
      } 
   }

   fnt = font;
   if(m->bar) 
   {
      font = smallfont;
   }

   get_menu_pos(m, c, &x, &y, &w);

   if (ugetc(m->menu[c].text)) 
   {
      i = 0;
      j = ugetc(m->menu[c].text);

      while ((j) && (j != '\t')) 
      {
	      i += usetc(buf+i, j);
	      j = ugetc(m->menu[c].text+i);
      }

      usetc(buf+i, 0);

      gui_textout_highlight(screen, buf, x+8, y+1, fg, bg, black, FALSE, TEXT_OUTLINE_2, font);

      if (j == '\t') 
      {
	      tok = m->menu[c].text+i + uwidth(m->menu[c].text+i);
	      gui_textout_highlight(screen, tok, x+w-gui_strlen(tok)-10, y+1, fg, bg, black, FALSE, TEXT_OUTLINE_2, font);
      }

      if ((m->menu[c].child) && (!m->bar)) 
      {
         my = y + text_height(font)/2;
         hline(screen, x+w-8, my+1, x+w-4, fg);
         hline(screen, x+w-8, my+0, x+w-5, fg);
         hline(screen, x+w-8, my-1, x+w-6, fg);
         hline(screen, x+w-8, my-2, x+w-7, fg);
         putpixel(screen, x+w-8, my-3, fg);
         hline(screen, x+w-8, my+2, x+w-5, fg);
         hline(screen, x+w-8, my+3, x+w-6, fg);
         hline(screen, x+w-8, my+4, x+w-7, fg);
         putpixel(screen, x+w-8, my+5, fg);
      }
      
   }
   else 
   {
      hline(screen, x, y+text_height(font)/2+1, x+w, gui_col3/*bg*/);
      hline(screen, x, y+text_height(font)/2+2, x+w, gui_col1/*fg*/);
   }

   if (m->menu[c].flags & D_SELECTED) 
   {
      line(screen, x+1, y+text_height(font)/2+1, x+3, y+text_height(font)+1, fg);
      line(screen, x+3, y+text_height(font)+1, x+6, y+2, fg);
   }

   if(fnt) 
   {
     font = fnt;
   }
}

static int menu_key_shortcut(int c, AL_CONST char *s)
{
   int d;

   while ((d = ugetxc(&s)) != 0) 
   {
      if (d == '&') 
      {
	      d = ugetc(s);
	      if ((d != '&') && (utolower(d) == utolower(c & 0xFF)))
	         return TRUE;
      }
   }

   return FALSE;
}

int menu_alt_key(int k, MENU *m)
{
   static unsigned char alt_table[] =
   {
      KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, 
      KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, 
      KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z
   };

   AL_CONST char *s;
   int c, d;

   if (k & 0xFF)
      return 0;

   k >>= 8;

   c = scancode_to_ascii(k);
   if (c) 
   {
      k = c;
   } 
   else 
   {
      for (c = 0; c < (int)sizeof(alt_table); c++) 
      {
	      if (k == alt_table[c]) 
         {
	         k = c + 'a';
	         break;
	      }
      }

      if (c >= (int)sizeof(alt_table))
	      return 0;
   }

   for (c=0; m[c].text; c++) 
   {
      s = m[c].text;
      while ((d = ugetxc(&s)) != 0) 
      {
	      if (d == '&') 
         {
	         d = ugetc(s);
	         if ((d != '&') && (utolower(d) == utolower(k)))
	            return k;
	      }
      }
   }

   return 0;
}

static int menu_mouse_object(MENU_INFO *m)
{
   int c;
   int x, y, w;

   for (c=0; c<m->size; c++) 
   {
      get_menu_pos(m, c, &x, &y, &w);

      if ((gui_mouse_x() >= x) && (gui_mouse_x() < x+w) &&
	       (gui_mouse_y() >= y) && (gui_mouse_y() < y+(text_height(font)+4)))
      {
	      return (ugetc(m->menu[c].text)) ? c : -1;
      }
   }

   return -1;
}

static int mouse_in_parent_menu(MENU_INFO *m) 
{
   int c;

   if (!m)
      return FALSE;

   c = menu_mouse_object(m);
   if ((c >= 0) && (c != m->sel))
      return TRUE;

   return mouse_in_parent_menu(m->parent);
}

static void get_menu_pos(MENU_INFO *m, int c, int *x, int *y, int *w)
{
   int c2;
   FONT *tmp;

   if (m->bar) 
   {
      *x = m->x+1;

      tmp = font;
      font = smallfont;

      for (c2=0; c2<c; c2++)
	      *x += gui_strlen(m->menu[c2].text) + 16;

      *y = m->y+1;
      *w = gui_strlen(m->menu[c].text) + 16;

      font = tmp;
   }
   else 
   {
      *x = m->x+1;
      *y = m->y+c*(text_height(font)+4)+1;
      *w = m->w-2;
   }
}

#ifndef DLG_EDIT

static DIALOG alert_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)        (dp)        (dp2) (dp3) */
   { box,               0,    0,    0,    0,    0,    0,    0,    0,       0,    0,          NULL,       NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,    0,    TRUE,TEXT_OUTLINE_2, NULL, NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,    0,    TRUE,TEXT_OUTLINE_2, NULL, NULL, NULL  },
   { label,             0,    0,    0,    0,    0,    0,    0,    0,    TRUE,TEXT_OUTLINE_2, NULL, NULL, NULL  },
   { button,            0,    0,    0,    0,    0,    0,    0,    D_EXIT,  0,             0, NULL, NULL, NULL  },
   { button,            0,    0,    0,    0,    0,    0,    0,    D_EXIT,  0,             0, NULL, NULL, NULL  },
   { button,            0,    0,    0,    0,    0,    0,    0,    D_EXIT,  0,             0, NULL, NULL, NULL  },
   { d_yield_proc,      0,    0,    0,    0,    0,    0,    0,    0,       0,             0, NULL, NULL, NULL  },
   { NULL,              0,    0,    0,    0,    0,    0,    0,    0,       0,             0, NULL, NULL, NULL  }
};


#define A_S1  1
#define A_S2  2
#define A_S3  3
#define A_B1  4
#define A_B2  5
#define A_B3  6



/* alert3:
 *  Displays a simple alert box, containing three lines of text (s1-s3),
 *  and with either one, two, or three buttons. The text for these buttons 
 *  is passed in b1, b2, and b3 (NULL for buttons which are not used), and
 *  the keyboard shortcuts in c1 and c2. Returns 1, 2, or 3 depending on 
 *  which button was selected.
 */
int ated_alert3(AL_CONST char *s1, AL_CONST char *s2, AL_CONST char *s3, AL_CONST char *b1, AL_CONST char *b2, AL_CONST char *b3, int c1, int c2, int c3)
{
   char tmp[16];
   int avg_w, avg_h;
   int len1, len2, len3;
   int maxlen = 0;
   int buttons = 0;
   int b[3];
   int c;

   #define SORT_OUT_BUTTON(x) {                                            \
      if (b##x) {                                                          \
	      alert_dialog[A_B##x].flags &= ~D_HIDDEN;                          \
	      alert_dialog[A_B##x].key = c##x;                                  \
	      alert_dialog[A_B##x].dp = (char *)b##x;                           \
	      len##x = gui_strlen(b##x);                                        \
	      b[buttons++] = A_B##x;                                            \
      }                                                                    \
      else {                                                               \
	      alert_dialog[A_B##x].flags |= D_HIDDEN;                           \
	      len##x = 0;                                                       \
      }                                                                    \
   }

   usetc(tmp+usetc(tmp, ' '), 0);

   avg_w = text_length(font, tmp);
   avg_h = text_height(font);

   alert_dialog[A_S1].dp = alert_dialog[A_S2].dp = alert_dialog[A_S3].dp = 
   alert_dialog[A_B1].dp = alert_dialog[A_B2].dp = empty_string;

   if (s1) 
   {
      alert_dialog[A_S1].dp = (char *)s1;
      maxlen = text_length(font, s1);
   }

   if (s2) 
   {
      alert_dialog[A_S2].dp = (char *)s2;
      len1 = text_length(font, s2);
      if (len1 > maxlen)
	      maxlen = len1;
   }

   if (s3) {
      alert_dialog[A_S3].dp = (char *)s3;
      len1 = text_length(font, s3);
      if (len1 > maxlen)
	      maxlen = len1;
   }

   SORT_OUT_BUTTON(1);
   SORT_OUT_BUTTON(2);
   SORT_OUT_BUTTON(3);

   len1 = MAX(len1, MAX(len2, len3)) + avg_w*3;
   if (len1*buttons > maxlen)
      maxlen = len1*buttons;

   maxlen += avg_w*4;
   alert_dialog[0].w = maxlen;
   alert_dialog[A_S1].x = alert_dialog[A_S2].x = alert_dialog[A_S3].x = 
						alert_dialog[0].x + maxlen/2;

   alert_dialog[A_B1].w = alert_dialog[A_B2].w = alert_dialog[A_B3].w = len1;

   alert_dialog[A_B1].x = alert_dialog[A_B2].x = alert_dialog[A_B3].x = 
				       alert_dialog[0].x + maxlen/2 - len1/2;

   if (buttons == 3) 
   {
      alert_dialog[b[0]].x = alert_dialog[0].x + maxlen/2 - len1*3/2 - avg_w;
      alert_dialog[b[2]].x = alert_dialog[0].x + maxlen/2 + len1/2 + avg_w;
   }
   else if (buttons == 2) 
   {
      alert_dialog[b[0]].x = alert_dialog[0].x + maxlen/2 - len1 - avg_w;
      alert_dialog[b[1]].x = alert_dialog[0].x + maxlen/2 + avg_w;
   }

   alert_dialog[0].h = avg_h*8;
   alert_dialog[A_S1].y = alert_dialog[0].y + avg_h;
   alert_dialog[A_S2].y = alert_dialog[0].y + avg_h*2;
   alert_dialog[A_S3].y = alert_dialog[0].y + avg_h*3;
   alert_dialog[A_S1].h = alert_dialog[A_S2].h = alert_dialog[A_S3].h = avg_h;
   alert_dialog[A_B1].y = alert_dialog[A_B2].y = alert_dialog[A_B3].y = alert_dialog[0].y + avg_h*5;
   alert_dialog[A_B1].h = alert_dialog[A_B2].h = alert_dialog[A_B3].h = avg_h*2;

   centre_dialog(alert_dialog);
   //set_dialog_color(alert_dialog, gui_fg_color, gui_bg_color);

   clear_keybuf();

   alert_dialog[A_S1].bg = alert_dialog[A_S2].bg = alert_dialog[A_S3].bg = white;
   alert_dialog[A_S1].fg = alert_dialog[A_S2].fg = alert_dialog[A_S3].fg = gray;

   alert_dialog[A_B1].fg = alert_dialog[A_B2].fg = alert_dialog[A_B3].fg = white;
   alert_dialog[A_B1].bg = alert_dialog[A_B2].bg = alert_dialog[A_B3].bg = gray;

   do {
   } while (gui_mouse_b());

   c = popup_dialog(alert_dialog, A_B1);

   wait_for_keys();

   if (c == A_B1)
      return 1;
   else if (c == A_B2)
      return 2;
   else
      return 3;
}


/* alert:
 *  Displays a simple alert box, containing three lines of text (s1-s3),
 *  and with either one or two buttons. The text for these buttons is passed
 *  in b1 and b2 (b2 may be null), and the keyboard shortcuts in c1 and c2.
 *  Returns 1 or 2 depending on which button was selected.
 */
int ated_alert(AL_CONST char *s1, AL_CONST char *s2, AL_CONST char *s3, AL_CONST char *b1, AL_CONST char *b2, int c1, int c2)
{
   int ret;

   ret = ated_alert3(s1, s2, s3, b1, b2, NULL, c1, c2, 0);

   if (ret > 2)
      ret = 2;

   return ret;
}

#endif // DLG_EDIT

typedef char *(*getfuncptr)(int, int *);

int listbox(int msg, DIALOG *d, int c)
{
   int listsize, i, bottom, height, bar, orig;
   char *sel = d->dp2;
   int redraw = FALSE;

   switch (msg) 
   {
      case MSG_START:
	      (*(getfuncptr)d->dp)(-1, &listsize);
	      _handle_scrollable_scroll(d, listsize, &d->d1, &d->d2);
         d->w -= 2;
         d->x += 2;
         if(!d->fg)
           d->fg = black;
         if(!d->bg)
           d->bg = white;
	      break;

      case MSG_END:
         d->w += 2;
         d->x -= 2;
         break;

      case MSG_DRAW:
         draw_ated_listbox(d);
	      break;

      case MSG_CLICK:
	      (*(getfuncptr)d->dp)(-1, &listsize);
	      height = (d->h-4) / text_height(font);
   	   bar = (listsize > height);
	      if ((!bar) || (gui_mouse_x() < d->x+d->w-13)) 
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
		            scare_mouse();
		            SEND_MESSAGE(d, MSG_DRAW, 0);
		            unscare_mouse();
	            }
	         }
	         _handle_listbox_click(d);
	         while (gui_mouse_b()) 
            {
	            broadcast_dialog_message(MSG_IDLE, 0);
	            d->flags |= D_INTERNAL;
	            _handle_listbox_click(d);
	            d->flags &= ~D_INTERNAL;
	         }
         }
	      else 
         {
	         _handle_scrollable_scroll_click(d, listsize, &d->d2, height);
	      }
	      break;

      case MSG_DCLICK:
	      (*(getfuncptr)d->dp)(-1, &listsize);
	      height = (d->h-4) / text_height(font);
	      bar = (listsize > height);
	      if ((!bar) || (gui_mouse_x() < d->x+d->w-13)) 
         {
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
	            scare_mouse();
	            SEND_MESSAGE(d, MSG_DRAW, 0);
	            unscare_mouse(); 
	         }
	      }
	      break;

      case MSG_KEY:
	      (*(getfuncptr)d->dp)(-1, &listsize);
	      if ((listsize) && (d->flags & D_EXIT))
	         return D_CLOSE;
	      break;

      case MSG_WANTFOCUS:
	      return D_WANTFOCUS;

      case MSG_CHAR:
	      (*(getfuncptr)d->dp)(-1, &listsize);

	      if (listsize) 
         {
	         c >>= 8;

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

	         /* if we changed something, better redraw... */ 
	         _handle_scrollable_scroll(d, listsize, &d->d1, &d->d2);
	         d->flags |= D_DIRTY;
	         return D_USED_CHAR;
	      }
	      break;
   }

   return D_O_K;
}

void draw_ated_listbox(DIALOG *d)
{
   int height, listsize, i, len, bar, x, y, w;
   int fg_color, fg, bg;
   char *sel = d->dp2;
   char s[1024];
   int rtm;

   (*(getfuncptr)d->dp)(-1, &listsize);
   height = (d->h-4) / text_height(font);
   bar = (listsize > height);
   w = (bar ? d->w-15 : d->w-3);
   fg_color = (d->flags & D_DISABLED) ? gui_col2 : d->fg;

   /* draw box contents */
   for (i=0; i<height; i++) 
   {
      if (d->d2+i < listsize) 
      {
	      if (d->d2+i == d->d1) 
         {
	         fg = d->bg;
	         bg = fg_color;
	      } 
	      else if ((sel) && (sel[d->d2+i])) 
         {            
	         fg = d->bg;
	         bg = gui_mg_color;
	      }
	      else 
         {
	         fg = fg_color;
	         bg = d->bg;
	      }
	      usetc(s, 0);
	      ustrncat(s, (*(getfuncptr)d->dp)(i+d->d2, NULL), sizeof(s)-ucwidth(0));
	      x = d->x + 2;
	      y = d->y + 2 + i*text_height(font);
	      rtm = text_mode(bg);
	      rectfill(screen, x, y, x+7, y+text_height(font)-1, bg); 
	      x += 8;
	      len = ustrlen(s);
	      while (text_length(font, s) >= MAX(d->w - 1 - (bar ? 22 : 10), 1)) 
         {
	         len--;
	         usetat(s, len, 0);
	      }
	      textout(screen, font, s, x, y, fg);
	      text_mode(rtm);
	      x += text_length(font, s);
	      if (x <= d->x+w) 
	         rectfill(screen, x, y, d->x+w, y+text_height(font)-1, bg);
      }
      else 
      {
	      rectfill(screen, d->x+2,  d->y+2+i*text_height(font), 
		            d->x+w, d->y+1+(i+1)*text_height(font), d->bg);
      }
   }

   if (d->y+2+i*text_height(font) <= d->y+d->h-3)
      rectfill(screen, d->x+2, d->y+2+i*text_height(font), d->x+w, d->y+d->h-3, d->bg);

   /* draw frame, maybe with scrollbar */
   draw_scrollable_ated_frame(d, listsize, d->d2, height, fg_color, d->bg);
}

void draw_scrollable_ated_frame(DIALOG *d, int listsize, int offset, int height, int fg_color, int bg)
{
   int i, len;
   BITMAP *pattern;
   int xx, yy;

   hline(screen, d->x - 1,        d->y - 1,        d->x + d->w - 1, black);
   vline(screen, d->x - 1,        d->y - 1,        d->y + d->h,     black);
   hline(screen, d->x,            d->y,            d->x + d->w - 1, gui_col3);
   vline(screen, d->x,            d->y,            d->y + d->h,     gui_col3);
   hline(screen, d->x - 1,        d->y + d->h - 1, d->x + d->w - 1, gui_col1);
   vline(screen, d->x + d->w - 1, d->y - 1,        d->y + d->h - 1, gui_col1);

   /* possibly draw scrollbar */
   if (listsize > height) 
   {
      vline(screen, d->x+d->w-13, d->y+1, d->y+d->h-2, fg_color);

      /* scrollbar with focus */ 
	   rect(screen, d->x+1, d->y+1, d->x+d->w-14, d->y+d->h-2, bg);
	   rect(screen, d->x+d->w-12, d->y+1, d->x+d->w-2, d->y+d->h-2, bg);

      /* create and draw the scrollbar */
      if(!(pattern = create_bitmap(2, 2)))
         fat_error("Out of memory!");
      i = ((d->h-5) * height + listsize/2) / listsize;
      xx = d->x+d->w-11;
      yy = d->y+2;

      putpixel(pattern, 0, 1, bg);
      putpixel(pattern, 1, 0, bg);
      putpixel(pattern, 0, 0, fg_color);
      putpixel(pattern, 1, 1, fg_color);

      if (offset > 0) 
      {
         len = (((d->h-5) * offset) + listsize/2) / listsize;
	      rectfill(screen, xx, yy, xx+8, yy+len, bg);
	      yy += len;
      }
      if (yy+i < d->y+d->h-3) 
      {
	      drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);
	      rectfill(screen, xx, yy, xx+8, yy+i, 0);
	      solid_mode();
	      yy += i+1;
	      rectfill(screen, xx, yy, xx+8, d->y+d->h-3, bg);
      }
      else 
      {
	      drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);
	      rectfill(screen, xx, yy, xx+8, d->y+d->h-3, 0);
	      solid_mode();
      }
      destroy_bitmap(pattern);
   }
   else 
   {
      /* no scrollbar necessary */
	   rect(screen, d->x+1, d->y+1, d->x+d->w-2, d->y+d->h-2, bg);
   }
}

int checkbox(int msg, DIALOG *d, int c) 
{
   int /*b, */col1, col2;
   //BITMAP *bmp;

   switch(msg) 
   {
      case MSG_START:
         if(!d->h)
            d->h = 12;//d->h = text_height(font);
         if(!d->w)
            d->w = 1.5*d->h + text_length(font, d->dp);

         if(!d->fg)
            d->fg = white;
         if(!d->bg)
            d->bg = gray;

         //d->dp2 = NULL;
         break;

      case MSG_END:
         if(d->dp2)
            destroy_bitmap(d->dp2);
         d->dp2 = NULL;
         break;

      case MSG_DRAW:
         rectfill(screen, d->x,            d->y,            d->x + d->h, d->y + d->h, gui_col4);
         hline   (screen, d->x,            d->y,            d->x + d->h - 1,          gui_col3);
         vline   (screen, d->x,            d->y,            d->y + d->h - 1,          gui_col3);
         hline   (screen, d->x + 1,        d->y + 1,        d->x + d->h - 2,          black);
         vline   (screen, d->x + 1,        d->y + 1,        d->y + d->h - 2,          black);
         hline   (screen, d->x,            d->y + d->h,     d->x + d->h,              gui_col1);
         vline   (screen, d->x + d->h,     d->y,            d->y + d->h,              gui_col1);
         hline   (screen, d->x - 1,        d->y + d->h - 1, d->x + d->h - 1,          gui_col2);
         vline   (screen, d->x + d->h - 1, d->y - 1,        d->y + d->h - 1,          gui_col2);
         if(d->flags & D_SELECTED) 
         {
            draw_sprite(screen, ateddata[CHECK].dat, d->x + 2, d->y + 2);
         }
         if(d->flags & D_GOTFOCUS) 
         {
            col1 = d->fg;
            col2 = d->fg;
         }
         else 
         {
            col1 = d->fg;
            col2 = d->bg;
         }
         gui_textout_highlight(screen, d->dp, d->x + 15, d->y + 6 - (text_height(font) >> 1), col1, col2, black, FALSE, TEXT_OUTLINE_2, font);
         return D_O_K;
   }

   return button(msg, d, 0);
}

extern DIALOG tile_sel[];

#ifndef DLG_EDIT

extern int tile_select_w;
extern int current_tile_select_w;

#define TILE_TYPE_TILE                  1
#define TILE_TYPE_TILE_MASKED           2
#define TILE_TYPE_TILE_ICON             3

int type, max_rows;

int tile_select(int msg, DIALOG *d, int c) 
{
   int i, mx, my, x, y/*, w, tile*/;
   //unsigned loc;
   BITMAP *bmp;


   switch(msg) 
   {
      case MSG_START:
      case MSG_UPDATE_INFO:
         if(tile_sel[12].flags & D_SELECTED) 
         {
            type = TILE_TYPE_TILE;
         } 
         else if (tile_sel[13].flags & D_SELECTED) 
         {
            type = TILE_TYPE_TILE_MASKED;
         }
         else 
         {
            type = TILE_TYPE_TILE_ICON;
         }

         switch(type) 
         {
            case TILE_TYPE_TILE:
               if((d->d1 * d->d2) > gfx_t_count) 
               {
                  max_rows = 0;
               }
               else 
               {
                  for(i = 0; i < gfx_t_count; i += d->d1)
                     ;
                  i /= d->d1;
                  max_rows = i - d->d2;
               }
               break;
            case TILE_TYPE_TILE_MASKED:
               if((d->d1 * d->d2) > gfx_tm_count) 
               {
                  max_rows = 0;
               }
               else
               {
                  for(i = 0; i < gfx_tm_count; i += d->d1)
                     ;
                  i /= d->d1;
                  max_rows = i - d->d2;
               }
               break;
            case TILE_TYPE_TILE_ICON:
               if((d->d1 * d->d2) > gfx_i_count) 
               {
                  max_rows = 0;
               }
               else
               {
                  for(i = 0; i < gfx_i_count; i += d->d1)
                     ;
                  i /= d->d1;
                     max_rows = i - d->d2;
               }
               break;
         }

         //
         //
         //

         for (i = current_tile_select_w; i != tile_select_w && i > 0; i -= tsize)
            ;

         if (i <= 0)
            tile_select_w = current_tile_select_w;  // tile_select_w was calculated to
                                                    // different tsize
      
         if (tile_select_w < tsize)
            tile_select_w = tsize;
         if (tile_select_w > current_tile_select_w)
            tile_select_w = current_tile_select_w;

         d->w  = tile_select_w;
         d->d1 = d->w / tsize;
         d->x = (SCREEN_W - d->w) / 2;
         break;
    
      case MSG_END:
         if(SelectMode) 
         {
            SelectMode = FALSE;

            SelX1 = SelY1 = SelX2 = SelY2 = -1;
         }
         break;

      case MSG_CLICK:
         mx = mouse_x;
         my = mouse_y;
         //
         // TODO: Optimize following code
         //
         //w = d->w / tsize;

         for(x = 0; x < (mx - d->x); x += tsize);
         x /= tsize;
         if(x)
            x--;
         for(y = 0; y < (my - d->y); y += tsize);
            y /= tsize;
         if(y)
            y--;

          //loc = ((ybase + y)*mapwidth) + (xbase + x);
         if(SelectMode) 
         {
            mx = x;
            my = y;

            switch(type) 
            {
               case TILE_TYPE_TILE:
                  my += tilebase;
                  break;

               case TILE_TYPE_TILE_MASKED:
                  my += tilembase;
                  break;

               case TILE_TYPE_TILE_ICON:
                  my += tileibase;
                  break;
            }

            if(mouse_b & 1) 
            {
               SelX1 = mx;
               SelY1 = my;

               if(((SelX2 == -1) && (SelY2 == -1)) || ((SelX1 > SelX2) || (SelY1 > SelY2))) 
               {
                  SelX2 = mx;
                  SelY2 = my;
               }
            }

            if(mouse_b & 2) 
            {
               SelX2 = mx;
               SelY2 = my;

               if(((SelX1 == -1) && (SelY1 == -1)) || ((SelX1 > SelX2) || (SelY1 > SelY2))) 
               {
                  SelX1 = mx;
                  SelY1 = my;
               }
            }

            SEND_MESSAGE(d, MSG_DRAW, 0);
         }
         else
         {
            switch(type) 
            {
               case TILE_TYPE_TILE:
                  if((((tilebase + y)*d->d1) + x) < gfx_t_count)
                     which_t = ((tilebase + y)*d->d1) + x;
                  break;
                  
               case TILE_TYPE_TILE_MASKED:
                  if((((tilembase + y)*d->d1) + x) < gfx_tm_count)
                     which_tm = ((tilembase + y)*d->d1) + x;
                  break;
                  
               case TILE_TYPE_TILE_ICON:
                  if((((tileibase + y)*d->d1) + x) < gfx_i_count)
                     which_i = ((tileibase + y)*d->d1) + x;
                  break;
            }
         }

         broadcast_dialog_message(MSG_UPDATE_INFO, 0);
         //return D_REDRAW;
         break;

      case MSG_XCHAR:
         switch (c >> 8)
         {
            case KEY_MINUS:
            case KEY_MINUS_PAD:
               tile_select_w -= tsize;
               if (tile_select_w < tsize)
                  tile_select_w = tsize;

               d->w  = tile_select_w;
               d->d1 = d->w / tsize;
               d->x = (SCREEN_W - d->w) / 2;

               broadcast_dialog_message(MSG_DRAW, 0);
               SEND_MESSAGE(d, MSG_START, 0);
               break;

            case KEY_EQUALS:
            case KEY_PLUS_PAD:
               tile_select_w += tsize;
               if (tile_select_w > current_tile_select_w)
                  tile_select_w = current_tile_select_w;

               d->w = tile_select_w;
               d->d1 = d->w / tsize;
               d->x = (SCREEN_W - d->w) / 2;

               SEND_MESSAGE(d, MSG_DRAW, 0);
               SEND_MESSAGE(d, MSG_START, 0);
               break;
               
            case KEY_C:
               SelectMode ^= 1;
               SelX1 = SelY1 = SelX2 = SelY2 = -1;

               SEND_MESSAGE(d, MSG_DRAW, 0);
               break;
               
            case KEY_ENTER:
               if(SelectMode)
               {
                  SelectMode = FALSE;

                  if((SelX1 != -1) && (SelY1 != -1) && (SelX2 != -1) && (SelY2 != -1))
                  {
                     TileCopy.MapOrTileSelect = type;
                     TileCopy.back            = (type == TILE_TYPE_TILE);
                     TileCopy.fore            = (type == TILE_TYPE_TILE_MASKED);
                     TileCopy.info            = (type == TILE_TYPE_TILE_ICON);
                     TileCopy.tile_select_w   = d->d1;
                     TileCopy.tile_select_h   = (max_rows < d->d2) ? max_rows : (max_rows + d->d2);
                     TileCopy.x = SelX1;
                     TileCopy.y = SelY1;
                     TileCopy.w = SelX2 - SelX1 + 1;
                     TileCopy.h = SelY2 - SelY1 + 1;

                     SelX1 = SelY1 = SelX2 = SelY2 = -1;
                  }

                  SEND_MESSAGE(d, MSG_DRAW, 0);
               }
               break;
               
            case KEY_SPACE:
               while(key[KEY_SPACE]);
               return D_EXIT;
         }
         break;
         
      case MSG_IDLE:
         if(key[KEY_LSHIFT] || key[KEY_RSHIFT]) // One-line scrolling(PAUSED)
         {
            if(key[KEY_UP]) 
            {
               switch(type) 
               {
                  case TILE_TYPE_TILE:
                     tilebase =  (--tilebase < 0)  ? 0 : tilebase;
                     break;
                  case TILE_TYPE_TILE_MASKED:
                     tilembase = (--tilembase < 0) ? 0 : tilembase;
                     break;
                  case TILE_TYPE_TILE_ICON:
                     tileibase = (--tileibase < 0) ? 0 : tileibase;
                     break;
               }
               SEND_MESSAGE(d, MSG_DRAW, 0);
               while(key[KEY_UP])
                  ;
            }
            if(key[KEY_DOWN]) 
            {
               switch(type) 
               {
                  case TILE_TYPE_TILE:
                     tilebase =  (++tilebase > max_rows)  ? max_rows : tilebase;
                     break;
                  case TILE_TYPE_TILE_MASKED:
                     tilembase = (++tilembase > max_rows) ? max_rows : tilembase;
                     break;
                  case TILE_TYPE_TILE_ICON:
                     tileibase = (++tileibase > max_rows) ? max_rows : tileibase;
                     break;
               }
               SEND_MESSAGE(d, MSG_DRAW, 0);
               while(key[KEY_DOWN])
                  ;
            }
         }
         else if(key[KEY_LCONTROL] || key[KEY_RCONTROL]) // Screen scrolling
         {
            if(key[KEY_UP]) 
            {
               switch(type) 
               {
                  case TILE_TYPE_TILE:
                     tilebase =  ((tilebase -= d->d2) < 0)  ? 0 : tilebase;
                     break;
                  case TILE_TYPE_TILE_MASKED:
                     tilembase = ((tilembase -= d->d2) < 0) ? 0 : tilembase;
                     break;
                  case TILE_TYPE_TILE_ICON:
                     tileibase = ((tileibase -= d->d2) < 0) ? 0 : tileibase;
                     break;
               }
               SEND_MESSAGE(d, MSG_DRAW, 0);
               while(key[KEY_UP])
                  ;
            }
            if(key[KEY_DOWN]) 
            {
               switch(type) 
               {
                  case TILE_TYPE_TILE:
                     tilebase =  ((tilebase += d->d2) > max_rows)  ? max_rows : tilebase;
                     break;
                  case TILE_TYPE_TILE_MASKED:
                     tilembase = ((tilembase += d->d2) > max_rows) ? max_rows : tilembase;
                     break;
                  case TILE_TYPE_TILE_ICON:
                     tileibase = ((tileibase += d->d2) > max_rows) ? max_rows : tileibase;
                     break;
               }
               SEND_MESSAGE(d, MSG_DRAW, 0);
               while(key[KEY_DOWN])
                  ;
            }
         }
         else  // One-line scrolling
         {
            if(key[KEY_UP]) 
            {
               switch(type) 
               {
                  case TILE_TYPE_TILE:
                     tilebase =  (--tilebase < 0)  ? 0 : tilebase;
                     break;
                  case TILE_TYPE_TILE_MASKED:
                     tilembase = (--tilembase < 0) ? 0 : tilembase;
                     break;
                  case TILE_TYPE_TILE_ICON:
                     tileibase = (--tileibase < 0) ? 0 : tileibase;
                     break;
               }
               SEND_MESSAGE(d, MSG_DRAW, 0);
            }
            if(key[KEY_DOWN]) 
            {
               switch(type) 
               {
                  case TILE_TYPE_TILE:
                     tilebase =  (++tilebase > max_rows)  ? max_rows : tilebase;
                     break;
                  case TILE_TYPE_TILE_MASKED:
                     tilembase = (++tilembase > max_rows) ? max_rows : tilembase;
                     break;
                  case TILE_TYPE_TILE_ICON:
                     tileibase = (++tileibase > max_rows) ? max_rows : tileibase;
                     break;
               }
               SEND_MESSAGE(d, MSG_DRAW, 0);
            }
         }
         if(key[KEY_PGUP]) // Screen scrolling
         {
            switch(type) 
            {
               case TILE_TYPE_TILE:
                  tilebase =  ((tilebase -= d->d2) < 0)  ? 0 : tilebase;
                  break;
               case TILE_TYPE_TILE_MASKED:
                  tilembase = ((tilembase -= d->d2) < 0) ? 0 : tilembase;
                  break;
               case TILE_TYPE_TILE_ICON:
                  tileibase = ((tileibase -= d->d2) < 0) ? 0 : tileibase;
                  break;
            }
            SEND_MESSAGE(d, MSG_DRAW, 0);
            while(key[KEY_PGUP]);
         }
         if(key[KEY_PGDN]) 
         {
            switch(type) 
            {
               case TILE_TYPE_TILE:
                  tilebase =  ((tilebase += d->d2) > max_rows)  ? max_rows : tilebase;
                  break;
               case TILE_TYPE_TILE_MASKED:
                  tilembase = ((tilembase += d->d2) > max_rows) ? max_rows : tilembase;
                  break;
               case TILE_TYPE_TILE_ICON:
                  tileibase = ((tileibase += d->d2) > max_rows) ? max_rows : tileibase;
                  break;
            }
            SEND_MESSAGE(d, MSG_DRAW, 0);
            while(key[KEY_PGDN]);
         }
         if(key[KEY_HOME]) // Begining
         {
            switch(type) 
            {
               case TILE_TYPE_TILE:
                  tilebase =  0;
                  break;
               case TILE_TYPE_TILE_MASKED:
                  tilembase = 0;
                  break;
               case TILE_TYPE_TILE_ICON:
                  tileibase = 0;
                  break;
            }
            SEND_MESSAGE(d, MSG_DRAW, 0);
            while(key[KEY_HOME]);
         }
         if(key[KEY_END])  // Ending
         {                                       
            switch(type) 
            {
               case TILE_TYPE_TILE:
                  tilebase =  max_rows;
                  break;
               case TILE_TYPE_TILE_MASKED:
                  tilembase = max_rows;
                  break;
               case TILE_TYPE_TILE_ICON:
                  tileibase = max_rows;
                  break;
            }
            SEND_MESSAGE(d, MSG_DRAW, 0);
            while(key[KEY_END]);
         }
         break;

      case MSG_DRAW:
         if(1) 
         {
            int len = strlen("Copy Mode") * 8;
            int cx = (SCREEN_W - len) / 2;

            if(SelectMode) 
            {
               print(screen, "Copy Mode", cx, 3);
            }
            else 
            {
               drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);
               rectfill(screen, cx-1, 1, cx + len + 1, 3 + 7, gray);
               drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
            }
         }

         if(!(bmp = create_bitmap(d->w+3, d->h+3)))
         {
            fat_error("Out of memory!");
         }

         clear_to_color (bmp, BkgndColor);
         hline (bmp, 0, 0,          bmp->w, black);
         vline (bmp, 0, 0,          bmp->h, black);
         hline (bmp, 1, 1,          bmp->w, gui_col3);
         vline (bmp, 1, 1,          bmp->h, gui_col3);
         hline (bmp, 0, bmp->h,     bmp->w, gui_col1);
         vline (bmp, bmp->w + 1, 0, bmp->h, gui_col1);
         
         x = 2;
         y = 2;

         switch(type) 
         {
            case TILE_TYPE_TILE:
               for(i = (tilebase * d->d1); i < gfx_t_count; i++) 
               {
                  draw_rle_sprite(bmp, gfx_t[i].dat, x, y);
                  if(GridMode) 
                  {
                     hline(bmp, x, y, x + tsize, white);
                     vline(bmp, x, y, y + tsize, white);
                  }
                  x += tsize;

                  if((x + tsize) > bmp->w)
                  {
                     x = 2;
                     y += tsize;
                     if((y + tsize) > bmp->h)
                     {
                        break;
                     }
                  }
               }
               break;
               
            case TILE_TYPE_TILE_MASKED:
               for(i = (tilembase * d->d1); i < gfx_tm_count; i++) 
               {
                  draw_rle_sprite(bmp, gfx_tm[i].dat, x, y);
                  if(GridMode) 
                  {
                     hline(bmp, x, y, x + tsize, white);
                     vline(bmp, x, y, y + tsize, white);
                  }
                  x += tsize;

                  if((x + tsize) > bmp->w)
                  {
                     x = 2;
                     y += tsize;
                     if((y + tsize) > bmp->h)
                     {
                        break;
                     }
                  }
               }
               break;
               
            case TILE_TYPE_TILE_ICON:
               for(i = (tileibase * d->d1); i < gfx_i_count; i++) 
               {
                  draw_rle_sprite(bmp, gfx_i[i].dat, x, y);
                  if(GridMode) 
                  {
                     hline(bmp, x, y, x + tsize, white);
                     vline(bmp, x, y, y + tsize, white);
                  }
                  x += tsize;

                  if((x + tsize) > bmp->w)
                  {
                     x = 2;
                     y += tsize;
                     if((y + tsize) > bmp->h)
                     {
                        break;
                     }
                  }
               }
               break;
         }

         if(SelectMode) 
         {
            int y1, y2;

            switch(type) 
            {
               case TILE_TYPE_TILE:
                  y1 = SelY1 - tilebase;
                  y2 = SelY2 - tilebase;
                  break;

               case TILE_TYPE_TILE_MASKED:
                  y1 = SelY1 - tilembase;
                  y2 = SelY2 - tilembase;
                  break;

               case TILE_TYPE_TILE_ICON:
                  y1 = SelY1 - tileibase;
                  y2 = SelY2 - tileibase;
                  break;
            }

            rect (bmp, (SelX1*tsize),     (y1*tsize),     (SelX2*tsize) + tsize,     (y2*tsize) + tsize,     black);
            rect (bmp, (SelX1*tsize) + 1, (y1*tsize) + 1, (SelX2*tsize) + tsize - 1, (y2*tsize) + tsize - 1, gray);
            rect (bmp, (SelX1*tsize) + 2, (y1*tsize) + 2, (SelX2*tsize) + tsize - 2, (y2*tsize) + tsize - 2, white);
            rect (bmp, (SelX1*tsize) + 3, (y1*tsize) + 3, (SelX2*tsize) + tsize - 3, (y2*tsize) + tsize - 3, gray);
            rect (bmp, (SelX1*tsize) + 4, (y1*tsize) + 4, (SelX2*tsize) + tsize - 4, (y2*tsize) + tsize - 4, black);
         }

         scare_mouse();
         //vsync();
         blit (bmp, screen, 0, 0, d->x - 1, d->y - 1, bmp->w, bmp->h);
         unscare_mouse();

         destroy_bitmap(bmp);
         break;
   }

   return D_O_K;
}

#endif // DLG_EDIT

int slider(int msg, DIALOG *d, int c) 
{
   //int offset;
   int retval = D_O_K;
   int mx, my, old_x;
   int (*proc)(void *cbpointer, int d2value);

   switch(msg) 
   {
      case MSG_DRAW:
         mx = d->x + (d->w * d->d2 / d->d1);
         my = d->y + (d->h >> 1);

         drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);
         rectfill(screen, d->x-3, d->y, d->x + d->w+3, d->y + d->h, gray);
         drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

         hline(screen, d->x,        my,     d->x + d->w, gui_col3);
         hline(screen, d->x,        my + 1, d->x + d->w, black);
         vline(screen, d->x,        my,     my + 2,      gui_col3);
         hline(screen, d->x,        my + 2, d->x + d->w, gui_col1);
         vline(screen, d->x + d->w, my,     my + 2,      gui_col1);

         drawing_mode(DRAW_MODE_COPY_PATTERN, skin, 0, 0);
         rectfill(screen, mx - 3, my - 4, mx + 3, my + 6, gray);
         drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

         hline(screen, mx - 3, my + 6, mx + 3, black);// pure black
         vline(screen, mx + 3, my - 4, my + 6, black);// pure black
         hline(screen, mx - 2, my + 5, mx + 2, gui_col3);
         vline(screen, mx + 2, my - 3, my + 5, gui_col3);
         hline(screen, mx - 3, my - 4, mx + 2, gui_col1);
         vline(screen, mx - 3, my - 4, my + 5, gui_col1);
         break;

      case MSG_CLICK:
         mx = -1;
         old_x = d->d2;

         while(mouse_b) 
         {
            d->d2 = (mouse_x - d->x) * d->d1 / (d->w - 2);
            d->d2 = MIN(d->d2, d->d1);
            if(d->d2 < 0)
               d->d2 = 0;

            if(mx != mouse_x) 
            {
               mx = mouse_x;
   
               SEND_MESSAGE(d, MSG_UPDATE, 0);
            }

            if(key[KEY_ESC]) 
            {
               d->d2 = old_x;
   
               SEND_MESSAGE(d, MSG_UPDATE, 0);
               while(key[KEY_ESC] || mouse_b);
            }
         }
         break;

      case MSG_UPDATE:
         if(d->dp2 != NULL) 
         {
            proc = d->dp2;
            retval |= (*proc)(d->dp3, d->d2);
         }
         scare_mouse();
         SEND_MESSAGE(d, MSG_DRAW, 0);
         unscare_mouse();
         break;

      case MSG_CHAR:
         switch(c >> 8) 
         {
            case KEY_LEFT:
               d->d2 = (--d->d2 < 0) ? 0 : d->d2;
               retval = D_USED_CHAR;
               break;
   
            case KEY_RIGHT:
               d->d2 = (++d->d2 > d->d1) ? d->d1 : d->d2;
               retval = D_USED_CHAR;
               break;
         }
         if(retval == D_USED_CHAR)
            SEND_MESSAGE(d, MSG_DRAW, 0);   
         return retval;

      case MSG_WANTFOCUS:
         return D_WANTFOCUS;
   }

   return retval;
}

/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*-----------------------------------  GFX MODE SEL  -------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/

typedef struct GFX_MODE_DATA
{
   int w, h;
   AL_CONST char *s;
} GFX_MODE_DATA;



static GFX_MODE_DATA gfx_mode_data[] =
{
   { 320,   200,  "320x200"   },
   { 320,   240,  "320x240"   },
   { 640,   400,  "640x400"   },
   { 640,   480,  "640x480"   },
   { 800,   600,  "800x600"   },
   { 1024,  768,  "1024x768"  },
   { 1280,  1024, "1280x1024" },
   { 1600,  1200, "1600x1200" },
/*
   { 80,    80,   "80x80"     },
   { 160,   120,  "160x120"   },
   { 256,   200,  "256x200"   },
   { 256,   224,  "256x224"   },
   { 256,   240,  "256x240"   },
   { 256,   256,  "256x256"   },
   { 320,   100,  "320x100"   },
   { 320,   350,  "320x350"   },
   { 320,   400,  "320x400"   },
   { 320,   480,  "320x480"   },
   { 320,   600,  "320x600"   },
   { 360,   200,  "360x200"   },
   { 360,   240,  "360x240"   },
   { 360,   270,  "360x270"   },
   { 360,   360,  "360x360"   },
   { 360,   400,  "360x400"   },
   { 360,   480,  "360x480"   },
   { 360,   600,  "360x600"   },
   { 376,   282,  "376x282"   },
   { 376,   308,  "376x308"   },
   { 376,   564,  "376x564"   },
   { 400,   150,  "400x150"   },
   { 400,   300,  "400x300"   },
   { 400,   600,  "400x600"   },
*/
   { 0,     0,    NULL        }
};



/* gfx_mode_getter:
 *  Listbox data getter routine for the graphics mode list.
 */
static AL_CONST char *gfx_mode_getter(int index, int *list_size)
{
   if (index < 0) {
      if (list_size)
	 *list_size = sizeof(gfx_mode_data) / sizeof(GFX_MODE_DATA) - 1;
      return NULL;
   }

   return get_config_text(gfx_mode_data[index].s);
}



static _DRIVER_INFO *driver_list;

static int *gfx_card_list;
static int gfx_card_count;



/* gfx_card_cmp:
 *  qsort() callback for sorting the graphics driver list.
 */
static int gfx_card_cmp(AL_CONST void *e1, AL_CONST void *e2)
{
   /* note: drivers don't have to be in this array in order to be listed
    * by the dialog. This is just to control how the list is sorted.
    */
   static int driver_order[] = 
   { 
      #ifdef GFX_AUTODETECT
	 GFX_AUTODETECT,
      #endif
      #ifdef GFX_VGA
	 GFX_VGA,
      #endif
      #ifdef GFX_MODEX
	 GFX_MODEX,
      #endif
      #ifdef GFX_VESA1
	 GFX_VESA1,
      #endif
      #ifdef GFX_VESA2B
	 GFX_VESA2B,
      #endif
      #ifdef GFX_VESA2L
	 GFX_VESA2L,
      #endif
      #ifdef GFX_VESA3
	 GFX_VESA3,
      #endif
      #ifdef GFX_VBEAF
	 GFX_VBEAF,
      #endif
      #ifdef GFX_XTENDED
	 GFX_XTENDED
      #endif
   };

   int d1 = *((int *)e1);
   int d2 = *((int *)e2);
   AL_CONST char *n1, *n2;
   int i;

   if (d1 < 0)
      return -1;
   else if (d2 < 0)
      return 1;

   n1 = ((GFX_DRIVER *)driver_list[d1].driver)->ascii_name;
   n2 = ((GFX_DRIVER *)driver_list[d2].driver)->ascii_name;

   d1 = driver_list[d1].id;
   d2 = driver_list[d2].id;

   for (i=0; i<(int)(sizeof(driver_order)/sizeof(int)); i++) {
      if (d1 == driver_order[i])
	 return -1;
      else if (d2 == driver_order[i])
	 return 1;
   }

   return stricmp(n1, n2);
}



/* setup_card_list:
 *  Fills the list of video cards with info about the available drivers.
 */
static void setup_card_list(int *list)
{
   if (system_driver->gfx_drivers)
      driver_list = system_driver->gfx_drivers();
   else
      driver_list = _gfx_driver_list;

   gfx_card_list = list;
   gfx_card_count = 0;

   while (driver_list[gfx_card_count].driver) {
      gfx_card_list[gfx_card_count] = gfx_card_count;
      gfx_card_count++;
   }

   gfx_card_list[gfx_card_count++] = -1;

   qsort(gfx_card_list, gfx_card_count, sizeof(int), gfx_card_cmp);
}



/* gfx_card_getter:
 *  Listbox data getter routine for the graphics card list.
 */
static AL_CONST char *gfx_card_getter(int index, int *list_size)
{
   int i;

   if (index < 0) {
      if (list_size)
	 *list_size = gfx_card_count;
      return NULL;
   }

   i = gfx_card_list[index];

   if (i >= 0)
      return get_config_text(((GFX_DRIVER *)driver_list[i].driver)->ascii_name);
   else
      return get_config_text("Autodetect");
}



/* gfx_depth_getter:
 *  Listbox data getter routine for the color depth list.
 */
static AL_CONST char *gfx_depth_getter(int index, int *list_size)
{
   static AL_CONST char *depth[] = {
      " 8 bpp (256 color)",
      "15 bpp (32K color)",
      "16 bpp (64K color)",
      "24 bpp (16M color)",
      "32 bpp (16M color)"
   };

   if (index < 0) {
      if (list_size)
	 *list_size = sizeof(depth) / sizeof(char *);
      return NULL;
   }

   return get_config_text(depth[index]);
}



static DIALOG gfx_mode_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp)              (dp2) (dp3) */
   { box,               0,    0,    313,  159,  0,    0,    0,    0,       0,    0,    NULL,             NULL, NULL  },
   { label,             156,  8,    1,    1,    0,    0,    0,    0,       0,    0,    NULL,             NULL, NULL  },
   { button,            196,  105,  101,  17,   0,    0,    0,    D_EXIT,  0,    0,    NULL,             NULL, NULL  },
   { button,            196,  127,  101,  17,   0,    0,    27,   D_EXIT,  0,    0,    NULL,             NULL, NULL  },
   { listbox,           16,   28,   165,  116,  0,    0,    0,    D_EXIT,  0,    0,    gfx_card_getter,  NULL, NULL  },
   { listbox,           196,  28,   101,  68,   0,    0,    0,    D_EXIT,  3,    0,    gfx_mode_getter,  NULL, NULL  },
   { d_yield_proc,      0,    0,    0,    0,    0,    0,    0,    0,       0,    0,    NULL,             NULL, NULL  },
   { NULL,              0,    0,    0,    0,    0,    0,    0,    0,       0,    0,    NULL,             NULL, NULL  }
};



static DIALOG gfx_mode_ex_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp)              (dp2) (dp3) */
   { box,               0,    0,    313,  159,  0,    0,    0,    0,       0,    0,    NULL,             NULL, NULL  },
   { label,             156,  8,    1,    1,    0,    0,    0,    0,       TRUE, 0,    NULL,             NULL, NULL  },
   { button,            196,  105,  101,  17,   0,    0,    0,    D_EXIT,  0,    0,    NULL,             NULL, NULL  },
   { button,            196,  127,  101,  17,   0,    0,    27,   D_EXIT,  0,    0,    NULL,             NULL, NULL  },
   { listbox,           16,   28,   165,  68,   0,    0,    0,    D_EXIT,  0,    0,    gfx_card_getter,  NULL, NULL  },
   { listbox,           196,  28,   101,  68,   0,    0,    0,    D_EXIT,  3,    0,    gfx_mode_getter,  NULL, NULL  },
   { listbox,           16,   105,  165,  44,   0,    0,    0,    D_EXIT,  0,    0,    gfx_depth_getter, NULL, NULL  },
   { d_yield_proc,      0,    0,    0,    0,    0,    0,    0,    0,       0,    0,    NULL,             NULL, NULL  },
   { NULL,              0,    0,    0,    0,    0,    0,    0,    0,          0, 0,    NULL,             NULL, NULL  }
};



#define GFX_TITLE          1
#define GFX_OK             2
#define GFX_CANCEL         3
#define GFX_DRIVER_LIST    4
#define GFX_MODE_LIST      5
#define GFX_DEPTH_LIST     6



/* gfx_mode_select:
 *  Displays the Allegro graphics mode selection dialog, which allows the
 *  user to select a screen mode and graphics card. Stores the selection
 *  in the three variables, and returns zero if it was closed with the 
 *  Cancel button, or non-zero if it was OK'd.
 */
int ated_gfx_mode_select(int *card, int *w, int *h)
{
   int card_list[256];
   int i, ret;

   clear_keybuf();

   do {
   } while (gui_mouse_b());

   gfx_mode_dialog[GFX_TITLE].dp = (void*)get_config_text("Graphics Mode");
   gfx_mode_dialog[GFX_OK].dp = (void*)get_config_text("OK");
   gfx_mode_dialog[GFX_CANCEL].dp = (void*)get_config_text("Cancel");

   setup_card_list(card_list);

   centre_dialog(gfx_mode_dialog);
   set_dialog_color(gfx_mode_dialog, gui_fg_color, gui_bg_color);
   ret = popup_dialog(gfx_mode_dialog, GFX_DRIVER_LIST);

   i = card_list[gfx_mode_dialog[GFX_DRIVER_LIST].d1];

   if (i >= 0)
      *card = driver_list[i].id;
   else
      *card = GFX_AUTODETECT;

   *w = gfx_mode_data[gfx_mode_dialog[GFX_MODE_LIST].d1].w;
   *h = gfx_mode_data[gfx_mode_dialog[GFX_MODE_LIST].d1].h;

   if (ret == GFX_CANCEL)
      return FALSE;
   else 
      return TRUE;
}



/* gfx_mode_select_ex:
 *  Extended version of the graphics mode selection dialog, which allows the 
 *  user to select the color depth as well as the resolution and hardware 
 *  driver. This version of the function reads the initial values from the 
 *  parameters when it activates, so you can specify the default values.
 */
int ated_gfx_mode_select_ex(int *card, int *w, int *h, int *color_depth)
{
   int card_list[256];
   static int depth_list[] = { 8, 15, 16, 24, 32, 0 };
   int i, ret;

   clear_keybuf();

   do {
   } while (gui_mouse_b());

   setup_card_list(card_list);

   gfx_mode_ex_dialog[GFX_DRIVER_LIST].d1 = 0;

   for (i=0; i<gfx_card_count; i++) {
      if ((card_list[i] >= 0) && (driver_list[card_list[i]].id == *card)) {
	 gfx_mode_ex_dialog[GFX_DRIVER_LIST].d1 = i;
	 break;
      }
   }

   for (i=0; gfx_mode_data[i].s; i++) {
      if ((gfx_mode_data[i].w == *w) && (gfx_mode_data[i].h == *h)) {
	 gfx_mode_ex_dialog[GFX_MODE_LIST].d1 = i;
	 break; 
      }
   }

   for (i=0; depth_list[i]; i++) {
      if (depth_list[i] == *color_depth) {
	 gfx_mode_ex_dialog[GFX_DEPTH_LIST].d1 = i;
	 break;
      }
   }

   gfx_mode_ex_dialog[GFX_TITLE].dp = (void*)get_config_text("Graphics Mode");
   gfx_mode_ex_dialog[GFX_OK].dp = (void*)get_config_text("OK");
   gfx_mode_ex_dialog[GFX_CANCEL].dp = (void*)get_config_text("Cancel");

   centre_dialog(gfx_mode_ex_dialog);
   set_dialog_color(gfx_mode_ex_dialog, gui_fg_color, gui_bg_color);

   ret = popup_dialog(gfx_mode_ex_dialog, GFX_DRIVER_LIST);

   i = card_list[gfx_mode_ex_dialog[GFX_DRIVER_LIST].d1];

   if (i >= 0)
      *card = driver_list[i].id;
   else
      *card = GFX_AUTODETECT;

   *w = gfx_mode_data[gfx_mode_ex_dialog[GFX_MODE_LIST].d1].w;
   *h = gfx_mode_data[gfx_mode_ex_dialog[GFX_MODE_LIST].d1].h;

   *color_depth = depth_list[gfx_mode_ex_dialog[GFX_DEPTH_LIST].d1];

   if (ret == GFX_CANCEL)
      return FALSE;
   else 
      return TRUE;
}

/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*-----------------------------------  GFX MODE SEL  -------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/



