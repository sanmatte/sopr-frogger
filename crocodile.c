#include "crocodile.h"
#include "design.h"
#include "utils.h"
#include "game.h"

#include <signal.h>

/**
 * @brief  Initiliaze the crocodile matrix
 * @param  crocodiles[][] matrix of crocodiles
 * @param  stream_speed[] speed of the streams
 */
void initializeCrocodile(Item **crocodiles, int stream_speed[STREAM_NUMBER]){
    int direction = (rand() % 2)? 1: -1;    // random direction for the first stream
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++) {
            // set crocodile properties
            crocodiles[i][j].id = CROCODILE_MIN_ID + (i * CROCODILE_STREAM_MAX_NUMBER) + j;
            crocodiles[i][j].y = SIDEWALK_HEIGHT_2 - CROCODILE_DIM_Y + 1 - (i * CROCODILE_DIM_Y);  
            crocodiles[i][j].x = direction == 1 ? -CROCODILE_DIM_X : GAME_WIDTH; 
            crocodiles[i][j].speed = stream_speed[i];
            crocodiles[i][j].extra = direction;
        }
        direction = - direction;           // change direction for the next stream
    }
}


/**
 * @brief  Crocodile bullet right controller
 * @param  bullet: Bullet item
 * @param  pipe_fds: Pipe file descriptors
 */
void crocodile_bullet_right_controller(Item *bullet, int *pipe_fds){
    bullet->extra = 0;
    while (TRUE) {
        if (bullet->x >= GAME_WIDTH) {
            _exit(0); // Bullet exits the screen
        }
        bullet->x += 1;
        write(pipe_fds[1], bullet, sizeof(Item));
        usleep(bullet->speed);
    }
}


/**
 * @brief  Crocodile bullet left controller
 * @param  bullet: Bullet item
 * @param  pipe_fds: Pipe file descriptors
 */
void crocodile_bullet_left_controller(Item *bullet, int *pipe_fds){
    bullet->extra = 0;
    while (TRUE) {
        if (bullet->x <= -1) {
            _exit(0); // Bullet exits the screen
        }
        bullet->x -= 1;
        write(pipe_fds[1], bullet, sizeof(Item));
        usleep(bullet->speed);
    }
}


// Global variable to check if the bullet has exploded
int exploded = FALSE;

/**
 * @brief  Handle signal for bullet explosion
 * @param  signum: Signal number
 */
void handlesignal(int signum) {
    if (signum == SIGUSR1) {
        exploded = TRUE;
    }
}


/**
 * @brief  Function to control if the crocodile is on the screen
 * @param  crocodile crocodile object
 * @return 1 if the crocodile is on the screen, 0 otherwise
 */
int crocodile_on_screen(Item *crocodile){
    // control of crocodiles going to the right
    if(crocodile->extra == 1 && crocodile->x + CROCODILE_DIM_X > 0 && crocodile->x < GAME_WIDTH - CROCODILE_DIM_X){
        return 1;
    }
    // control of crocodiles going to the left
    else if(crocodile->extra == -1 && crocodile->x < GAME_WIDTH - CROCODILE_DIM_X && crocodile->x > 0){
        return 1;
    }
    else return 0;
}


/**
 * @brief  Crocodile process
 * @param  pipe_fds: Pipe file descriptors
 * @param  crocodile: Crocodile item
 * @param  group_pid: Group PID
 */
void crocodile(int *pipe_fds, Item *crocodile, int group_pid) {
    close(pipe_fds[0]);

    int random_shot;               // Random number to decide if the crocodile will shoot
    int active = FALSE;            // Check if the bullet is active
    pid_t bullet_pid = -1; 

    signal(SIGUSR1, handlesignal); // Ignore SIGUSR1 signal

    while ((crocodile->extra == 1) ? crocodile->x < GAME_WIDTH + 1 : crocodile->x > -CROCODILE_DIM_X-1) {

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

        // Check if the bullet has exploded
        if (bullet_pid != -1 && exploded)
        {
            int status;
            kill(bullet_pid, SIGKILL);
            pid_t result = waitpid(bullet_pid, &status, WNOHANG);

            if (result == bullet_pid) {
                active = FALSE; // Reset active when the bullet exits
            }

            bullet_pid = -1;
            exploded = FALSE;
        }

        crocodile->x += crocodile->extra; // move crocodile

        // shoot bullet
        if (random_shot == 1 && active == FALSE && crocodile_on_screen(crocodile)) {

            bullet_pid = fork();

            if (bullet_pid == 0) {

                setpgid(0, group_pid); 

                int x_offset = current_difficulty.shotload_time / crocodile->speed + 1;    // space that the crocodile moves in the 1 sec
                Item bullet = {crocodile->id - 2 + CROCODILE_MIN_BULLETS_ID, BULLET_BEFORE_SHOT, crocodile->x + CROCODILE_DIM_X + x_offset, shot_speed, 1};
                bullet.x = crocodile->extra == 1 ? crocodile->x + CROCODILE_DIM_X +1 + x_offset : crocodile->x - x_offset;  // set the x of the projectile based on the direction of the crocodile
                
                write(pipe_fds[1], &bullet, sizeof(Item));
                usleep(current_difficulty.shotload_time);     // time to load the bullet 
                
                bullet.y = crocodile->y + 1;

                if(crocodile->extra == 1){
                    crocodile_bullet_right_controller(&bullet, pipe_fds);
                }
                else{
                    crocodile_bullet_left_controller(&bullet, pipe_fds);
                }
            }

            active = TRUE;
        }
        // Write crocodile position to the pipe
        if (pipe_fds != NULL) {
            write(pipe_fds[1], crocodile, sizeof(Item));
        }
        // Sleep for the crocodile speed
        usleep(crocodile->speed);
    }
}