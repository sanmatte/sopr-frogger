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

// Define global variables
bool sigintdetected = FALSE;
int manche = 3, score = 0;
bool endgame = FALSE, dens[DENS_NUMBER] = {TRUE, TRUE, TRUE, TRUE, TRUE};

/**
 * @brief  function for the timer
 * @param  pipe_fds: pipe file descriptors
 */
void game_timer(int *pipe_fds){
    Item msg = {TIMER_ID, 0, 0, 0, 0};
    while(1){
        write(pipe_fds[1], &msg, sizeof(Item));
        usleep(TIMER_SPEED);
    }
}


/**
 * @brief  function for calculating the score
 * @param  timer time left
 * @return score value
 */
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


/**
 * @brief  function for killing all processes
 * @param  frog: frog process id
 * @param  pidgroup: process group id of remaining process
 */
void kill_all(pid_t frog, pid_t pidgroup){
    killpg(pidgroup, SIGKILL);
    pid_t pid;
    while ((pid = waitpid(-pidgroup, NULL, 0)) > 0) { // wait for all child processes which process group id is pidgroup
    }
    kill(frog, SIGKILL);
    waitpid(frog, NULL, 0);
}

/**
 * @brief  ctrl+c signal handler
 * @param  signum: signal number
 */
void ctrlc_handler(int signum){
    if (signum == SIGINT)
    {
        sigintdetected = TRUE;
    }
}


/**
 * @brief  function for the start of the game
 * @param  game game window
 */
void startGame(WINDOW *game) {
    setlocale(LC_ALL, "");
    manche = 3;
    werase(game);  // Clear the window
    while (endgame == FALSE)
    {
        switch (play(game))
        {
        case MANCHE_LOST:                 // manche lost
            manche--;
            if (manche == 0) {
                endgame = true;
            }
            break;
        case MANCHE_WON:                 // manche won
            endgame = true; 
            for(int i = 0; i < 5; i++){
                if (dens[i] != FALSE) {
                    endgame = false;
                    break;
                }
            }
            break;
        case GAME_QUIT:                 // quit the game
            exit(0);
            break;
        case BACK_TO_MENU:              // back to menu
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


/**
 * @brief  run management function
 * @param  game game window
 * @return int return value, 0 if you lose the manche or 1 if you enter a den
 */
int play(WINDOW *game) {
    // frog initialization
    Item *frog = malloc(sizeof(Item));
    frog->id = FROG_ID;
    frog->y = GAME_HEIGHT - 4;
    frog->x = (GAME_WIDTH / 2) - FROG_DIM_X / 2;
    frog->speed = 0;
    frog->extra = 0;

    // crocodile initialization
    Item **crocodiles = malloc(STREAM_NUMBER * sizeof(Item *));
    for (int i = 0; i < STREAM_NUMBER; i++) {
        crocodiles[i] = malloc(CROCODILE_STREAM_MAX_NUMBER * sizeof(Item));
    }
    // generate random speed for each stream
    int stream_speed[STREAM_NUMBER];
    for(int i=0; i<STREAM_NUMBER; i++){
        stream_speed[i] = rand_range(current_difficulty.crocodile_speed_min, current_difficulty.crocodile_speed_max);
    }
    initializeCrocodile(crocodiles, stream_speed);

    // right frog bullet initialization
    Item *bullet_right = malloc(sizeof(Item));
    bullet_right->id = BULLET_ID_RIGHT;
    bullet_right->x = -1;
    bullet_right->y = -1;
    bullet_right->speed = 0;
    bullet_right->extra = 1;

    // left frog bullet initialization
    Item *bullet_left = malloc(sizeof(Item));
    bullet_left->id = BULLET_ID_LEFT;
    bullet_left->x = -1;
    bullet_left->y = -1;
    bullet_left->speed = 0;
    bullet_left->extra = -1;

    // timer initialization
    Item *timer = malloc(sizeof(Item));
    timer->id = TIMER_ID;
    timer->x = TIMER_MAX;
    timer->y = 0;
    timer->speed = 0;
    timer->extra = 0;

    // crocodile bullets initialization
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
    int pipe_fds[2], is_bullet_frog_active = 0;

    if(pipe(pipe_fds) != 0){
        exit(0);
    }
    
    // variables for managing pids
    int group_pid = 0;
    pid_t child_pids[(STREAM_NUMBER * CROCODILE_STREAM_MAX_NUMBER) + 1];

    // create timer process
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

    // create frog process
    pid_t pid_frog = fork();
    if (pid_frog < 0) {
        perror("Errore nella fork");
        endwin();
        exit(EXIT_FAILURE);
    } else if (pid_frog == 0) {
        frog_controller(pipe_fds);
        _exit(0);
    } else {
        child_pids[0] = pid_frog;
    }
    setpgid(0,0);
    
    // create crocodile processes
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

    Item msg;
    pid_t bullet_pid_left, bullet_pid_right;     // frog bullets pid
    int manche_result = -1;
    signal(SIGINT, ctrlc_handler);
    while (TRUE) {
        if (read(pipe_fds[0], &msg, sizeof(Item)) > 0) {
            switch (msg.id) {
                // FROG case
                case FROG_ID:
                    // frog movement if it is inside the game window
                    if (frog->x + msg.x >= 0 && frog->x + msg.x <= GAME_WIDTH - FROG_DIM_X && frog->y + msg.y >= 0 && frog->y + msg.y <= GAME_HEIGHT - FROG_DIM_Y) {
                        frog->y += msg.y;
                        frog->x += msg.x;
                        frog->extra = msg.extra;
                        // frog bullet activation
                        if (frog->extra == 1 && is_bullet_frog_active == 0) {
                            is_bullet_frog_active = 1;
                            // create frog right bullet 
                            bullet_pid_right = fork();
                            if (bullet_pid_right == 0) {
                                bullet_right->x = frog->x + FROG_DIM_X;
                                bullet_right->y = frog->y + 1;
                                bullet_right_controller(bullet_right, pipe_fds);
                            }
                            // create frog left bullet 
                            bullet_pid_left = fork();
                            if (bullet_pid_left == 0) {
                                bullet_left->x = frog->x - 1;
                                bullet_left->y = frog->y + 1;
                                bullet_left_controller(bullet_left, pipe_fds);
                            }
                        }

                        // check if the bullets are finished
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

                        // reset the flag if both are finished
                        if (bullet_pid_right == -1 && bullet_pid_left == -1) {
                            is_bullet_frog_active = 0;
                        }
                    }
                    break;
                case BULLET_ID_LEFT:
                    if (bullet_pid_left != -1) {
                        *bullet_left = msg;
                    }
                    break;
                case BULLET_ID_RIGHT:
                    if (bullet_pid_right != -1) {
                        *bullet_right = msg;
                    }
                    break;
                case TIMER_ID:
                    timer->x -= 1;
                    // check if timer is expired
                    if(timer->x == 0){
                        manche_result = MANCHE_LOST;
                    }
                    break;
                case CROCODILE_MIN_BULLETS_ID ... CROCODILE_MAX_BULLETS_ID:     // crocodile bullets
                
                    if(crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x != RESET_CROCODILE_BULLET || msg.extra == 1){
                        crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID] = msg;
                    }

                    // Ccheck if the bullet hit the frog
                    if(frog->y == (crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].y - 1) && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x >= frog->x && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x <= frog->x + FROG_DIM_X){
                        manche_result = MANCHE_LOST;
                    }
                    break;
                case PAUSE_ID:
                    WINDOW *pause = newwin(7, 28, (LINES - 7)/2 + 3,  (COLS - 28)/2);
                    print_pause(pause, game);
                    killpg(group_pid, SIGSTOP);  // pause all child processes
                    int ch = getchar();          // wait for user input
                    while (ch != 'p' && ch != 'q' && ch != 'm') {
                        ch = getchar();
                    }
                    if(ch == 'p'){
                        killpg(group_pid, SIGCONT);  // Resume all child processes
                    }
                    else if(ch == 'q'){
                        endwin();
                        manche_result = GAME_QUIT;
                    }
                    else if(ch == 'm'){
                        manche_result = BACK_TO_MENU;
                    }
                    break;
                case QUIT_ID:
                    endwin();
                    manche_result = GAME_QUIT;
                    break;  
                default:
                    // check if the message is a crocodile
                    if(msg.id >= CROCODILE_MIN_ID && msg.id <= CROCODILE_MAX_ID){
                        for(int i = 0; i < STREAM_NUMBER; i++){
                            for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                                if(crocodiles[i][j].id == msg.id){
                                    crocodiles[i][j] = msg;
                                    int stream = i;
                                    if(frog->y >= SIDEWALK_HEIGHT_1 && frog->y < SIDEWALK_HEIGHT_2){
                                        // check if the frog is on a crocodile
                                        if(frog->y == crocodiles[stream][j].y && frog->x >= crocodiles[stream][j].x && frog->x <= crocodiles[stream][j].x + CROCODILE_DIM_X - FROG_DIM_X){
                                            if (crocodiles[stream][j].extra == -1)
                                            {
                                                if(frog->x == 0) continue;                           // check if the frog is at the edge of the screen                                         
                                                else frog->x += crocodiles[stream][j].extra;
                                            }
                                            else
                                            {
                                                if(frog->x == GAME_WIDTH - FROG_DIM_X) continue;     // check if the frog is at the edge of the screen                                             
                                                else frog->x += crocodiles[stream][j].extra;
                                            }
                                        }
                                    }
                                }

                                // crocodiles respawn when they reach the end of the screen
                                if(crocodiles[i][j].extra == 1 && crocodiles[i][j].x == GAME_WIDTH+1){
                                    kill(child_pids[i*CROCODILE_STREAM_MAX_NUMBER+j+1], SIGKILL);
                                    crocodiles[i][j].x = -CROCODILE_DIM_X;
                                    pid_t pid_croc = fork();
                                    if(pid_croc == 0){
                                        setpgid(0, group_pid);
                                        continue_usleep(rand_range(0, crocodiles[i][j].speed * (CROCODILE_DIM_X) / 3));
                                        crocodile(pipe_fds, &crocodiles[i][j], group_pid);
                                        _exit(0);
                                    }
                                    else{
                                        setpgid(pid_croc, group_pid);
                                        child_pids[i*CROCODILE_STREAM_MAX_NUMBER+j+1] = pid_croc;
                                    }
                                }
                                else if(crocodiles[i][j].extra == -1 && crocodiles[i][j].x == -CROCODILE_DIM_X-1){
                                    kill(child_pids[i*CROCODILE_STREAM_MAX_NUMBER+j+1], SIGKILL);
                                    crocodiles[i][j].x = GAME_WIDTH;
                                    pid_t pid_croc = fork();
                                    if(pid_croc == 0){
                                        setpgid(0, group_pid);
                                        continue_usleep(rand_range(0, crocodiles[i][j].speed * (CROCODILE_DIM_X) / 3));
                                        crocodile(pipe_fds, &crocodiles[i][j], group_pid);
                                        _exit(0);
                                    }
                                    else{
                                        setpgid(pid_croc, group_pid);
                                        child_pids[i*CROCODILE_STREAM_MAX_NUMBER+j+1] = pid_croc;
                                    }
                                }
                            }
                        }
                    }
                break;
            }

            // check if the manche is over
            if(manche_result != -1){
                break; //break the while loop
            }
            
            // collision between frog and crocodile
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
                // lost manche if the frog is not on a crocodile
                if (frog_on_crocodile == FALSE)
                {
                    manche_result = MANCHE_LOST;
                }
            }

            // Collision between frog bullets and crocodile bullet
            if(is_bullet_frog_active == 1){
                stream = ((SIDEWALK_HEIGHT_2 + 1 - bullet_right->y) / FROG_DIM_Y);
                if( stream >= 0 && stream < STREAM_NUMBER ) {
                    for(int i=stream*CROCODILE_STREAM_MAX_NUMBER; i<stream*CROCODILE_STREAM_MAX_NUMBER+3; i++){
                        if((crocodiles_bullets[i].x - bullet_right->x) <= 1 && (crocodiles_bullets[i].x - bullet_right->x) >= -1 && crocodiles_bullets[i].y == bullet_right->y){
                            if (bullet_pid_right != -1)
                            {

                                kill(bullet_pid_right, SIGKILL);
                                kill(child_pids[i+1], SIGUSR1);
                                bullet_pid_right = -1;

                                bullet_right->x = RESET_FROG_BULLET;
                            }
                            crocodiles_bullets[i].x = RESET_CROCODILE_BULLET;
                        }
                        if((crocodiles_bullets[i].x - bullet_left->x) <= 1 && (crocodiles_bullets[i].x - bullet_left->x) >= -1 && crocodiles_bullets[i].y == bullet_left->y){
                            if(bullet_pid_left != -1){

                                kill(bullet_pid_left, SIGKILL);
                                kill(child_pids[i+1], SIGUSR1);
                                bullet_pid_left = -1;

                                bullet_left->x = RESET_FROG_BULLET;
                            }
                            crocodiles_bullets[i].x = RESET_CROCODILE_BULLET;
                        }
                    }
                }
            }

            // check if the frog is in a den
            if(frog->y < DENS_HEIGHT){
                switch(frog->x){
                    case DENS_1:
                        if(dens[0] == TRUE){
                            score = compute_score(timer->x);
                            dens[0] = FALSE;
                            manche_result = MANCHE_WON;
                        }
                        else{
                            manche_result = MANCHE_LOST;
                        }
                        break;
                    case DENS_2:
                        if(dens[1] == TRUE){
                            score = compute_score(timer->x);
                            dens[1] = FALSE;
                            manche_result = MANCHE_WON;
                        }
                        else{
                            manche_result = MANCHE_LOST;
                        }
                        break;
                    case DENS_3:
                        if(dens[2] == TRUE){
                            score = compute_score(timer->x);
                            dens[2] = FALSE;
                            manche_result = MANCHE_WON;
                        }
                        else{
                            manche_result = MANCHE_LOST;
                        }
                        break;
                    case DENS_4:
                        if(dens[3] == TRUE){
                            score = compute_score(timer->x);
                            dens[3] = FALSE;
                            manche_result = MANCHE_WON;
                        }
                        else{
                            manche_result = MANCHE_LOST;
                        }
                        break;
                    case DENS_5:
                        if(dens[4] == TRUE){
                            score = compute_score(timer->x);
                            dens[4] = FALSE;
                            manche_result = MANCHE_WON;
                        }
                        else{
                            manche_result = MANCHE_LOST;
                        }
                        break;
                    default:      // lost manche if the frog is not in a den and hit the upper edge
                        manche_result = MANCHE_LOST;
                }
            }

            // check if the manche is over
            if (manche_result != -1)
            {
                break;
            }
            
            // curstom ctrl+c signal handler
            if (sigintdetected == TRUE) {
                endwin();
                manche_result = GAME_QUIT;
                break;
            }

            // print the game
            print_score(game, manche, timer->x, score);
            print_background(game, dens);
            for(int i = 0; i < STREAM_NUMBER; i++){
                for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                    print_crocodile(game, &crocodiles[i][j], crocodiles_bullets[i * CROCODILE_STREAM_MAX_NUMBER + j].extra);
                }
            }
            for (int i = 0; i < CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1; i++) {
                print_bullets(game, &crocodiles_bullets[i], -1);
            }
            print_frog(game, frog);
            print_bullets(game, bullet_left, BULLET_ID_LEFT);
            print_bullets(game, bullet_right, BULLET_ID_RIGHT);
            wrefresh(game);
        }
    }
    // kill all processes and free the memory
    kill_all(pid_frog, group_pid);
    free(frog);
    for (int i = 0; i < STREAM_NUMBER; i++) {
        free(crocodiles[i]);
    }
    free(crocodiles);
    free(bullet_right);
    free(bullet_left);
    free(timer);
    free(crocodiles_bullets);
    return manche_result;
}