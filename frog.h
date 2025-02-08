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
<<<<<<< HEAD


void frog_fun(int *pipe_fds, Item *frog);

=======
void frog_controller(int *pipe_fds, Item *frog);
void bullet_right_controller(Item *bullet_right, int *pipe_fds);
void bullet_left_controller(Item *bullet_left, int *pipe_fds);
>>>>>>> refs/remotes/origin/multi-process
#define RESET_MANCHE_SIG SIGUSR2
#endif