#ifndef __CL_INPUT_H__
#define __CL_INPUT_H__

#define CL_MAX_DOWN        2

#define KEY_BUTTON_DOWN           1
#define KEY_BUTTON_IMPULSE_DOWN   2
#define KEY_BUTTON_IMPULSE_UP     4

typedef struct
{
   char     name[32];            // for debugging
   int      down[CL_MAX_DOWN];   // key nums holding it down
   unsigned downtime;	         // msec timestamp
   unsigned msec;			         // msec down this frame
   int      state;
} key_button;

/*
void cl_init_input         (void);
void cl_send_cmd           (void);
void cl_send_move          (usercmd *cmd);

void cl_clear_state        (void);

void cl_read_packets       (void);

int  cl_read_from_server   (void);
void cl_write_to_server    (usercmd_t *cmd);
void cl_base_move          (usercmd_t *cmd);
*/
void  key_up               (key_button *key, ustring *argv);
void  key_down             (key_button *key, ustring *argv);
float key_state            (key_button *key);

#endif // __CL_INPUT_H__