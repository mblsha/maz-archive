#include "def.h"
//#include "console.h"
//#include "net.h"
//#include "dosisms.h"
//#include "crc.h"

// modem stuff is here
#include "net_comx.c"

// serial protocol

#define SERIAL_PROTOCOL_VERSION 1

// The serial protocol is message oriented.  The high level message format is
// a one byte message type (MTYPE_xxx), data, and a 16-bit checksum.  All
// multi-byte fields are sent in network byte order.  There are currently 4
// MTYPEs defined.  Their formats are as follows:
//
// MTYPE_RELIABLE     sequence      data_length   data       checksum   eom
// MTYPE_UNRELIABLE   sequence      data_length   data       checksum   eom
// MTYPE_ACK          sequence      checksum      eom
// MTYPE_CONTROL      data_length   data          checksum   eom
//
// sequence is an 8-bit unsigned value starting from 0
// data_length is a 16-bit unsigned value; it is the length of the data only
// the checksum is a 16-bit value.  the CRC formula used is defined in crc.h.
// the checksum covers the entire messages, excluding itself
// eom is a special 2 byte sequence used to mark the End Of Message.  This is
// needed for error recovery.
//
// A lot of behavior is based on knowledge of the upper level Quake network
// layer.  For example, only one reliable message can be outstanding (pending
// reception of an MTYPE_ACK) at a time.
//
// The low level routines used to communicate with the modem are not part of
// this protocol.
//
// The CONTROL messages are only used for session establishment.  They are
// not reliable or sequenced.

#define MTYPE_RELIABLE			0x01
#define MTYPE_UNRELIABLE		0x02
#define MTYPE_CONTROL			0x03
#define MTYPE_ACK			0x04
#define MTYPE_CLIENT			0x80

#define ESCAPE_COMMAND			0xe0
#define ESCAPE_EOM			0x19

static boolean listening = false;


typedef struct serial_line_s
{
  struct serial_line_s	*next;
  socket_t		*sock;
  int			length_stated;
  int			length_found;
  int			tty;
  boolean		connected;
  boolean		connecting;
  boolean		client;
  unsigned long		connect_time;
  unsigned short	crc_stated;
  unsigned short	crc_value;
  byte			curr_state;
  byte			prev_state;
  byte			mtype;
  byte			sequence;
} serial_line_t;

#define STATE_READY		0
#define STATE_SEQUENCE	        1
#define STATE_LENGTH1	        2
#define STATE_LENGTH2	        3
#define STATE_DATA		4
#define STATE_CRC1		5
#define STATE_CRC2		6
#define STATE_EOM		7
#define STATE_ESCAPE	        8
#define STATE_ABORT		9

serial_line_t serial_line[NUM_COM_PORTS];

int my_driver_level;

static void serial_send_ack (serial_line_t *p, byte sequence);


static void reset_serial_line_protocol (serial_line_t *p)
{
  p->connected    = false;
  p->connecting   = false;
  p->curr_state   = STATE_READY;
  p->prev_state   = STATE_READY;
  p->length_found = 0;
}


static int process_in_queue(serial_line_t *p)
{
  int	b;

  while (1)
  {
    b = tty_read_byte(p->tty);
    if (b == ERR_TTY_NODATA)
      break;

    if (b == ERR_TTY_LINE_STATUS)
    {
      p->curr_state = STATE_ABORT;
      continue;
    }

    if (b == ERR_TTY_MODEM_STATUS)
    {
      p->curr_state = STATE_ABORT;
      return -1;
    }

    if (b == ESCAPE_COMMAND)
    {
      if (p->curr_state != STATE_ESCAPE)
      {
        p->prev_state = p->curr_state;
        p->curr_state = STATE_ESCAPE;
        continue;
      }
    }

    if (p->curr_state == STATE_ESCAPE)
    {
      if (b == ESCAPE_EOM)
      {
        if (p->prevState == STATE_ABORT)
        {
          p->curr_state = STATE_READY;
          p->length_found = 0;
          continue;
        }

        if (p->prevState != STATE_EOM)
        {
          p->curr_state = STATE_READY;
          p->length_found = 0;
          console_dprintf("Serial: premature EOM\n");
          continue;
        }

        switch (p->mtype)
        {
          case MTYPE_RELIABLE:
            console_dprintf("Serial: sending ack %u\n", p->sequence);
            serial_send_ack (p, p->sequence);
            if (p->sequence == p->sock->receiveSequence)
            {
              p->sock->receive_sequence = (p->sequence + 1) & 0xff;
              p->sock->receive_message_length += p->length_found;
            }
            else
              console_dprintf("Serial: reliable out of order; got %u wanted %u\n", p->sequence, p->sock->receive_sequence);
            break;

          case MTYPE_UNRELIABLE:
            p->sock->unreliable_receive_sequence = (p->sequence + 1) & 0xff;
            p->sock->receive_message_length += p->length_found;
            break;

          case MTYPE_ACK:
            console_dprintf("Serial: got ack %u\n", p->sequence);
            if (p->sequence == p->sock->send_sequence)
            {
              p->sock->send_sequence = (p->sock->send_sequence + 1) & 0xff;
              p->sock->can_send = true;
            }
            else
              console_dprintf("Serial: ack out of order; got %u wanted %u\n",p->sequence, p->sock->send_sequence);
            break;

          case MTYPE_CONTROL:
            p->sock->receive_message_length += p->length_found;
            break;
        }

        p->curr_state = STATE_READY;
        p->length_found = 0;
        continue;
      }


      if (b != ESCAPE_COMMAND)
      {
        p->curr_state = STATE_ABORT;
        console_dprintf("Serial: Bad escape sequence\n");
        continue;
      }

      // b == ESCAPE_COMMAND
      p->curr_state = p->prevState;
    }

    p->prevState = p->curr_state;

    //DEBUG
    if (p->sock->receive_message_length + p->length_found > NET_MAXMESSAGE)
    {
      console_dprintf("Serial blew out receive buffer: %u\n", p->sock->receive_message_length + p->length_found);
      p->curr_state = STATE_ABORT;
    }

    if (p->sock->receive_message_length + p->length_found == NET_MAXMESSAGE)
    {
      console_dprintf("Serial hit receive buffer limit: %u\n", p->sock->receive_message_length + p->length_found);
      p->curr_state = STATE_ABORT;
    }
    //end DEBUG

    switch (p->curr_state)
    {
      case STATE_READY:
        crc_init(&p->crc_value);
        crc_process_byte(&p->crc_value, b);
        if (p->client)
        {
          if ((b & MTYPE_CLIENT) != 0)
          {
            p->curr_state = STATE_ABORT;
            console_dprintf("Serial: client got own message\n");
            break;
          }
        }
        else
        {
          if ((b & MTYPE_CLIENT) == 0)
          {
            p->curr_state = STATE_ABORT;
            console_dprintf("Serial: server got own message\n");
            break;
          }
          b &= 0x7f;
        }
        p->mtype = b;

        if (b != MTYPE_CONTROL)
          p->curr_state = STATE_SEQUENCE;
        else
          p->curr_state = STATE_LENGTH1;

        if (p->mtype < MTYPE_ACK)
        {
          p->sock->receive_message[p->sock->receive_message_length] = b;
          p->length_found++;
        }
        break;

      case STATE_SEQUENCE:
        p->sequence = b;
        crc_process_byte(&p->crc_value, b);

        if (p->mtype != MTYPE_ACK)
          p->curr_state = STATE_LENGTH1;
        else
          p->curr_state = STATE_CRC1;
        break;

      case STATE_LENGTH1:
        p->length_stated = b * 256;
        crc_process_byte(&p->crc_value, b);
        p->curr_state = STATE_LENGTH2;
        break;

      case STATE_LENGTH2:
        p->length_stated += b;
        crc_process_byte(&p->crc_value, b);
        if (p->mtype == MTYPE_RELIABLE && p->length_stated > MAX_MSGLEN)
        {
          p->curr_state = STATE_ABORT;
          console_dprintf("Serial: bad reliable message length %u\n", p->length_stated);
        }
        else if (p->mtype == MTYPE_UNRELIABLE && p->length_stated > MAX_DATAGRAM)
        {
          p->curr_state = STATE_ABORT;
          console_dprintf("Serial: bad unreliable message length %u\n", p->length_stated);
        }
        else
        {
          p->curr_state = STATE_DATA;
          if (p->mtype < MTYPE_ACK)
          {
            *(short *)&p->sock->receive_message [p->sock->receive_message_length + 1] = p->length_stated;
            p->length_found += 2;
          }
        }
        break;

      case STATE_DATA:
        p->sock->receive_message[p->sock->receive_message_length + p->length_found] = b;
        p->length_found++;
        crc_process_byte(&p->crc_value, b);
        if (p->length_found == p->length_stated + 3)
        p->curr_state = STATE_CRC1;
        break;

      case STATE_CRC1:
        p->crcStated = b * 256;
        p->curr_state = STATE_CRC2;
        break;

      case STATE_CRC2:
        p->crcStated += b;
        if (p->crcStated == crc_value(p->crc_value))
        {
          p->curr_state = STATE_EOM;
        }
        else
        {
          p->curr_state = STATE_ABORT;
          console_dprintf("Serial: Bad crc\n");
        }
        break;

      case STATE_EOM:
        p->curr_state = STATE_ABORT;
        console_dprintf("Serial: Bad message format\n");
        break;

      case STATE_ABORT:
        break;
    }
  }

  return 0;
}


int serial_init (void)
{
  int     n;

// LATER do Win32 serial support
#ifdef	_WIN32
  return -1;
#endif

  if (com_check_parm("-nolan"))
    return -1;

  if (com_check_parm ("-noserial"))
    return -1;

  my_driver_level = net_driverlevel;

  if (tty_init())
    return -1;

  for (n = 0; n < NUM_COM_PORTS; n++)
  {
    serial_line[n].tty = TTY_Open(n);
    reset_serial_line_protocol (&serial_line[n]);
  }

  console_printf("Serial driver initialized\n");
  serial_available = true;

  return 0;
}


void serial_shutdown (void)
{
  int     n;

  for (n = 0; n < NUM_COM_PORTS; n++)
  {
    if (serial_line[n].connected)
      serial_close(serial_line[n].sock);
  }

  tty_shutdown();
}


void serial_listen (boolean state)
{
  listening = state;
}


boolean serial_can_send_message (socket_t *sock)
{
  return sock->can_send;
}


boolean serial_can_send_unreliable_message (socket_t *sock)
{
  return tty_output_queue_is_empty(((serial_line_t *)sock->driver_data)->tty);
}


int serial_send_message (socket_t *sock, sizebuf_t *message)
{
  serial_line_t  *p;
  int            n;
  unsigned short crc;
  byte           b;

  p = (serial_line_t *)sock->driver_data;
  crc_init (&crc);

  // message type
  b = MTYPE_RELIABLE;

  if (p->client)
    b |= MTYPE_CLIENT;

  tty_write_byte(p->tty, b);
  crc_process_byte (&crc, b);

  // sequence
  b = p->sock->send_sequence;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);

  // data length
  b = message->cursize >> 8;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);
  b = message->cursize & 0xff;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);

  // data
  for (n = 0; n < message->cursize; n++)
  {
    b = message->data[n];
    tty_write_byte(p->tty, b);

    if (b == ESCAPE_COMMAND)
      tty_write_byte(p->tty, b);

    crc_process_byte (&crc, b);
  }

  // checksum
  b = crc_value (crc) >> 8;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  b = crc_value (crc) & 0xff;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  // end of message
  tty_write_byte(p->tty, ESCAPE_COMMAND);
  tty_write_byte(p->tty, ESCAPE_EOM);

  tty_flush(p->tty);

  // mark sock as busy and save the message for possible retransmit
  sock->can_send = false;
  memcpy(sock->sendMessage, message->data, message->cursize);
  sock->send_message_length = message->cursize;
  sock->last_send_time = net_time;

  return 1;
}


static void re_send_message (socket_t *sock)
{
  sizebuf_t       temp;

  console_dprintf("Serial: re-sending reliable\n");
  temp.data = sock->sendMessage;
  temp.maxsize = sock->send_message_length;
  temp.cursize = sock->send_message_length;
  serial_send_message (sock, &temp);
}


int serial_send_unreliable_message (socket_t *sock, sizebuf_t *message)
{
  serial_line_t  *p;
  int            n;
  unsigned short crc;
  byte     b;

  p = (serial_line_t *)sock->driver_data;

  if (!tty_output_queue_is_empty(p->tty))
  {
    tty_flush(p->tty);
    return 1;
  }

  crc_init (&crc);

  // message type
  b = MTYPE_UNRELIABLE;

  if (p->client)
    b |= MTYPE_CLIENT;

  tty_write_byte(p->tty, b);
  crc_process_byte (&crc, b);

  // sequence
  b = p->sock->unreliable_send_sequence;
  p->sock->unreliable_send_sequence = (b + 1) & 0xff;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);

  // data length
  b = message->cursize >> 8;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);
  b = message->cursize & 0xff;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);

  // data
  for (n = 0; n < message->cursize; n++)
  {
    b = message->data[n];
    tty_write_byte(p->tty, b);

    if (b == ESCAPE_COMMAND)
      tty_write_byte(p->tty, b);

    crc_process_byte (&crc, b);
  }

  // checksum
  b = crc_value (crc) >> 8;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  b = crc_value (crc) & 0xff;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  // end of message
  tty_write_byte(p->tty, ESCAPE_COMMAND);
  tty_write_byte(p->tty, ESCAPE_EOM);

  tty_flush(p->tty);

  return 1;
}


static void serial_send_ack (serial_line_t *p, byte sequence)
{
  unsigned short crc;
  byte           b;

  crc_init (&crc);

  // message type
  b = MTYPE_ACK;

  if (p->client)
    b |= MTYPE_CLIENT;

  tty_write_byte(p->tty, b);
  crc_process_byte (&crc, b);

  // sequence
  b = sequence;
  tty_write_byte(p->tty, b);
  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);

  // checksum
  b = crc_value (crc) >> 8;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  b = crc_value (crc) & 0xff;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  // end of message
  tty_write_byte(p->tty, ESCAPE_COMMAND);
  tty_write_byte(p->tty, ESCAPE_EOM);

  tty_flush(p->tty);
}


static void serial_send_control_message (serial_line_t *p, sizebuf_t *message)
{
  unsigned short crc;
  int            n;
  byte           b;

  crc_init (&crc);

  // message type
  b = MTYPE_CONTROL;
  if (p->client)
    b |= MTYPE_CLIENT;

  tty_write_byte(p->tty, b);
  crc_process_byte (&crc, b);

  // data length
  b = message->cursize >> 8;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);
  b = message->cursize & 0xff;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  crc_process_byte (&crc, b);

  // data
  for (n = 0; n < message->cursize; n++)
  {
    b = message->data[n];
    tty_write_byte(p->tty, b);

    if (b == ESCAPE_COMMAND)
      tty_write_byte(p->tty, b);

    crc_process_byte (&crc, b);
  }

  // checksum
  b = crc_value (crc) >> 8;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  b = crc_value (crc) & 0xff;
  tty_write_byte(p->tty, b);

  if (b == ESCAPE_COMMAND)
    tty_write_byte(p->tty, b);

  // end of message
  tty_write_byte(p->tty, ESCAPE_COMMAND);
  tty_write_byte(p->tty, ESCAPE_EOM);

  tty_flush(p->tty);
}


static int _serial_get_message (serial_line_t *p)
{
  byte	ret;
  short	length;

  if (process_in_queue(p))
    return -1;

  if (p->sock->receive_message_length == 0)
    return 0;

  ret = p->sock->receive_message[0];
  length = *(short *)&p->sock->receive_message[1];
  if (ret == MTYPE_CONTROL)
    ret = 1;

  SZ_Clear (&net_message);
  SZ_Write (&net_message, &p->sock->receive_message[3], length);

  length += 3;
  p->sock->receive_message_length -= length;

  if (p->sock->receive_message_length + p->length_found)
    memcpy(p->sock->receive_message, &p->sock->receive_message[length], p->sock->receive_message_length + p->length_found);

  return ret;
}

int serial_get_message (socket_t *sock)
{
  serial_line_t *p;
  int		ret;

  p = (serial_line_t *)sock->driver_data;

  ret = _serial_get_message (p);

  if (ret == 1)
    messages_received++;

  if (!sock->can_send)
  {
    if ((float)((net_time - sock->last_send_time)/1000) > 1.0)
    {
      re_send_message (sock);
      sock->last_send_time = net_time;
    }
  }

  return ret;
}


void serial_close (socket_t *sock)
{
  serial_line_t *p = (serial_line_t *)sock->driver_data;
  tty_close(p->tty);
  reset_serial_line_protocol (p);
}


char    *com_types[] = {"direct", "modem"};
unsigned com_bauds[] = {9600, 14400, 19200, 28800, 57600};

void serial_search_for_hosts (boolean xmit)
{
  int		n;
  serial_line_t *p;

  if (sv.active)
    return;

  if (host_cache_count == HOSTCACHESIZE)
    return;

  // see if we've already answered
  for (n = 0; n < host_cache_count; n++)
    if (strcmp (host_cache[n].cname, "#") == 0)
      return;

  for (n = 0; n < NUM_COM_PORTS; n++)
    if (tty_is_enabled(n))
      break;

  if (n == NUM_COM_PORTS)
    return;

  p = &serial_line[n];

  if (tty_is_modem(p->tty))
    return;

  sprintf    (host_cache [host_cache_count].name, "COM%u", n+1);
  strcpy     (host_cache [host_cache_count].map, "");
  host_cache [host_cache_count].users = 0;
  host_cache [host_cache_count].maxusers = 0;
  host_cache [host_cache_count].driver = net_driverlevel;
  strcpy     (host_cache[host_cache_count].cname, "#");
  host_cache_count++;

  return;
}


static socket_t *_serial_connect (char *host, serial_line_t *p)
{
  int		ret;
  unsigned long	start_time;
  unsigned long	last_time;

  p->client = true;
  if (tty_connect(p->tty, host))
    return NULL;

  p->sock = net_new_socket ();
  p->sock->driver = my_driver_level;

  if (p->sock == NULL)
  {
    console_printf("No sockets available\n");
    return NULL;
  }

  p->sock->driver_data = p;

  // send the connection request
  start_time = SetNetTime();
  last_time = 0.0;

  SZ_Clear(&net_message);
  msg_write_byte(&net_message, CCRECONNECT);
  msg_write_string(&net_message, NET_GAME_NAME);

  do
  {
    SetNetTime();
    if ((float)((net_time - last_time)/1000) >= 1.0)
    {
      serial_send_control_message (p, &net_message);
      last_time = net_time;
      console_printf("trying...\n");
      SCR_UpdateScreen ();
    }

    ret = _serial_get_message (p);
  }

  while (ret == 0 && (float)((net_time - start_time)/1000) < 5.0)
    ;

  if (ret == 0)
  {
    console_printf("Unable to connect, no response\n");
    goto error_return;
  }

  if (ret == -1)
  {
    console_printf("Connection request error\n");
    goto error_return;
  }

  msg_begin_reading ();
  ret = msg_read_byte();

  if (ret == CCREP_REJECT)
  {
    console_printf(msg_read_string());
    goto error_return;
  }

  if (ret != CCREP_ACCEPT)
  {
    console_printf("Unknown connection response\n");
    goto error_return;
  }

  p->connected = true;
  p->sock->last_message_time = net_time;

  console_printf ("Connection accepted\n");

  return p->sock;

error_return:

  tty_disconnect(p->tty);
  return NULL;
}

socket_t *serial_connect (char *host)
{
  int	    n;
  socket_t  *ret = NULL;

  // see if this looks like a phone number
  if (*host == '#')
    host++;

  for (n = 0; n < strlen(host); n++)
    if (host[n] == '.' || host[n] == ':')
      return NULL;

  for (n = 0; n < NUM_COM_PORTS; n++)
    if (tty_is_enabled(n) && !serial_line[n].connected)
      if ((ret = _serial_connect (host, &serial_line[n])))
        break;

  return ret;
}


static socket_t *_serial_check_new_connections (serial_line_t *p)
{
  int	command;

  p->client = false;
  if (!tty_check_for_connection(p->tty))
    return NULL;

  if (tty_is_modem(p->tty))
  {
    if (!p->connecting)
    {
      p->connecting = true;
      p->connect_time = net_time;
    }
    else if ((float)((net_time - p->connect_time)/1000) > 15.0)
    {
      p->connecting = false;
      tty_disconnect(p->tty);
      return NULL;
    }
  }

  p->sock = net_new_socket ();
  p->sock->driver = my_driver_level;

  if (p->sock == NULL)
  {
    console_printf("No sockets available\n");
    return NULL;
  }

  p->sock->driver_data = p;

  SZ_Clear(&net_message);

  if (_serial_get_message(p) != 1)
  {
    net_free_socket(p->sock);
    return NULL;
  }

  msg_begin_reading ();
  command = msg_read_byte();

  if (command == CCRESERVER_INFO)
  {
    if (strcmp(msg_read_string(), NET_GAME_NAME) != 0)
      return NULL;

    if (msg_read_byte() != SERIAL_PROTOCOL_VERSION)
      return NULL;

    SZ_Clear                            (&net_message);
    msg_write_byte                      (&net_message, CCREP_SERVER_INFO);
    msg_write_string                    (&net_message, hostname.string);
    msg_write_string                    (&net_message, sv.name);
    msg_write_byte                      (&net_message, net_activeconnections);
    msg_write_byte                      (&net_message, svs.maxclients);
    serial_send_control_message         (p,            &net_message);
    SZ_Clear                            (&net_message);
    return NULL;
  }

  if (command != CCRECONNECT)
    return NULL;

  if (strcmp(msg_read_string(), NET_GAME_NAME) != 0)
    return NULL;

  // send him back the info about the server connection he has been allocated
  SZ_Clear                        (&net_message);
  msg_write_byte                  (&net_message, CCREP_ACCEPT);
  serial_send_control_message     (p,            &net_message);
  SZ_Clear                        (&net_message);

  p->connected  = true;
  p->connecting = false;
  p->sock->last_message_time = net_time;
  sprintf(p->sock->address, "COM%u", (int)((p - serial_line) + 1));

  return p->sock;
}

socket_t *serial_check_new_connections (void)
{
  int		n;
  socket_t	*ret = NULL;

  for (n = 0; n < NUM_COM_PORTS; n++)
    if (tty_is_enabled(n) && !serial_line[n].connected)
      if ((ret = _serial_check_new_connections (&serial_line[n])))
        break;

  return ret;
}