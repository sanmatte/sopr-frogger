#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <locale.h>
#include "game.h"

bool sigintdetected = FALSE;
int manche = 3, score = 0;
bool endgame = FALSE, dens[DENS_NUMBER] = {TRUE, TRUE, TRUE, TRUE, TRUE};

void game_timer(int *pipe_fds){
    Item msg = {TIMER_ID, 0, 0, 0, 0};
    while(1){
        write(pipe_fds[1], &msg, sizeof(Item));
        usleep(TIMER_SPEED);
    }
}

int compute_score(int timer){
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

void kill_all(pid_t frog, pid_t pidgroup){
    killpg(pidgroup, SIGKILL);
    pid_t pid;
    while ((pid = waitpid(-pidgroup, NULL, 0)) > 0) {
    }
    kill(frog, SIGKILL);
    waitpid(frog, NULL, 0);
}

void ctrlc_handler(int signum){
    if (signum == SIGINT)
    {
        sigintdetected = TRUE;
    }
}

void startGame(WINDOW *game) {
    setlocale(LC_ALL, "");
    
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
        case 2:
            exit(0);
            break;
        case 3:
            return;
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


int play(WINDOW *game) {
    /* Ritorna 0 se la manche è persa (manche --), ritorna 1 se è stata presa una tana */
    Item *frog = malloc(sizeof(Item));
    frog->id = FROG_ID;
    frog->y = GAME_HEIGHT - 4;
    frog->x = (GAME_WIDTH / 2) - FROG_DIM_X / 2;
    frog->speed = 0;
    frog->extra = 0;

    Item **crocodiles = malloc(STREAM_NUMBER * sizeof(Item *));
    for (int i = 0; i < STREAM_NUMBER; i++) {
        crocodiles[i] = malloc(CROCODILE_STREAM_MAX_NUMBER * sizeof(Item));
    }
    

    Item *bullet_right = malloc(sizeof(Item));
    bullet_right->id = BULLETS_ID;
    bullet_right->x = -1;
    bullet_right->y = -1;
    bullet_right->speed = 0;
    bullet_right->extra = 1;

    Item *bullet_left = malloc(sizeof(Item));
    bullet_left->id = BULLETS_ID;
    bullet_left->x = -1;
    bullet_left->y = -1;
    bullet_left->speed = 0;
    bullet_left->extra = -1;

    Item *timer = malloc(sizeof(Item));
    timer->id = TIMER_ID;
    timer->x = TIMER_MAX;
    timer->y = 0;
    timer->speed = 0;
    timer->extra = 0;

    int bullet_count = CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1;
    Item *crocodiles_bullets = malloc(bullet_count * sizeof(Item));
    for (int i = 0; i < CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1; i++) {
        crocodiles_bullets[i].id = CROCODILE_MIN_BULLETS_ID + i;
        crocodiles_bullets[i].x = -2;
        crocodiles_bullets[i].y = -2;
        crocodiles_bullets[i].speed = CROCODILE_BULLET_SPEED;
        crocodiles_bullets[i].extra = 0;
    }
    
    // Definizione variabili
    int pipe_fds[2], stream_speed[STREAM_NUMBER], is_bullet_frog_active = 0;

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
        frog_controller(pipe_fds);
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
                setpgid(0, group_pid);
                continue_usleep(distance);
                crocodile(pipe_fds, &crocodiles[i][j], group_pid);
                _exit(0);
            } else {
                setpgid(pid_croc, group_pid); 
                child_pids[i * CROCODILE_STREAM_MAX_NUMBER + j + 1] = pid_croc;
                // calculate mindistance between crocodiles
                distance += crocodiles[i][j].speed * CROCODILE_DIM_X;
                // add random distance between crocodiles
                distance += rand_range(crocodiles[i][j].speed * (CROCODILE_DIM_X / 2), crocodiles[i][j].speed * (CROCODILE_DIM_X));
            }
        }
    }

    Item *msg = malloc(sizeof(Item));
    pid_t bullet_pid_left, bullet_pid_right;

    signal(SIGINT, ctrlc_handler);
    while (TRUE) {
        if (read(pipe_fds[0], msg, sizeof(Item)) > 0) {
            switch (msg->id) {
                // FROG case
                case FROG_ID:
                    if (frog->x + msg->x >= 0 && frog->x + msg->x <= GAME_WIDTH - FROG_DIM_X && frog->y + msg->y >= 0 && frog->y + msg->y <= GAME_HEIGHT - FROG_DIM_Y) {
                        frog->y += msg->y;
                        frog->x += msg->x;
                        frog->extra = msg->extra;

                        if (frog->extra == 1 && is_bullet_frog_active == 0) {
                            is_bullet_frog_active = 1;

                            // Creazione del processo per il proiettile destro
                            bullet_pid_right = fork();
                            if (bullet_pid_right == 0) {
                                bullet_right->x = frog->x + FROG_DIM_X;
                                bullet_right->y = frog->y + 1;
                                bullet_right_controller(bullet_right, pipe_fds);
                            }

                            // Creazione del processo per il proiettile sinistro
                            bullet_pid_left = fork();
                            if (bullet_pid_left == 0) {
                                bullet_left->x = frog->x - 1;
                                bullet_left->y = frog->y + 1;
                                bullet_left_controller(bullet_left, pipe_fds);
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
                    if (msg->extra == 1) {
                        bullet_right->x = msg->x;
                        bullet_right->y = msg->y;
                    } else {
                        bullet_left->x = msg->x;
                        bullet_left->y = msg->y;
                    }
                    break;
                case TIMER_ID:
                    timer->x -= 1;
                    if(timer->x == 0){
                        kill_all(pid_frog, group_pid);
                        return 0;
                    }
                    break;
                case CROCODILE_MIN_BULLETS_ID ... CROCODILE_MAX_BULLETS_ID:
                    crocodiles_bullets[msg->id - CROCODILE_MIN_BULLETS_ID] = *msg;
                    // Collision beetwen frog and crocodile bullet
                    if(frog->y == (crocodiles_bullets[msg->id - CROCODILE_MIN_BULLETS_ID].y - 1) && crocodiles_bullets[msg->id - CROCODILE_MIN_BULLETS_ID].x >= frog->x && crocodiles_bullets[msg->id - CROCODILE_MIN_BULLETS_ID].x <= frog->x + FROG_DIM_X){
                        kill_all(pid_frog, group_pid);
                        return 0;
                    }
                    break;
                //CROCODILE case
                case PAUSE_ID:
                    WINDOW *pause = newwin(5, 25, (GAME_HEIGHT/2) + 5,  (GAME_WIDTH/2) + 10);
                    box(pause, 0, 0);
                    mvwprintw(pause, 1, 1 , "Press Q to quit");
	                mvwprintw(pause, 2, 1 , "Press P to resume");
                    mvwprintw(pause, 3, 1 , "Press M to go to menu");
                    wrefresh(pause);
                    killpg(group_pid, SIGSTOP);  // Pause all child processes
                    int ch = getchar();  // Wait for user input
                    while (ch != 'p' && ch != 'q' && ch != 'm') {
                        ch = getchar();
                    }
                    if(ch == 'p'){
                        killpg(group_pid, SIGCONT);  // Resume all child processes
                    }
                    else if(ch == 'q'){
                        kill_all(pid_frog, group_pid);
                        endwin();
                        return 2;
                    }
                    else if(ch == 'm'){
                        kill_all(pid_frog, group_pid);
                        return 3;
                    }

                    break;
                case QUIT_ID:
                    kill_all(pid_frog, group_pid);
                    endwin();
                    return 2;
                    break;  
                default:
                    if(msg->id >= CROCODILE_MIN_ID && msg->id <= CROCODILE_MAX_ID){
                        for(int i = 0; i < STREAM_NUMBER; i++){
                            for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                                if(crocodiles[i][j].id == msg->id){
                                    crocodiles[i][j] = *msg;
                                    int stream = i;
                                    if(frog->y >= SIDEWALK_HEIGHT_1 && frog->y < SIDEWALK_HEIGHT_2){
                                        if(frog->y == crocodiles[stream][j].y && frog->x >= crocodiles[stream][j].x && frog->x <= crocodiles[stream][j].x + CROCODILE_DIM_X - FROG_DIM_X){
                                            if (crocodiles[stream][j].extra == 1)
                                            {
                                                if(frog->x == 0) continue;                                                   
                                                else frog->x -= crocodiles[stream][j].extra;
                                            }
                                            else
                                            {
                                                if(frog->x == GAME_WIDTH - FROG_DIM_X) continue;                                                   
                                                else frog->x -= crocodiles[stream][j].extra;
                                            }
                                            print_frog(game, frog);
                                            wrefresh(game);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
            }
            int frog_on_crocodile = FALSE;
            int stream = ((SIDEWALK_HEIGHT_2 + 1 - frog->y) / FROG_DIM_Y) -1;
            if(frog->y >= SIDEWALK_HEIGHT_1 && frog->y < SIDEWALK_HEIGHT_2){
                for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++)
                {
                    if (stream >= 0 && stream < STREAM_NUMBER)
                    {
                        if( (frog->y == crocodiles[stream][j].y && frog->x >= crocodiles[stream][j].x && frog->x <= crocodiles[stream][j].x + CROCODILE_DIM_X - FROG_DIM_X)){
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
            stream = ((SIDEWALK_HEIGHT_2 + 1 - bullet_right->y) / FROG_DIM_Y);
            for(int i=stream*CROCODILE_STREAM_MAX_NUMBER; i<stream*CROCODILE_STREAM_MAX_NUMBER+3; i++){
                if((crocodiles_bullets[i].x - bullet_right->x) <= 1 && (crocodiles_bullets[i].x - bullet_right->x) >= -1 && crocodiles_bullets[i].y == bullet_right->y){
                    kill(bullet_pid_right, SIGKILL);
                    kill(child_pids[i+1], SIGUSR1);
                    bullet_right->x = -8;
                    crocodiles_bullets[i].x = -2;
                }
                if((crocodiles_bullets[i].x - bullet_left->x) <= 1 && (crocodiles_bullets[i].x - bullet_left->x) >= -1 && crocodiles_bullets[i].y == bullet_left->y){
                    kill(bullet_pid_left, SIGKILL);
                    kill(child_pids[i+1], SIGUSR1);
                    bullet_left->x = -8;
                    crocodiles_bullets[i].x = -2;
                }
            }

            if(frog->y < DENS_HEIGHT){
                switch(frog->x){
                    case DENS_1:
                        if(dens[0] == TRUE){
                            score = compute_score(timer->x);
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
                            score = compute_score(timer->x);
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
                            score = compute_score(timer->x);
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
                            score = compute_score(timer->x);
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
                            score = compute_score(timer->x);
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

            print_score(game, manche, timer->x, score);
            print_background(game, dens);

            for(int i = 0; i < STREAM_NUMBER; i++){
                for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                    print_crocodile(game, &crocodiles[i][j], crocodiles_bullets[i * CROCODILE_STREAM_MAX_NUMBER + j].extra);
                }
            }

            for (int i = 0; i < CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1; i++) {
                print_bullets(game, &crocodiles_bullets[i]);
            }
            print_frog(game, frog);
            print_bullets(game, bullet_left);
            print_bullets(game, bullet_right);
            
            wrefresh(game);

            if (sigintdetected == TRUE) {
                kill_all(pid_frog, group_pid);  // Kill all relevant processes
                endwin(); // End ncurses mode
                system("clear"); // Clear the screen
                exit(0); // Exit cleanly
            }
        }
    }
    free(frog);
    free(crocodiles);
    free(bullet_right);
    free(bullet_left);
    free(timer);
    free(crocodiles_bullets);
}