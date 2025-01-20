#include "frog.h"
#include "utils.h"

int newmanche = FALSE; 

void signal_handler(int sig){
    if(sig == FROG_ON_CROCODILE_SIG){
        frog_on_crocodile = TRUE;
    }
    if (sig == RESET_MANCHE_SIG)
    {
        newmanche = TRUE;
    }
    
}

void reset_manche(Item *frog){
    frog->y = LINES-4;
    frog->x = (COLS/2)-FROG_DIM_X;
    newmanche = FALSE;
}
void Frog(int *pipe_fds, Item *frog, Item *bullet_left, Item *bullet_right, int stream_speed[STREAM_NUMBER]){
    close(pipe_fds[0]); 
    int max_y, max_x, ch;
    int projectile_active = 0;
    pid_t bullet_pid_right = -1;
    pid_t bullet_pid_left = -1;
    bool has_moved = TRUE;
    getmaxyx(stdscr, max_y, max_x);
    signal(FROG_ON_CROCODILE_SIG, signal_handler);
    signal(RESET_MANCHE_SIG, signal_handler);
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

                        while (bullet_right->x < max_x) {
                            bullet_right->x += 1;
                            if (pipe_fds != NULL) {
                                srand(time(0)+1);
                                usleep(1000*(rand() % (3)));
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
                                usleep(1000*(rand() % (3)));
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
        
        int stream = ((SIDEWALK_HEIGHT_2 + 1 - frog->y) / FROG_DIM_Y) -1;
        // if (frog_on_crocodile) {
        //     if (frog->x + 1 < max_x - FROG_DIM_X) { 
        //         frog->x += 1;
        //         frog_on_crocodile = FALSE;
        //         debuglog("Frog on crocodile %d\n", stream_speed[stream]);
        //         write(pipe_fds[1], frog, sizeof(Item));
        //     }
            
        // }
        
 int speed = 100000;
if (frog_on_crocodile) {
    if (frog->x + 1 < max_x - FROG_DIM_X) { 
        frog->x += 1;
        frog_on_crocodile = FALSE;
        has_moved = TRUE;
        // Use nanosleep for precise sleep handling
        struct timespec req, rem;
        req.tv_sec = stream_speed[stream] / 1000000;               // Convert microseconds to seconds
        req.tv_nsec = (stream_speed[stream] % 1000000) * 1000;     // Convert remaining microseconds to nanoseconds

        while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
            req = rem; // Retry with remaining time if interrupted
        }
    }
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
