
#define	MAX_LOOPBACK	4

typedef struct
{
   byte  data[MAX_MSGLEN];
   int   datalen;
} loopmsg_t;

typedef struct
{
   loopmsg_t   msgs[MAX_LOOPBACK];
   int         get, send;
} loopback_t;


cvar_t         *net_shownet;
static cvar_t  *noudp;
static cvar_t  *noipx;

loopback_t  loopbacks[2];
int         ip_sockets[2];
int         ipx_sockets[2];

char *net_error_string (void);

//=============================================================================

void netadr_to_sockadr (netadr_t *a, struct sockaddr *s)
{
   memset (s, 0, sizeof(*s));

   if (a->type == NA_BROADCAST)
   {
      ((struct sockaddr_in *)s)->sin_family = AF_INET;
      ((struct sockaddr_in *)s)->sin_port = a->port;
      ((struct sockaddr_in *)s)->sin_addr.s_addr = INADDR_BROADCAST;
   }
   else if (a->type == NA_IP)
   {
      ((struct sockaddr_in *)s)->sin_family = AF_INET;
      ((struct sockaddr_in *)s)->sin_addr.s_addr = *(int *)&a->ip;
      ((struct sockaddr_in *)s)->sin_port = a->port;
   }
   else if (a->type == NA_IPX)
   {
      ((struct sockaddr_ipx *)s)->sa_family = AF_IPX;
      memcpy(((struct sockaddr_ipx *)s)->sa_netnum, &a->ipx[0], 4);
      memcpy(((struct sockaddr_ipx *)s)->sa_nodenum, &a->ipx[4], 6);
      ((struct sockaddr_ipx *)s)->sa_socket = a->port;
   }
   else if (a->type == NA_BROADCAST_IPX)
   {
      ((struct sockaddr_ipx *)s)->sa_family = AF_IPX;
      memset(((struct sockaddr_ipx *)s)->sa_netnum, 0, 4);
      memset(((struct sockaddr_ipx *)s)->sa_nodenum, 0xff, 6);
      ((struct sockaddr_ipx *)s)->sa_socket = a->port;
   }
}

void sockadr_to_netadr (struct sockaddr *s, netadr_t *a)
{
   if (s->sa_family == AF_INET)
   {
      a->type = NA_IP;
      *(int *)&a->ip = ((struct sockaddr_in *)s)->sin_addr.s_addr;
      a->port = ((struct sockaddr_in *)s)->sin_port;
   }
   else if (s->sa_family == AF_IPX)
   {
      a->type = NA_IPX;
      memcpy(&a->ipx[0], ((struct sockaddr_ipx *)s)->sa_netnum, 4);
      memcpy(&a->ipx[4], ((struct sockaddr_ipx *)s)->sa_nodenum, 6);
      a->port = ((struct sockaddr_ipx *)s)->sa_socket;
   }
}

/*
===================
NET_CompareBaseAdr

Compares without the port
===================
*/
boolean  net_compare_base_adr (netadr_t a, netadr_t b)
{
   if (a.type != b.type)
      return false;

   if (a.type == NA_LOOPBACK)
      return TRUE;

   if (a.type == NA_IP)
   {
      if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3])
         return true;
      return false;
   }

   if (a.type == NA_IPX)
   {
      if ((memcmp(a.ipx, b.ipx, 10) == 0))
         return true;
      return false;
   }
}

boolean  net_compare_adr (netadr_t a, netadr_t b)
{
   if (net_compare_base_adr (a, b))
   {
      if (a.type == NA_IP || a.type == NA_IPX)
      {
         if (a.port == b.port)
            return true;

         return false;
      }

      return true;
   }

   return false;
}


char  *net_adr_to_string (netadr_t a)
{
   static   char  s[64];

   if (a.type == NA_LOOPBACK)
      sprintf (s, "loopback");
   else if (a.type == NA_IP)
      sprintf (s, "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));
   else
      sprintf (s, "%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x:%i", a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9], ntohs(a.port));

   return s;
}


/*
=============
NET_StringToAdr

localhost
idnewt
idnewt:28000
192.246.40.70
192.246.40.70:28000
=============
*/
#define DO(src,dest)                               \
        copy[0] = s[src];                          \
        copy[1] = s[src + 1];                      \
        sscanf (copy, "%x", &val);                 \
        ((struct sockaddr_ipx *)sadr)->dest = val

boolean  net_string_to_sockaddr (char *s, struct sockaddr *sadr)
{
   struct hostent *h;
   char   *colon;
   int    val;
   char   copy[128];

   memset (sadr, 0, sizeof(*sadr));

   if ((strlen(s) >= 23) && (s[8] == ':') && (s[21] == ':'))   // check for an IPX address
   {
      ((struct sockaddr_ipx *)sadr)->sa_family = AF_IPX;
      copy[2] = 0;
      DO(0, sa_netnum[0]);
      DO(2, sa_netnum[1]);
      DO(4, sa_netnum[2]);
      DO(6, sa_netnum[3]);
      DO(9, sa_nodenum[0]);
      DO(11, sa_nodenum[1]);
      DO(13, sa_nodenum[2]);
      DO(15, sa_nodenum[3]);
      DO(17, sa_nodenum[4]);
      DO(19, sa_nodenum[5]);
      sscanf (&s[22], "%u", &val);
      ((struct sockaddr_ipx *)sadr)->sa_socket = htons((unsigned short)val);
   }
   else
   {
      ((struct sockaddr_in *)sadr)->sin_family = AF_INET;

      ((struct sockaddr_in *)sadr)->sin_port = 0;

      strcpy (copy, s);
      // strip off a trailing :port if present
      for (colon = copy ; *colon ; colon++)
      {
         if (*colon == ':')
         {
            *colon = 0;
            ((struct sockaddr_in *)sadr)->sin_port = htons((short)atoi(colon+1));
         }
      }
      
      if (copy[0] >= '0' && copy[0] <= '9')
      {
         *(int *)&((struct sockaddr_in *)sadr)->sin_addr = inet_addr(copy);
      }
      else
      {
         if (! (h = gethostbyname(copy)) )
            return false;
         *(int *)&((struct sockaddr_in *)sadr)->sin_addr = *(int *)h->h_addr_list[0];
      }
   }

   return true;
}

#undef DO

/*
=============
NET_StringToAdr

localhost
idnewt
idnewt:28000
192.246.40.70
192.246.40.70:28000
=============
*/
boolean  net_string_to_adr (char *s, netadr_t *a)
{
   struct sockaddr sadr;

   if (!strcmp (s, "localhost"))
   {
      memset (a, 0, sizeof(*a));
      a->type = NA_LOOPBACK;
      return true;
   }

   if (!net_string_to_sockaddr (s, &sadr))
      return false;

   sockadr_to_netadr (&sadr, a);

   return true;
}


boolean  net_is_local_address (netadr_t adr)
{
   return adr.type == NA_LOOPBACK;
}

/*
=============================================================================

LOOPBACK BUFFERS FOR LOCAL PLAYER

=============================================================================
*/

boolean  net_get_loop_packet (netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
   int         i;
   loopback_t  *loop;

   loop = &loopbacks[sock];

   if (loop->send - loop->get > MAX_LOOPBACK)
      loop->get = loop->send - MAX_LOOPBACK;

   if (loop->get >= loop->send)
      return false;

   i = loop->get & (MAX_LOOPBACK-1);
   loop->get++;

   memcpy (net_message->data, loop->msgs[i].data, loop->msgs[i].datalen);
   net_message->cursize = loop->msgs[i].datalen;
   memset (net_from, 0, sizeof(*net_from));
   net_from->type = NA_LOOPBACK;
   return true;

}


void net_send_loop_packet (netsrc_t sock, int length, void *data, netadr_t to)
{
   int         i;
   loopback_t  *loop;

   loop = &loopbacks[sock^1];

   i = loop->send & (MAX_LOOPBACK-1);
   loop->send++;

   memcpy (loop->msgs[i].data, data, length);
   loop->msgs[i].datalen = length;
}

//=============================================================================

boolean  net_get_packet (netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
   int               ret;
   struct sockaddr   from;
   int               fromlen;
   int               net_socket;
   int               protocol;
   int               err;

   if (net_get_loop_packet (sock, net_from, net_message))
      return true;

   for (protocol = 0 ; protocol < 2 ; protocol++)
   {
      if (protocol == 0)
         net_socket = ip_sockets[sock];
      else
         net_socket = ipx_sockets[sock];

      if (!net_socket)
         continue;

      fromlen = sizeof(from);
      ret = recvfrom (net_socket, net_message->data, net_message->maxsize
                      , 0, (struct sockaddr *)&from, &fromlen);
                      
      if (ret == -1)
      {
         err = WSAGetLastError();

         if (err == WSAEWOULDBLOCK)
            continue;
         if (dedicated->value)   // let dedicated servers continue after errors
            Com_Printf ("NET_GetPacket: %s", NET_ErrorString());
         else
            Com_Error (ERR_DROP, "NET_GetPacket: %s", NET_ErrorString());
         continue;
      }

      sockadr_to_netadr (&from, net_from);

      if (ret == net_message->maxsize)
      {
         console.printf ("Oversize packet from %s\n", net_adr_to_string (*net_from));
         continue;
      }

      net_message->cursize = ret;
      return true;
   }

   return false;
}

//=============================================================================

void net_send_packet (netsrc_t sock, int length, void *data, netadr_t to)
{
   int               ret;
   struct sockaddr   addr;
   int               net_socket;

   if ( to.type == NA_LOOPBACK )
   {
      net_send_loop_packet (sock, length, data, to);
      return;
   }

   if (to.type == NA_BROADCAST)
   {
      net_socket = ip_sockets[sock];
      if (!net_socket)
         return;
   }
   else if (to.type == NA_IP)
   {
      net_socket = ip_sockets[sock];
      if (!net_socket)
         return;
   }
   else if (to.type == NA_IPX)
   {
      net_socket = ipx_sockets[sock];
      if (!net_socket)
         return;
   }
   else if (to.type == NA_BROADCAST_IPX)
   {
      net_socket = ipx_sockets[sock];
      if (!net_socket)
         return;
   }
   else
      com_error (err_fatal, "net_send_packet: bad address type");

   netadr_to_sockadr (&to, &addr);

   ret = sendto (net_socket, data, length, 0, &addr, sizeof(addr) );
   if (ret == -1)
   {
      int err = WSAGetLastError();

      // wouldblock is silent
      if (err == WSAEWOULDBLOCK)
         return;

      // some PPP links dont allow broadcasts
      if ((err == WSAEADDRNOTAVAIL) && ((to.type == NA_BROADCAST) || (to.type == NA_BROADCAST_IPX)))
         return;

      if (dedicated->value)   // let dedicated servers continue after errors
      {
         console.printf ("net_send_packet ERROR: %s\n", net_error_string());
      }
      else
      {
         if (err == WSAEADDRNOTAVAIL)
         {
            console.dprintf ("net_send_packet Warning: %s : %s\n", net_error_string(), net_adr_to_string (to));
         }
         else
         {
            com_error (err_drop, "net_send_packet ERROR: %s\n", net_error_string());
         }
      }
   }
}


//=============================================================================


/*
====================
NET_Socket
====================
*/
int net_ip_socket (char *net_interface, int port)
{
   int                  newsocket;
   struct sockaddr_in   address;
   boolean              _true = true;
   int                  i = 1;
   int                  err;

   if ((newsocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
   {
      err = WSAGetLastError();
      if (err != WSAEAFNOSUPPORT)
         console.printf ("WARNING: UDP_OpenSocket: socket: %s", NET_ErrorString());
      return 0;
   }

   // make it non-blocking
   if (ioctlsocket (newsocket, FIONBIO, &_true) == -1)
   {
      Com_Printf ("WARNING: UDP_OpenSocket: ioctl FIONBIO: %s\n", NET_ErrorString());
      return 0;
   }

   // make it broadcast capable
   if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == -1)
   {
      Com_Printf ("WARNING: UDP_OpenSocket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString());
      return 0;
   }

   if (!net_interface || !net_interface[0] || !stricmp(net_interface, "localhost"))
      address.sin_addr.s_addr = INADDR_ANY;
   else
      NET_StringToSockaddr (net_interface, (struct sockaddr *)&address);

   if (port == PORT_ANY)
      address.sin_port = 0;
   else
       address.sin_port = htons((short)port);

   address.sin_family = AF_INET;

   if( bind (newsocket, (void *)&address, sizeof(address)) == -1)
   {
      Com_Printf ("WARNING: UDP_OpenSocket: bind: %s\n", NET_ErrorString());
      closesocket (newsocket);
      return 0;
   }

   return newsocket;
}


/*
====================
NET_OpenIP
====================
*/
void net_open_ip (void)
{
   cvar_t   *ip;
   int      port;
   int      dedicated;

	ip = Cvar_Get ("ip", "localhost", CVAR_NOSET);

	dedicated = Cvar_VariableValue ("dedicated");

	if (!ip_sockets[NS_SERVER])
	{
		port = Cvar_Get("ip_hostport", "0", CVAR_NOSET)->value;
		if (!port)
		{
			port = Cvar_Get("hostport", "0", CVAR_NOSET)->value;
			if (!port)
			{
				port = Cvar_Get("port", va("%i", PORT_SERVER), CVAR_NOSET)->value;
			}
		}
		ip_sockets[NS_SERVER] = NET_IPSocket (ip->string, port);
		if (!ip_sockets[NS_SERVER] && dedicated)
			Com_Error (ERR_FATAL, "Couldn't allocate dedicated server IP port");
	}


	// dedicated servers don't need client ports
	if (dedicated)
		return;

	if (!ip_sockets[NS_CLIENT])
	{
		port = Cvar_Get("ip_clientport", "0", CVAR_NOSET)->value;
		if (!port)
		{
			port = Cvar_Get("clientport", va("%i", PORT_CLIENT), CVAR_NOSET)->value;
			if (!port)
				port = PORT_ANY;
		}
		ip_sockets[NS_CLIENT] = NET_IPSocket (ip->string, port);
		if (!ip_sockets[NS_CLIENT])
			ip_sockets[NS_CLIENT] = NET_IPSocket (ip->string, PORT_ANY);
	}
}


/*
====================
IPX_Socket
====================
*/
int net_ipx_socket (int port)
{
   int                  newsocket;
   struct sockaddr_ipx  address;
   int                  _true = 1;
   int                  err;

	if ((newsocket = socket (PF_IPX, SOCK_DGRAM, NSPROTO_IPX)) == -1)
	{
		err = WSAGetLastError();
		if (err != WSAEAFNOSUPPORT)
			Com_Printf ("WARNING: IPX_Socket: socket: %s\n", NET_ErrorString());
		return 0;
	}

	// make it non-blocking
	if (ioctlsocket (newsocket, FIONBIO, &_true) == -1)
	{
		Com_Printf ("WARNING: IPX_Socket: ioctl FIONBIO: %s\n", NET_ErrorString());
		return 0;
	}

	// make it broadcast capable
	if (setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&_true, sizeof(_true)) == -1)
	{
		Com_Printf ("WARNING: IPX_Socket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString());
		return 0;
	}

	address.sa_family = AF_IPX;
	memset (address.sa_netnum, 0, 4);
	memset (address.sa_nodenum, 0, 6);
	if (port == PORT_ANY)
		address.sa_socket = 0;
	else
		address.sa_socket = htons((short)port);

	if( bind (newsocket, (void *)&address, sizeof(address)) == -1)
	{
		Com_Printf ("WARNING: IPX_Socket: bind: %s\n", NET_ErrorString());
		closesocket (newsocket);
		return 0;
	}

	return newsocket;
}


/*
====================
NET_OpenIPX
====================
*/
void NET_OpenIPX (void)
{
	int		port;
	int		dedicated;

	dedicated = Cvar_VariableValue ("dedicated");

	if (!ipx_sockets[NS_SERVER])
	{
		port = Cvar_Get("ipx_hostport", "0", CVAR_NOSET)->value;
		if (!port)
		{
			port = Cvar_Get("hostport", "0", CVAR_NOSET)->value;
			if (!port)
			{
				port = Cvar_Get("port", va("%i", PORT_SERVER), CVAR_NOSET)->value;
			}
		}
		ipx_sockets[NS_SERVER] = NET_IPXSocket (port);
	}

	// dedicated servers don't need client ports
	if (dedicated)
		return;

	if (!ipx_sockets[NS_CLIENT])
	{
		port = Cvar_Get("ipx_clientport", "0", CVAR_NOSET)->value;
		if (!port)
		{
			port = Cvar_Get("clientport", va("%i", PORT_CLIENT), CVAR_NOSET)->value;
			if (!port)
				port = PORT_ANY;
		}
		ipx_sockets[NS_CLIENT] = NET_IPXSocket (port);
		if (!ipx_sockets[NS_CLIENT])
			ipx_sockets[NS_CLIENT] = NET_IPXSocket (PORT_ANY);
	}
}


/*
====================
NET_Config

A single player game will only use the loopback code
====================
*/
void net_config (boolean multiplayer)
{
   int      i;
   static   boolean  old_config;

   if (old_config == multiplayer)
      return;

   old_config = multiplayer;

   if (!multiplayer)
   {  //
      // shut down any existing sockets
      //
      for (i=0 ; i<2 ; i++)
      {
         if (ip_sockets[i])
         {
            closesocket (ip_sockets[i]);
            ip_sockets[i] = 0;
         }
         if (ipx_sockets[i])
         {
            closesocket (ipx_sockets[i]);
            ipx_sockets[i] = 0;
         }
      }
   }
   else
   {  //
      // open sockets
      //
      if (! noudp->value)
         net_open_ip ();
      if (! noipx->value)
         net_open_ipx ();
   }
}

// sleeps msec or until net socket is ready
void net_sleep(int msec)
{
   struct timeval timeout;
   fd_set         fdset;
   extern cvar_t *dedicated;
   int i;

   if (!dedicated || !dedicated->value)
      return; // we're not a server, just run full speed

   FD_ZERO(&fdset);
   i = 0;
   if (ip_sockets[NS_SERVER])
   {
      FD_SET(ip_sockets[NS_SERVER], &fdset); // network socket
      i = ip_sockets[NS_SERVER];
   }
   if (ipx_sockets[NS_SERVER])
   {
      FD_SET(ipx_sockets[NS_SERVER], &fdset); // network socket
      if (ipx_sockets[NS_SERVER] > i)
         i = ipx_sockets[NS_SERVER];
   }
   timeout.tv_sec = msec/1000;
   timeout.tv_usec = (msec%1000)*1000;
   select(i+1, &fdset, NULL, NULL, &timeout);
}

//===================================================================


static WSADATA    winsockdata;

/*
====================
NET_Init
====================
*/
void net_init (void)
{
   WORD  wVersionRequested;
   int   r;

   wVersionRequested = MAKEWORD(1, 1);

   r = WSAStartup (MAKEWORD(1, 1), &winsockdata);

   if (r)
      Com_Error (ERR_FATAL,"Winsock initialization failed.");

   Com_Printf("Winsock Initialized\n");

   noudp = Cvar_Get ("noudp", "0", CVAR_NOSET);
   noipx = Cvar_Get ("noipx", "0", CVAR_NOSET);

   net_shownet = Cvar_Get ("net_shownet", "0", 0);
}


/*
====================
NET_Shutdown
====================
*/
void net_shutdown (void)
{
   net_config (false);     // close sockets

   //WSACleanup ();
}


/*
====================
NET_ErrorString
====================
*/
char *NET_ErrorString (void)
{
	int		code;

	code = WSAGetLastError ();
	switch (code)
	{
	case WSAEINTR: return "WSAEINTR";
	case WSAEBADF: return "WSAEBADF";
	case WSAEACCES: return "WSAEACCES";
	case WSAEDISCON: return "WSAEDISCON";
	case WSAEFAULT: return "WSAEFAULT";
	case WSAEINVAL: return "WSAEINVAL";
	case WSAEMFILE: return "WSAEMFILE";
	case WSAEWOULDBLOCK: return "WSAEWOULDBLOCK";
	case WSAEINPROGRESS: return "WSAEINPROGRESS";
	case WSAEALREADY: return "WSAEALREADY";
	case WSAENOTSOCK: return "WSAENOTSOCK";
	case WSAEDESTADDRREQ: return "WSAEDESTADDRREQ";
	case WSAEMSGSIZE: return "WSAEMSGSIZE";
	case WSAEPROTOTYPE: return "WSAEPROTOTYPE";
	case WSAENOPROTOOPT: return "WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP: return "WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT";
	case WSAEADDRINUSE: return "WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL";
	case WSAENETDOWN: return "WSAENETDOWN";
	case WSAENETUNREACH: return "WSAENETUNREACH";
	case WSAENETRESET: return "WSAENETRESET";
	case WSAECONNABORTED: return "WSWSAECONNABORTEDAEINTR";
	case WSAECONNRESET: return "WSAECONNRESET";
	case WSAENOBUFS: return "WSAENOBUFS";
	case WSAEISCONN: return "WSAEISCONN";
	case WSAENOTCONN: return "WSAENOTCONN";
	case WSAESHUTDOWN: return "WSAESHUTDOWN";
	case WSAETOOMANYREFS: return "WSAETOOMANYREFS";
	case WSAETIMEDOUT: return "WSAETIMEDOUT";
	case WSAECONNREFUSED: return "WSAECONNREFUSED";
	case WSAELOOP: return "WSAELOOP";
	case WSAENAMETOOLONG: return "WSAENAMETOOLONG";
	case WSAEHOSTDOWN: return "WSAEHOSTDOWN";
	case WSASYSNOTREADY: return "WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED: return "WSANOTINITIALISED";
	case WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN: return "WSATRY_AGAIN";
	case WSANO_RECOVERY: return "WSANO_RECOVERY";
	case WSANO_DATA: return "WSANO_DATA";
	default: return "NO ERROR";
	}
}

//=======================================================================================

#ifdef __OLD_NET_H__

#include "console.h"
#include "net.h"
//#include "net_vcr.h"

socket_t *net_active_sockets = NULL;
socket_t *net_free_sockets = NULL;
int      net_numsockets = 0;

boolean  serial_available = false;
boolean  ipx_available = false;
boolean  tcpip_avail = false;

int      net_hostport;
int      DEFAULT_net_hostport = 26000;

char     my_ipx_address   [NET_NAMELEN];
char     my_tcpip_address [NET_NAMELEN];

void            (*get_com_port_config) (int portNumber, int *port, int *irq, int *baud, boolean *useModem);
void            (*set_com_port_config) (int portNumber, int port, int irq, int baud, boolean useModem);
void            (*get_modem_config) (int portNumber, char *dialType, char *clear, char *init, char *hangup);
void            (*set_modem_config) (int portNumber, char *dialType, char *clear, char *init, char *hangup);

static boolean listening         = false;

boolean        slist_in_progress = false;
boolean        slist_silent      = false;
booleans       list_local        = true;

static unsigned long slist_start_time;
static int           slist_last_shown;

static void slist_send(void);
static void slist_poll(void);
poll_procedure slist_send_procedure = {NULL, 0.0, slist_send};
poll_procedure slist_poll_procedure = {NULL, 0.0, slist_poll};


sizebuf_t  net_message;
int        net_active_connections = 0;

int messages_sent                = 0;
int messages_received            = 0;
int unreliable_messages_sent     = 0;
int unreliable_messages_received = 0;

cvar net_messagetimeout    = {"net_messagetimeout",      "300"};
cvar hostname              = {"hostname",                "UNNAMED"};

boolean  config_restored       = false;
cvar     config_com_port       = {"__config_com_port",       "0x3f8", true};
cvar     config_com_irq        = {"__config_com_irq",        "4",     true};
cvar     config_com_baud       = {"__config_com_baud",       "57600", true};
cvar     config_com_modem      = {"__config_com_modem",      "1",     true};
cvar     config_modem_dialtype = {"__config_modem_dialtype", "T",     true};
cvar     config_modem_clear    = {"__config_modem_clear",    "ATZ",   true};
cvar     config_modem_init     = {"__config_modem_init",     "",      true};
cvar     config_modem_hangup   = {"__config_modem_hangup",   "AT H",  true};

#ifdef MAZGODS // ;-)
cvar   mazgods = {"mazgods", "0"};
#endif

int vcr_file = -1;
boolean recording = false;

// these two macros are to make the code more readable
#define sfunc	net_drivers[sock->driver]
#define dfunc	net_drivers[net_driverlevel]

int	        net_driverlevel;


unsigned long	net_time;

unsigned long set_net_time(void)
{
  net_time = timer;
  return net_time;
}


/*
===================
net_new_socket

Called by drivers when a new communications endpoint is required
The sequence and buffer fields will be filled in properly
===================
*/
socket_t *net_new_socket (void)
{
  socket_t	*sock;

  if (net_free_sockets == NULL)
    return NULL;

  if (net_active_connections >= svs.maxclients)
    return NULL;

  // get one from free list
  sock = net_free_sockets;
  net_free_sockets = sock->next;

  // add it to active list
  sock->next = net_active_sockets;
  net_active_sockets = sock;

  sock->disconnected = false;
  sock->connect_time = net_time;
  strcpy (sock->address,"UNSET ADDRESS");
  sock->driver                      = net_driverlevel;
  sock->socket                      = 0;
  sock->driverdata                  = NULL;
  sock->can_send                    = true;
  sock->send_next                   = false;
  sock->last_message_time           = net_time;
  sock->ack_sequence                = 0;
  sock->send_sequence               = 0;
  sock->unreliable_send_sequence    = 0;
  sock->send_message_length         = 0;
  sock->receive_sequence            = 0;
  sock->unreliable_receive_sequence = 0;
  sock->receive_message_length      = 0;

  return sock;
}


void net_free_socket(socket_t *sock)
{
  socket_t	*s;

  // remove it from active list
  if (sock == net_active_sockets)
    net_active_sockets = net_active_sockets->next;
  else
  {
    for (s = net_active_sockets; s; s = s->next)
    {
      if (s->next == sock)
      {
        s->next = sock->next;
        break;
      }
    }
    
    if (!s)
      sys_error ("net_free_socket: not active\n");
  }

  // add it to free list
  sock->next = net_free_sockets;
  net_free_sockets = sock;
  sock->disconnected = true;
}


static void net_listen_f (void)
{
  if (cmd_argc () != 2)
  {
    console_printf ("\"listen\" is \"%u\"\n", listening ? 1 : 0);
    return;
  }

  listening = atoi(cmd_argv(1)) ? true : false;

  for (net_driverlevel=0 ; net_driverlevel<net_numdrivers; net_driverlevel++)
  {
    if (net_drivers[net_driverlevel].initialized == false)
      continue;
      dfunc.listen (listening);
  }
}


static void max_players_f (void)
{
  int 	n;

  if (cmd_argc () != 2)
  {
    console_printf ("\"maxplayers\" is \"%u\"\n", svs.maxclients);
    return;
  }

  if (sv.active)
  {
    console_printf ("maxplayers can not be changed while a server is running.\n");
    return;
  }

  n = atoi(cmd_argv(1));
  if (n < 1)
    n = 1;
  if (n > svs.maxclientslimit)
  {
    n = svs.maxclientslimit;
    console_printf ("\"maxplayers\" set to \"%u\"\n", n);
  }

  if ((n == 1) && listening)
    cbuf_add_text ("listen 0\n");

  if ((n > 1) && (!listening))
    cbuf_add_text ("listen 1\n");

  svs.maxclients = n;
  if (n == 1)
    cvar_set ("deathmatch", "0");
  else
    cvar_set ("deathmatch", "1");
}


static void net_port_f (void)
{
  int 	n;

  if (cmd_argc () != 2)
  {
    console_printf ("\"port\" is \"%u\"\n", net_hostport);
    return;
  }

  n = atoi(cmd_argv(1));
  if (n < 1 || n > 65534)
  {
    console_printf ("Bad value, must be between 1 and 65534\n");
    return;
  }

  DEFAULT_net_hostport = n;
  net_hostport = n;

  if (listening)
  {
    // force a change to the new port
    cbuf_add_text ("listen 0\n");
    cbuf_add_text ("listen 1\n");
  }
}


static void print_slist_header(void)
{
  console_printf(" Server        | Map           | Users\n");
  console_printf("---------------+---------------+------\n");
  //              12345678901234567890123
  slist_last_shown = 0;
}


static void print_slist(void)
{
  int n;

  for (n = slist_last_shown; n < host_cache_count; n++)
  {
    if (host_cache[n].maxusers)
      console_printf("%-15.15s|%-15.15s|%2u/%2u\n", host_cache[n].name, host_cache[n].map, host_cache[n].users, host_cache[n].maxusers);
    else
      console_printf("%-15.15s|%-15.15s|\n", host_cache[n].name, host_cache[n].map);
  }

  slist_last_shown = n;
}


static void print_slist_trailer(void)
{
  if (host_cache_count)
    console_printf("---------------+---------------+------\n\n");
  else
    console_printf("No %s servers found.\n\n", NET_GAME_NAME);
}


void net_slist_f (void)
{
  if (slist_in_progress)
    return;

  if (! slist_silent)
  {
    console_printf("Looking for %s servers...\n", NET_GAME_NAME);
    print_slist_header();
  }

  slist_in_progress = true;
  slist_start_time = timer;

  schedule_poll_procedure(&slist_send_procedure, 0.0);
  schedule_poll_procedure(&slist_poll_procedure, 0.1);

  host_cache_count = 0;
}


static void slist_send(void)
{
  for (net_driverlevel=0; net_driverlevel < net_numdrivers; net_driverlevel++)
  {
    if (!slist_local && net_driverlevel == 0)
      continue;

    if (net_drivers[net_driverlevel].initialized == false)
      continue;

    dfunc.search_for_hosts (true);
  }

  if ((float)((timer - slist_start_time)/1000) < 0.5)
    schedule_poll_procedure(&slist_send_procedure, 0.75);
}


static void slist_poll(void)
{
  for (net_driverlevel=0; net_driverlevel < net_numdrivers; net_driverlevel++)
  {
    if (!slist_local && net_driverlevel == 0)
      continue;

    if (net_drivers[net_driverlevel].initialized == false)
      continue;

    dfunc.search_for_hosts (false);
  }

  if (! slist_silent)
    print_slist();

  if ((float)((timer - slist_start_time)/1000) < 1.5)
  {
    schedule_poll_procedure(&slist_poll_procedure, 0.1);
    return;
  }

  if (! slist_silent)
    print_slist_trailer();

  slist_in_progress = false;
  slist_silent      = false;
  slist_local       = true;
}


/*
===================
net_connect
===================
*/

int host_cache_count = 0;
hostcache_t host_cache[HOSTCACHESIZE];

socket_t *net_connect (char *host)
{
  socket_t	*ret;
  int		n;
  int		numdrivers = net_numdrivers;

  set_net_time();

  if (host && *host == 0)
    host = NULL;

  if (host)
  {
    if (strcasecmp (host, "local") == 0)
    {
      numdrivers = 1;
      goto just_do_it;
    }

    if (host_cache_count)
    {
      for (n = 0; n < host_cache_count; n++)
      {
        if (strcasecmp (host, host_cache[n].name) == 0)
        {
          host = host_cache[n].cname;
          break;
        }
      }

      if (n < host_cache_count)
        goto just_do_it;
    }
  }

  slist_silent = host ? true : false;
    net_slist_f ();

  while(slist_in_progress)
    net_poll();

  if (host == NULL)
  {
    if (host_cache_count != 1)
      return NULL;

    host = hostcache[0].cname;
    console_printf("Connecting to...\n%s @ %s\n\n", hostcache[0].name, host);
  }

  if (host_cache_count)
  {
    for (n = 0; n < host_cache_count; n++)
    {
      if (strcasecmp (host, hostcache[n].name) == 0)
      {
        host = hostcache[n].cname;
        break;
      }
    }
  }

just_do_it:

  for (net_driverlevel=0 ; net_driverlevel<numdrivers; net_driverlevel++)
  {
    if (net_drivers[net_driverlevel].initialized == false)
      continue;

    ret = dfunc.connect (host);
    if (ret)
      return ret;
  }

  if (host)
  {
    console_printf("\n");
    print_slist_header();
    print_slist();
    print_slist_trailer();
  }
	
  return NULL;
}


/*
===================
net_check_new_connections
===================
*/

struct
{
  unsigned long	time;
  int		op;
  long	session;
} vcr_connect;

socket_t *net_check_new_connections (void)
{
  socket_t	*ret;

  set_net_time();

  for (net_driverlevel=0 ; net_driverlevel<net_numdrivers; net_driverlevel++)
  {
    if (net_drivers[net_driverlevel].initialized == false)
      continue;

    if (net_driverlevel && listening == false)
      continue;

    ret = dfunc.check_new_connections ();

    if (ret)
    {
      if (recording)
      {
        vcr_connect.time = host_time;
        vcr_connect.op = VCR_OP_CONNECT;
        vcr_connect.session = (long)ret;
        Sys_FileWrite (vcr_file, &vcr_connect, sizeof(vcr_connect));
        Sys_FileWrite (vcr_file, ret->address, NET_NAMELEN);
      }

      return ret;
    }
  }
	
  if (recording)
  {
    vcr_connect.time = host_time;
    vcr_connect.op = VCR_OP_CONNECT;
    vcr_connect.session = 0;
    Sys_FileWrite (vcr_file, &vcr_connect, sizeof(vcr_connect));
  }

  return NULL;
}

/*
===================
net_close
===================
*/
void net_close (socket_t *sock)
{
  if (!sock)
    return;

  if (sock->disconnected)
    return;

  set_net_time();

  // call the driver_close function
  sfunc.close (sock);

  net_free_socket(sock);
}


/*
=================
net_get_message

If there is a complete message, return it in net_message

returns  0 if no data is waiting
returns  1 if a message was received
returns -1 if connection is invalid
=================
*/

struct
{
  unsigned long	time;
  int		op;
  long	        session;
  int		ret;
  int		len;
} vcr_get_message;

extern void print_stats(socket_t *s);

int net_get_message (socket_t *sock)
{
  int ret;

  if (!sock)
    return -1;

  if (sock->disconnected)
  {
    console_printf("net_get_message: disconnected socket\n");
    return -1;
  }

  set_net_time();

  ret = sfunc.get_message(sock);

  // see if this connection has timed out
  if (ret == 0 && sock->driver)
  {
    if (net_time - sock->last_message_time > net_messagetimeout.value)
    {
      net_close(sock);
      return -1;
    }
  }

  if (ret > 0)
  {
    if (sock->driver)
    {
      sock->last_message_time = net_time;
      if (ret == 1)
        messages_received++;
      else if (ret == 2)
        unreliable_messages_received++;
    }

    if (recording)
    {
      vcr_get_message.time = host_time;
      vcr_get_message.op = VCR_OP_GETMESSAGE;
      vcr_get_message.session = (long)sock;
      vcr_get_message.ret = ret;
      vcr_get_message.len = net_message.cursize;
      Sys_FileWrite (vcr_file, &vcr_get_message, 24);
      Sys_FileWrite (vcr_file, net_message.data, net_message.cursize);
    }
  }
  else
  {
    if (recording)
    {
      vcr_get_message.time = host_time;
      vcr_get_message.op = VCR_OP_GETMESSAGE;
      vcr_get_message.session = (long)sock;
      vcr_get_message.ret = ret;
      Sys_FileWrite (vcr_file, &vcr_get_message, 20);
    }
  }

  return ret;
}


/*
==================
net_send_message

Try to send a complete length+message unit over the reliable stream.
returns  0 if the message cannot be delivered reliably, but the connection
              is still considered valid

returns  1 if the message was sent properly
returns -1 if the connection died
==================
*/
struct
{
  unsigned long	time;
  int		op;
  long	        session;
  int		r;
} vcr_send_message;

int net_send_message (socket_t *sock, sizebuf_t *data)
{
  int		r;
	
  if (!sock)
    return -1;

  if (sock->disconnected)
  {
    console_printf("net_send_message: disconnected socket\n");
    return -1;
  }

  set_net_time();
  r = sfunc.send_message(sock, data);
  if (r == 1 && sock->driver)
    messages_sent++;

  if (recording)
  {
    vcr_send_message.time = host_time;
    vcr_send_message.op = VCR_OP_SENDMESSAGE;
    vcr_send_message.session = (long)sock;
    vcr_send_message.r = r;
    Sys_FileWrite (vcr_file, &vcr_send_message, 20);
  }
	
  return r;
}


int net_send_unreliable_message (socket_t *sock, sizebuf_t *data)
{
  int		r;
	
  if (!sock)
    return -1;

  if (sock->disconnected)
  {
    console_printf("net_send_message: disconnected socket\n");
    return -1;
  }

  set_net_time();
  r = sfunc.send_unreliable_message(sock, data);
  if (r == 1 && sock->driver)
    unreliable_messages_sent++;

  if (recording)
  {
    vcr_send_message.time = host_time;
    vcr_send_message.op = VCR_OP_SENDMESSAGE;
    vcr_send_message.session = (long)sock;
    vcr_send_message.r = r;
    Sys_FileWrite (vcr_file, &vcr_send_message, 20);
  }
	
  return r;
}


/*
==================
net_can_send_message

Returns true or false if the given socket can currently accept a
message to be transmitted.
==================
*/
boolean net_can_send_message (socket_t *sock)
{
  int		r;
	
  if (!sock)
    return false;

  if (sock->disconnected)
    return false;

  set_net_time();

  r = sfunc.can_send_message(sock);
	
  if (recording)
  {
    vcr_send_message.time = host_time;
    vcr_send_message.op = VCR_OP_CANSENDMESSAGE;
    vcr_send_message.session = (long)sock;
    vcr_send_message.r = r;
    Sys_FileWrite (vcr_file, &vcr_send_message, 20);
  }
	
  return r;
}


int net_send_to_all(sizebuf_t *data, int blocktime)
{
  unsigned long	start;
  int		i;
  int		count = 0;
  boolean	state1 [MAX_SCOREBOARD];
  boolean	state2 [MAX_SCOREBOARD];

  for (i=0, host_client = svs.clients ; i<svs.maxclients ; i++, host_client++)
  {
    if (!host_client->netconnection)
      continue;

    if (host_client->active)
    {
      if (host_client->netconnection->driver == 0)
      {
        net_send_message(host_client->netconnection, data);
        state1[i] = true;
        state2[i] = true;
        continue;
      }
      count++;

      state1[i] = false;
      state2[i] = false;
    }
    else
    {
      state1[i] = true;
      state2[i] = true;
    }
  }

  start = timer;

  while (count)
  {
    count = 0;
    for (i=0, host_client = svs.clients ; i<svs.maxclients ; i++, host_client++)
    {
      if (! state1[i])
      {
        if (net_can_send_message (host_client->netconnection))
        {
          state1[i] = true;
          net_send_message(host_client->netconnection, data);
        }
      	else
        {
          net_get_message (host_client->netconnection);
        }

        count++;
        continue;
      }

      if (! state2[i])
      {
        if (net_can_send_message (host_client->netconnection))
        {
          state2[i] = true;
        }
        else
        {
          net_get_message (host_client->netconnection);
        }

        count++;
        continue;
      }
    }

    if ((float)((timer - start)/1000) > blocktime)
      break;
  }

  return count;
}


//=============================================================================

/*
====================
net_init
====================
*/

void net_init (void)
{
  int		i;
  int		controlSocket;
  socket_t	*s;

  if (com_check_parm("-playback"))
  {
    net_numdrivers = 1;
    net_drivers[0].init = vcr_init;
  }

  if (com_check_parm("-record"))
    recording = true;

  i = com_check_parm ("-port");
  if (!i)
    i = com_check_parm ("-udpport");
  if (!i)
    i = com_check_parm ("-ipxport");

  if (i)
  {
    if (i < com_argc-1)
      DEFAULT_net_hostport = atoi (com_argv[i+1]);
    else
      sys_error ("net_init: you must specify a number after -port");
  }

  net_hostport = DEFAULT_net_hostport;

  if (com_check_parm("-listen") || cls.state == ca_dedicated)
    listening = true;

  net_numsockets = svs.maxclientslimit;
  if (cls.state != ca_dedicated)
    net_numsockets++;

  set_net_time();

  for (i = 0; i < net_numsockets; i++)
  {
    s = (socket_t *)Hunk_AllocName(sizeof(socket_t), "socket");
    s->next = net_free_sockets;
    net_free_sockets = s;
    s->disconnected = true;
  }

  // allocate space for network message buffer
  SZ_Alloc (&net_message, NET_MAXMESSAGE);

  cvar_register_variable (&net_messagetimeout);
  cvar_register_variable (&hostname);
  cvar_register_variable (&config_com_port);
  cvar_register_variable (&config_com_irq);
  cvar_Register_variable (&config_com_baud);
  cvar_Register_variable (&config_com_modem);
  cvar_Register_variable (&config_modem_dialtype);
  cvar_Register_variable (&config_modem_clear);
  cvar_Register_variable (&config_modem_init);
  cvar_Register_variable (&config_modem_hangup);
#ifdef IDGODS
  cvar_register_variable (&idgods);
#endif

  cmd_add_command ("slist",             net_slist_f);
  cmd_add_command ("listen",            net_listen_f);
  cmd_add_command ("maxplayers",        max_players_f);
  cmd_add_command ("port",              net_port_f);

  // initialize all the drivers
  for (net_driverlevel=0 ; net_driverlevel<net_numdrivers ; net_driverlevel++)
  {
    controlSocket = net_drivers[net_driverlevel].Init();
    if (controlSocket == -1)
      continue;

    net_drivers[net_driverlevel].initialized = true;
    net_drivers[net_driverlevel].controlSock = controlSocket;
    if (listening)
      net_drivers[net_driverlevel].listen (true);
  }

  if (*my_ipx_address)
    console_dprintf("IPX address %s\n", my_ipx_address);
  if (*my_tcpip_address)
    console_dprintf("TCP/IP address %s\n", my_tcpip_address);
}

/*
====================
net_shutdown
====================
*/

void	net_shutdown (void)
{
  socket_t	*sock;

  set_net_time();

  for (sock = net_active_sockets; sock; sock = sock->next)
    net_close(sock);

  //
  // shutdown the drivers
  //
  for (net_driverlevel = 0; net_driverlevel < net_numdrivers; net_driverlevel++)
  {
    if (net_drivers[net_driverlevel].initialized == true)
    {
      net_drivers[net_driverlevel].Shutdown ();
      net_drivers[net_driverlevel].initialized = false;
    }
  }

  if (vcr_file != -1)
  {
    console_printf ("Closing vcrfile...\n");
    Sys_FileClose(vcr_file);
  }
}


static poll_procedure *pollProcedureList = NULL;

void net_poll(void)
{
  poll_procedure *pp;
  boolean	 useModem;

  if (!config_restored)
  {
    if (serial_available)
    {
      if (config_com_modem.value == 1.0)
        use_modem = true;
      else
        use_modem = false;

      set_com_port_config (0, (int)config_com_port.value, (int)config_com_irq.value, (int)config_com_baud.value, useModem);
      set_modem_config    (0, config_modem_dialtype.string, config_modem_clear.string, config_modem_init.string, config_modem_hangup.string);
    }

    config_restored = true;
  }

  set_net_time();

  for (pp = pollProcedureList; pp; pp = pp->next)
  {
    if (pp->next_time > net_time)
      break;
    pollProcedureList = pp->next;
    pp->procedure(pp->arg);
  }
}


void schedule_poll_procedure(poll_procedure *proc, double timeOffset)
{
  poll_procedure *pp, *prev;

  proc->next_time = timer + time_offset;
  for (pp = pollProcedureList, prev = NULL; pp; pp = pp->next)
  {
    if (pp->next_time >= proc->next_time)
      break;

    prev = pp;
  }

  if (prev == NULL)
  {
    proc->next = pollProcedureList;
    pollProcedureList = proc;
    return;
  }

  proc->next = pp;
  prev->next = proc;
}


#ifdef  MAZGODS
#define MAZNET	0xc0f62800

boolean is_maz(struct sockaddr *addr)
{
  if (mazgods.value == 0.0)
    return false;

  if (addr->sa_family != 2)
    return false;

  if ((big_long(*(int *)&addr->sa_data[2]) & 0xffffff00) == MAZNET)
    return true;

  return false;
}
#endif

#endif
