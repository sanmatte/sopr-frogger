#ifndef FROG_H
#define FROG_H
#include <stdbool.h>
#define FROG_ID 1
#define FROG_DIM_Y 4
#define FROG_DIM_X 10
#define FROG_ON_CROCODILE_SIG SIGUSR1
#include <errno.h>
extern int manche;
extern bool frog_on_crocodile;
#include "design.h"
#include "struct.h"
void Frog(int *pipe_fds, Item *frog, Item *bullet_left, Item *bullet_right, int stream_speed[STREAM_NUMBER]);
#define RESET_MANCHE_SIG SIGUSR2
#endif