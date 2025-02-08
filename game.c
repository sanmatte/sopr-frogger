#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
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

int compute_score(int timer, int score){
    switch(timer){
        case 31 ... TIMER_MAX:
            score += 30;
            break;
        case 11 ... 30:
            score += 20;
            break;
        case 1 ... 10:
            score += 10;
            break;
    }
    return score;
}

void startGame(WINDOW *game) {
    setlocale(LC_ALL, "");
    init_color(COLOR_DARKGREEN, 0, 400, 0);
    init_color(COLOR_GREY, 600, 600, 600);
    init_color(COLOR_LIGHTDARKGREEN, 28, 163, 32);
    init_color(COLOR_SAND, 745, 588, 313);
    init_color(COLOR_FROG_EYE, 90, 113, 749);
    init_color(COLOR_FROG_BODY, 62, 568, 184);
    init_color(COLOR_FROG_BODY_DETAILS, 8, 639, 176);
    init_color(COLOR_DARK_ORANGE, 815, 615, 98);
    init_color(COLOR_ENDGAME_BACKGROUND, 8, 372, 600);
    init_color(COLOR_BULLET_TRIGGER_DARK, 149, 8, 8);
    init_color(COLOR_BULLET_TRIGGER, 478, 4, 4);


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
    init_pair(18, COLOR_BULLET_TRIGGER, COLOR_BLUE);
    init_pair(19, COLOR_BULLET_TRIGGER_DARK, COLOR_BULLET_TRIGGER);
    init_pair(20, COLOR_FROG_BODY_DETAILS, COLOR_FROG_BODY);


    werase(game);  // Clear the window
    while (endgame == FALSE)
    {
        switch (play(game))
        {
        case 0: // manche lost
            manche--;
            if (manche == 0) {
                endgame = true;
            }
            break;
        case 1:
            endgame = true; 
            for(int i = 0; i < 5; i++){
                if (dens[i] != FALSE) {
                    endgame = false;
                    break;
                }
            }
            break;
        }
        werase(game);
        refresh();
    }

    print_endgame(game, manche, dens, score);
    manche = 3;
    score = 0;
    endgame = FALSE;
    for(int i = 0; i < DENS_NUMBER; i++){
        dens[i] = TRUE;
    }
       

}

void continue_usleep(long microseconds) {
    long elapsed = 0;
    while (elapsed < microseconds) {
        usleep(1000);
        elapsed += 1000;
    }
}





int play(WINDOW *game) {
    /* Ritorna 0 se la manche è persa (manche --), ritorna 1 se è stata presa una tana */
    int is_bullet_frog_active = 0;

    Item frog = {FROG_ID, GAME_HEIGHT-4, (GAME_WIDTH/2)-FROG_DIM_X/2, 0, 0};
    Item crocodiles[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER];
    Item bullet_right = {BULLETS_ID, -1, -1, 0, 0};
    Item bullet_left = {BULLETS_ID, -1, -1, 0, 0};
    Item timer = {TIMER_ID, 0, TIMER_MAX, 0, 0};
    
    Item crocodiles_bullets[CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1];
    for (int i = 0; i < CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1; i++) {
        crocodiles_bullets[i].id = CROCODILE_MIN_BULLETS_ID + i;
        crocodiles_bullets[i].x = -2;
        crocodiles_bullets[i].y = -2;
        crocodiles_bullets[i].speed = CROCODILE_BULLET_SPEED;
        crocodiles_bullets[i].extra = 0;
    }

    

    
    // Definizione variabili
    int pipe_fds[2];
    int stream_speed[STREAM_NUMBER];

    for(int i=0; i<STREAM_NUMBER; i++){
        stream_speed[i] = rand_range(current_difficulty.crocodile_speed_min, current_difficulty.crocodile_speed_max);
    }

    if(pipe(pipe_fds) != 0){
        exit(0);
    }
    
    initializeCrocodile(crocodiles, stream_speed);
    int group_pid = 0;
    pid_t child_pids[(STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1];
    pid_t pid_timer = fork();
    group_pid = pid_timer;
    if (pid_timer < 0) {
        perror("Errore nella fork");
        endwin();
        exit(EXIT_FAILURE);
    } else if (pid_timer == 0) {
        game_timer(pipe_fds);
        _exit(0);
    }
    setpgid(pid_timer, group_pid);
    //Creazione del processo Frog
    pid_t pid_frog = fork();
    if (pid_frog < 0) {
        perror("Errore nella fork");
        endwin();
        exit(EXIT_FAILURE);
    } else if (pid_frog == 0) {
        frog_fun(pipe_fds, &frog);
        _exit(0);
    } else {
        //setpgid(pid_frog, group_pid);
        child_pids[0] = pid_frog;
    }
    setpgid(0,0);
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
                // if (i == 0 && j == 0) {
                //     group_pid = getpid();  // First child sets the group leader
                // }
                setpgid(0, group_pid);
                continue_usleep(distance);
                //usleep(distance); 
                crocodile(pipe_fds, &crocodiles[i][j], group_pid);
                _exit(0);
            } else {
            // if (i == 0 && j == 0) {
            //     group_pid = pid_croc;  // Assign first child's PID as group leader
            // }
            setpgid(pid_croc, group_pid); 
                child_pids[i * CROCODILE_STREAM_MAX_NUMBER + j + 1] = pid_croc;
                // calculate mindistance between crocodiles
                distance += crocodiles[i][j].speed * CROCODILE_DIM_X;
                // add random distance between crocodiles
                distance += rand_range(crocodiles[i][j].speed * (CROCODILE_DIM_X / 2), crocodiles[i][j].speed * (CROCODILE_DIM_X));
            }
        }
    }

    Item msg;
    pid_t bullet_pid_left, bullet_pid_right;

    while (TRUE) {
    if (read(pipe_fds[0], &msg, sizeof(Item)) > 0) {
        switch (msg.id) {
            // FROG case
            case FROG_ID:
                if (frog.x + msg.x >= 0 && frog.x + msg.x <= GAME_WIDTH - FROG_DIM_X && frog.y + msg.y >= 0 && frog.y + msg.y <= GAME_HEIGHT - FROG_DIM_Y) {
                    frog.y += msg.y;
                    frog.x += msg.x;
                    frog.extra = msg.extra;

                    if (frog.extra == 1 && is_bullet_frog_active == 0) {
                        is_bullet_frog_active = 1;

                        // Creazione del processo per il proiettile destro
                        bullet_pid_right = fork();
                        if (bullet_pid_right == 0) {
                            bullet_right.x = frog.x + FROG_DIM_X;
                            bullet_right.y = frog.y + 1;
                            bullet_right.extra = frog.y + 1;
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
                            bullet_left.extra = frog.y + 1;
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
                            is_bullet_frog_active = 0;
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
                        kill_all(pid_frog, group_pid);
                        return 0;
                    }
                    break;
                case CROCODILE_MIN_BULLETS_ID ... CROCODILE_MAX_BULLETS_ID:
                    crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID] = msg;

                                        // Collision beetwen frog and crocodile bullet
                    if(frog.y == (crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].y - 1) && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x >= frog.x && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x <= frog.x + FROG_DIM_X){
                        kill_all(pid_frog, group_pid);
                        return 0;
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
                case PAUSE_ID:
                    killpg(group_pid, SIGSTOP);  // Pause all child processes
                    //printf("Game Paused. Press any key to continue...\n");
                    int ch = getchar();  // Wait for user input
                    while (ch != 'p') {
                        ch = getchar();
                    }
                    
                    killpg(group_pid, SIGCONT);  // Resume all child processes
                    break;
                default:
                    if(msg.id >= CROCODILE_MIN_ID && msg.id <= CROCODILE_MAX_ID){
                        for(int i = 0; i < STREAM_NUMBER; i++){
                            for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                                if(crocodiles[i][j].id == msg.id){
                                    crocodiles[i][j] = msg;
                                    int stream = i;
                                    if(frog.y >= SIDEWALK_HEIGHT_1 && frog.y < SIDEWALK_HEIGHT_2){
                                        if(frog.y == crocodiles[stream][j].y && frog.x >= crocodiles[stream][j].x && frog.x <= crocodiles[stream][j].x + CROCODILE_DIM_X - FROG_DIM_X){
                                            if (crocodiles[stream][j].extra == 1)
                                            {
                                                if(frog.x == 0) continue;                                                   
                                                else frog.x -= crocodiles[stream][j].extra;
                                            }
                                            else
                                            {
                                                if(frog.x == GAME_WIDTH - FROG_DIM_X) continue;                                                   
                                                else frog.x -= crocodiles[stream][j].extra;
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
                    kill_all(pid_frog, group_pid);
                    return 0;
                }
            }
            // Collision between frog bullets and crocodile bullet
            stream = ((SIDEWALK_HEIGHT_2 + 1 - bullet_right.extra) / FROG_DIM_Y);
            for(int i=stream*CROCODILE_STREAM_MAX_NUMBER; i<stream*CROCODILE_STREAM_MAX_NUMBER+3; i++){
                if((crocodiles_bullets[i].x - bullet_right.x) <= 1 && (crocodiles_bullets[i].x - bullet_right.x) >= -1 && crocodiles_bullets[i].y == bullet_right.y){
                    kill(bullet_pid_right, SIGKILL);
                    kill(child_pids[i+1], SIGUSR1);
                    bullet_right.x = -1;
                    bullet_right.y = -1;
                    crocodiles_bullets[i].x = -2;
                }
                if((crocodiles_bullets[i].x - bullet_left.x) <= 1 && (crocodiles_bullets[i].x - bullet_left.x) >= -1 && crocodiles_bullets[i].y == bullet_left.y){
                    kill(bullet_pid_left, SIGKILL);
                    kill(child_pids[i+1], SIGUSR1);
                    bullet_left.x = -1;
                    bullet_left.y = -1;
                    crocodiles_bullets[i].x = -2;
                }
            }

            if(frog.y < DENS_HEIGHT){
                switch(frog.x){
                    case DENS_1:
                        if(dens[0] == TRUE){
                            score = compute_score(timer.x, score);
                            dens[0] = FALSE;
                            kill_all(pid_frog, group_pid);
                            return 1;
                        }
                        else{
                            kill_all(pid_frog, group_pid);
                            return 0;
                        }
                        break;
                    case DENS_2:
                        if(dens[1] == TRUE){
                            score = compute_score(timer.x, score);
                            dens[1] = FALSE;
                            kill_all(pid_frog, group_pid);
                            return 1;
                        }
                        else{
                            kill_all(pid_frog, group_pid);
                            return 0;
                        }
                        break;
                    case DENS_3:
                        if(dens[2] == TRUE){
                            score = compute_score(timer.x, score);
                            dens[2] = FALSE;
                            kill_all(pid_frog, group_pid);
                            return 1;
                        }
                        else{
                            kill_all(pid_frog, group_pid);
                            return 0;
                        }
                        break;
                    case DENS_4:
                        if(dens[3] == TRUE){
                            score = compute_score(timer.x, score);
                            dens[3] = FALSE;
                            kill_all(pid_frog, group_pid);
                            return 1;
                        }
                        else{
                            kill_all(pid_frog, group_pid);
                            return 0;
                        }
                        break;
                    case DENS_5:
                        if(dens[4] == TRUE){
                            score = compute_score(timer.x, score);
                            dens[4] = FALSE;
                            kill_all(pid_frog, group_pid);
                            return 1;
                        }
                        else{
                            kill_all(pid_frog, group_pid);
                            return 0;
                        }
                        break;
                    default:
                        kill_all(pid_frog, group_pid);
                        return 0;
                }
            }

            //All prints

            print_score(game, manche, timer.x, score);
            print_background(game, dens);

            for(int i = 0; i < STREAM_NUMBER; i++){
                for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                    print_crocodile(game, &crocodiles[i][j], crocodiles_bullets[i * CROCODILE_STREAM_MAX_NUMBER + j].extra);
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
}

void kill_all(pid_t frog, pid_t pidgroup){
    killpg(pidgroup, SIGKILL);
    kill(frog, SIGKILL);
}