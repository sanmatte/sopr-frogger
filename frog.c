#include "frog.h"

int newmanche = FALSE; 

/**
 * @brief  function that manages the frog movement
 */
void* frog_controller() {
    int ch;
    bool has_moved = TRUE;               // flag to check if the frog has moved
    Item frog = {FROG_ID, 0, 0, 0, 0};
    while (1) {
        // read the last input from the keyboard
        do {
            ch = getch();
        } while (getch() != ERR);
        frog.id = FROG_ID;
        switch (ch) {
            case KEY_UP:
                frog.y = -FROG_DIM_Y;
                frog.x = 0;
                has_moved = TRUE;
                break;
            case KEY_DOWN:
                frog.y = +FROG_DIM_Y;
                frog.x = 0;
                has_moved = TRUE;
                break;
            case KEY_LEFT:
                frog.y = 0;
                frog.x = -1;
                has_moved = TRUE;
                break;
            case KEY_RIGHT:
                frog.y = 0;
                frog.x = +1;
                has_moved = TRUE;
                break;
            case 32:                     // space bar to shoot
                frog.extra = 1;
                frog.y = 0;
                frog.x = 0;
                has_moved = TRUE;
                break;
            case 'q':                    // quit the game
                frog.id = QUIT_ID;
                has_moved = TRUE;
                break;
            //detect escape key press
            case 'p':                    // pause the game
                frog.id = PAUSE_ID;
                has_moved = TRUE;
                break;
        }
        suspend_thread();                // suspend the thread 
        if(has_moved){
            buffer_push(&buffer, frog);
        }
        frog.extra = 0;
        has_moved = FALSE;
        usleep(current_difficulty.frog_movement_limit); //default 0
    }
    return NULL;
}


/**
 * @brief  function for the movement of the frog's right projectile
 * @param  arg bullet args
 */
void* bullet_right_fun(void *arg) {
    Item bullet = *(Item *)arg;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // move the bullet until the screen limit
    while (bullet.x < GAME_WIDTH + 1) {
        bullet.x += 1;
        suspend_thread();
        buffer_push(&buffer, bullet);
        usleep(current_difficulty.bullets_speed);
    }
    bullet.extra = 0;
    buffer_push(&buffer, bullet);
    return NULL;
}


/**
 * @brief  function for the movement of the frog's left projectile
 * @param  arg bullet args
 */
void* bullet_left_fun(void *arg) {
    Item bullet = *(Item *)arg;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // move the bullet until the screen limit
    while (bullet.x >= 0) {
        bullet.x -= 1;
        suspend_thread();
        buffer_push(&buffer, bullet);
        usleep(current_difficulty.bullets_speed);
    }
    bullet.extra = 0;
    buffer_push(&buffer, bullet);
    return NULL;
}