#ifndef FROG_H
#define FROG_H
#include <stdbool.h>
#include <errno.h>
#include "design.h"
#include "struct.h"

extern int manche;
extern bool frog_on_crocodile;

// define frog
#define FROG_ID 1
#define FROG_DIM_Y 4
#define FROG_DIM_X 10
#define FROG_ON_CROCODILE_SIG SIGUSR1

// define bullets
#define BULLET_ID_RIGHT 26
#define BULLET_ID_LEFT 27
#define BULLETS_DIM 1
#define BULLETS_SPEED 20000

void frog_controller(int *pipe_fds);
void bullet_right_controller(Item *bullet_right, int *pipe_fds);
void bullet_left_controller(Item *bullet_left, int *pipe_fds);

#define RESET_MANCHE_SIG SIGUSR2
#endif