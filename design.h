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
#include "crocodile.h"
// define game zones
#define GAME_WIDTH 150
#define GAME_HEIGHT 46
#define SCORE_HEIGHT 1
#define DENS_HEIGHT 6
#define SIDEWALK_HEIGHT_1 10
#define SIDEWALK_HEIGHT_2 GAME_HEIGHT-5

extern pthread_mutex_t m_suspend_mutex;
extern pthread_cond_t m_resume_cond;


#define STREAM_NUMBER 8

// define bullets
#define BULLET_ID_RIGHT 26
#define BULLET_ID_LEFT 27
#define BULLETS_DIM 1
#define BULLETS_SPEED 20000

// timer
#define TIMER_ID 28
#define TIMER_SPEED 1000000
#define TIMER_MAX 60

#define PAUSE_ID 69

// function prototipes

void print_score(WINDOW *game, int manche, int timer, int score);
void print_background(WINDOW *game, bool *dens);
void print_frog(WINDOW *game, Item *frog);
void print_crocodile(WINDOW *game, Item *crocodile);
void print_bullets(WINDOW *game, Item *bullet, int itemtype);
void print_endgame(WINDOW *game, int manche, bool *dens, int score);
void startGame(WINDOW *game);
void print_frogger_sprite(WINDOW *win);
int play(WINDOW *game);
void pkill_all(pthread_t thread_timer, pthread_t thread_frog, pthread_t thread_crocodile[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER]);
#endif