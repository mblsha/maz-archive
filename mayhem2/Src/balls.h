#define MAX_BALLS       100
#define BALL_GRAD       25

extern BITMAP *ball_bmp[MAX_BALLS];

extern cvar balls_display;
extern cvar balls_move;
extern cvar balls_speed;

void init_balls();
void draw_balls(BITMAP *bmp);
void move_balls();

