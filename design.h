#include <ncurses.h>
//#include "struct.h"

//colours define
#define COLOR_GREY 10
#define COLOR_DARKGREEN 11

typedef struct {
    int ID;
    int y;
    int x;
} Item;

// define game zones
#define SCORE_HEIGHT 5
#define DENS_HEIGHT 10
#define SIDEWALK_HEIGHT_1 14
#define SIDEWALK_HEIGHT_2 LINES-5

// define frog
#define FROG_ID 1
#define FROG_DIM_Y 4
#define FROG_DIM_X 10

// define crocodile
#define CROCCODILE_ID 2
#define CROCCODILE_DIM_Y 4
#define CROCCODILE_DIM_X 30


// function prototipes
void print_background();
void print_frog(Item *frog);
void print_crocodile(Item *crocodile);