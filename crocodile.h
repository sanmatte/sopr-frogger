#ifndef CROCODILE_H
#define CROCODILE_H

#define CROCODILE_MIN_ID 2
#define CROCODILE_MAX_ID STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER + CROCODILE_MIN_ID - 1
#define CROCODILE_DIM_Y 4
#define CROCODILE_DIM_X 30
#define CROCODILE_SPEED_MAX 150000
#define CROCODILE_SPEED_MIN 50000
#define CROCODILE_STREAM_MAX_NUMBER 1

extern int manche;
#define STREAM_NUMBER 8
#include "design.h"
#include "struct.h"
void InitializeCrocodile(Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER], int direction, int stream_speed[STREAM_NUMBER]);
void Crocodile(int *pipe_fds, Item *crocodile, int random_number);

#endif