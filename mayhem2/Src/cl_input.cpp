#include "def.h"

/*
===============================================================================

KEY BUTTONS

Continuous button event tracking is complicated by the fact that two different
input sources (say, mouse button 1 and the control key) can both press the
same button, but the button should only be released when both of the
pressing key have been released.

When a key event issues a button command (+forward, +attack, etc), it appends
its key number as a parameter to the command so it can be matched up with
the release.

state bit 0 is the current state of the key
state bit 1 is edge triggered on the up to down transition
state bit 2 is edge triggered on the down to up transition


Key_Event (int key, qboolean down, unsigned time);

  +mlook src time

===============================================================================
*/

void key_down (key_button *b, ustring *argv)
{
   int   key, i;
	
   key = argv[1];
   if (!key)
      key = -1;		// typed manually at the console for continuous down
   
   for (i = 0; i < CL_MAX_DOWN; i++)
	   if (key == b->down[i])
		   return;		// repeating key
	
   for (i = 0; i < CL_MAX_DOWN; i++)
   {
      if ( !b->down[i] )
      {
         b->down[i] = key;
         break;
      }
   }

   if (i == CL_MAX_DOWN)
	{
		console.printf ("%d keys down for a button!\n", i+1);
		return;
	}
	
	if (b->state & KEY_BUTTON_DOWN)
		return;		// still down

	// save timestamp
	b->downtime = argv[2];
	if (!b->downtime)
		b->downtime = curtime - 100;

	b->state |= KEY_BUTTON_DOWN | KEY_BUTTON_IMPULSE_DOWN;	// down + impulse down
}

void key_up (key_button *b, ustring *argv)
{
	int      key, i, count = 0;
	unsigned uptime;

   key = argv[1];
	if (!key)
	{  // typed manually at the console, assume for unsticking, so clear all
      for (i = 0; i < CL_MAX_DOWN; i++)
		   b->down[i] = 0;
		b->state = KEY_BUTTON_IMPULSE_UP;	// impulse up
		return;
	}

   for (i = 0; i < CL_MAX_DOWN; i++)
   {
	   if (b->down[i] == key)
		   b->down[i] = 0;
      else if (b->down[i])
         count++;
   }

	//if (i == CL_MAX_DOWN)
	// return;		// key up without coresponding down (menu pass through)

	if (count)
		return;		// some other key is still holding it down

	if ( !(b->state & KEY_BUTTON_DOWN) )
		return;		// still up (this should not happen)

	// save timestamp
	uptime = argv[2];
	if (uptime)
		b->msec += uptime - b->downtime;
	else
		b->msec += 10;

	b->state &= ~KEY_BUTTON_DOWN;		   // now up
	b->state |= KEY_BUTTON_IMPULSE_UP; 	// impulse up
}


/*
===============
CL_KeyState

Returns the fraction of the frame that the key was down
===============
*/
float key_state (key_button *key)
{
	float val;
	int	msec;

	key->state &= KEY_BUTTON_DOWN;		// clear impulses

	msec = key->msec;
	key->msec = 0;

	if (key->state)
	{	//
      // still down
      //
		msec += curtime - key->downtime;
		key->downtime = curtime;
	}

	val = (float)msec / frame_msec;
	if (val < 0)
		val = 0;
	if (val > 1)
		val = 1;
/*
#ifdef DEBUG
	if (msec)
	{
	   console.printf ("%s %i %f\n", key->name, msec, val);
	}   
#endif
*/
	return val;
}

