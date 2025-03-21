#ifndef CROCODILE_H
#define CROCODILE_H

#include "struct.h"

//crocodiles macros
#define CROCODILE_MIN_ID 2
#define CROCODILE_MAX_ID STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER + CROCODILE_MIN_ID - 1
#define CROCODILE_DIM_Y 4
#define CROCODILE_DIM_X 30
#define CROCODILE_SPEED_MAX 200000
#define CROCODILE_SPEED_MIN 100000
#define CROCODILE_STREAM_MAX_NUMBER 3
#define STREAM_NUMBER 8

//crocodiles bullets
#define CROCODILE_MIN_BULLETS_ID 29
#define CROCODILE_MAX_BULLETS_ID STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER + CROCODILE_MIN_BULLETS_ID - 1
#define CROCODILE_BULLET_SPEED 40000


extern int manche;

// crocodile functions prototypes
void initializeCrocodile(Item **crocodiles, int stream_speed[STREAM_NUMBER]);
void crocodile(int *pipe_fds, Item *crocodile, int group_pid);
void crocodile_bullet_right_controller(Item *bullet, int *pipe_fds);
void crocodile_bullet_left_controller(Item *bullet, int *pipe_fds);
int crocodile_on_screen(Item *crocodile);
int uniform_shot_speed(int crocodile_speed);

#endif