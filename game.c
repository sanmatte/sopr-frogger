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

int manche = 3, score = 0;
bool endgame = FALSE;
bool dens[DENS_NUMBER] = {TRUE, TRUE, TRUE, TRUE, TRUE};

void game_timer(int *pipe_fds){
    Item msg = {TIMER_ID, 0, 0, 0, 0};
    while(1){
        write(pipe_fds[1], &msg, sizeof(Item));
        sleep(1);
    }
}

int main(){
    setlocale(LC_ALL, "");
    initscr();start_color();curs_set(0);keypad(stdscr, TRUE);noecho();cbreak();nodelay(stdscr, TRUE);srand(time(NULL));
    
    int bullet_frog = 0;

    WINDOW *game = newwin(GAME_HEIGHT, GAME_WIDTH, 0, 0);

    Item frog = {FROG_ID, GAME_HEIGHT-4, (GAME_WIDTH/2)-FROG_DIM_X/2, 0, 0};
    Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER];
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
            crocodiles[i][j].free = 0;
        }
    }
    Item bullet_right = {BULLETS_ID, -1, -1, 0, 0};
    Item bullet_left = {BULLETS_ID, -1, -1, 0, 0};
    Item timer = {TIMER_ID, 0, 60, 0, 0};
    
    Item crocodiles_bullets[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER];
    for (int i = 0; i < STREAM_NUMBER; i++) {
        for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
            crocodiles_bullets[i][j].id = CROCODILE_MIN_BULLETS_ID + (i * CROCODILE_STREAM_MAX_NUMBER) + j;
            crocodiles_bullets[i][j].x = -1;
            crocodiles_bullets[i][j].y = -1;
            crocodiles_bullets[i][j].speed = CROCODILE_BULLET_SPEED;
            crocodiles_bullets[i][j].free = 1;
        }
    }

    init_color(COLOR_DARKGREEN, 0, 400, 0);
    init_color(COLOR_GREY, 600, 600, 600);
    init_color(COLOR_LIGHTDARKGREEN, 28, 163, 32);
    init_color(COLOR_SAND, 745, 588, 313);
    init_color(COLOR_FROG_EYE, 90, 113, 749);
    init_color(COLOR_FROG_BODY, 62, 568, 184);
    init_color(COLOR_DARK_ORANGE, 815, 615, 98);
    init_color(COLOR_ENDGAME_BACKGROUND, 8, 372, 600);

    // Definizione delle coppie di colori
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    init_pair(2, COLOR_GREY, COLOR_GREY);  
    init_pair(3, COLOR_BLACK, COLOR_BLUE);
    init_pair(4, COLOR_DARKGREEN, COLOR_BLUE);
    init_pair(5, COLOR_RED, COLOR_RED);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_LIGHTDARKGREEN, COLOR_DARKGREEN);
    init_pair(8, COLOR_DARKGREEN, COLOR_DARKGREEN);
    init_pair(9, COLOR_BLACK, COLOR_GREEN);
    init_pair(10, COLOR_SAND, COLOR_SAND);
    init_pair(11, COLOR_DARKGREEN, COLOR_BLACK);
    init_pair(12, COLOR_FROG_EYE, COLOR_FROG_BODY);
    init_pair(13, COLOR_FROG_BODY, COLOR_FROG_BODY);
    init_pair(14, COLOR_DARK_ORANGE, COLOR_BLACK);
    init_pair(15, COLOR_ENDGAME_BACKGROUND, COLOR_FROG_BODY);

    
    // Definizione variabili
    int pipe_fds[2], direction = rand_range(0, 1);
    int stream_speed[STREAM_NUMBER];

    for(int i=0; i<STREAM_NUMBER; i++){
        stream_speed[i] = rand_range(CROCODILE_SPEED_MIN, CROCODILE_SPEED_MAX);
    }

    if(pipe(pipe_fds) != 0){
        exit(0);
    }
    
    InitializeCrocodile(crocodiles, direction, stream_speed);

    pid_t child_pids[(STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1];

    pid_t pid_timer = fork();
    if (pid_timer < 0) {
        perror("Errore nella fork");
        endwin();
        exit(EXIT_FAILURE);
    } else if (pid_timer == 0) {
        game_timer(pipe_fds);
        exit(0);
    }

    //Creazione del processo Frog
    pid_t pid_frog = fork();
    if (pid_frog < 0) {
        perror("Errore nella fork");
        endwin();
        exit(EXIT_FAILURE);
    } else if (pid_frog == 0) {
        Frog(pipe_fds, &frog);
        exit(0);
    } else {
        child_pids[0] = pid_frog;
    }

    // Creazione dei processi Crocodile
    for (int i = 0; i < STREAM_NUMBER; i++) {
        int distance = 1000000;
        for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
            pid_t pid_croc = fork();
            if (pid_croc < 0) {
                perror("Errore nella fork del coccodrillo");
                endwin();
                exit(EXIT_FAILURE);
            } else if (pid_croc == 0) { 
                srand(timestamp() + crocodiles[i][j].id + getpid()); 
                distance += rand_range(400000, 1000000);
                usleep(distance); 
                Crocodile(pipe_fds, &crocodiles[i][j], direction, crocodiles_bullets);
                exit(0);
            } else {
                child_pids[i * CROCODILE_STREAM_MAX_NUMBER + j + 1] = pid_croc;
                distance += 8000000;
            }
        }
        
        direction = 1 - direction;
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
    pid_t bullet_pid_left, bullet_pid_right;
    while (endgame == FALSE) {
    if (read(pipe_fds[0], &msg, sizeof(Item)) > 0) {
        switch (msg.id) {
            // FROG case
            case FROG_ID:
                if (frog.x + msg.x >= 0 && frog.x + msg.x <= COLS - FROG_DIM_X && frog.y + msg.y >= 0 && frog.y + msg.y <= LINES - FROG_DIM_Y) {
                    frog.y += msg.y;
                    frog.x += msg.x;
                    frog.free = msg.free;

                    if (frog.free == 1 && bullet_frog == 0) {
                        bullet_frog = 1;

                        // Creazione del processo per il proiettile destro
                        bullet_pid_right = fork();
                        if (bullet_pid_right == 0) {
                            bullet_right.x = frog.x + FROG_DIM_X;
                            bullet_right.y = frog.y + 1;
                            while (bullet_right.x < GAME_WIDTH + 1) {
                                bullet_right.x += 1;
                                write(pipe_fds[1], &bullet_right, sizeof(Item));
                                usleep(BULLETS_SPEED);
                            }
                            exit(0);
                        }

                        // Creazione del processo per il proiettile sinistro
                        bullet_pid_left = fork();
                        if (bullet_pid_left == 0) {
                            bullet_left.x = frog.x - 1;
                            bullet_left.y = frog.y + 1;
                            while (bullet_left.x > -1) {
                                bullet_left.x -= 1;
                                write(pipe_fds[1], &bullet_left, sizeof(Item));
                                usleep(BULLETS_SPEED);
                            }
                            exit(0);
                        }
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
                        // Resetta se entrambi i proiettili sono terminati
                        if (bullet_pid_right == -1 && bullet_pid_left == -1) {
                            bullet_frog = 0;
                        }
                }
                break;

                case BULLETS_ID:
                    if (msg.x < frog.x) {
                        bullet_left = msg;
                    } else {
                        bullet_right = msg;
                    }
                    break;
                case TIMER_ID:
                    timer.x -= 1;
                    if(timer.x == 0){
                        manche--;
                        frog.y = GAME_HEIGHT-4;
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        timer.x = 60;   
                    }
                    break;
                case CROCODILE_MIN_BULLETS_ID ... CROCODILE_MAX_BULLETS_ID:
                    for(int i = 0; i < STREAM_NUMBER; i++){
                        for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                            if(crocodiles_bullets[i][j].id == msg.id){
                                crocodiles_bullets[i][j] = msg;
                            }
                        }
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
                                    if(frog.y >= SIDEWALK_HEIGHT_1 && frog.y < SIDEWALK_HEIGHT_2){
                                        if(frog.y == crocodiles[stream][j].y && frog.x >= crocodiles[stream][j].x && frog.x <= crocodiles[stream][j].x + CROCODILE_DIM_X - FROG_DIM_X){
                                                if (stream % 2 != 0)
                                                {   
                                                    if(frog.x == GAME_WIDTH-FROG_DIM_X || frog.x == 0) continue;                                                   
                                                    else frog.x += odd;
                                                }
                                                else
                                                {   
                                                    if(frog.x == GAME_WIDTH-FROG_DIM_X || frog.x == 0) continue;
                                                    else frog.x += even;
                                                }
                                                
                                                print_frog(game, &frog);
                                                wrefresh(game);
                                        }
                                        
                                    }
                            }
                        }
                    }
                    break;
                }
            }
            int frog_on_crocodile = FALSE;
            int stream = ((SIDEWALK_HEIGHT_2 + 1 - frog.y) / FROG_DIM_Y) -1;
            if(frog.y >= SIDEWALK_HEIGHT_1 && frog.y < SIDEWALK_HEIGHT_2){
                for (size_t j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++)
                {
                    if (stream >= 0 && stream < STREAM_NUMBER)
                    {
                        if( (frog.y == crocodiles[stream][j].y && frog.x >= crocodiles[stream][j].x && frog.x <= crocodiles[stream][j].x + CROCODILE_DIM_X - FROG_DIM_X)){
                            frog_on_crocodile = TRUE;
                        }
                    }

                }
                if (frog_on_crocodile == FALSE)
                {
                    // manche--;
                    // frog.y = GAME_HEIGHT-4;
                    // frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                    // timer.x = 60;
                    // erase();
                    // print_death();
                    // refresh();
                    // sleep(5);
                }
            }

            if(frog.y < DENS_HEIGHT){
                switch(frog.x){
                    case DENS_1:
                        switch(timer.x)
                        {
                            case 41 ... 60:
                                score += 30;
                                break;
                            case 21 ... 40:
                                score += 20;
                                break;
                            case 1 ... 20:
                                score += 10;
                                break;
                        }
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        frog.y = GAME_HEIGHT-4;
                        dens[0] = FALSE;
                        timer.x = 60;
                        break;
                    case DENS_2:
                        switch(timer.x)
                        {
                            case 41 ... 60:
                                score += 30;
                                break;
                            case 21 ... 40:
                                score += 20;
                                break;
                            case 1 ... 20:
                                score += 10;
                                break;
                        }
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        frog.y = GAME_HEIGHT-4;
                        dens[1] = FALSE;
                        timer.x = 60;
                        break;
                    case DENS_3:
                        switch(timer.x)
                        {
                            case 41 ... 60:
                                score += 30;
                                break;
                            case 21 ... 40:
                                score += 20;
                                break;
                            case 1 ... 20:
                                score += 10;
                                break;
                        }
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        frog.y = GAME_HEIGHT-4;
                        dens[2] = FALSE;
                        timer.x = 60;
                        break;
                    case DENS_4:
                        score += 20;
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        frog.y = GAME_HEIGHT-4;
                        dens[3] = FALSE;
                        timer.x = 60;
                        break;
                    case DENS_5:
                        switch(timer.x)
                        {
                            case 41 ... 60:
                                score += 30;
                                break;
                            case 21 ... 40:
                                score += 20;
                                break;
                            case 1 ... 20:
                                score += 10;
                                break;
                        }
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        frog.y = GAME_HEIGHT-4;
                        dens[4] = FALSE;
                        timer.x = 60;
                        break;
                    default:
                        manche--;
                        frog.y = GAME_HEIGHT-4;
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        timer.x = 60;
                }
            }

            //manche end
            if(manche == 0){
                endgame = TRUE;
            }

            endgame = true;  // Assume che tutti i valori siano FALSE
            for(int i = 0; i < 5; i++){
                if (dens[i] != FALSE) {
                    endgame = false;  // Se almeno uno è diverso da FALSE, endgame deve essere false
                    break;
                }
            }

            print_score(game, manche, timer.x, score);
            print_background(game, dens);

            for(int i = 0; i < STREAM_NUMBER; i++){
                for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                    if ((direction+i) % 2 == 0)
                    {
                        print_crocodile_right(game, &crocodiles[i][j]);
                    }
                    else
                    {
                        print_crocodile_left(game, &crocodiles[i][j]);
                    }
                    
                }
            }
            // for(int i=0; i<STREAM_NUMBER; i++){
            //     for(int j=0; j<CROCODILE_STREAM_MAX_NUMBER; j++){
            //         print_bullets(game, &crocodiles_bullets[i][j]);
            //     }
            // }
            print_frog(game, &frog);
            print_bullets(game, &bullet_left);
            print_bullets(game, &bullet_right);
            
            wrefresh(game);
        }
    }

    print_endgame(game, manche, dens);

    for (int i = 0; i < (STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1; i++) {
        kill(child_pids[i], SIGTERM);  // Invia il segnale di terminazione
    }

    for (int i = 0; i < (STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1; i++) {
        waitpid(child_pids[i], NULL, 0);
    }
    endwin();
    return 0;
}
