#ifndef DESIGN_H
#define DESIGN_H

#include <ncurses.h>
#include "struct.h"
#include "design.h"
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
#define COLOR_LIGHTDARKGREEN 12
#define COLOR_SAND 13
#define COLOR_FROG_EYE 14
#define COLOR_FROG_BODY 15
#define COLOR_DARK_ORANGE 16
#define COLOR_ENDGAME_BACKGROUND 17

//dens
#define DENS_NUMBER 5
#define DENS_HEIGHT 6
#define DENS_DIM 10
#define DENS_1 10
#define DENS_2 40
#define DENS_3 70
#define DENS_4 100
#define DENS_5 130

// define game zones
#define GAME_WIDTH 150
#define GAME_HEIGHT 46
#define SCORE_HEIGHT 1
#define DENS_HEIGHT 6
#define SIDEWALK_HEIGHT_1 10
#define SIDEWALK_HEIGHT_2 GAME_HEIGHT-5


#define STREAM_NUMBER 8

// define bullets
#define BULLETS_ID 26
#define BULLETS_DIM 1
#define BULLETS_SPEED 20000

// timer
#define TIMER_ID 27
#define TIMER_SPEED 1000000
#define TIMER_MAX 60

// function prototipes

void print_score(WINDOW *game, int manche, int timer, int score);
void print_background(WINDOW *game, bool *dens);
void print_frog(WINDOW *game, Item *frog);
void print_crocodile(WINDOW *game, Item *crocodile);
void print_bullets(WINDOW *game, Item *bullet);
void print_endgame(WINDOW *game, int manche, bool *dens, int score);

#endif