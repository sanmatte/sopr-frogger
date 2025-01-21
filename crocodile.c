#include "crocodile.h"

void InitializeCrocodile(Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER], int direction, int stream_speed[STREAM_NUMBER]){
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++) {
            if (crocodiles[i][j].free == 0){
                crocodiles[i][j].id = CROCODILE_MIN_ID + (i * CROCODILE_STREAM_MAX_NUMBER) + j;
                crocodiles[i][j].y = SIDEWALK_HEIGHT_2 - CROCODILE_DIM_Y + 1 - (i * CROCODILE_DIM_Y);  
                if (direction == 0) {
                    crocodiles[i][j].x = -CROCODILE_DIM_X;  // Da sinistra verso destra
                } else {
                    crocodiles[i][j].x = COLS;  // Da destra verso sinistra
                }
                crocodiles[i][j].speed = stream_speed[i];
                crocodiles[i][j].free = 1;
            }
        }
        direction= 1 - direction;
    }
}

void Crocodile(int *pipe_fds, Item *crocodile, int random_number){
    close(pipe_fds[0]);
    while(manche>0){
        if (random_number == 0 && crocodile->x < COLS) {
            crocodile->x += 1;
            if(crocodile->x == COLS){
                crocodile->x = -CROCODILE_DIM_X;
                sleep(rand() % 3);
            }
        }
        else if (random_number == 1 && crocodile->x > -CROCODILE_DIM_X) {
            crocodile->x -= 1;
            if(crocodile->x == -CROCODILE_DIM_X){
                crocodile->x = COLS;
                sleep(rand() % 3);
            }
        }
        if (pipe_fds != NULL) {
            write(pipe_fds[1], crocodile, sizeof(Item));
        }
        usleep(crocodile->speed);
    }
}