#include "crocodile.h"

/**
 * @brief  Initialization of the crocodile matrix
 * @param  crocodiles[][] matrix of crocodiles
 * @param  stream_speed[] array of stream speed
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
 * @brief  Function to free crocodile bullets
 * @param  arg arguments
 */
void bullet_cleanup_function(void *arg) {
    free(arg);
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
 * @brief  Function for the movement of the crocodile bullets to the right
 * @param  arg bullet_obj
 */
void* bullet_right_crocodile(void *arg) {
    Item* bullet = (Item *)arg;
    pthread_cleanup_push(bullet_cleanup_function, arg);
    int expected = bullet->id;

    // management of the crocodile bullet before shooting
    int  tmp_y = bullet->y;
    bullet->y = BULLET_BEFORE_SHOT;
    buffer_push(&buffer, *bullet);
    usleep(current_difficulty.shotload_time);
    bullet->y = tmp_y;
    bullet->extra = 0;

    // movement of the bullet
    while (bullet->x < GAME_WIDTH + 1) {
        bullet->x += 1;
        expected = bullet->id;
        // atomic control (more efficient than mutex) if true exits the loop and sets collided_bullet to -1
        if ( atomic_compare_exchange_strong(&collided_bullet, &expected, -1) ) {
            break;
        }
        suspend_thread();
        buffer_push(&buffer, *bullet);
        usleep(bullet->speed);
    }
    // reset bullet position to avoid collisions
    bullet->x = GAME_WIDTH + 10;
    buffer_push(&buffer, *bullet);
    pthread_cleanup_pop(arg);
    return NULL;
}

/**
 * @brief  Function for the movement of the crocodile bullets to the left
 * @param  arg bullet_obj
 */
void* bullet_left_crocodile(void *arg) {
    Item* bullet = (Item *)arg;
    pthread_cleanup_push(bullet_cleanup_function, arg);
    int expected = bullet->id;

    // management of the crocodile bullet before shooting
    int  tmp_y = bullet->y;
    bullet->y = -5;
    buffer_push(&buffer, *bullet);
    usleep(current_difficulty.shotload_time);
    bullet->y = tmp_y;
    bullet->extra = 0;

    // movement of the bullet
    while (bullet->x >= 0) {
        bullet->x -= 1;
        expected = bullet->id;
        // atomic control (more efficient than mutex) if true exits the loop and sets collided_bullet to -1
        if ( atomic_compare_exchange_strong(&collided_bullet, &expected, -1) ) {
            break;
        }
        suspend_thread();
        buffer_push(&buffer, *bullet);
        usleep(bullet->speed);
    }

    bullet->x = GAME_WIDTH + 10;
    buffer_push(&buffer, *bullet);
    pthread_cleanup_pop(arg);
    return NULL;
}

/**
 * @brief  cleanup for the crocodile thread which kills the bullet thread if it is not terminated
 * @param  arg arguments
 */
void crocodile_cleanup_function(void *arg) {
    void **args = (void **)arg;
    pthread_t child_thread = *(pthread_t *)args[0];
    int active = *(int *)args[1];
    if (child_thread != 0) {
        if(active == TRUE){
            // if the bullet thread is not terminated it is killed
            if(pthread_tryjoin_np(child_thread, NULL) != 0){
                pthread_cancel(child_thread);
                pthread_join(child_thread, NULL);
            }
        }
    }
    free(args);
}

/**
 * @brief  Function for the movement of the crocodile
 * @param  arg -> crocodile_obj, distance
 */
void* crocodile_fun(void *arg) {
    void **args = (void **)arg;
    Item crocodile = *(Item *)args[0];
    int distance = *(int*)args[1];
    free(args[1]);
    free(args);
    int random_shot, active = FALSE;
    pthread_t thread_bullet = 0;
    continue_usleep(distance);        // wait for the spawn of the crocodile

    // cleanup function that kills the bullet thread if it is not terminated
    void **args_cleanup = malloc(2 * sizeof(void*));
    args_cleanup[0] = &thread_bullet;
    args_cleanup[1] = &active;
    pthread_cleanup_push(crocodile_cleanup_function, args_cleanup);

    // movement of the crocodile
    while ( (crocodile.extra == 1) ? crocodile.x < GAME_WIDTH + 1 : crocodile.x > -CROCODILE_DIM_X-1 ) {
        random_shot = rand_range(0, current_difficulty.shot_range);                      // generate a random number to shoot
        int shot_speed = crocodile.speed - current_difficulty.crocodile_bullet_speed;    // calculate and set the speed of the bullet
        crocodile.x += crocodile.extra;
        // if the random number is 1 and the crocodile is not active, a bullet is shot
        if(random_shot == 1 && active == FALSE && crocodile_on_screen(&crocodile)) {
            void *arg;
            Item* bullet = malloc(sizeof(Item));
            int x_offset = current_difficulty.shotload_time / crocodile.speed + 1;
            // set bullet properties
            bullet->id = crocodile.id - 2 + CROCODILE_MIN_BULLETS_ID;
            bullet->y = crocodile.y + 1;
            bullet->x = crocodile.extra == 1 ? crocodile.x + CROCODILE_DIM_X + 1 + x_offset : crocodile.x - x_offset;
            bullet->speed = shot_speed;
            bullet->extra = 1;
            arg = bullet;
            // create the bullet thread and set active to TRUE
            if (crocodile.extra == 1)
                pthread_create(&thread_bullet, NULL, bullet_right_crocodile, arg);
            else
                pthread_create(&thread_bullet, NULL, bullet_left_crocodile, arg);
            active = TRUE;
        }
        //check if the bullet is still active
        else if (active == TRUE)
        {   
            if(pthread_tryjoin_np(thread_bullet, NULL) == 0)
            {
                active = FALSE;
            }
        }

        // check if there are any pending threads
        pthread_testcancel();
        // pause the thread (runs if the user requests a break)
        suspend_thread();

        buffer_push(&buffer, crocodile); 
        usleep(crocodile.speed);
    }
    pthread_cleanup_pop(1);
    return 0;
}
