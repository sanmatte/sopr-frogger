#ifndef DESIGN_H
#define DESIGN_H

#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <locale.h>

#include "struct.h"
#include "game.h"
#include "menu.h"

//colours define
#define COLOR_GREY 10
#define COLOR_DARKGREEN 11
#define COLOR_DEEPGREEN 12
#define COLOR_SAND 13
#define COLOR_FROG_EYE 14
#define COLOR_FROG_BODY 15
#define COLOR_DARK_ORANGE 16
#define COLOR_ENDGAME_BACKGROUND 17
#define COLOR_BULLET_TRIGGER_DARK 18
#define COLOR_BULLET_TRIGGER 19
#define COLOR_FROG_BODY_DETAILS 20
#define COLOR_RIVER_EASY 21
#define COLOR_RIVER_HARD 22
#define COLOR_ORANGE 23



// function prototipes
void print_score(WINDOW *game, int manche, int timer, int score);
void print_background(WINDOW *game, bool *dens);
void print_frog(WINDOW *game, Item *frog);
void print_crocodile(WINDOW *game, Item *crocodile, int color_trigger);
void print_bullets(WINDOW *game, Item *bullet, int itemtype);
void print_endgame(WINDOW *game, int manche, bool *dens, int score);
void print_frogger_sprite(WINDOW *win);
void print_pause(WINDOW *win, WINDOW *game);

#endif