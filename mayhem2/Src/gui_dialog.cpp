#include "def.h"
#include "allegro/internal/aintern.h"
#include "gui.h"
#include "gui_dialog.h"
#include "screen.h"

static DIALOG           *dlg = NULL;
static DIALOG_PLAYER    *player;

int send_message (DIALOG *d, int msg, int c)
{
   if (msg == MSG_DRAW)
   {
      if (d->flags & D_HIDDEN)
         return D_O_K;
   }

   return d->proc(msg, d, c);
}

void send_dialog_message (int msg, int c)
{
   DIALOG *d;

   for (d = dlg; d->proc; d++)
   {
      send_message (d, msg, c);
   }
}

void gui_close_dialog ()
{
   if(!dlg)
      return;

   shutdown_dialog (player);
}

void gui_open_dialog (DIALOG *d)
{
   DIALOG *dialog;

   dialog = dlg;

   gui_close_dialog ();

   if(d)
      dlg = d;
   else
      dlg = dialog;

   if(!dlg)
      return;

   centre_dialog (dlg);
   player = init_dialog (dlg, -1);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////          DIALOG PLAYER SPECIFIC         ////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_OBJECTS     512

typedef struct OBJ_LIST
{
   int index;
   int diff;
} OBJ_LIST;

#ifndef MESSAGE
#define MESSAGE(i, msg, c)                         \
{                                                  \
   r = SEND_MESSAGE(player->dialog + i, msg, c);   \
   if (r != D_O_K) {                               \
      player->res |= r;                            \
      player->obj = i;                             \
   }                                               \
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////
/* Checking for double clicks is complicated. The user could release the
 * mouse button at almost any point, and I might miss it if I am doing some
 * other processing at the same time (eg. sending the single-click message).
 * To get around this I install a timer routine to do the checking for me,
 * so it will notice double clicks whenever they happen.
 */

static volatile int dclick_status, dclick_time;

//static int gui_install_count = 0;
static int gui_install_time = 0;

#define DCLICK_START      0
#define DCLICK_RELEASE    1
#define DCLICK_AGAIN      2
#define DCLICK_NOT        3



/* dclick_check:
 *  Double click checking user timer routine.
 */
static void dclick_check(void)
{
   if (dclick_status == DCLICK_START)           // first click...
   {
      if (!mouse_b)                             // aah! released first
      {
	      dclick_status = DCLICK_RELEASE;
	      dclick_time = 0;
	      return;
      }
   }
   else if (dclick_status == DCLICK_RELEASE)    // wait for second click
   {
      if (mouse_b)
      {
	      dclick_status = DCLICK_AGAIN;          // yes! the second click
	      dclick_time = 0;
	      return;
      }
   }
   else
      return;

   // timeout?
   if (dclick_time++ > 20)
   {
      dclick_status = DCLICK_NOT;
   }
}
END_OF_STATIC_FUNCTION(dclick_check);

/* gui_switch_callback:
 *  Sets the dirty flags whenever a display switch occurs.
 */
static void gui_switch_callback (void)
{
   if (player)
     player->res |= D_REDRAW;
}

static int find_mouse_object (DIALOG *d)
{
   int mouse_object = -1;
   int c;

   for (c = 0; d[c].proc; c++)
   {
      if ((mouse_x >= d[c].x)          && (mouse_y >= d[c].y)          &&
	       (mouse_x <  d[c].x + d[c].w) && (mouse_y <  d[c].y + d[c].h) &&
	       (!(d[c].flags & (D_HIDDEN | D_DISABLED))))
      {
	      mouse_object = c;
      }
   }

   return mouse_object;
}

/*
static void check_for_redraw (DIALOG_PLAYER *player)
{
   int c, r;

   // need to draw it?
   if (player->res & D_REDRAW) 
   {
      player->res ^= D_REDRAW;
      player->res |= dialog_message(player->dialog, MSG_DRAW, 0, &player->obj);
   }

   // check if any widget has to be redrawn
   for (c = 0; player->dialog[c].proc; c++) {
      if ((player->dialog[c].flags & (D_DIRTY | D_HIDDEN)) == D_DIRTY) 
      {
	      player->dialog[c].flags &= ~D_DIRTY;
	      scare_mouse();
	      MESSAGE(c, MSG_DRAW, 0);
	      unscare_mouse();
      }
   }
}
*/

static int obj_list_cmp(AL_CONST void *e1, AL_CONST void *e2)
{
   return (((OBJ_LIST *)e1)->diff - ((OBJ_LIST *)e2)->diff);
}

static int cmp_tab(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int ret = (int)d2 - (int)d1;

   if (ret < 0)
      ret += 0x10000;

   return ret;
}

static int cmp_shift_tab(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int ret = (int)d1 - (int)d2;

   if (ret < 0)
      ret += 0x10000;

   return ret;
}

static int cmp_right(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int ret = (d2->x - d1->x) + ABS(d1->y - d2->y) * 8;

   if (d1->x >= d2->x)
      ret += 0x10000;

   return ret;
}

static int cmp_left(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int ret = (d1->x - d2->x) + ABS(d1->y - d2->y) * 8;

   if (d1->x <= d2->x)
      ret += 0x10000;

   return ret;
}

static int cmp_down(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int ret = (d2->y - d1->y) + ABS(d1->x - d2->x) * 8;

   if (d1->y >= d2->y)
      ret += 0x10000;

   return ret;
}

static int cmp_up(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2)
{
   int ret = (d1->y - d2->y) + ABS(d1->x - d2->x) * 8;

   if (d1->y <= d2->y)
      ret += 0x10000;

   return ret;
}

static int move_focus(DIALOG *d, int ascii, int scan, int *focus_obj)
{
   int (*cmp)(AL_CONST DIALOG *d1, AL_CONST DIALOG *d2);
   OBJ_LIST obj[MAX_OBJECTS];
   int obj_count = 0;
   int fobj, c;
   int res = D_O_K;

   // choose a comparison function
   switch (scan)
   {
      case KEY_TAB:   cmp = (ascii == '\t') ? cmp_tab : cmp_shift_tab; break;
      case KEY_RIGHT: cmp = cmp_right; break;
      case KEY_LEFT:  cmp = cmp_left;  break;
      case KEY_DOWN:  cmp = cmp_down;  break;
      case KEY_UP:    cmp = cmp_up;    break;
      default:        return D_O_K;
   }

   // fill temporary table
   for (c = 0; d[c].proc; c++)
   {
      if ((*focus_obj < 0) || (c != *focus_obj))
      {
         obj[obj_count].index = c;
         if (*focus_obj >= 0)
	         obj[obj_count].diff = cmp(d+*focus_obj, d+c);
         else
            obj[obj_count].diff = c;

         obj_count++;
         if (obj_count >= MAX_OBJECTS)
            break;
      }
   }

   // sort table
   qsort(obj, obj_count, sizeof(OBJ_LIST), obj_list_cmp);

   // find an object to give the focus to
   fobj = *focus_obj;
   for (c = 0; c < obj_count; c++)
   {
      res |= offer_focus(d, obj[c].index, focus_obj, FALSE);
      if (fobj != *focus_obj)
         break;
   }

   return res;
}

void gui_key_event(int k)
{
   int scan, ascii = 0, combo, r, c;

   // ESC closes dialog
   if (k == KEY_ESC)
   {
      cbuf_add_text("menu close\n");
      return;
   }

   scan = k;
/*
   if (scan >= KEY_MOUSE_START && scan <= KEY_MOUSE_MAX)
   {
      if (down)
      {
         MESSAGE (player->mouse_obj, MSG_LPRESS, new_mouse_b);
      }
      return;
   }
*/
   // fake joystick input by converting it to key presses
   if(k == KEY_JOY1_LEFT)
   {
      ascii = 0;
      scan = KEY_LEFT;
      player->joy_on = TRUE;
   }
   else if (k == KEY_JOY1_RIGHT)
   {
      ascii = 0;
      scan = KEY_RIGHT;
      player->joy_on = TRUE;
   }
   else if (k == KEY_JOY1_UP)
   {
      ascii = 0;
      scan = KEY_UP;
      player->joy_on = TRUE;
   }
   else if (k == KEY_JOY1_DOWN)
   {
      ascii = 0;
      scan = KEY_DOWN;
      player->joy_on = TRUE;
   }
   else if (k == KEY_JOY1_1 || k == KEY_JOY1_2)
   {
      ascii = ' ';
      scan = KEY_SPACE;
      player->joy_on = TRUE;
   }

   // deal with keyboard input
   if ( !ascii )
      ascii = (return_printable(scan) != -1) ? return_printable(scan) : 0;

   combo = (scan<<8) | ((ascii <= 255) ? ascii : '^');

   // let object deal with the key
   if (player->focus_obj >= 0)
   {
      MESSAGE (player->focus_obj, MSG_SCAN,  scan);
      if (player->res & D_USED_CHAR)
         goto getout;

      MESSAGE (player->focus_obj, MSG_CHAR,  combo);
      if (player->res & D_USED_CHAR)
         goto getout;

      MESSAGE (player->focus_obj, MSG_UCHAR, ascii);
      if (player->res & D_USED_CHAR)
         goto getout;
   }

   // keyboard shortcut?
   for (c = 0; player->dialog[c].proc; c++)
   {
      if ((((ascii > 0) &&
            (utolower (player->dialog[c].key) == utolower (ascii))) ||
            ((!ascii) && (player->dialog[c].key == (scan<<8)))) &&
            (!(player->dialog[c].flags & (D_HIDDEN | D_DISABLED))))
      {
         MESSAGE (c, MSG_KEY, combo);
         goto getout;
      }
   }

   // broadcast in case any other objects want it
   for (c = 0; player->dialog[c].proc; c++)
   {
      if (!(player->dialog[c].flags & (D_HIDDEN | D_DISABLED)))
      {
         MESSAGE (c, MSG_XCHAR, combo);
         if (player->res & D_USED_CHAR)
            goto getout;
      }
   }

   if (((scan == KEY_ENTER) || (scan == KEY_ENTER_PAD) || (scan == KEY_SPACE)) &&
        (player->focus_obj >= 0))
   {
      MESSAGE (player->focus_obj, MSG_KEY, combo);
      goto getout;
   }

   // move focus around the dialog
   player->res |= move_focus (player->dialog, ascii, scan, &player->focus_obj);

getout:

   return;
}

void gui_animate()
{
   int c, r, ret, nowhere, z;
   int new_mouse_b;

   if (player->res & D_CLOSE)
   {
      if(key_dest == key_menu)
         key_dest = key_game;

      return;
   }

   // deal with mouse buttons presses and releases
   new_mouse_b = mouse_b;
   if (new_mouse_b != player->mouse_b)
   {
      player->res |= offer_focus (player->dialog, player->mouse_obj, &player->focus_obj, FALSE);

      if (player->mouse_obj >= 0)
      {
         // send press and release messages
         if ((new_mouse_b & 1) && !(player->mouse_b & 1))
            MESSAGE (player->mouse_obj, MSG_LPRESS, new_mouse_b);
         if (!(new_mouse_b & 1) && (player->mouse_b & 1))
            MESSAGE (player->mouse_obj, MSG_LRELEASE, new_mouse_b);

         if ((new_mouse_b & 4) && !(player->mouse_b & 4))
            MESSAGE (player->mouse_obj, MSG_MPRESS, new_mouse_b);
         if (!(new_mouse_b & 4) && (player->mouse_b & 4))
            MESSAGE (player->mouse_obj, MSG_MRELEASE, new_mouse_b);

         if ((new_mouse_b & 2) && !(player->mouse_b & 2))
            MESSAGE (player->mouse_obj, MSG_RPRESS, new_mouse_b);
         if (!(new_mouse_b & 2) && (player->mouse_b & 2))
            MESSAGE (player->mouse_obj, MSG_RRELEASE, new_mouse_b);

         player->mouse_b = new_mouse_b;
      }
   }

   // need to reinstall the dclick and switch handlers?
   if (gui_install_time != _allegro_count)
   {
      install_int (dclick_check, 20);

      if (get_display_switch_mode() == SWITCH_AMNESIA)
         set_display_switch_callback (SWITCH_IN, gui_switch_callback);

      gui_install_time = _allegro_count;
   }

   // are we dealing with a mouse click?
   if (player->click_wait)
   {
      if ((ABS(player->mouse_ox - mouse_x) > 8) ||
          (ABS(player->mouse_oy - mouse_y) > 8))
      {
         dclick_status = DCLICK_NOT;
      }

      // waiting...
      if ((dclick_status != DCLICK_AGAIN) && (dclick_status != DCLICK_NOT))
      {
         return;
      }

      player->click_wait = FALSE;

      // double click!
      if ((dclick_status == DCLICK_AGAIN) &&
          (mouse_x >= player->dialog[player->mouse_obj].x) &&
          (mouse_y >= player->dialog[player->mouse_obj].y) &&
          (mouse_x <= player->dialog[player->mouse_obj].x + player->dialog[player->mouse_obj].w) &&
          (mouse_y <= player->dialog[player->mouse_obj].y + player->dialog[player->mouse_obj].h))
      {
         MESSAGE(player->mouse_obj, MSG_DCLICK, 0);
      }

      goto getout;
   }

   player->res &= ~D_USED_CHAR;

   // need to give the input focus to someone?
   if (player->res & D_WANTFOCUS)
   {
      player->res ^= D_WANTFOCUS;
      player->res |= offer_focus(player->dialog, player->obj, &player->focus_obj, FALSE);
   }

   // has mouse object changed?
   c = find_mouse_object (player->dialog);
   if (c != player->mouse_obj)
   {
      if (player->mouse_obj >= 0)
      {
         player->dialog[player->mouse_obj].flags &= ~D_GOTMOUSE;
         MESSAGE(player->mouse_obj, MSG_LOSTMOUSE, 0);
      }

      if (c >= 0)
      {
         player->dialog[c].flags |= D_GOTMOUSE;
         MESSAGE(c, MSG_GOTMOUSE, 0);
      }

      player->mouse_obj = c;

      // move the input focus as well?
      if (player->mouse_obj != player->focus_obj)
         player->res |= offer_focus (player->dialog, player->mouse_obj, &player->focus_obj, TRUE);
   }

   // deal with mouse button clicks
   if (new_mouse_b)
   {
      player->res |= offer_focus (player->dialog, player->mouse_obj, &player->focus_obj, FALSE);

      if (player->mouse_obj >= 0)
      {
         dclick_time = 0;
         dclick_status = DCLICK_START;
         player->mouse_ox = mouse_x;
         player->mouse_oy = mouse_y;

         // send click message
         MESSAGE(player->mouse_obj, MSG_CLICK, new_mouse_b);

         //if (player->res == D_O_K)
         player->click_wait = TRUE;
      }
      else
         dialog_message(player->dialog, MSG_IDLE, 0, &nowhere);

      // goto getout;
   }

   // deal with mouse wheel clicks
   z = mouse_z;

   if (z != player->mouse_oz)
   {
      player->res |= offer_focus(player->dialog, player->mouse_obj, &player->focus_obj, FALSE);

      if (player->mouse_obj >= 0)
      {
         MESSAGE(player->mouse_obj, MSG_WHEEL, z-player->mouse_oz);
      }
      else
      {
         dialog_message(player->dialog, MSG_IDLE, 0, &nowhere);
      }

      player->mouse_oz = z;
      goto getout;
   }

   // send idle messages
   player->res |= dialog_message(player->dialog, MSG_IDLE, 0, &player->obj);

getout:

   ret = (!(player->res & D_CLOSE));
   player->res &= ~D_CLOSE;
}

void gui_draw(BITMAP *bmp)
{
   gui_set_bmp (bmp);
   send_dialog_message (MSG_DRAW, 0);

   //
   // Draw mouse pointer
   //
   draw_sprite(bmp, mouse_pointer, mouse_x, mouse_y);
}

