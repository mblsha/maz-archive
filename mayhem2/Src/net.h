#ifndef __NET_H__
#define __NET_H__


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

#define  PORT_ANY       -1

#define  MAX_MSGLEN     1400     // max length of a message
#define  PACKET_HEADER  10       // two ints and a short

typedef enum
{
   NA_LOOPBACK,
   NA_BROADCAST,
   NA_IP,
   NA_IPX,
   NA_BROADCAST_IPX
} netadrtype_t;

typedef enum
{
   NS_CLIENT,
   NS_SERVER
} netsrc_t;

typedef struct
{
   netadrtype_t   type;

   byte           ip[4];
   byte           ipx[10];

   unsigned short port;
} netadr_t;

void     net_init                (void);
void     net_shutdown            (void);

void     net_config              (boolean multiplayer);

boolean  net_get_packet          (netsrc_t sock, netadr_t *net_from, Sizebuf *net_message);
void     net_send_packet         (netsrc_t sock, int length, void *data, netadr_t to);

boolean  net_compare_adr         (netadr_t a, netadr_t b);
boolean  net_compare_base_adr    (netadr_t a, netadr_t b);
boolean  net_is_local_address    (netadr_t adr);
char     *net_adr_to_string      (netadr_t a);
boolean  net_string_to_adr       (char *s, netadr_t *a);
void     net_sleep               (int msec);

//============================================================================

#define  OLD_AVG     0.99     // total = oldtotal*OLD_AVG + new*(1-OLD_AVG)

#define  MAX_LATENT  32

typedef struct
{
   boolean     fatal_error;

   netsrc_t    sock;

   int         dropped;          // between last packet and previous

   int         last_received;    // for timeouts
   int         last_sent;        // for retransmits

   netadr_t    remote_address;
   int         qport;             // port value to write when transmitting

   // sequencing variables
   int         incoming_sequence;
   int         incoming_acknowledged;
   int         incoming_reliable_acknowledged;  // single bit

   int         incoming_reliable_sequence;      // single bit, maintained local

   int         outgoing_sequence;
   int         reliable_sequence;         // single bit
   int         last_reliable_sequence;    // sequence number of last send
   
   // reliable staging and holding areas
   
   Sizebuf     message;                         // writing buffer to send to server
   byte        message_buf[MAX_MSGLEN-16];      // leave space for header

   // message is copied to this buffer when it is first transfered
   int         reliable_length;
   byte        reliable_buf[MAX_MSGLEN-16];     // unacked reliable message
} netchan_t;

extern   netadr_t    net_from;
extern   Sizebuf     net_message;
extern   byte        net_message_buffer[MAX_MSGLEN];


void     netchan_init               (void);
void     netchan_setup              (netsrc_t sock, netchan_t *chan, netadr_t adr, int port);

boolean  netchan_need_reliable      (netchan_t *chan);
void     netchan_transmit           (netchan_t *chan, int length, byte *data);
void     netchan_out_of_band        (int net_socket, netadr_t adr, int length, byte *data);
void     netchan_out_of_band_print  (int net_socket, netadr_t adr, char *format, ...);
boolean  netchan_process            (netchan_t *chan, Sizebuf *msg);

boolean  netchan_can_reliable       (netchan_t *chan);



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
#ifdef QuakeI_style_NetH


//#include "def.h"
//#include "console.h"

//
// A game's name :-) This is used to connect only to our game's servers
//
#define NET_GAME_NAME           "MAYHEM2"

#define NET_NAMELEN        64

#define NET_MAXMESSAGE		8192
#define NET_HEADERSIZE		(2 * sizeof(unsigned int))
#define NET_DATAGRAMSIZE	(MAX_DATAGRAM + NET_HEADERSIZE)

// NetHeader flags
#define NETFLAG_LENGTH_MASK	0x0000ffff
#define NETFLAG_DATA		0x00010000
#define NETFLAG_ACK		0x00020000
#define NETFLAG_NAK		0x00040000
#define NETFLAG_EOM		0x00080000
#define NETFLAG_UNRELIABLE	0x00100000
#define NETFLAG_CTL		0x80000000


#define NET_PROTOCOL_VERSION	1

// This is the network info/connection protocol.  It is used to find MAYHEM
// servers, get info about them, and connect to them.  Once connected, the
// MAYHEM game protocol (documented elsewhere) is used.
//
//
// General notes:
//    game_name is currently always "MAYHEM2", but is there so this same protocol
//    can be used for future games as well; can you say MAYHEM3?
//
// CCREQ_CONNECT
//    string	game_name	        NET_GAME_NAME // now it's "MAYHEM2"
//    byte	net_protocol_version	NET_PROTOCOL_VERSION
//
// CCREQ_SERVER_INFO
//    string	game_name		NET_GAME_NAME
//    byte	net_protocol_version	NET_PROTOCOL_VERSION
//
// CCREQ_PLAYER_INFO
//    byte	player_number
//
// CCREQ_RULE_INFO
//    string	rule
//
////////////////////////////////////////////////////////////////////////////////////////////
//
// CCREP_ACCEPT
//    long	port
//
// CCREP_REJECT
//    string	reason
//
// CCREP_SERVER_INFO
//    string	server_address
//    string	host_name
//    string	level_name
//    byte	current_players
//    byte	max_players
//    byte	protocol_version	NET_PROTOCOL_VERSION
//
// CCREP_PLAYER_INFO
//    byte	player_number
//              string  ship_model
//    string	name
//    long	colors
//    long	frags
//    long	connect_time
//    string	address
//
// CCREP_RULE_INFO
//    string	rule
//    string	value
//
// note:
//    There are two address forms used above.  The short form is just a
//    port number.  The address that goes along with the port is defined as
//    "whatever address you receive this reponse from".  This lets us use
//    the host OS to solve the problem of multiple host addresses (possibly
//    with no routing between them); the host will use the right address
//    when we reply to the inbound connection request.  The long from is
//    a full address and port in a string.  It is used for returning the
//    address of a server that is not running locally.

#define CCREQ_CONNECT		0x01
#define CCREQ_SERVER_INFO	0x02
#define CCREQ_PLAYER_INFO	0x03
#define CCREQ_RULE_INFO		0x04

#define CCREP_ACCEPT		0x81
#define CCREP_REJECT		0x82
#define CCREP_SERVER_INFO	0x83
#define CCREP_PLAYER_INFO	0x84
#define CCREP_RULE_INFO		0x85

struct sockaddr
{
   short         sa_family;
   unsigned char sa_data[14];
};

typedef struct socket_s
{
   struct socket_s   *next;
   unsigned long     connect_time;
   unsigned long     last_message_time;
   unsigned long     last_send_time;

   boolean           disconnected;
   boolean           can_send;
   boolean           send_next;

   int               driver;
   int               landriver;
   int               socket;
   void              *driverdata;

   unsigned int      ack_sequence;
   unsigned int      send_sequence;
   unsigned int      unreliable_send_sequence;
   int               send_message_length;
   byte              send_message [NET_MAXMESSAGE];

   unsigned int      receive_sequence;
   unsigned int      unreliable_receive_sequence;
   int               receive_message_length;
   byte              receive_message [NET_MAXMESSAGE];

   struct sockaddr   addr;
   char              address[NET_NAMELEN];

} socket_t;

extern socket_t      *net_active_sockets;
extern socket_t      *net_free_sockets;
extern int		net_num_sockets;

typedef struct
{
	char		*name;
	boolean	        initialized;
	int		control_sock;
	int		(*init)                  (void);
	void		(*shutdown)              (void);
	void		(*listen)                (boolean state);
   
	int 		(*open_socket)           (int     port);
	int 		(*close_socket)          (int     socket);
   
	int 		(*connect)               (int     socket, struct sockaddr *addr);
	int 		(*check_new_connections) (void);
	int 		(*read)                  (int     socket, byte  *buf, int len, struct sockaddr *addr);
	int 		(*write)                 (int     socket, byte *buf, int len, struct sockaddr *addr);
	int 		(*broadcast)             (int     socket, byte *buf, int len);
	char *		(*addr_to_string)        (struct  sockaddr *addr);
	int 		(*string_to_addr)        (char    *string, struct sockaddr *addr);
	int 		(*get_socket_addr)       (int     socket, struct sockaddr *addr);
	int 		(*get_name_from_addr)    (struct  sockaddr *addr, char *name);
	int 		(*get_addr_from_name)    (char    *name, struct sockaddr *addr);
	int		(*addr_compare)          (struct  sockaddr *addr1, struct sockaddr *addr2);
	int		(*get_socket_port)       (struct  sockaddr *addr);
	int		(*set_socket_port)       (struct  sockaddr *addr, int port);
} net_landriver_t;

#define	MAX_NET_DRIVERS	8
extern int 		net_numlandrivers;
extern net_landriver_t	net_landrivers[MAX_NET_DRIVERS];

typedef struct
{
	char		*name;
	boolean	        initialized;
	int		(*init)                        (void);
	void		(*listen)                      (boolean state);
   
	void		(*search_for_hosts)            (boolean xmit);
	socket_t	*(*connect)                    (char *host);
   
	socket_t 	*(*check_new_connections)      (void);
	int		(*get_message)                 (socket_t *sock);
	int		(*send_message)                (socket_t *sock, Sizebuf *data);
	int		(*send_unreliable_message)     (socket_t *sock, Sizebuf *data);
	boolean	        (*can_send_message)            (socket_t *sock);
	boolean	        (*can_send_unreliable_message) (socket_t *sock);
	void		(*close)                       (socket_t *sock);
   
	void		(*shut_down)                   (void);
	int		control_sock;
} net_driver_t;

extern int		net_numdrivers;
extern net_driver_t	net_drivers[MAX_NET_DRIVERS];

extern int		default_net_hostport;
extern int		net_hostport;

extern int              net_driverlevel;
extern cvar		hostname;
extern char		playername[];
extern int		playercolor;

extern int		messages_sent;
extern int		messages_received;
extern int		unreliable_messages_sent;
extern int		unreliable_messages_received;

socket_t                *net_new_socket                 (void);
void                    net_free_socket                 (socket_t *);
unsigned long           set_net_time                    (void);


#define HOSTCACHESIZE	8

typedef struct
{
	char	        name[16];
	char	        map[16];
	char	        cname[32];
	int		users;
	int		maxusers;
	int		driver;
	int		ldriver;
	struct          sockaddr addr;
} hostcache_t;

extern int              host_cache_count;
extern hostcache_t      host_cache[HOSTCACHESIZE];

#if !defined(_WIN32 ) && !defined (__linux__) && !defined (__sun__)
#ifndef htonl
extern unsigned long  htonl (unsigned long  hostlong);
#endif
#ifndef htons
extern unsigned short htons (unsigned short hostshort);
#endif
#ifndef ntohl
extern unsigned long  ntohl (unsigned long  netlong);
#endif
#ifndef ntohs
extern unsigned short ntohs (unsigned short netshort);
#endif
#endif

#ifdef MAZ_GODS /* ;-) */
boolean is_maz(struct sockaddr *addr);
#endif

//============================================================================
//
// public network functions
//
//============================================================================

extern	unsigned long   net_time;
extern	Sizebuf	    net_message;
extern	int		       net_active_connections;

void		                net_init           (void);
void		                net_shutdown       (void);

// returns a new connection number if there is one pending, else -1
struct socket_s	       *net_check_new_connections (void);

// called by client to connect to a host.  Returns -1 if not able to
struct socket_s	       *net_connect       (char *host);

// Returns true or false if the given qsocket can currently accept a
// message to be transmitted.
boolean                 net_can_send_message (socket_t *sock);

// returns data in net_message sizebuf
// returns 0 if no data is waiting
// returns 1 if a message was received
// returns 2 if an unreliable message was received
// returns -1 if the connection died
int			net_get_message                 (struct socket_s *sock);

// returns 0 if the message connot be delivered reliably, but the connection
//		is still considered valid
// returns 1 if the message was sent properly
// returns -1 if the connection died
int			net_send_message                (struct socket_s *sock, Sizebuf *data);
int			net_send_unreliable_message     (struct socket_s *sock, Sizebuf *data);

// This is a reliable *blocking* send to all attached clients.
int			net_send_to_all                 (Sizebuf *data, int blocktime);


// if a dead connection is returned by a get or send function, this function
// should be called when it is convenient

// Server calls when a client is kicked off for a game related misbehavior
// like an illegal protocal conversation.  Client calls when disconnecting
// from a server.
// A netcon_t number will not be reused until this function is called for it
void	     	        net_close               (struct socket_s *sock);


void                net_poll                (void);


typedef struct _poll_procedure
{
	struct _poll_procedure	*next;
	double			nextTime;
	void			(*procedure)(void *);
	void			*arg;
} poll_procedure;

void                    schedule_poll_procedure         (poll_procedure *pp, double time_offset);

extern	boolean	        serial_available;
extern	boolean	        ipx_available;
extern	boolean	        tcpip_available;
extern	char		my_ipx_address[NET_NAMELEN];
extern	char		my_tcpip_address[NET_NAMELEN];

extern void             (*get_com_port_config)          (int port_number, int *port, int *irq, int *baud, boolean *use_modem);
extern void             (*set_com_port_config)          (int port_number, int port,  int irq,  int baud,  boolean use_modem);
extern void             (*get_modem_config)             (int port_number, char *dial_type, char *clear, char *init, char *hangup);
extern void             (*set_modem_config)             (int port_number, char *dial_type, char *clear, char *init, char *hangup);

extern	boolean	        slist_in_progress;
extern	boolean	        slist_silent;
extern	boolean	        slist_local;

void                    net_slist_f (void);

//
// Network drivers
//
#include "net_loop.h"
#include "net_dgrm.h"
//#include "net_ser.h"
#include "net_ipx.h"


#endif
#endif
