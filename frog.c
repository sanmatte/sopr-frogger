#include "frog.h"
#include "utils.h"
#include "buffer.h"
#include <sys/types.h>
int newmanche = FALSE; 

void reset_manche(Item *frog){
    frog->y = LINES-4;
    frog->x = (COLS/2)-FROG_DIM_X;
    newmanche = FALSE;
}

void* Frog() {
    //shared_buffer_t buffer = *(shared_buffer_t*)arg;
    int ch;
    bool has_moved = TRUE;
    Item frogtest = {FROG_ID, 0, 0, 0, 0};

    while (manche > 0) {

        // Read the latest key press (ignore older ones)
        do {
            ch = getch();
        } while (getch() != ERR); // Drain extra inputs
        frogtest.id = FROG_ID;
        switch (ch) {
            case KEY_UP:
                frogtest.y = -FROG_DIM_Y;
                frogtest.x = 0;
                has_moved = TRUE; //! move inside if
                break;
            case KEY_DOWN:
                frogtest.y = +FROG_DIM_Y;
                frogtest.x = 0;
                has_moved = TRUE;
                break;
            case KEY_LEFT:
                frogtest.y = 0;
                frogtest.x = -1;
                has_moved = TRUE;
                break;
            case KEY_RIGHT:
                frogtest.y = 0;
                frogtest.x = +1;
                has_moved = TRUE;
                break;
            case 32:
                frogtest.extra = 1;
                frogtest.y = 0;
                frogtest.x = 0;
                has_moved = TRUE;
                break;
            case 'q': 
                //kill_all(getpid(), getpgrp());
                break;
            //detect escape key press
            case 'p':
                frogtest.id = PAUSE_ID;
                has_moved = TRUE;
                break;
        }

        if(has_moved){
            buffer_push(&buffer, frogtest);
        }
        frogtest.extra = 0;
        has_moved = FALSE;
        usleep(current_difficulty.frog_movement_limit); //default 0
    }
    return NULL;
}

void* bullet_right_fun(void *arg) {
    void **args = (void **)arg;
    // Ottieni i puntatori alle strutture
    Item* bullet = (Item *)args[0];
    shared_buffer_t* buffer = (shared_buffer_t *)args[1];

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // Muovi il proiettile fino al limite del gioco
    while (bullet->x < GAME_WIDTH + 1) {
        bullet->x += 1;

        suspend_thread();

        buffer_push(buffer, *bullet);
        usleep(current_difficulty.bullets_speed);
    }
    bullet->extra = 0;
    buffer_push(buffer, *bullet);
    free(args);
    return NULL;
}

void* bullet_left_fun(void *arg) {
    void **args = (void **)arg;

    Item* bullet = (Item *)args[0];
    shared_buffer_t* buffer = (shared_buffer_t *)args[1];
    
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // Muovi il proiettile fino al limite del gioco
    while (bullet->x >= 0) {
        bullet->x -= 1;

        suspend_thread();

        buffer_push(buffer, *bullet);
        usleep(current_difficulty.bullets_speed);
    }
    bullet->extra = 0;
    buffer_push(buffer, *bullet);
    free(args);
    return NULL;
}