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
        usleep(TIMER_SPEED);
    }
}

int startGame(WINDOW *game) {
    setlocale(LC_ALL, "");
    int bullet_frog = 0;

    Item frog = {FROG_ID, GAME_HEIGHT-4, (GAME_WIDTH/2)-FROG_DIM_X/2, 0, 0};
    Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER];
    // for (int i = 0; i < STREAM_NUMBER; i++) {
    //     for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
    //         crocodiles[i][j].extra = 0;
    //     }
    // }
    Item bullet_right = {BULLETS_ID, -1, -1, 0, 0};
    Item bullet_left = {BULLETS_ID, -1, -1, 0, 0};
    Item timer = {TIMER_ID, 0, 60, 0, 0};
    
    Item crocodiles_bullets[CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1];
    for (int i = 0; i < CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1; i++) {
        crocodiles_bullets[i].id = CROCODILE_MIN_BULLETS_ID + i;
        crocodiles_bullets[i].x = -2;
        crocodiles_bullets[i].y = -2;
        crocodiles_bullets[i].speed = CROCODILE_BULLET_SPEED;
        //crocodiles_bullets[i].extra.s = 1;
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
    init_pair(16, COLOR_FROG_BODY, COLOR_BLACK);
    init_pair(17, COLOR_BLACK, COLOR_BLACK);

    
    // Definizione variabili
    int pipe_fds[2], direction = (rand() % 2)? 1: -1;
    int stream_speed[STREAM_NUMBER];

    for(int i=0; i<STREAM_NUMBER; i++){
        stream_speed[i] = rand_range(current_difficulty.crocodile_speed_min, current_difficulty.crocodile_speed_max);
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
        _exit(0);
    }

    //Creazione del processo Frog
    pid_t pid_frog = fork();
    if (pid_frog < 0) {
        perror("Errore nella fork");
        endwin();
        exit(EXIT_FAILURE);
    } else if (pid_frog == 0) {
        Frog(pipe_fds, &frog);
        _exit(0);
    } else {
        child_pids[0] = pid_frog;
    }

    // Creazione dei processi Crocodile
    for (int i = 0; i < STREAM_NUMBER; i++) {
        int distance = rand_range(0, stream_speed[i] * CROCODILE_DIM_X);
        for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
            pid_t pid_croc = fork();
            if (pid_croc < 0) {
                perror("Errore nella fork del coccodrillo");
                endwin();
                exit(EXIT_FAILURE);
            } else if (pid_croc == 0) { 
                srand(timestamp() + crocodiles[i][j].id + getpid());
                debuglog("speed: %d\n", crocodiles[i][j].speed);
                debuglog("speed by stream: %d\n", stream_speed[i]);
                usleep(distance); 
                Crocodile(pipe_fds, &crocodiles[i][j], direction);
                _exit(0);
            } else {
                child_pids[i * CROCODILE_STREAM_MAX_NUMBER + j + 1] = pid_croc;
                // calculate mindistance between crocodiles
                distance += crocodiles[i][j].speed * CROCODILE_DIM_X;
                // add random distance between crocodiles
                distance += rand_range(crocodiles[i][j].speed * (CROCODILE_DIM_X / 2), crocodiles[i][j].speed * (CROCODILE_DIM_X));
            }
        }
        
        direction = -direction;
    }

    int even, odd;
    if (direction == -1) {
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
                if (frog.x + msg.x >= 0 && frog.x + msg.x <= GAME_WIDTH - FROG_DIM_X && frog.y + msg.y >= 0 && frog.y + msg.y <= GAME_HEIGHT - FROG_DIM_Y) {
                    frog.y += msg.y;
                    frog.x += msg.x;
                    frog.extra = msg.extra;

                    if (frog.extra == 1 && bullet_frog == 0) {
                        bullet_frog = 1;

                        // Creazione del processo per il proiettile destro
                        bullet_pid_right = fork();
                        if (bullet_pid_right == 0) {
                            bullet_right.x = frog.x + FROG_DIM_X;
                            bullet_right.y = frog.y + 1;
                            while (bullet_right.x < GAME_WIDTH + 1) {
                                bullet_right.x += 1;
                                write(pipe_fds[1], &bullet_right, sizeof(Item));
                                usleep(current_difficulty.bullets_speed);
                            }
                            _exit(0);
                        }

                        // Creazione del processo per il proiettile sinistro
                        bullet_pid_left = fork();
                        if (bullet_pid_left == 0) {
                            bullet_left.x = frog.x - 1;
                            bullet_left.y = frog.y + 1;
                            while (bullet_left.x > -1) {
                                bullet_left.x -= 1;
                                write(pipe_fds[1], &bullet_left, sizeof(Item));
                                usleep(current_difficulty.bullets_speed);
                            }
                            _exit(0);
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
                        timer.x = TIMER_MAX;   
                    }
                    break;
                case CROCODILE_MIN_BULLETS_ID ... CROCODILE_MAX_BULLETS_ID:
                    crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID] = msg;

                                        // Collision beetwen frog and crocodile bullet
                    if(frog.y == (crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].y - 1) && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x >= frog.x && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x <= frog.x + FROG_DIM_X){
                        manche--;
                        frog.y = GAME_HEIGHT-4;
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        timer.x = TIMER_MAX;
                    }
                    if ((bullet_right.x == msg.x && bullet_right.y == msg.y) || (bullet_left.x == msg.x && bullet_left.y == msg.y)) {
                        // Resetta i proiettili in caso di collisione
                        bullet_right.x = -1;
                        bullet_right.y = -1;
                        bullet_left.x = -1;
                        bullet_left.y = -1;
                        // Termina i processi dei proiettili
                        if (bullet_pid_right > 0) {
                            kill(bullet_pid_right, SIGKILL);
                            bullet_pid_right = -1;
                        }
                        if (bullet_pid_left > 0) {
                            kill(bullet_pid_left, SIGKILL);
                            bullet_pid_left = -1;
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
                for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++)
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
                    manche--;
                    frog.y = GAME_HEIGHT-4;
                    frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                    timer.x = TIMER_MAX;
                    // erase();
                    // print_death();
                    // refresh();
                    // sleep(5);
                }
            }
            // Collision between frog bullets and crocodile bullet
            
            for(int i=0; i<=CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID; i++){
                if(crocodiles_bullets[i].x == bullet_right.x && crocodiles_bullets[i].y == bullet_right.y){
                    //kill(bullet_pid_right, SIGKILL);
                    //kill(child_pids[i+1], SIGUSR1);
                }
                if(crocodiles_bullets[i].x == bullet_left.x && crocodiles_bullets[i].y == bullet_left.y){
                    //kill(bullet_pid_left, SIGKILL);
                    //kill(child_pids[i+1], SIGUSR1);
                    debuglog("collision with bullet %d\n", crocodiles_bullets[i].id);
                    debuglog("crocodile %d\n\n", crocodiles[i/CROCODILE_STREAM_MAX_NUMBER][i%CROCODILE_STREAM_MAX_NUMBER].id);
                    break;
                }
            }

            if(frog.y < DENS_HEIGHT){
                switch(frog.x){
                    case DENS_1:
                        switch(timer.x)
                        {
                            case 41 ... TIMER_MAX:
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
                        timer.x = TIMER_MAX;
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
                        timer.x = TIMER_MAX;
                        break;
                    case DENS_3:
                        switch(timer.x)
                        {
                            case 41 ... TIMER_MAX:
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
                        timer.x = TIMER_MAX;
                        break;
                    case DENS_4:
                        score += 20;
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        frog.y = GAME_HEIGHT-4;
                        dens[3] = FALSE;
                        timer.x = TIMER_MAX;
                        break;
                    case DENS_5:
                        switch(timer.x)
                        {
                            case 41 ... TIMER_MAX:
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
                        timer.x = TIMER_MAX;
                        break;
                    default:
                        manche--;
                        frog.y = GAME_HEIGHT-4;
                        frog.x = rand_range(0, GAME_WIDTH - FROG_DIM_X);
                        timer.x = TIMER_MAX;
                }
            }

            endgame = true;  // Assume che tutti i valori siano FALSE
            for(int i = 0; i < 5; i++){
                if (dens[i] != FALSE) {
                    endgame = false;  // Se almeno uno è diverso da FALSE, endgame deve essere false
                    break;
                }
            }

            //manche end
            if(manche == 0){
                endgame = true;
            }

            print_score(game, manche, timer.x, score);
            print_background(game, dens);

            for(int i = 0; i < STREAM_NUMBER; i++){
                for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                    print_crocodile(game, &crocodiles[i][j]);
                }
            }

            for (int i = 0; i < CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1; i++) {
                print_bullets(game, &crocodiles_bullets[i]);
            }
            print_frog(game, &frog);
            print_bullets(game, &bullet_left);
            print_bullets(game, &bullet_right);
            
            wrefresh(game);
        }
    }

    print_endgame(game, manche, dens, score);

    for (int i = 0; i < (STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1; i++) {
        kill(child_pids[i], SIGTERM);  // Invia il segnale di terminazione
    }

    for (int i = 0; i < (STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1; i++) {
        waitpid(child_pids[i], NULL, 0);
    }
    return 0;
}
