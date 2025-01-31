#include "crocodile.h"

void InitializeCrocodile(Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER], int direction, int stream_speed[STREAM_NUMBER]){
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++) {
            crocodiles[i][j].id = CROCODILE_MIN_ID + (i * CROCODILE_STREAM_MAX_NUMBER) + j;
            crocodiles[i][j].y = SIDEWALK_HEIGHT_2 - CROCODILE_DIM_Y + 1 - (i * CROCODILE_DIM_Y);  
            // if (direction == 0) {
            //     crocodiles[i][j].x = -CROCODILE_DIM_X;  // Da sinistra verso destra
            // } else {
            //     crocodiles[i][j].x = GAME_WIDTH;  // Da destra verso sinistra
            // }
            crocodiles[i][j].x = direction == -1 ? -CROCODILE_DIM_X : GAME_WIDTH; // Da sinistra verso destra se TRUE
            crocodiles[i][j].speed = stream_speed[i];
            crocodiles[i][j].extra = direction;
        
        }
        direction = - direction;
    }
}

// void Crocodile(int *pipe_fds, Item *crocodile, int direction) {
//     close(pipe_fds[0]);
//     direction = crocodile->extra;
//     int random_shot;
//     int active = FALSE;
//     while (1) {
//         random_shot = rand_range(0, 5);
//         int shot_speed = rand_range(50000, crocodile->speed-50000);
//         if (direction == -1 && crocodile->x < GAME_WIDTH) {
//             crocodile->x += 1;
//             if (random_shot == 1 && active == FALSE) {
//                 pid_t bullet_pid = fork();
//                 if(bullet_pid == 0){
//                     Item bullet = {crocodile->id - 2 + CROCODILE_MIN_BULLETS_ID, crocodile->y + 1, crocodile->x + CROCODILE_DIM_X + 1, 0, 0};
//                     while(TRUE){
//                         bullet.x += 1;
//                         write(pipe_fds[1], &bullet, sizeof(Item));
//                         usleep(shot_speed);
//                         if (bullet.x == GAME_WIDTH) {
//                             exit(0);
//                         }
//                     }
//                 }
//                 active = TRUE;
//                 int status;
//                 pid_t result = waitpid(bullet_pid, &status, WNOHANG);
//                 if (result == bullet_pid) {
//                     active = FALSE;
//                 }
//             }
//             if (crocodile->x == GAME_WIDTH) {
//                 crocodile->x = -CROCODILE_DIM_X;
//                 sleep(rand_range(1, 3));
//             }

//         } else if (direction == 1 && crocodile->x > -CROCODILE_DIM_X) {
//             crocodile->x -= 1;
//             random_shot = rand_range(0, 5);
//             if (random_shot == 1 && active == FALSE) {
//                 //debuglog("Crocodile %d shot\n", crocodile->id);
//                 pid_t bullet_pid = fork();
//                 if(bullet_pid == 0){
//                     Item bullet = {crocodile->id - 2 + CROCODILE_MIN_BULLETS_ID, crocodile->y + 1, crocodile->x - 1, 0, 0};
//                     while(TRUE){
//                         bullet.x -= 1;
//                         write(pipe_fds[1], &bullet, sizeof(Item));
//                         usleep(shot_speed);
//                         if (bullet.x == -1) {
//                             exit(0);
//                         }
//                     }
//                 }
//                 active = TRUE;
//                 int status;
//                 pid_t result = waitpid(bullet_pid, &status, WNOHANG);
//                 if (result == bullet_pid) {
//                     active = FALSE;
//                 }
//             }
//             if (crocodile->x == -CROCODILE_DIM_X) {
//                 crocodile->x = COLS;
//                 sleep(rand_range(1, 3));
//             }
//         }
//         if (pipe_fds != NULL) {
//             write(pipe_fds[1], crocodile, sizeof(Item));
//         }
//         usleep(crocodile->speed);
        
//     }
// }
int exploded = FALSE;
void handlesignal(int signum) {
    if (signum == SIGUSR1) {
    exploded = FALSE;
        return;
    }
}
void Crocodile(int *pipe_fds, Item *crocodile, int direction) {
    close(pipe_fds[0]);
    direction = crocodile->extra;
    int random_shot;
    int active = FALSE;
    pid_t bullet_pid = -1; // Store the bullet's PID
    //signal(SIGUSR1, handlesignal); // Ignore SIGUSR1 signal
    while (1) {
        random_shot = rand_range(0, current_difficulty.shot_range);
        int shot_speed = crocodile->speed - current_difficulty.crocodile_bullet_speed;

        // Check if the bullet process has exited
        if (active) {
            int status;
            pid_t result = waitpid(bullet_pid, &status, WNOHANG);
            if (result == bullet_pid) {
                active = FALSE; // Reset active when the bullet exits
            }
        }

        if (direction == -1 && crocodile->x < GAME_WIDTH+1) {
            crocodile->x += 1;
            if (random_shot == 1 && active == FALSE) {
                bullet_pid = fork();
                if (bullet_pid == 0) {
                    // Child process: bullet logic
                    Item bullet = {crocodile->id - 2 + CROCODILE_MIN_BULLETS_ID, crocodile->y + 1, crocodile->x + CROCODILE_DIM_X + 1, 0, 0};
                    while (TRUE) {
                        if (bullet.x >= GAME_WIDTH || exploded) {
                            _exit(0); // Bullet exits the screen
                        }
                        bullet.x += 1;
                        write(pipe_fds[1], &bullet, sizeof(Item));
                        usleep(shot_speed);
                    }
                }
                active = TRUE; // Set active when a bullet is fired
            }
            if (crocodile->x == GAME_WIDTH + 1) {
                crocodile->x = -CROCODILE_DIM_X;
                usleep(rand_range(0, crocodile->speed * (CROCODILE_DIM_X) / 3));
            }
        } else if (direction == 1 && crocodile->x > -CROCODILE_DIM_X-1) {
            crocodile->x -= 1;
            random_shot = rand_range(0, current_difficulty.shot_range);
            if (random_shot == 1 && active == FALSE) {
                bullet_pid = fork();
                if (bullet_pid == 0) {
                    // Child process: bullet logic
                    Item bullet = {crocodile->id - 2 + CROCODILE_MIN_BULLETS_ID, crocodile->y + 1, crocodile->x - 1, 0, 0};
                    while (TRUE) {
                        if (bullet.x <= -1 || exploded) {
                            _exit(0); // Bullet exits the screen
                        }
                        bullet.x -= 1;
                        write(pipe_fds[1], &bullet, sizeof(Item));
                        usleep(shot_speed);
                    }
                }
                active = TRUE; // Set active when a bullet is fired
            }
            if (crocodile->x == -CROCODILE_DIM_X - 1) {
                crocodile->x = COLS;
                usleep(rand_range(0, crocodile->speed * (CROCODILE_DIM_X / 3)));
            }
        }

        // Write crocodile position to the pipe
        if (pipe_fds != NULL) {
            write(pipe_fds[1], crocodile, sizeof(Item));
        }

        usleep(crocodile->speed);
    }
}