#include "frog.h"
#include "utils.h"
#include "buffer.h"
#include "game.h"
#include <sys/types.h>
int newmanche = FALSE; 

void* Frog() {
    int ch;
    bool has_moved = TRUE;
    Item frog = {FROG_ID, 0, 0, 0, 0};

    while (1) {

        // legge l'ultimo input
        do {
            ch = getch();
        } while (getch() != ERR); // svuota il buffer di input
        frog.id = FROG_ID;
        switch (ch) {
            case KEY_UP:
                frog.y = -FROG_DIM_Y;
                frog.x = 0;
                has_moved = TRUE; //! move inside if
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
            case 32:
                frog.extra = 1;
                frog.y = 0;
                frog.x = 0;
                has_moved = TRUE;
                break;
            case 'q': 
                frog.id = QUIT_ID;
                has_moved = TRUE;
                break;
            //detect escape key press
            case 'p':
                frog.id = PAUSE_ID;
                has_moved = TRUE;
                break;
        }

        suspend_thread();

        if(has_moved){
            buffer_push(&buffer, frog);
        }
        frog.extra = 0;
        has_moved = FALSE;
        usleep(current_difficulty.frog_movement_limit); //default 0
    }
    return NULL;
}

void* bullet_right_fun(void *arg) {
    Item bullet = *(Item *)arg;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // muove il proiettile fino al limite dello schermo
    while (bullet.x < GAME_WIDTH + 1) {
        bullet.x += 1;

        suspend_thread();

        buffer_push(&buffer, bullet);
        usleep(current_difficulty.bullets_speed);
    }
    bullet.extra = 0;
    buffer_push(&buffer, bullet);
    debuglog("bullet extra: %d\n", bullet.extra);
    return NULL;
}

void* bullet_left_fun(void *arg) {
    Item bullet = *(Item *)arg;
    
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // muove il proiettile fino al limite dello schermo
    while (bullet.x >= 0) {
        bullet.x -= 1;

        suspend_thread();

        buffer_push(&buffer, bullet);
        usleep(current_difficulty.bullets_speed);
    }
    bullet.extra = 0;
    buffer_push(&buffer, bullet);
    debuglog("bullet extra: %d\n", bullet.extra);
    return NULL;
}