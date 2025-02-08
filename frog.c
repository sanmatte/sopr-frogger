#include "frog.h"
#include "utils.h"

int newmanche = FALSE; 

void reset_manche(Item *frog){
    frog->y = LINES-4;
    frog->x = (COLS/2)-FROG_DIM_X;
    newmanche = FALSE;
}

void frog_controller(int *pipe_fds, Item *frog){
    close(pipe_fds[0]); 
    int ch;
    bool has_moved = TRUE;
    Item frogtest = {FROG_ID, 0, 0, 0, 0};

    while (manche > 0) {
        if (newmanche)
        {
            reset_manche(frog);
        }

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
            if (pipe_fds != NULL) {
                write(pipe_fds[1], &frogtest, sizeof(Item));
            }
        }
        frogtest.extra = 0;
        has_moved = FALSE;
        usleep(current_difficulty.frog_movement_limit); //default 0
    }
}

void bullet_right_controller(Item *bullet_right, int *pipe_fds){
    //close(pipe_fds[0]);
    bullet_right->extra = 1;
    while (bullet_right->x < GAME_WIDTH + 1) {
        bullet_right->x += 1;
        write(pipe_fds[1], bullet_right, sizeof(Item));
        usleep(current_difficulty.bullets_speed);
    }
    _exit(0);
}

void bullet_left_controller(Item *bullet_left, int *pipe_fds){
    //close(pipe_fds[0]);
    bullet_left->extra = -1;
    while (bullet_left->x > -1) {
        bullet_left->x -= 1;
        write(pipe_fds[1], bullet_left, sizeof(Item));
        usleep(current_difficulty.bullets_speed);
    }
    _exit(0);
}
