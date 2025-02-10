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
            crocodiles[i][j].x = direction == -1 ? -CROCODILE_DIM_X : GAME_WIDTH; // from left to right if true
            crocodiles[i][j].speed = stream_speed[i];
            crocodiles[i][j].extra = direction;
        }
        direction = - direction;
    }
}

void* bullet_right_crocodile(void *arg) {
    Item* bullet = (Item *)arg;

    int expected = bullet->id;

    int  tmp_y = bullet->y;
    bullet->y = -5;
    buffer_push(&buffer, *bullet);
    usleep(current_difficulty.shotload_time);
    bullet->y = tmp_y;
    bullet->extra = 0;

    while (bullet->x < GAME_WIDTH + 1) {
        bullet->x += 1;
        expected = bullet->id;
        if ( atomic_compare_exchange_strong(&collided_bullet, &expected, -1) ) {
            break;
        }
        
        suspend_thread();
        
        buffer_push(&buffer, *bullet);
        usleep(bullet->speed);
    }
    bullet->x = GAME_WIDTH + 10;
    buffer_push(&buffer, *bullet);
    free(arg);
    return NULL;
}

void* bullet_left_crocodile(void *arg) {
    Item* bullet = (Item *)arg;

    int expected = bullet->id;
    int  tmp_y = bullet->y;
    bullet->y = -5;
    buffer_push(&buffer, *bullet);
    usleep(current_difficulty.shotload_time);
    bullet->y = tmp_y;
    bullet->extra = 0;

    while (bullet->x >= 0) {
        bullet->x -= 1;
        expected = bullet->id;
        if ( atomic_compare_exchange_strong(&collided_bullet, &expected, -1) ) {
            break;
        }

        suspend_thread();

        buffer_push(&buffer, *bullet);
        usleep(bullet->speed);
    }
    bullet->x = GAME_WIDTH + 10;
    buffer_push(&buffer, *bullet);
    free(arg);
    return NULL;
}

void crocodile_cleanup_function(void *arg) {
    void **args = (void **)arg;
    pthread_t child_thread = *(pthread_t *)args[0];
    int active = *(int *)args[1];
    if (child_thread != 0) {
        if(active == TRUE){
            if(pthread_tryjoin_np(child_thread, NULL) != 0){
                pthread_cancel(child_thread);
                pthread_join(child_thread, NULL);
            }
        }

    }
    free(args);
    
}

void* Crocodile(void *arg) {

    void **args = (void **)arg;
    Item crocodile = *(Item *)args[0];
    int distance = *(int*)args[1];
    free(args[1]);
    free(args);

    int random_shot, active = FALSE;

    pthread_t thread_bullet = 0;

    continue_usleep(distance);

    // set cleanup function
    void **args_cleanup = malloc(2 * sizeof(void*));
    args_cleanup[0] = &thread_bullet;
    args_cleanup[1] = &active;
    pthread_cleanup_push(crocodile_cleanup_function, args_cleanup);

    while (1) {
        
        random_shot = rand_range(0, current_difficulty.shot_range);
        int shot_speed = crocodile.speed - current_difficulty.crocodile_bullet_speed;
        
        if (crocodile.extra == -1 && crocodile.x < GAME_WIDTH+1) {

            crocodile.x += 1;

            if(random_shot == 1 && active == FALSE) {

                void *arg = malloc(sizeof(void*));
                Item* bullet = malloc(sizeof(Item));

                int x_offset = current_difficulty.shotload_time / crocodile.speed + 1;

                bullet->id = crocodile.id - 2 + CROCODILE_MIN_BULLETS_ID;
                bullet->y = crocodile.y + 1;
                bullet->x = crocodile.x + CROCODILE_DIM_X + 1 + x_offset;
                bullet->speed = shot_speed;
                bullet->extra = 1;

                arg = bullet;
                pthread_create(&thread_bullet, NULL, bullet_right_crocodile, arg);

                active = TRUE;
            }
            else if (active == TRUE)
            {   
                if(pthread_tryjoin_np(thread_bullet, NULL) == 0)
                {
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

                void *arg = malloc(sizeof(void*));
                Item* bullet = malloc(sizeof(Item));

                int x_offset = current_difficulty.shotload_time / crocodile.speed + 1;
                
                bullet->id = crocodile.id - 2 + CROCODILE_MIN_BULLETS_ID;
                bullet->y = crocodile.y + 1;
                bullet->x = crocodile.x - x_offset;
                bullet->speed = shot_speed;
                bullet->extra = 1;

                arg = bullet;
                pthread_create(&thread_bullet, NULL, bullet_left_crocodile, arg);

                active = TRUE;
            }
            else if (active == TRUE)
            {   
                if(pthread_tryjoin_np(thread_bullet, NULL) == 0)
                {
                    active = FALSE;
                }
            }
            if (crocodile.x == -CROCODILE_DIM_X - 1) {

                crocodile.x = GAME_WIDTH;
                usleep(rand_range(0, crocodile.speed * (CROCODILE_DIM_X / 3)));

            }
        }

        //test for cancellation request before writing on buffer
        pthread_testcancel();
        
        suspend_thread();

        // Write crocodile position in the buffer
        buffer_push(&buffer, crocodile); 
        usleep(crocodile.speed);
    }
    pthread_cleanup_pop(0);
    return 0;
}
