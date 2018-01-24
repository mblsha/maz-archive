#ifndef __GUI_DIALOG_H__
#define __GUI_DIALOG_H__

void    gui_animate                             (void);
void    gui_draw                                (BITMAP *bmp);

void    gui_key_event                           (int k);

void    gui_open_dialog                         (DIALOG *d);
void    gui_close_dialog                        (void);

int     send_message                            (DIALOG *d, int msg, int c);
void    send_dialog_message                     (int msg, int c);

#endif
