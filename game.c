#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>
#include "design.h"
#include "struct.h"
#include "utils.h"
#include "crocodile.h"
#include "frog.h"

int manche = 3;
bool frog_on_crocodile = FALSE;
void handlesigs(int sig){
    if(sig == FROG_ON_CROCODILE_SIG){
        frog_on_crocodile = TRUE;
    }
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
    int even, odd;
    if (direction == 0) {
        even = 1;
        odd = -1;
    } else {
        odd = 1;
        even = -1;
    }
    Item msg;
    signal(FROG_ON_CROCODILE_SIG, handlesigs);
    while (manche > 0) {
        if (read(pipe_fds[0], &msg, sizeof(Item)) > 0) {
            switch(msg.id){
                //FROG case
                case FROG_ID:
                    frog.y += msg.y;
                    debuglog("frog.x %d\n", frog.x);
                    frog.x += msg.x;
                    break;
                
                //BULLETS cas
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
                                    int stream = i;
                                    if(frog.y > SIDEWALK_HEIGHT_1 && frog.y < SIDEWALK_HEIGHT_2){ 
                                        if ( stream >= 0 && stream < STREAM_NUMBER)
                                        {
                                            if(frog.y == crocodiles[stream][j].y && frog.x >= crocodiles[stream][j].x && frog.x <= crocodiles[stream][j].x + CROCODILE_DIM_X - FROG_DIM_X){
                                                    if (stream % 2 != 0)
                                                    {
                                                        frog.x += odd;
                                                    }
                                                    else
                                                    {
                                                        frog.x += even;
                                                    }
                                                    
                                                    
                                                    
                                                    print_frog(&frog);
                                                    //refresh();
                                                    debuglog("cock %d\n", crocodiles[stream][j].speed);
                                            }
                                        }
                                    }
                            }
                        }
                    }
                    break;
                }
            }
            int stream = ((SIDEWALK_HEIGHT_2 + 1 - frog.y) / FROG_DIM_Y) -1;
            if(frog.y > SIDEWALK_HEIGHT_1 || frog.y < SIDEWALK_HEIGHT_2){
                for (size_t j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++)
                {
                    if ( stream >= 0 && stream < STREAM_NUMBER)
                    {
                        if( !(frog.y == crocodiles[stream][j].y && frog.x >= crocodiles[stream][j].x && frog.x <= crocodiles[stream][j].x + CROCODILE_DIM_X - FROG_DIM_X)){
                            manche--;
                            frog.y = LINES-4;
                            frog.x = (COLS/2)-FROG_DIM_X;
                        }
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

// void start_game(){
    
// }