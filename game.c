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
#include "buffer.h"
int testvar = 0;
int manche = 3, score = 0;
bool endgame = FALSE;
bool dens[DENS_NUMBER] = {TRUE, TRUE, TRUE, TRUE, TRUE};
shared_buffer_t buffer;

void* game_timer(void *arg){
    Item msg = *(Item *)arg;
    while(1){
        buffer_push(&buffer, msg);
        usleep(TIMER_SPEED);
    }
}

void startGame(WINDOW *game) {
    setlocale(LC_ALL, "");
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
    }

    buffer_init(&buffer);

    int stream_speed[STREAM_NUMBER];

    for(int i=0; i<STREAM_NUMBER; i++){
        stream_speed[i] = rand_range(current_difficulty.crocodile_speed_min, current_difficulty.crocodile_speed_max);
    }
    
    InitializeCrocodile(crocodiles, stream_speed);

    pthread_t thread_timer, thread_frog, thread_bullet_right, thread_bullet_left;
    pthread_t thread_crocodile[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER];

    pthread_create(&thread_timer, NULL, game_timer, &timer);

    pthread_create(&thread_frog, NULL, Frog, &buffer);

    for (int i = 0; i < STREAM_NUMBER; i++) {
        int distance = rand_range(0, stream_speed[i] * CROCODILE_DIM_X);
        for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++) {
            // Allocate memory for the arguments
            void** args = malloc(3 * sizeof(void*));
            if (args == NULL) {
                perror("Errore nell'allocazione della memoria per gli argomenti");
                return 1;
            }
            int *distance_ptr = malloc(sizeof(int));
            *distance_ptr = distance;
            args[0] = &buffer;
            args[1] = &crocodiles[i][j];
            args[2] = distance_ptr;
            
            if (pthread_create(&thread_crocodile[i][j], NULL, Crocodile, args) != 0) {
                perror("Errore nella creazione del thread coccodrillo");
                free(distance_ptr);
                free(args);  // Free the allocated memory in case of error
                return 1;
            }
            distance += crocodiles[i][j].speed * CROCODILE_DIM_X;
            // add random distance between crocodiles
            distance += rand_range(crocodiles[i][j].speed * (CROCODILE_DIM_X / 2), crocodiles[i][j].speed * (CROCODILE_DIM_X));
        }
    }

    Item msg;

    while (TRUE) {
        msg = buffer_pop(&buffer);
        switch (msg.id) {
            // FROG case
            case FROG_ID:
                if (frog.x + msg.x >= 0 && frog.x + msg.x <= GAME_WIDTH - FROG_DIM_X && frog.y + msg.y >= 0 && frog.y + msg.y <= GAME_HEIGHT - FROG_DIM_Y) {
                    frog.y += msg.y;
                    frog.x += msg.x;
                    frog.extra = msg.extra;

                    if (frog.extra == 1){
                        if(is_bullet_frog_active == 0) {
                            is_bullet_frog_active = 1;
                            void** args = malloc(3 * sizeof(void*));
                            args[0] = &frog;          
                            args[1] = &bullet_right;  
                            args[2] = &buffer;        

                            if (pthread_create(&thread_bullet_right, NULL, bullet_right_fun, args) != 0) {
                                perror("Errore nella creazione del thread per il proiettile destro");
                                free(args); 
                            }

                            void** args2 = malloc(3 * sizeof(void*));
                            args2[0] = &frog;
                            args2[1] = &bullet_left;
                            args2[2] = &buffer;

                            if (pthread_create(&thread_bullet_left, NULL, bullet_left_fun, args2) != 0) {
                                perror("Errore nella creazione del thread per il proiettile destro");
                                free(args2);
                            }
                        }
                        else{
                            if(pthread_tryjoin_np(thread_bullet_right, NULL) == 0 && pthread_tryjoin_np(thread_bullet_left, NULL) == 0) {
                                is_bullet_frog_active = 0;
                                
                            }                   
                        }
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
                    kill(getpid(), SIGUSR1);
                    pkill_all(thread_timer, thread_frog, thread_crocodile);
                    return 0;
                }
                break;
            case CROCODILE_MIN_BULLETS_ID ... CROCODILE_MAX_BULLETS_ID:
                crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID] = msg;

                // Collision beetwen frog and crocodile bullet
                if(frog.y == (crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].y - 1) && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x >= frog.x && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x <= frog.x + FROG_DIM_X){
                    kill(getpid(), SIGUSR1);
                    pkill_all(thread_timer, thread_frog, thread_crocodile);
                    return 0;
                }
                if ((bullet_right.x == msg.x && bullet_right.y == msg.y) || (bullet_left.x == msg.x && bullet_left.y == msg.y)) {
                    // Resetta i proiettili in caso di collisione
                    // bullet_right.x = -1;
                    // bullet_right.y = -1;
                    // bullet_left.x = -1;
                    // bullet_left.y = -1;
                    // // Termina i processi dei proiettili
                    // if (bullet_pid_right > 0) {
                    //     kill(bullet_pid_right, SIGKILL);
                    //     bullet_pid_right = -1;
                    // }
                    // if (bullet_pid_left > 0) {
                    //     kill(bullet_pid_left, SIGKILL);
                    //     bullet_pid_left = -1;
                    // }
                }

                break;
            
            // case PAUSE_ID:
            //     killpg(group_pid, SIGSTOP);  // Pause all child processes
            //     //printf("Game Paused. Press any key to continue...\n");
            //     int ch = getchar();  // Wait for user input
            //     while (ch != 'p') {
            //         ch = getchar();
            //     }
                
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
                    // print_frog(game, &frog);
                    // wrefresh(game);
                    kill(getpid(), SIGUSR1);
                    pkill_all(thread_timer, thread_frog, thread_crocodile);
                    usleep(1000000);
                    return 0;
                }
            }
            // Collision between frog bullets and crocodile bullet
            stream = ((SIDEWALK_HEIGHT_2 + 1 - bullet_right.y) / FROG_DIM_Y);
            debuglog("Stream: %d\n", stream);
            for(int i=stream*CROCODILE_STREAM_MAX_NUMBER; i<stream*CROCODILE_STREAM_MAX_NUMBER+3; i++){
                if((crocodiles_bullets[i].x - bullet_right.x) <= 1 && (crocodiles_bullets[i].x - bullet_right.x) >= -1 && crocodiles_bullets[i].y == bullet_right.y){
                    pthread_cancel(thread_bullet_right);
                    pthread_join(thread_bullet_right, NULL);
                    debuglog("Collision Right%d\n", i);
                    pthread_kill(thread_crocodile[stream][i%CROCODILE_STREAM_MAX_NUMBER], SIGUSR1);
                    bullet_right.x = -1;
                    bullet_right.y = -1;
                    is_bullet_frog_active = 0;
                    crocodiles_bullets[i].x = -2;
                }
                if((crocodiles_bullets[i].x - bullet_left.x) <= 1 && (crocodiles_bullets[i].x - bullet_left.x) >= -1 && crocodiles_bullets[i].y == bullet_left.y){
                    pthread_cancel(thread_bullet_left);
                    pthread_join(thread_bullet_left, NULL);
                    debuglog("Collision Left%d\n", i);
                    pthread_kill(thread_crocodile[stream][i%CROCODILE_STREAM_MAX_NUMBER], SIGUSR1);
                    bullet_left.x = -1;
                    bullet_left.y = -1;
                    is_bullet_frog_active = 0;
                    crocodiles_bullets[i].x = -2;
                }
            }
            // array 24 posti y = stream -> i - 0,1,2/3,4,5/
            // Collision between frog bullets and crocodile bullet
            
            // for(int i=0; i<CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID; i++){
            //     if(crocodiles_bullets[i].x == bullet_right.x && crocodiles_bullets[i].y == bullet_right.y){
            //         bullet_right.x = GAME_WIDTH;
            //         bullet_right.y = GAME_HEIGHT;
            //         write(pipe_fds[1], &bullet_right, sizeof(Item));
            //     }
            // }


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
                        dens[0] = FALSE;
                        kill(getpid(), SIGUSR1);
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
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
                        dens[1] = FALSE;
                        kill(getpid(), SIGUSR1);
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
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
                        dens[2] = FALSE;
                        kill(getpid(), SIGUSR1);
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
                        break;
                    case DENS_4:
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
                        dens[3] = FALSE;
                        kill(getpid(), SIGUSR1);
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
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
                        dens[4] = FALSE;
                        kill(getpid(), SIGUSR1);
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
                        break;
                    default:
                        kill(getpid(), SIGUSR1);
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 0;
                }
            }

            print_score(game, manche, timer.x, score);
            print_background(game, dens);

            for(int i = 0; i < STREAM_NUMBER; i++){
                for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                    print_crocodile(game, &crocodiles[i][j]);
                }
            }

            for (int i = 0; i < CROCODILE_MAX_BULLETS_ID - CROCODILE_MIN_BULLETS_ID + 1; i++) {
                print_bullets(game, &crocodiles_bullets[i], -1);
            }
            print_frog(game, &frog);
            print_bullets(game, &bullet_left, BULLETS_ID);
            print_bullets(game, &bullet_right, BULLETS_ID);
            
            wrefresh(game);
        }
    
}

void pkill_all(pthread_t thread_timer, pthread_t thread_frog, pthread_t thread_crocodile[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER]){
    pthread_cancel(thread_timer);
    pthread_join(thread_timer, NULL);
    pthread_cancel(thread_frog);
    pthread_join(thread_frog, NULL);
    for(int i=0; i < STREAM_NUMBER; i++){
        for(int j=0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
            pthread_cancel(thread_crocodile[i][j]);
            pthread_join(thread_crocodile[i][j], NULL);
        }
    }
}