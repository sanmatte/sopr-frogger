#ifndef CROCODILE_H
#define CROCODILE_H

#define CROCODILE_MIN_ID 2
#define CROCODILE_MAX_ID STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER + CROCODILE_MIN_ID - 1
#define CROCODILE_DIM_Y 4
#define CROCODILE_DIM_X 30
#define CROCODILE_SPEED_MAX 200000
#define CROCODILE_SPEED_MIN 100000
#define CROCODILE_STREAM_MAX_NUMBER 3

//crocodiles bullets
#define CROCODILE_MIN_BULLETS_ID 28
#define CROCODILE_MAX_BULLETS_ID STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER + CROCODILE_MIN_BULLETS_ID - 1
#define CROCODILE_BULLET_SPEED 40000


extern int manche;
#define STREAM_NUMBER 8
#include "design.h"
#include "struct.h"
#include "utils.h"
void InitializeCrocodile(Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER], int direction, int stream_speed[STREAM_NUMBER]);
void Crocodile(int *pipe_fds, Item *crocodile, int random_number);

#endif