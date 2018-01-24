#ifndef __M_PROC_H__
#define __M_PROC_H__

int menu_button(int msg, DIALOG *d, int c);

class MENU_BUTTON {
private:
  int   x1, y1, x2, y2;
  int   col_foc, col_norm, col_bordr;
  char  *txt;
  bool  focused, redraw;
  BITMAP pic;
public:
  MENU_BUTTON(int x1, int y1, int x2, int y2, char *txt, int foc, int norm, int bordr);
  ~MENU_BUTTON();
  void update();
  void draw();
};

#endif
