#ifndef DESIGN_H
#define DESIGN_H

#include <ncurses.h>
#include "struct.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <locale.h>

//colours define
#define COLOR_GREY 10
#define COLOR_DARKGREEN 11

// define game zones
#define SCORE_HEIGHT 1
#define DENS_HEIGHT 6
#define SIDEWALK_HEIGHT_1 10
#define SIDEWALK_HEIGHT_2 LINES-5

#define STREAM_NUMBER 8

// define bullets
#define BULLETS_ID 26
#define BULLETS_DIM 1
#define BULLETS_SPEED 20000


// function prototipes

void print_background();
void print_frog(Item *frog);
void print_crocodile(Item *crocodile);
void print_bullets(Item *bullet);

#endif