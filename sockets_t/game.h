#ifndef GAME_H
#define GAME_H

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <locale.h>
#include <stdatomic.h>

#include "design.h"
#include "struct.h"
#include "utils.h"
#include "crocodile.h"
#include "frog.h"
#include "buffer.h"

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
#define CROCODILE_STREAM_MAX_NUMBER 3
#define STREAM_NUMBER 8

// define timer
#define TIMER_ID 28
#define TIMER_SPEED 1000000
#define TIMER_MAX 60
#define PAUSE_ID 69
#define QUIT_ID 55

// define reset bullets
#define RESET_FROG_BULLET -8
#define RESET_CROCODILE_BULLET -18
#define BULLET_BEFORE_SHOT -5

// endgame ids
#define MANCHE_LOST 0
#define MANCHE_WON 1
#define GAME_QUIT 2
#define BACK_TO_MENU 3

// function prototipes
void startGame(WINDOW *game);
int play(WINDOW *game);
void pkill_all(pthread_t thread_timer, pthread_t thread_frog, pthread_t thread_crocodile[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER], pthread_t thread_ullet_left, pthread_t thread_bullet_right);
#endif