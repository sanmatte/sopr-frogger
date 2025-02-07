#include "crocodile.h"

/**
 * @brief  Initiliaze the crocodile matrix
 * @param  crocodiles[][] matrix of crocodiles
 * @param  stream_speed[] speed of the streams
 */
void InitializeCrocodile(Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER], int stream_speed[STREAM_NUMBER]){
    
    int direction = (rand() % 2)? 1: -1; // set random direction for the first stream
    
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++) {
            crocodiles[i][j].id = CROCODILE_MIN_ID + (i * CROCODILE_STREAM_MAX_NUMBER) + j;
            crocodiles[i][j].y = SIDEWALK_HEIGHT_2 - CROCODILE_DIM_Y + 1 - (i * CROCODILE_DIM_Y);  
            crocodiles[i][j].x = direction == -1 ? -CROCODILE_DIM_X : GAME_WIDTH; // Da sinistra verso destra se TRUE
            crocodiles[i][j].speed = stream_speed[i];
            crocodiles[i][j].extra = direction;
        }
        direction = - direction;
    }
}

int exploded = FALSE;
void handlesignal(int signum) {
    if (signum == SIGUSR1) {
        exploded = TRUE;
    }
}

void* Crocodile(void *arg) {
    void **args = (void **)arg;
    Item crocodile = *(Item *)args[1];
    int distance = *(int*)args[2];

    int random_shot;
    int active = FALSE;
    continue_usleep(distance);
    //pid_t bullet_pid = -1; // Store the bullet's PID
    signal(SIGUSR1, handlesignal); // Ignore SIGUSR1 signal
    while (1) {
        
        random_shot = rand_range(0, current_difficulty.shot_range);
        int shot_speed = crocodile.speed - current_difficulty.crocodile_bullet_speed;

        // Check if the bullet process has exited
        // if (active) {
        //     int status;
        //     pid_t result = waitpid(bullet_pid, &status, WNOHANG);
        //     if (result == bullet_pid) {
        //         active = FALSE; // Reset active when the bullet exits
        //     }
        // }
        // if (bullet_pid != -1 && exploded)
        // {
        //     int status;
        //     kill(bullet_pid, SIGKILL);
        //     pid_t result = waitpid(bullet_pid, &status, WNOHANG);
        //     if (result == bullet_pid) {
        //         active = FALSE; // Reset active when the bullet exits
        //     }
        //     bullet_pid = -1;
        //     exploded = FALSE;
        // }
        

        if (crocodile.extra == -1 && crocodile.x < GAME_WIDTH+1) {
            crocodile.x += 1;
            // if (random_shot == 1 && active == FALSE) {
            //     bullet_pid = fork();
            //     if (bullet_pid == 0) {
            //         // Child process: bullet logic
            //         setpgid(0, group_pid); 
            //         Item bullet = {crocodile->id - 2 + CROCODILE_MIN_BULLETS_ID, crocodile->y + 1, crocodile->x + CROCODILE_DIM_X + 1, 0, 0};
            //         while (TRUE) {
            //             if (bullet.x >= GAME_WIDTH) {
            //                 _exit(0); // Bullet exits the screen
            //             }
            //             bullet.x += 1;
            //             write(pipe_fds[1], &bullet, sizeof(Item));
            //             usleep(shot_speed);
            //         }
            //     }
            //     active = TRUE; // Set active when a bullet is fired
            // }
            if (crocodile.x == GAME_WIDTH + 1) {
                crocodile.x = -CROCODILE_DIM_X;
                usleep(rand_range(0, crocodile.speed * (CROCODILE_DIM_X) / 3));
            }
        } else if (crocodile.extra == 1 && crocodile.x > -CROCODILE_DIM_X-1) {
            crocodile.x -= 1;
            // random_shot = rand_range(0, current_difficulty.shot_range);
            // if (random_shot == 1 && active == FALSE) {
            //     bullet_pid = fork();
            //     if (bullet_pid == 0) {
            //         setpgid(0, group_pid); 
            //         Item bullet = {crocodile->id - 2 + CROCODILE_MIN_BULLETS_ID, crocodile->y + 1, crocodile->x - 1, 0, 0};
            //         while (TRUE) {
            //             if (bullet.x <= -1) {
            //                 _exit(0); // Bullet exits the screen
            //             }
            //             bullet.x -= 1;
            //             write(pipe_fds[1], &bullet, sizeof(Item));
            //             usleep(shot_speed);
            //         }
            //     }
            //     active = TRUE; // Set active when a bullet is fired
            // }
            if (crocodile.x == -CROCODILE_DIM_X - 1) {
                crocodile.x = GAME_WIDTH;
                usleep(rand_range(0, crocodile.speed * (CROCODILE_DIM_X / 3)));
            }
        }

        // Write crocodile position to the pipe
        buffer_push(&buffer, crocodile); 
        usleep(crocodile.speed);
    }
    free(args[2]);
    free(args);
}