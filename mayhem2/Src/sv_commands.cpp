#include "def.h"

/*
===============================================================================

OPERATOR CONSOLE ONLY COMMANDS

These commands can only be entered from stdin or by a remote operator datagram
===============================================================================
*/

/*
====================
SV_SetMaster_f

Specify a list of master servers
====================
*/
BEGIN_COMMAND (setmaster, NULL)
{
	int		i, slot;

	// only dedicated servers send heartbeats
	if (!dedicated->value)
	{
		console.printf ("Only dedicated servers use masters.\n");
		return;
	}

	// make sure the server is listed public
	Cvar_Set ("public", "1");

	for (i=1 ; i<MAX_MASTERS ; i++)
		memset (&master_adr[i], 0, sizeof(master_adr[i]));

	slot = 1;		// slot 0 will always contain the id master
	for (i=1 ; i<Cmd_Argc() ; i++)
	{
		if (slot == MAX_MASTERS)
			break;

		if (!net_string_to_adr (argv[i], &master_adr[i]))
		{
			console.printf ("Bad address: %s\n", Cmd_Argv(i));
			continue;
		}
		if (master_adr[slot].port == 0)
			master_adr[slot].port = big_short (PORT_MASTER);

		console.printf ("Master server at %s\n", NET_AdrToString (master_adr[slot]));

		console.printf ("Sending a ping.\n");

		netchan_out_of_band_print (NS_SERVER, master_adr[slot], "ping");

		slot++;
	}

	svs.last_heartbeat = -9999999;
}
END_COMMAND (setmaster)


/*
==================
SV_SetPlayer

Sets sv_client and sv_player to the player with idnum Cmd_Argv(1)
==================
*/
boolean sv_set_player (int argc, ustring *argv)
{
	Client	*cl;
	int		i;
	int		idnum;
	char		s[64];

	if (argc < 2)
		return false;

	ustrcpy (s, argv[1]);

	// numeric values are just slot numbers
	if (ugatat(s, 0) >= '0' && ugetat(s, 0) <= '9')
	{
		idnum = argv[1]);
		if (idnum < 0 || idnum >= maxclients->value)
		{
			console.printf ("Bad client slot: %i\n", idnum);
			return false;
		}

		sv_client = &svs.clients[idnum];
		sv_player = sv_client->edict;
		if (!sv_client->state)
		{
			console.printf ("Client %i is not active\n", idnum);
			return false;
		}

		return true;
	}

	// check for a name match
	for (i = 0, cl = svs.clients; i < maxclients->value; i++, cl++)
	{
		if (!cl->state)
			continue;

		if (!ustrcmp(cl->name, s))
		{
			sv_client = cl;
			sv_player = sv_client->edict;
			return true;
		}
	}

	console.printf ("Userid %s is not on the server\n", s);
	return false;
}


/*
===============================================================================

SAVEGAME FILES

===============================================================================
*/

/*
=====================
SV_WipeSavegame

Delete save/<XXX>/
=====================
*/
void SV_WipeSavegame (char *savename)
{
	char	name[MAX_OSPATH];
	char	*s;

	Com_DPrintf("SV_WipeSaveGame(%s)\n", savename);

	Com_sprintf (name, sizeof(name), "%s/save/%s/server.ssv", FS_Gamedir (), savename);
	remove (name);
	Com_sprintf (name, sizeof(name), "%s/save/%s/game.ssv", FS_Gamedir (), savename);
	remove (name);

	Com_sprintf (name, sizeof(name), "%s/save/%s/*.sav", FS_Gamedir (), savename);
	s = Sys_FindFirst( name, 0, 0 );
	while (s)
	{
		remove (s);
		s = Sys_FindNext( 0, 0 );
	}
	Sys_FindClose ();
	Com_sprintf (name, sizeof(name), "%s/save/%s/*.sv2", FS_Gamedir (), savename);
	s = Sys_FindFirst(name, 0, 0 );
	while (s)
	{
		remove (s);
		s = Sys_FindNext( 0, 0 );
	}
	Sys_FindClose ();
}


/*
================
CopyFile
================
*/
void CopyFile (char *src, char *dst)
{
	FILE	*f1, *f2;
	int		l;
	byte	buffer[65536];

	Com_DPrintf ("CopyFile (%s, %s)\n", src, dst);

	f1 = fopen (src, "rb");
	if (!f1)
		return;
	f2 = fopen (dst, "wb");
	if (!f2)
	{
		fclose (f1);
		return;
	}

	while (1)
	{
		l = fread (buffer, 1, sizeof(buffer), f1);
		if (!l)
			break;
		fwrite (buffer, 1, l, f2);
	}

	fclose (f1);
	fclose (f2);
}


/*
================
SV_CopySaveGame
================
*/
void SV_CopySaveGame (char *src, char *dst)
{
	char	name[MAX_OSPATH], name2[MAX_OSPATH];
	int		l, len;
	char	*found;

	Com_DPrintf("SV_CopySaveGame(%s, %s)\n", src, dst);

	SV_WipeSavegame (dst);

	// copy the savegame over
	Com_sprintf (name, sizeof(name), "%s/save/%s/server.ssv", FS_Gamedir(), src);
	Com_sprintf (name2, sizeof(name2), "%s/save/%s/server.ssv", FS_Gamedir(), dst);
	FS_CreatePath (name2);
	CopyFile (name, name2);

	Com_sprintf (name, sizeof(name), "%s/save/%s/game.ssv", FS_Gamedir(), src);
	Com_sprintf (name2, sizeof(name2), "%s/save/%s/game.ssv", FS_Gamedir(), dst);
	CopyFile (name, name2);

	Com_sprintf (name, sizeof(name), "%s/save/%s/", FS_Gamedir(), src);
	len = strlen(name);
	Com_sprintf (name, sizeof(name), "%s/save/%s/*.sav", FS_Gamedir(), src);
	found = Sys_FindFirst(name, 0, 0 );
	while (found)
	{
		strcpy (name+len, found+len);

		Com_sprintf (name2, sizeof(name2), "%s/save/%s/%s", FS_Gamedir(), dst, found+len);
		CopyFile (name, name2);

		// change sav to sv2
		l = strlen(name);
		strcpy (name+l-3, "sv2");
		l = strlen(name2);
		strcpy (name2+l-3, "sv2");
		CopyFile (name, name2);

		found = Sys_FindNext( 0, 0 );
	}
	Sys_FindClose ();
}


/*
==============
SV_WriteLevelFile

==============
*/
void SV_WriteLevelFile (void)
{
	char	name[MAX_OSPATH];
	FILE	*f;

	Com_DPrintf("SV_WriteLevelFile()\n");

	Com_sprintf (name, sizeof(name), "%s/save/current/%s.sv2", FS_Gamedir(), sv.name);
	f = fopen(name, "wb");
	if (!f)
	{
		Com_Printf ("Failed to open %s\n", name);
		return;
	}
	fwrite (sv.configstrings, sizeof(sv.configstrings), 1, f);
	CM_WritePortalState (f);
	fclose (f);

	Com_sprintf (name, sizeof(name), "%s/save/current/%s.sav", FS_Gamedir(), sv.name);
	ge->WriteLevel (name);
}

/*
==============
SV_ReadLevelFile

==============
*/
void SV_ReadLevelFile (void)
{
	char	name[MAX_OSPATH];
	FILE	*f;

	Com_DPrintf("SV_ReadLevelFile()\n");

	Com_sprintf (name, sizeof(name), "%s/save/current/%s.sv2", FS_Gamedir(), sv.name);
	f = fopen(name, "rb");
	if (!f)
	{
		Com_Printf ("Failed to open %s\n", name);
		return;
	}
	FS_Read (sv.configstrings, sizeof(sv.configstrings), f);
	CM_ReadPortalState (f);
	fclose (f);

	Com_sprintf (name, sizeof(name), "%s/save/current/%s.sav", FS_Gamedir(), sv.name);
	ge->ReadLevel (name);
}

/*
==============
SV_WriteServerFile

==============
*/
void SV_WriteServerFile (qboolean autosave)
{
	FILE	*f;
	cvar_t	*var;
	char	name[MAX_OSPATH], string[128];
	char	comment[32];
	time_t	aclock;
	struct tm	*newtime;

	Com_DPrintf("SV_WriteServerFile(%s)\n", autosave ? "true" : "false");

	Com_sprintf (name, sizeof(name), "%s/save/current/server.ssv", FS_Gamedir());
	f = fopen (name, "wb");
	if (!f)
	{
		Com_Printf ("Couldn't write %s\n", name);
		return;
	}
	// write the comment field
	memset (comment, 0, sizeof(comment));

	if (!autosave)
	{
		time (&aclock);
		newtime = localtime (&aclock);
		Com_sprintf (comment,sizeof(comment), "%2i:%i%i %2i/%2i  ", newtime->tm_hour
			, newtime->tm_min/10, newtime->tm_min%10,
			newtime->tm_mon+1, newtime->tm_mday);
		strncat (comment, sv.configstrings[CS_NAME], sizeof(comment)-1-strlen(comment) );
	}
	else
	{	// autosaved
		Com_sprintf (comment, sizeof(comment), "ENTERING %s", sv.configstrings[CS_NAME]);
	}

	fwrite (comment, 1, sizeof(comment), f);

	// write the mapcmd
	fwrite (svs.mapcmd, 1, sizeof(svs.mapcmd), f);

	// write all CVAR_LATCH cvars
	// these will be things like coop, skill, deathmatch, etc
	for (var = cvar_vars ; var ; var=var->next)
	{
		if (!(var->flags & CVAR_LATCH))
			continue;
		if (strlen(var->name) >= sizeof(name)-1
			|| strlen(var->string) >= sizeof(string)-1)
		{
			Com_Printf ("Cvar too long: %s = %s\n", var->name, var->string);
			continue;
		}
		memset (name, 0, sizeof(name));
		memset (string, 0, sizeof(string));
		strcpy (name, var->name);
		strcpy (string, var->string);
		fwrite (name, 1, sizeof(name), f);
		fwrite (string, 1, sizeof(string), f);
	}

	fclose (f);

	// write game state
	Com_sprintf (name, sizeof(name), "%s/save/current/game.ssv", FS_Gamedir());
	ge->WriteGame (name, autosave);
}

/*
==============
SV_ReadServerFile

==============
*/
void SV_ReadServerFile (void)
{
	FILE	*f;
	char	name[MAX_OSPATH], string[128];
	char	comment[32];
	char	mapcmd[MAX_TOKEN_CHARS];

	Com_DPrintf("SV_ReadServerFile()\n");

	Com_sprintf (name, sizeof(name), "%s/save/current/server.ssv", FS_Gamedir());
	f = fopen (name, "rb");
	if (!f)
	{
		Com_Printf ("Couldn't read %s\n", name);
		return;
	}
	// read the comment field
	FS_Read (comment, sizeof(comment), f);

	// read the mapcmd
	FS_Read (mapcmd, sizeof(mapcmd), f);

	// read all CVAR_LATCH cvars
	// these will be things like coop, skill, deathmatch, etc
	while (1)
	{
		if (!fread (name, 1, sizeof(name), f))
			break;
		FS_Read (string, sizeof(string), f);
		Com_DPrintf ("Set %s = %s\n", name, string);
		Cvar_ForceSet (name, string);
	}

	fclose (f);

	// start a new game fresh with new cvars
	SV_InitGame ();

	strcpy (svs.mapcmd, mapcmd);

	// read game state
	Com_sprintf (name, sizeof(name), "%s/save/current/game.ssv", FS_Gamedir());
	ge->ReadGame (name);
}


//=========================================================




/*
==================
SV_DemoMap_f

Puts the server in demo mode on a specific map/cinematic
==================
*/
BEGIN_COMMAND (demomap, NULL)
{
	sv_map (true, argv[1], false );
}
END_COMMAND (demomap)

/*
==================
SV_GameMap_f

Saves the state of the map just being exited and goes to a new map.

If the initial character of the map string is '*', the next map is
in a new unit, so the current savegame directory is cleared of
map files.

Example:

*inter.cin+jail

Clears the archived maps, plays the inter.cin cinematic, then
goes to map jail.bsp.
==================
*/
void sv_gamemap_f (int argc, ustring *argv)
{
	char		map[64];
	int		i;
	Client	*cl;
	boolean	*savedInuse;

	if (argc != 2)
	{
		console.printf ("USAGE: gamemap <map>\n");
		return;
	}

	console.dprintf ("sv_game_map (%s)\n", argv[1].get_s ());

	// check for clearing the current savegame
	ustrcpy (map, argv[1]);
	if (ugetat (map, 0) == '*')
	{
		// wipe all the *.sav files
		SV_WipeSavegame ("current");
	}
	else
	{	// save the map just exited
		if (sv.state == ss_game)
		{
			// clear all the client inuse flags before saving so that
			// when the level is re-entered, the clients will spawn
			// at spawn points instead of occupying body shells
			savedInuse = malloc(maxclients->value * sizeof(qboolean));
			for (i=0,cl=svs.clients ; i<maxclients->value; i++,cl++)
			{
				savedInuse[i] = cl->edict->inuse;
				cl->edict->inuse = false;
			}

			SV_WriteLevelFile ();

			// we must restore these for clients to transfer over correctly
			for (i=0,cl=svs.clients ; i<maxclients->value; i++,cl++)
				cl->edict->inuse = savedInuse[i];
			free (savedInuse);
		}
	}

	// start up the next map
	sv_map (false, argv[1], false );

	// archive server state
	ustrncpy (svs.mapcmd, argv[1], sizeof(svs.mapcmd)-1);

	// copy off the level to the autosave slot
	if (!dedicated->value)
	{
		SV_WriteServerFile (true);
		SV_CopySaveGame ("current", "save0");
	}
}

BEGIN_COMMAND (gamemap, NULL)
{
   sv_gamemap_f (argc, argv);
}
END_COMMAND (gamemap)

/*
==================
SV_Map_f

Goes directly to a given map without any savegame archiving.
For development work
==================
*/
BEGIN_COMMAND (map, MULL)
{
	char	map[64];
	char	expanded[256];

	// if not a pcx, demo, or cinematic, check to make sure the level exists
	ustrcpy (map, argv[1];
	if (!ustrstr (map, "."))
	{
		usprintf (expanded, "data/maps/%s.atm", map);
		if ( !exists(expanded) )
		{
			console.printf (con_error, "Can't find %s\n", expanded);
			return;
		}

   	sv.state = ss_dead;		// don't save current level when changing
   	sv_gamemap_f (argc, argv);
	}
}
END_COMMAND (map)

//===============================================================

/*
==================
SV_Kick_f

Kick a user off of the server
==================
*/
BEGIN_COMMAND (kick, "Kick a user off of the server")
{
	if (!svs.initialized)
	{
		console.printf ("No server running.\n");
		return;
	}

	if (argc != 2)
	{
		console.printf ("USAGE: kick <userid>\n");
		return;
	}

	if (!sv_set_player ())
		return;

	sv_broadcast_printf (PRINT_HIGH, "%s was kicked\n", sv_client->name);
	// print directly, because the dropped client won't get the
	// SV_BroadcastPrintf message
	sv_client_printf (sv_client, PRINT_HIGH, "You were kicked from the game\n");
	sv_drop_client   (sv_client);
	sv_client->lastmessage = svs.realtime;	// min case there is a funny zombie
}
END_COMMAND (kick)

/*
================
SV_Status_f
================
*/
BEGIN_COMMAND (status, NULL)
{
	int		i, j, l;
	Client	*cl;
	char		*s;
	int		ping;

	if (!svs.clients)
	{
		console.printf ("No server running.\n");
		return;
	}

	console.printf ("map              : %s\n", sv.name);
	console.printf ("num score ping name            lastmsg address               qport \n");
	console.printf ("--- ----- ---- --------------- ------- --------------------- ------\n");

	for (i = 0, cl = svs.clients; i < maxclients->value; i++, cl++)
	{
		if (!cl->state)
			continue;

		console.printf ("%3i ", i);
		console.printf ("%5i ", cl->edict->client->ps.stats[STAT_FRAGS]);

		if (cl->state == cs_connected)
			console.printf ("CNCT ");
		else if (cl->state == cs_zombie)
			console.printf ("ZMBI ");
		else
		{
			ping = cl->ping < 9999 ? cl->ping : 9999;
			console.printf ("%4i ", ping);
		}

		console.printf ("%s", cl->name);
		l = 16 - strlen(cl->name);
		for (j=0 ; j<l ; j++)
			console.printf (" ");

		console.printf ("%7i ", svs.realtime - cl->lastmessage );

		s = NET_AdrToString ( cl->netchan.remote_address);
		console.printf ("%s", s);
		l = 22 - strlen(s);
		for (j=0 ; j<l ; j++)
			console.printf (" ");
		
		console.printf ("%5i", cl->netchan.qport);

		console.printf ("\n");
	}
	console.printf ("\n");
}
END_COMMAND (status)

/*
==================
SV_ConSay_f
==================
*/
BEGIN_COMMAND (consay, NULL)
{
	Client   *client;
	int	   j;
	ustring  text;

	if (argc < 2)
		return;

	text = "console: ";
   text += build_string (argc - 1, &argv[1]);

	for (j = 0, client = svs.clients; j < maxclients->value; j++, client++)
	{
		if (client->state != cs_spawned)
			continue;
		sv_client_printf (client, PRINT_CHAT, "%s\n", text);
	}
}
END_COMMAND (consay)

/*
==================
SV_Heartbeat_f
==================
*/
BEGIN_COMMAND (heartbeat, NULL)
{
	svs.last_heartbeat = -9999999;
}
END_COMMAND (heartbeat)

/*
===========
SV_Serverinfo_f

  Examine or change the serverinfo string
===========
*/
BEGIN_COMMAND (serverinfo, "Examine or change the serverinfo string")
{
	console.printf ("Server info settings:\n");
	info_print     (cvar_serverinfo());
}
END_COMMAND (serverinfo)

/*
===========
SV_DumpUser_f

Examine all a users info strings
===========
*/
BEGIN_COMMAND (dumpuser, "Examine all a users info strings")
{
	if (argc != 2)
	{
		console.printf ("USAGE: info <userid>\n");
		return;
	}

	if (!sv_set_player ())
		return;

	console.printf ("userinfo\n");
	console.printf ("--------\n");
	info_print     (sv_client->userinfo);

}
END_COMMAND (dumpuser)

/*
==============
SV_ServerRecord_f

Begins server demo recording.  Every entity and every message will be
recorded, but no playerinfo will be stored.  Primarily for demo merging.
==============
*/
BEGIN_COMMAND (serverrecord, NULL)
{
	char	   name[MAX_OSPATH];
	char	   buf_data[32768];
	Sizebuf	buf;
	int		len;
	int		i;

	if (argc != 2)
	{
		console.printf ("USAGE: serverrecord <demoname>\n");
		return;
	}

	if (svs.demofile)
	{
		console.printf ("Already recording.\n");
		return;
	}

	if (sv.state != ss_game)
	{
		console.printf ("You must be in a level to record.\n");
		return;
	}

	//
	// open the demo file
	//
	usprintf (name, "data/demos/%s.dem", argv[1].get_s ());

	console.printf ("recording to %s.\n", name);

	svs.demofile = pack_fopen (name, "wp");
	if (!svs.demofile)
	{
		console.printf (con_error, "ERROR: couldn't open.\n");
		return;
	}

	// setup a buffer to catch all multicasts
	sz_init (&svs.demo_multicast, svs.demo_multicast_buf, sizeof(svs.demo_multicast_buf));

	//
	// write a single giant fake message with all the startup info
	//
	sz_init (&buf, buf_data, sizeof(buf_data));

	//
	// serverdata needs to go over for all types of servers
	// to make sure the protocol is right, and to set the gamedir
	//
	// send the serverdata
	msg_write_byte (&buf, svc_serverdata);
	msg_write_long (&buf, PROTOCOL_VERSION);
	msg_write_long (&buf, svs.spawncount);
	// 2 means server demo
	msg_write_byte (&buf, 2);	// demos are always attract loops
	//msg_WriteString (&buf, Cvar_VariableString ("gamedir"));
	//msg_WriteShort (&buf, -1);
	// send full levelname
	msg_write_string (&buf, sv.configstrings[CS_NAME]);

	for (i=0 ; i<MAX_CONFIGSTRINGS ; i++)
		if (sv.configstrings[i][0])
		{
			msg_write_byte   (&buf, svc_configstring);
			msg_write_short  (&buf, i);
			msg_write_string (&buf, sv.configstrings[i]);
		}

	// write it to the demo file
	console.dprintf ("signon message length: %i\n", buf.cursize);
	len = little_long (buf.cursize);
	pack_fwrite (&len, 4, 1, svs.demofile);
	pack_fwrite (buf.data, buf.cursize, 1, svs.demofile);

	// the rest of the demo file will be individual frames
}
END_COMMAND (serverrecord)

/*
==============
SV_ServerStop_f

Ends server demo recording
==============
*/
BEGIN_COMMAND (serverstop, "Ends server demo recording")
void SV_ServerStop_f (void)
{
	if (!svs.demofile)
	{
		Com_Printf ("Not doing a serverrecord.\n");
		return;
	}
	fclose (svs.demofile);
	svs.demofile = NULL;
	Com_Printf ("Recording completed.\n");
}
END_COMMAND (serverstop)

/*
===============
SV_KillServer_f

Kick everyone off, possibly in preparation for a new game

===============
*/
BEGIN_COMMAND (killserver, "Kick everyone off")
{
	if (!svs.initialized)
		return;

	sv_shutdown ("Server was killed.\n", false);
	net_config ( false );	// close network sockets
}
END_COMMAND (killserver)

/*
===============
SV_ServerCommand_f

Let the game dll handle a command
===============
*/
BEGIN_COMMAND (sv, NULL)
{
	if (!ge)
	{
		console.printf ("No game loaded.\n");
		return;
	}

	ge->server_command();
}
END_COMMAND (sv_servercommand)

//===========================================================

/*
==================
SV_InitOperatorCommands
==================
*/
void SV_InitOperatorCommands (void)
{
	Cmd_AddCommand ("heartbeat", SV_Heartbeat_f);
	Cmd_AddCommand ("kick", SV_Kick_f);
	Cmd_AddCommand ("status", SV_Status_f);
	Cmd_AddCommand ("serverinfo", SV_Serverinfo_f);
	Cmd_AddCommand ("dumpuser", SV_DumpUser_f);

	Cmd_AddCommand ("map", SV_Map_f);
	Cmd_AddCommand ("demomap", SV_DemoMap_f);
	Cmd_AddCommand ("gamemap", SV_GameMap_f);
	Cmd_AddCommand ("setmaster", SV_SetMaster_f);

	if ( dedicated->value )
		Cmd_AddCommand ("say", SV_ConSay_f);

	Cmd_AddCommand ("serverrecord", SV_ServerRecord_f);
	Cmd_AddCommand ("serverstop", SV_ServerStop_f);

	Cmd_AddCommand ("save", SV_Savegame_f);
	Cmd_AddCommand ("load", SV_Loadgame_f);

	Cmd_AddCommand ("killserver", SV_KillServer_f);

	Cmd_AddCommand ("sv", SV_ServerCommand_f);
}

