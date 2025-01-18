#include <ncurses.h>
#include "struct.h"

//colours define
#define COLOR_GREY 10
#define COLOR_DARKGREEN 11

// define game zones
#define SCORE_HEIGHT 1
#define DENS_HEIGHT 6
#define SIDEWALK_HEIGHT_1 10
#define SIDEWALK_HEIGHT_2 LINES-5

// define frog
#define FROG_ID 1
#define FROG_DIM_Y 4
#define FROG_DIM_X 10
#define FROG_ON_CROCODILE_SIG 302
//#define FROG_ON_CROCODILE_SIG_RIGHT 301

// define crocodile
#define CROCODILE_MIN_ID 2
#define CROCODILE_MAX_ID 25
#define CROCODILE_DIM_Y 4
#define CROCODILE_DIM_X 30
#define STREAM_NUMBER 8
#define CROCODILE_SPEED_MAX 200000
#define CROCODILE_SPEED_MIN 100000
#define CROCODILE_STREAM_MAX_NUMBER 1

// define bullets
#define BULLETS_ID 26
#define BULLETS_DIM 1
#define BULLETS_SPEED 20000


// function prototipes
void print_background();
void print_frog(Item *frog);
void print_crocodile(Item *crocodile);
void print_bullets(Item *bullet);