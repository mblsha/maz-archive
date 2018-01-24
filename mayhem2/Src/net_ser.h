#ifndef __NET_SER_H__
#define __NET_SER_H__

//#include "console.h"
//#include "net.h"

int		serial_init                        (void);
void		serial_listen                      (boolean state);
void		serial_search_for_hosts            (boolean xmit);
socket_t	*serial_connect                    (char *host);
socket_t 	*serial_check_new_connections      (void);
int		serial_get_message                 (socket_t *sock);
int		serial_send_message                (socket_t *sock, sizebuf_t *data);
int		serial_send_unreliable_message     (socket_t *sock, sizebuf_t *data);
boolean	        serial_can_send_message            (socket_t *sock);
boolean	        serial_can_send_unreliable_message (socket_t *sock);
void		serial_close                       (socket_t *sock);
void		serial_shutdown                    (void);

#endif
