#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <locale.h>

#include "design.h"

int manche = 3;
bool frog_on_crocodile = FALSE;

void move_on_c(int sig){
    if(sig == FROG_ON_CROCODILE_SIG){
        frog_on_crocodile = TRUE;
    }
}
void Frog(int *pipe_fds, Item *frog, Item *bullet_left, Item *bullet_right, int stream_speed[STREAM_NUMBER]){
    close(pipe_fds[0]);  
    int max_y, max_x, ch;
    int projectile_active = 0;
    pid_t bullet_pid_right = -1;
    pid_t bullet_pid_left = -1;
    bool has_moved = TRUE;
    
    getmaxyx(stdscr, max_y, max_x);
    signal(FROG_ON_CROCODILE_SIG, move_on_c);

    while (manche > 0) {
        ch = getch(); 
        switch (ch) {
            case KEY_UP:
                if (frog->y > SCORE_HEIGHT + 1) frog->y -= FROG_DIM_Y;
                has_moved = TRUE;
                break;
            case KEY_DOWN:
                if (frog->y < max_y - FROG_DIM_Y) frog->y += FROG_DIM_Y;
                has_moved = TRUE;
                break;
            case KEY_LEFT:
                if (frog->x > 0) frog->x -= 1;
                has_moved = TRUE;
                break;
            case KEY_RIGHT:
                if (frog->x < max_x - FROG_DIM_X) frog->x += 1;
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
        
        int stream = (frog->y - SIDEWALK_HEIGHT_1) / FROG_DIM_Y;
        if (frog_on_crocodile) {
            if (frog->x + 1 < max_x - FROG_DIM_X) { 
                frog->x += 1;
                has_moved = TRUE;
                usleep(stream_speed[stream]);
            }
            frog_on_crocodile = FALSE;
        }

        if(has_moved){
            // Scrive la posizione della rana nella pipe
            if (pipe_fds != NULL) {
                write(pipe_fds[1], frog, sizeof(Item));
            }
        }
        has_moved = FALSE;
    }
}

void InitializeCrocodile(Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER], int direction, int stream_speed[STREAM_NUMBER]){
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++) {
            if (crocodiles[i][j].free == 0){
                crocodiles[i][j].id = CROCODILE_MIN_ID + (i * CROCODILE_STREAM_MAX_NUMBER) + j;
                crocodiles[i][j].y = SIDEWALK_HEIGHT_2 - CROCODILE_DIM_Y + 1 - (i * CROCODILE_DIM_Y);  
                if (direction == 0) {
                    crocodiles[i][j].x = -CROCODILE_DIM_X;  // Da sinistra verso destra
                } else {
                    crocodiles[i][j].x = COLS;  // Da destra verso sinistra
                }
                crocodiles[i][j].speed = stream_speed[i];
                crocodiles[i][j].free = 1;
            }
        }
        direction= 1 - direction;
    }
}

void Crocodile(int *pipe_fds, Item *crocodile, int random_number){
    close(pipe_fds[0]);
    while(manche>0){
        if (random_number == 0 && crocodile->x < COLS) {
            crocodile->x += 1;
            if(crocodile->x == COLS){
                crocodile->x = -CROCODILE_DIM_X;
                sleep(2);
            }
        }
        else if (random_number == 1 && crocodile->x > -CROCODILE_DIM_X) {
            crocodile->x -= 1;
            if(crocodile->x == -CROCODILE_DIM_X){
                crocodile->x = COLS;
                sleep(2);
            }
        }
        if (pipe_fds != NULL) {
            write(pipe_fds[1], crocodile, sizeof(Item));
        }
        usleep(crocodile->speed);
    }
}

time_t timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL); // Get unix time in seconds and the microseconds
    time_t milliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000; // Calc unix time in milliseconds
    return milliseconds;
}

int main(){
    setlocale(LC_ALL, "");
    initscr();start_color();curs_set(0);keypad(stdscr, TRUE);noecho();cbreak();nodelay(stdscr, TRUE);srand(time(NULL));

    Item frog = {FROG_ID, LINES-4, (COLS/2)-FROG_DIM_X, 0, 1};
    Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER];
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
            crocodiles[i][j].free = 0;
        }
    }
    Item bullet_right = {BULLETS_ID, -1, -1, 0, 0};
    Item bullet_left = {BULLETS_ID, -1, -1, 0, 0};

    init_color(COLOR_DARKGREEN, 0, 400, 0);
    init_color(COLOR_GREY, 600, 600, 600);

    // Definizione delle coppie di colori
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_GREY);  
    init_pair(3, COLOR_BLACK, COLOR_BLUE);
    init_pair(4, COLOR_DARKGREEN, COLOR_BLUE);
    init_pair(5, COLOR_RED, COLOR_RED);
    
    // Definizione variabili
    int pipe_fds[2], direction = rand() % 2;
    int stream_speed[STREAM_NUMBER];

    for(int i=0; i<STREAM_NUMBER; i++){
        stream_speed[i] = rand() % (CROCODILE_SPEED_MAX - CROCODILE_SPEED_MIN) + CROCODILE_SPEED_MIN;
    }

    if(pipe(pipe_fds) != 0){
        exit(0);
    }
    
    InitializeCrocodile(crocodiles, direction, stream_speed);

    pid_t child_pids[(STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1];

    // Creazione del processo Frog
    pid_t pid_frog = fork();
    if (pid_frog < 0) {
        perror("Errore nella fork");
        endwin();
        exit(EXIT_FAILURE);
    } else if (pid_frog == 0) {
        Frog(pipe_fds, &frog, &bullet_left, &bullet_right, stream_speed);
        exit(0);
    } else {
        child_pids[0] = pid_frog;
    }
    
    // Creazione dei processi Crocodile
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
            pid_t pid_croc = fork();
            if (pid_croc < 0) {
                perror("Errore nella fork del coccodrillo");
                endwin();
                exit(EXIT_FAILURE);
            } else if (pid_croc == 0) { 
                srand(timestamp() + crocodiles[i][j].id + getpid());   
                usleep((rand() % 3000000) + 1000000); 
                Crocodile(pipe_fds, &crocodiles[i][j], direction);
                exit(0);
            } else {
                child_pids[i * CROCODILE_STREAM_MAX_NUMBER + j + 1] = pid_croc;
            }
            direction = 1 - direction;
        }
    }
    
    Item msg;
    while (manche > 0) {
        if (read(pipe_fds[0], &msg, sizeof(Item)) > 0) {
            switch(msg.id){
                //FROG case
                case FROG_ID:
                    frog = msg;
                    break;
                
                //BULLETS case
                case BULLETS_ID:
                    if(msg.x < frog.x){
                        bullet_left = msg;
                    }else{
                        bullet_right = msg;
                    }
                    break;
                
                //CROCODILE case
                default:
                    if(msg.id >= CROCODILE_MIN_ID && msg.id <= CROCODILE_MAX_ID){
                        for(int i = 0; i < STREAM_NUMBER; i++){
                            for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                                if(crocodiles[i][j].id == msg.id){
                                    crocodiles[i][j] = msg;
                                }
                            }
                        }
                    }
                    break;
            }

            //int stream = ((LINES-1-frog.y+1) / FROG_DIM_Y)+1;
            for(int i = 0; i < STREAM_NUMBER; i++){
                for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                    if(crocodiles[i][j].y >= 0 && frog.y == crocodiles[i][j].y && frog.x >= crocodiles[i][j].x && frog.x <= crocodiles[i][j].x + CROCODILE_DIM_X - FROG_DIM_X) {
                        kill(child_pids[0], FROG_ON_CROCODILE_SIG);
                    }
                }
            }

            print_background();

            for(int i = 0; i < STREAM_NUMBER; i++){
                for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                    print_crocodile(&crocodiles[i][j]);
                }
            }
            
            print_frog(&frog);
            print_bullets(&bullet_left);
            print_bullets(&bullet_right);
            
            refresh();
        }
    }

    for (int i = 0; i < (STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1; i++) {
        kill(child_pids[i], SIGTERM);  // Invia il segnale di terminazione
    }

    for (int i = 0; i < (STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1; i++) {
        waitpid(child_pids[i], NULL, 0);
    }
    endwin();
    return 0;
}