#ifndef __CMD_BASE_H__
#define __CMD_BASE_H__

extern  cvar                    max_fps;//                 = {"__max_fps",         "100", true};
//extern  DATAFILE                *condata;
void    set_gfx_mode_f                                  (int argc, char **argv);
void    quit_f                                          (void);

void    cmd_register();

#endif
