#ifndef GAME_H
#define GAME_H

#include "design.h"
#include "struct.h"
#include "utils.h"
#include "crocodile.h"
#include "game.h"
#include "frog.h"

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

// timer
#define TIMER_ID 27
#define TIMER_SPEED 1000000
#define TIMER_MAX 60

#define ESC 27
#define PAUSE_ID 69

#define QUIT_ID 55

// function prototipes
void startGame(WINDOW *game);
int play(WINDOW *game);
void kill_all(pid_t frog, pid_t pid_group);
void ctrlc_handler(int signum);

#endif