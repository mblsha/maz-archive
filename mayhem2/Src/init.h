#ifndef __INIT_H__
#define __INIT_H__

void init(int argc, char *argv[]);
//void intro();
int init_print(char *txt, char *err, int cond, bool fatal, AL_METHOD(void, proc, ()));
const char* argfind(const char* arg, const int argc, const char* argv[]);
void load_prefs();
void load_gfx();
void quit(char *reason);
void error(char *reason);
void shut_down();

#endif
