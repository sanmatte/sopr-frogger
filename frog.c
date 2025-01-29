#include "frog.h"
#include "utils.h"

int newmanche = FALSE; 

void reset_manche(Item *frog){
    frog->y = LINES-4;
    frog->x = (COLS/2)-FROG_DIM_X;
    newmanche = FALSE;
}

void Frog(int *pipe_fds, Item *frog){
    close(pipe_fds[0]); 
    int ch;
    bool has_moved = TRUE;
    //signal(FROG_ON_CROCODILE_SIG, signal_handler);
    //signal(RESET_MANCHE_SIG, signal_handler);
    Item frogtest = {FROG_ID, 0, 0, 0, 0};
    

    while (manche > 0) {
        if (newmanche)
        {
            reset_manche(frog);
        }
        
        ch = getch(); 
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
                frogtest.free = 1;
                frogtest.y = 0;
                frogtest.x = 0;
                has_moved = TRUE;
                break;
            case 'q': 
                endwin();
                break;
        }

        if(has_moved){
            if (pipe_fds != NULL) {
                write(pipe_fds[1], &frogtest, sizeof(Item));
            }
        }
        frogtest.free = 0;
        has_moved = FALSE;
    }
}
