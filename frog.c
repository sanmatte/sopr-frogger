#include "frog.h"
#include "utils.h"

int newmanche = FALSE; 

void reset_manche(Item *frog){
    frog->y = LINES-4;
    frog->x = (COLS/2)-FROG_DIM_X;
    newmanche = FALSE;
}

void Frog(int *pipe_fds, Item *frog, Item *bullet_left, Item *bullet_right){
    close(pipe_fds[0]); 
    int ch, projectile_active = 0;
    pid_t bullet_pid_right = -1;
    pid_t bullet_pid_left = -1;
    bool has_moved = TRUE;
    //signal(FROG_ON_CROCODILE_SIG, signal_handler);
    //signal(RESET_MANCHE_SIG, signal_handler);
    Item frogtest = {FROG_ID, 0, 0, 0, 1};
    

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
            case ' ':
                if (projectile_active == 0) {
                    projectile_active = 1;

                    // Processo per il proiettile a destra
                    bullet_pid_right = fork();
                    if (bullet_pid_right == 0) {
                        bullet_right->x = frog->x + FROG_DIM_X;
                        bullet_right->y = frog->y+1;

                        while (bullet_right->x < GAME_WIDTH) {
                            bullet_right->x += 1;
                            if (pipe_fds != NULL) {
                                srand(time(0)+1);
                                usleep(1000*(rand_range(0, 3)));
                                write(pipe_fds[1], bullet_right, sizeof(Item));
                            }
                            usleep(BULLETS_SPEED);
                        }
                        exit(0);
                    }

                    // Processo per il proiettile a sinistra
                    bullet_pid_left = fork();
                    if (bullet_pid_left == 0) {
                        bullet_left->x = frog->x - 1;
                        bullet_left->y = frog->y + 1;
                        while (bullet_left->x > -1) {
                            bullet_left->x -= 1;
                            if (pipe_fds != NULL) {
                                srand(time(0)+2);
                                usleep(1000*(rand_range(0, 3)));
                                write(pipe_fds[1], bullet_left, sizeof(Item));
                            }
                            usleep(BULLETS_SPEED);  
                        }
                        exit(0);
                    }
                }
                break;
            case 'q': 
            endwin();
        }

        // Controlla se i proiettili sono terminati
        if (bullet_pid_right > 0) {
            int status;
            pid_t result = waitpid(bullet_pid_right, &status, WNOHANG);
            if (result == bullet_pid_right) {
                bullet_pid_right = -1;
            }
        }

        if (bullet_pid_left > 0) {
            int status;
            pid_t result = waitpid(bullet_pid_left, &status, WNOHANG);
            if (result == bullet_pid_left) {
                bullet_pid_left = -1;
            }
        }

        // Resetta projectile_active se entrambi i proiettili sono terminati
        if (bullet_pid_right == -1 && bullet_pid_left == -1) {
            projectile_active = 0;
        }

        if(has_moved){
            // Scrive la posizione della rana nella pipe
            if (pipe_fds != NULL) {
                write(pipe_fds[1], &frogtest, sizeof(Item));
            }
        }
        has_moved = FALSE;
    }
}
