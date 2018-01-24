#ifndef __SERVER_H__
#define __SERVER_H__

//=============================================================================

#define	MAX_MASTERS	8				// max recipients for heartbeat packets

typedef enum 
{
	ss_dead,	      // no map loaded
	ss_loading,		// spawning level edicts
	ss_game,			// actively running
	ss_cinematic,
	ss_demo,
	ss_pic
} Server_state;
// some qc commands are only valid before the server has finished
// initializing (precache commands, static sounds / objects, etc)

typedef struct
{
	Server_state	state;			// precache commands are only valid during load

	boolean	      attractloop;	// running cinematics and demos for the local system only
	boolean	      loadgame;		// client begins should reuse existing entity

	unsigned	      time;				// always sv.framenum * 100 msec
	int			   framenum;

	char		      name[MAX_QPATH];			// map name, or cinematic name
	struct cmodel_s		*models[MAX_MODELS];

	char		configstrings[MAX_CONFIGSTRINGS][MAX_QPATH];
	entity_state_t	baselines[MAX_EDICTS];

	// the multicast buffer is used to send a message to a set of clients
	// it is only used to marshall data until SV_Multicast is called
	Sizebuf	      multicast;
	byte		      multicast_buf[MAX_MSGLEN];

	// demo server information
	FILE		*demofile;
	boolean	timedemo;		// don't time sync
} Server;



typedef enum
{
	cs_free,		   // can be reused for a new connection
	cs_zombie,		// client has been disconnected, but don't reuse
					   // connection for a couple seconds
	cs_connected,	// has been assigned to a client_t, but not in game yet
	cs_spawned		// client is fully in game
} Client_state;

typedef struct
{
	int					areabytes;
	byte				   areabits[MAX_MAP_AREAS/8];		// portalarea visibility bits
	player_state		ps;
	int					num_entities;
	int					first_entity;		// into the circular sv_packet_entities[]
	int					senttime;			// for ping calculations
} Client_frame;


#define	LATENCY_COUNTS	16
#define	RATE_MESSAGES	10

typedef struct
{
	Client_state	state;

	char			   userinfo[MAX_INFO_STRING];		// name, etc

	int				lastframe;		// for delta compression
	usercmd_t		lastcmd;			// for filling in big drops

	int				command_msec;	// every seconds this is reset, if user
										   // commands exhaust it, assume time cheating

	int				frame_latency[LATENCY_COUNTS];
	int				ping;

	int				message_size[RATE_MESSAGES];	// used to rate drop packets
	int				rate;
	int				surpress_count;	// number of messages rate supressed

	edict_t			*edict;				// EDICT_NUM(clientnum+1)
	char			   name[32*2];			// extracted from userinfo, high bits masked
	int				messagelevel;		// for filtering printed messages

	// The datagram is written to by sound calls, prints, temp ents, etc.
	// It can be harmlessly overflowed.
	Sizebuf		   datagram;
	byte			   datagram_buf[MAX_MSGLEN];

	Client_frame	frames[UPDATE_BACKUP];	// updates can be delta'd from here

	byte			   *download;			// file being downloaded
	int				downloadsize;		// total bytes (can't use EOF because of paks)
	int				downloadcount;		// bytes sent

	int				lastmessage;		// sv.framenum when packet was last received
	int				lastconnect;

	int				challenge;			// challenge of this user, randomly generated

	Netchan		   netchan;
} Client;

// a client can leave the server in one of four ways:
// dropping properly by quiting or disconnecting
// timing out if no valid messages are received for timeout.value seconds
// getting kicked off by the server operator
// a program error, like an overflowed reliable buffer

//=============================================================================

// MAX_CHALLENGES is made large to prevent a denial
// of service attack that could cycle all of them
// out before legitimate users connected
#define	MAX_CHALLENGES	1024

typedef struct
{
	Netadr	   adr;
	int			challenge;
	int			time;
} Challenge;


typedef struct
{
	boolean	   initialized;				// sv_init has completed
	int			realtime;					// always increasing, no clamping, etc

	char		   mapcmd[MAX_TOKEN_CHARS];	// ie: *intro.cin+base 

	int			spawncount;					// incremented each server start
											      // used to check late spawns

	Client	   *clients;					// [maxclients->value];
	int			num_client_entities;		// maxclients->value*UPDATE_BACKUP*MAX_PACKET_ENTITIES
	int			next_client_entities;	// next client_entity to use
	entity_state_t	*client_entities;		// [num_client_entities]

	int			last_heartbeat;

	Challenge	challenges[MAX_CHALLENGES];	// to prevent invalid IPs from connecting

	// serverrecord values
	PACKFILE	   *demofile;
	Sizebuf	   demo_multicast;
	byte		   demo_multicast_buf[MAX_MSGLEN];
} Server_static;

//=============================================================================

extern	Netadr	   net_from;
extern	Sizebuf	   net_message;

extern	Netadr	   master_adr[MAX_MASTERS];	// address of the master server

extern	Server_static	svs;				// persistant server info
extern	Server		   sv;				// local server

extern	cvar_t		*sv_paused;
extern	cvar_t		*maxclients;
extern	cvar_t		*sv_noreload;			// don't reload level state when reentering
extern	cvar_t		*sv_airaccelerate;		// don't reload level state when reentering
											// development tool
extern	cvar_t		*sv_enforcetime;

extern	Client	   *sv_client;
extern	edict_t		*sv_player;

//===========================================================

//
// sv_main.c
//
void sv_final_message (char *message, qboolean reconnect);
void sv_drop_client (client_t *drop);

int sv_model_index (char *name);
int sv_sound_index (char *name);
int sv_image_index (char *name);

void sv_write_clientdata_to_message (Client *client, Sizebuf *msg);

void sv_execute_user_command     (char *s);
void sv_init_operator_commands   (void);

void sv_send_serverinfo          (Client *client);
void sv_userinfo_changed         (Client *cl);

void master_heartbeat (void);
void master_packet (void);

//
// sv_init.c
//
void sv_init_game    (void);
void sv_map          (boolean attractloop, char *levelstring, boolean loadgame);


//
// sv_phys.c
//
void sv_prep_world_frame (void);

//
// sv_send.c
//
typedef enum 
{
   rd_none,
   rd_client,
   rd_packet
} redirect_t;

#define	SV_OUTPUTBUF_LENGTH	(MAX_MSGLEN - 16)

extern char sv_outputbuf[SV_OUTPUTBUF_LENGTH];

void sv_flush_redirect (int sv_redirected, char *outputbuf);

void sv_demo_completed        (void);
void sv_send_client_messages  (void);

void sv_multicast    (Vec2 origin, multicast_t to);
void sv_start_sound  (Vec2 origin, edict_t *entity, int channel,
					int soundindex, float volume,
					float attenuation, float timeofs);
void sv_client_printf      (Client *cl, int level, char *fmt, ...);
void sv_broadcast_printf   (int level, char *fmt, ...);
void sv_broadcast_command  (char *fmt, ...);

//
// sv_user.c
//
void sv_next_server (void);
void sv_execute_client_message (client_t *cl);

//
// sv_ccmds.c
//
void sv_read_level_file (void);
void sv_status_f (void);

//
// sv_ents.c
//
void sv_write_frame_to_client    (Client *client, Sizebuf *msg);
void sv_record_demo_message      (void);
void sv_build_client_frame       (Client *client);


void sv_error (char *error, ...);

//
// sv_game.c
//
extern	game_export_t	*ge;

void sv_InitGameProgs (void);
void sv_ShutdownGameProgs (void);
void sv_InitEdict (edict_t *e);



//============================================================

//
// high level object sorting to reduce interaction tests
//

void SV_ClearWorld (void);
// called after the world model has been loaded, before linking any entities

void SV_UnlinkEdict (edict_t *ent);
// call before removing an entity, and before trying to move one,
// so it doesn't clip against itself

void SV_LinkEdict (edict_t *ent);
// Needs to be called any time an entity changes origin, mins, maxs,
// or solid.  Automatically unlinks if needed.
// sets ent->v.absmin and ent->v.absmax
// sets ent->leafnums[] for pvs determination even if the entity
// is not solid

int SV_AreaEdicts (vec3_t mins, vec3_t maxs, edict_t **list, int maxcount, int areatype);
// fills in a table of edict pointers with edicts that have bounding boxes
// that intersect the given area.  It is possible for a non-axial bmodel
// to be returned that doesn't actually intersect the area on an exact
// test.
// returns the number of pointers filled in
// ??? does this always return the world?

//===================================================================

//
// functions that interact with everything apropriate
//
int SV_PointContents (vec3_t p);
// returns the CONTENTS_* value from the world at the given point.
// Quake 2 extends this to also check entities, to allow moving liquids


trace_t SV_Trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passedict, int contentmask);
// mins and maxs are relative

// if the entire move stays in a solid volume, trace.allsolid will be set,
// trace.startsolid will be set, and trace.fraction will be 0

// if the starting point is in a solid, it will be allowed to move out
// to an open area

// passedict is explicitly excluded from clipping checks (normally NULL)


//===================================================================
//===================================================================
//===================================================================

#ifdef DUMMY

typedef struct
{
   int			max_clients;
   int			max_clients_limit;
   struct      client_s	*clients;		// [maxclients]
   int			server_flags;		      // level completion information
   boolean	   change_level_issued;	   // cleared when at sv_spawn_server
} server_static_t;

//=============================================================================

typedef enum { ss_loading, ss_active } server_state_t;

typedef struct
{
   boolean	        active;				// false if only a net client

   boolean	        paused;
   boolean	        loadgame;			   // handle connections specially

   unsigned long	  time;
	
   int		        last_check;			// used by pf_checkclient
   unsigned long	  last_check_time;
	
   char		        name[64];			   // map name

//#ifdef QUAKE2
//   char		        startspot[64];
//#endif

   char		        map_name[255];	   // maps/<name>.bsp, for model_precache[0]

/*   struct model_s   *worldmodel;
   char		        *model_precache[MAX_MODELS];	// NULL terminated
   struct model_s	  *models[MAX_MODELS];
   char		        *sound_precache[MAX_SOUNDS];	// NULL terminated
   char		        *lightstyles[MAX_LIGHTSTYLES];
   int			num_edicts;
   int			max_edicts;
   edict_t		*edicts;			         // can NOT be array indexed, because
                                       // edict_t is variable sized, but can
*/                                       // be used to reference the world ent

   server_state_t	  state;			      // some actions are only valid during load

   sizebuf_t	     datagram;
   byte		        datagram_buf[MAX_DATAGRAM];

   sizebuf_t	     reliable_datagram; // copied to all clients at end of frame
   byte		        reliable_datagram_buf[MAX_DATAGRAM];

   sizebuf_t	     signon;
   byte		        signon_buf[8192];
} server_t;


#define	NUM_PING_TIMES		   16
#define	NUM_SPAWN_PARMS		16

typedef struct client_s
{
   boolean		     active;				      // false = client is free
   boolean		     spawned;			         // false = don't send datagrams
   boolean		     dropasap;			         // has been told to go to another level
   boolean		     privileged;			      // can execute any host command
   boolean		     send_signon;			      // only valid before spawned

   unsigned long	  last_message;	        	// reliable messages must be sent
							                        // periodically

   struct socket_s  *netconnection;         	// communications handle

   usercmd_t		  cmd;	          			// movement
   vec3_t		     wish_dir;			         // intended motion calced from cmd

   sizebuf_t		  message;			         // can be added to at any time,
                                             // copied and clear once per frame
   byte			     msgbuf[MAX_MSGLEN];

   SHIP             *ship;
//  edict_t		*edict;				// EDICT_NUM(clientnum+1)

   char			     name[32];			         // for printing to other people
   int			     colors;
		
   float         	  ping_times[NUM_PING_TIMES];
   int			     num_pings;			      // ping_times[num_pings%NUM_PING_TIMES]

   // spawn parms are carried from level to level
   float			     spawn_parms[NUM_SPAWN_PARMS];

   // client known data for deltas
   int			     old_frags;
} client_t;


//=============================================================================

// edict->movetype values
#define	MOVETYPE_NONE			   0		// never moves
#define	MOVETYPE_ANGLENOCLIP    1
#define	MOVETYPE_ANGLECLIP		2
#define	MOVETYPE_WALK			   3		// gravity
#define	MOVETYPE_STEP			   4		// gravity, special edge handling
#define	MOVETYPE_FLY			   5
#define	MOVETYPE_TOSS			   6		// gravity
#define	MOVETYPE_PUSH			   7		// no clip to world, push and crush
#define	MOVETYPE_NOCLIP			8
#define	MOVETYPE_FLYMISSILE		9		// extra size to monsters
#define	MOVETYPE_BOUNCE			10

#ifdef QUAKE2
#define MOVETYPE_BOUNCEMISSILE	11		// bounce w/o gravity
#define MOVETYPE_FOLLOW			   12		// track movement of aiment
#endif

// edict->solid values
#define	SOLID_NOT			      0		// no interaction with other objects
#define	SOLID_TRIGGER			   1		// touch on edge, but not blocking
#define	SOLID_BBOX			      2		// touch on edge, block
#define	SOLID_SLIDEBOX			   3		// touch on edge, but not an onground
#define	SOLID_BSP			      4		// bsp clip, touch on edge, block

// edict->deadflag values
#define	DEAD_NO				      0
#define	DEAD_DYING			      1
#define	DEAD_DEAD			      2

#define	DAMAGE_NO			      0
#define	DAMAGE_YES			      1
#define	DAMAGE_AIM			      2

// edict->flags
#define	FL_FLY				      1
#define	FL_SWIM				      2
//#define	FL_GLIMPSE			     4
#define	FL_CONVEYOR			      4
#define	FL_CLIENT			      8
#define	FL_INWATER			      16
#define	FL_MONSTER			      32
#define	FL_GODMODE			      64
#define	FL_NOTARGET			      128
#define	FL_ITEM				      256
#define	FL_ONGROUND			      512
#define	FL_PARTIALGROUND		   1024	// not all corners are valid
#define	FL_WATERJUMP			   2048	// player jumping out of water
#define	FL_JUMPRELEASED			4096	// for jump debouncing

#ifdef QUAKE2
#define FL_FLASHLIGHT			   8192
#define FL_ARCHIVE_OVERRIDE		1048576
#endif

// entity effects
#define	EF_BRIGHTFIELD			   1
#define	EF_MUZZLEFLASH 			2
#define	EF_BRIGHTLIGHT 			4
#define	EF_DIMLIGHT 			   8

#ifdef QUAKE2
#define EF_DARKLIGHT			      16
#define EF_DARKFIELD			      32
#define EF_LIGHT			         64
#define EF_NODRAW			         128
#endif

#define	SPAWNFLAG_NOT_EASY		   256
#define	SPAWNFLAG_NOT_MEDIUM		   512
#define	SPAWNFLAG_NOT_HARD		   1024
#define	SPAWNFLAG_NOT_DEATHMATCH	2048

#ifdef QUAKE2
// server flags
#define	SFL_EPISODE_1	     	     1
#define	SFL_EPISODE_2		        2
#define	SFL_EPISODE_3		        4
#define	SFL_EPISODE_4		        8
#define	SFL_NEW_UNIT		        16
#define	SFL_NEW_EPISODE		     32
#define	SFL_CROSS_TRIGGERS	     65280
#endif

//============================================================================

extern	cvar                   	  teamplay;
extern	cvar                      skill;
extern	cvar	                    deathmatch;
extern	cvar	                    coop;
extern	cvar	                    fraglimit;
extern	cvar	                    timelimit;

extern	server_static_t	        svs;				// persistant server info
extern	server_t		              sv;				   // local server

extern	client_t	                *host_client;

extern	jmp_buf 	                 host_abortserver;

extern	unsigned long	           host_time;

extern	edict_t	                *sv_player;

//===========================================================

void sv_init (void);

void sv_start_particle                  (vec3_t org, vec3_t dir, int color, int count);
void sv_start_sound                     (edict_t *entity, int channel, char *sample, int volume, float attenuation);

void sv_drop_client                     (boolean crash);

void sv_send_client_messages            (void);
void sv_clear_datagram                  (void);

int  sv_model_index                     (char *name);

void sv_set_ideal_pitch                 (void);

void sv_add_updates                     (void);

void sv_client_think                    (void);
void sv_add_client_to_server            (struct socket_s	*ret);

void sv_client_printf                   (char *fmt, ...);
void sv_broadcast_printf                (char *fmt, ...);

void sv_physics                         (void);

boolean sv_check_bottom                 (edict_t *ent);
boolean sv_move_step                    (edict_t *ent, vec3_t move, qboolean relink);

void sv_write_client_data_to_message    (edict_t *ent, sizebuf_t *msg);

void sv_move_to_goal                    (void);

void sv_check_for_new_clients           (void);
void sv_run_clients                     (void);
void sv_save_spawnparms                 ();
//#ifdef QUAKE2
//void sv_SpawnServer                     (char *server, char *startspot);
//#else
void sv_spawn_server                    (char *server);
//#endif


#endif // DUMMY
#endif
