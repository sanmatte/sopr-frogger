#include "crocodile.h"
extern int testvar;
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
void* bullet_right_crocodile(void *arg) {
    void **args = (void **)arg;
    Item* bullet = (Item *)args[0];
    shared_buffer_t* buffer = (shared_buffer_t *)args[1];

    // Muovi il proiettile fino al limite del gioco
    while (bullet->x < GAME_WIDTH + 1) {
        bullet->x += 1;
        buffer_push(buffer, *bullet);
        usleep(bullet->speed);
    }
    buffer_push(buffer, *bullet);
    free(args);
    return NULL;
}

void* bullet_left_crocodile(void *arg) {
    void **args = (void **)arg;
    Item* bullet = (Item *)args[0];
    shared_buffer_t* buffer = (shared_buffer_t *)args[1];
    
    // Muovi il proiettile fino al limite del gioco
    while (bullet->x >= 0) {
        bullet->x -= 1;
        buffer_push(buffer, *bullet);
        usleep(bullet->speed);
    }
    buffer_push(buffer, *bullet);
    free(args);
    return NULL;
}
void* Crocodile(void *arg) {
    void **args = (void **)arg;
    Item crocodile = *(Item *)args[1];
    int distance = *(int*)args[2];
    free(args[2]);
    free(args);
    testvar += 1;
    int random_shot;
    int active = FALSE;
    pthread_t thread_bullet;
    continue_usleep(distance);
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
            if(random_shot == 1 && active == FALSE) {
                void **args = malloc(3 * sizeof(void*));
                Item* bullet = malloc(sizeof(Item));
                bullet->id = crocodile.id - 2 + CROCODILE_MIN_BULLETS_ID;
                bullet->y = crocodile.y + 1;
                bullet->x = crocodile.x + CROCODILE_DIM_X + 1;
                bullet->speed = shot_speed;
                bullet->extra = 0;
                args[0] = bullet;
                args[1] = &buffer;
                pthread_create(&thread_bullet, NULL, bullet_right_crocodile, args);
                active = TRUE;
            }
            else if (active == TRUE)
            {
                if(pthread_tryjoin_np(thread_bullet, NULL) == 0) {
                    active = FALSE;
                }
            }
            if (crocodile.x == GAME_WIDTH + 1) {
                crocodile.x = -CROCODILE_DIM_X;
                usleep(rand_range(0, crocodile.speed * (CROCODILE_DIM_X) / 3));
            }
        
        } else if (crocodile.extra == 1 && crocodile.x > -CROCODILE_DIM_X-1) {
            crocodile.x -= 1;
            if(random_shot == 1 && active == FALSE) {
                void **args = malloc(3 * sizeof(void*));
                Item* bullet = malloc(sizeof(Item));
                bullet->id = crocodile.id - 2 + CROCODILE_MIN_BULLETS_ID;
                bullet->y = crocodile.y + 1;
                bullet->x = crocodile.x - 1;
                bullet->speed = shot_speed;
                bullet->extra = 0;
                args[0] = bullet;
                args[1] = &buffer;
                pthread_create(&thread_bullet, NULL, bullet_left_crocodile, args);
                active = TRUE;
            }
            else if (active == TRUE)
            {
                if(pthread_tryjoin_np(thread_bullet, NULL) == 0) {
                    active = FALSE;
                }
            }
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