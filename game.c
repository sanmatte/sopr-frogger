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
#include <stdatomic.h>
#include "game.h"

int pause_flag = 0;
int manche = 3, score = 0;
bool endgame = FALSE;
bool dens[DENS_NUMBER] = {TRUE, TRUE, TRUE, TRUE, TRUE};
shared_buffer_t buffer;

atomic_int collided_bullet = -1;  // -1 = nessuna collisione

void* game_timer(void *arg){
    Item msg = *(Item *)arg;
    while(1){
        suspend_thread();
        buffer_push(&buffer, msg);
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

int is_thread_active(pthread_t thread) {
    return pthread_tryjoin_np(thread, NULL) == EBUSY;
}
void startGame(WINDOW *game) {
    
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
    int is_bullet_frog_active = 0;
    atomic_store(&collided_bullet, -1);

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
    bullet_right->id = BULLET_ID_RIGHT;
    bullet_right->x = -1;
    bullet_right->y = -1;
    bullet_right->speed = 0;
    bullet_right->extra = 1;

    Item *bullet_left = malloc(sizeof(Item));
    bullet_left->id = BULLET_ID_LEFT;
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

    buffer_init(&buffer);
    init_suspend_resume();

    int stream_speed[STREAM_NUMBER];

    for(int i=0; i<STREAM_NUMBER; i++){
        stream_speed[i] = rand_range(current_difficulty.crocodile_speed_min, current_difficulty.crocodile_speed_max);
    }
    
    initializeCrocodile(crocodiles, stream_speed);

    pthread_t thread_timer, thread_frog, thread_bullet_right, thread_bullet_left;
    pthread_t thread_crocodile[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER];

    pthread_create(&thread_timer, NULL, game_timer, timer);

    pthread_create(&thread_frog, NULL, Frog, NULL);

    for (int i = 0; i < STREAM_NUMBER; i++) {
        int distance = rand_range(0, stream_speed[i] * CROCODILE_DIM_X);
        for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++) {
            void** args = malloc(2 * sizeof(void*));
            if (args == NULL) {
                perror("Errore nell'allocazione della memoria per gli argomenti");
                return 1;
            }
            int *distance_ptr = malloc(sizeof(int));
            *distance_ptr = distance;
            args[0] = &crocodiles[i][j];
            args[1] = distance_ptr;
            
            if (pthread_create(&thread_crocodile[i][j], NULL, Crocodile, args) != 0) {
                perror("Errore nella creazione del thread coccodrillo");
                free(distance_ptr);
                free(args);
                return 1;
            }
            //distanza minima
            distance += crocodiles[i][j].speed * CROCODILE_DIM_X;
            // distanza randomica
            distance += rand_range(crocodiles[i][j].speed * (CROCODILE_DIM_X / 2), crocodiles[i][j].speed * (CROCODILE_DIM_X));
        }
    }

    Item msg;

    while (TRUE) {
        msg = buffer_pop(&buffer);
        switch (msg.id) {
            // FROG case
            case FROG_ID:
                if (frog->x + msg.x >= 0 && frog->x + msg.x <= GAME_WIDTH - FROG_DIM_X && frog->y + msg.y >= 0 && frog->y + msg.y <= GAME_HEIGHT - FROG_DIM_Y) {
                    frog->y += msg.y;
                    frog->x += msg.x;
                    frog->extra = msg.extra;
                    if (frog->extra == 1 && is_bullet_frog_active == 0) {
                        is_bullet_frog_active = 1;
                        bullet_right->x = frog->x + FROG_DIM_X;
                        bullet_right->y = frog->y + 1;
                        bullet_right->extra = 1;
                         
                        void *argr = malloc(sizeof(void*)); 
                        argr = bullet_right;      

                        if (pthread_create(&thread_bullet_right, NULL, bullet_right_fun, argr) != 0) {
                            perror("Errore nella creazione del thread per il proiettile destro");
                        }

                        bullet_left->x = frog->x - 1;
                        bullet_left->y = frog->y + 1;
                        bullet_left->extra = -1;

                        void* argl = malloc(sizeof(void*));
                        argl = bullet_left;

                        if (pthread_create(&thread_bullet_left, NULL, bullet_left_fun, argl) != 0) {
                            perror("Errore nella creazione del thread per il proiettile destro");
                        }
                        
                    }
                    else
                        {
                            if (bullet_left->extra == 0 && bullet_right->extra == 0)
                            {
                                is_bullet_frog_active = 0;
                            }
                        } 
                }
                break;

            case BULLET_ID_RIGHT:
                if(bullet_right->extra != 0){
                    bullet_right->x = msg.x;
                    bullet_right->y = msg.y;
                }
                break;

            case BULLET_ID_LEFT:
                if(bullet_left->extra != 0){
                    bullet_left->x = msg.x;
                    bullet_left->y = msg.y;
                }
                break;

            case TIMER_ID:
                timer->x -= 1;
                if(timer->x == 0){
                    pkill_all(thread_timer, thread_frog, thread_crocodile);
                    return 0;
                }
                break;
            case CROCODILE_MIN_BULLETS_ID ... CROCODILE_MAX_BULLETS_ID:
                crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID] = msg;

                // collisione tra rana e proiettile coccodrillo
                if(frog->y == (crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].y - 1) && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x >= frog->x && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x <= frog->x + FROG_DIM_X){  
                    pkill_all(thread_timer, thread_frog, thread_crocodile);
                    return 0;
                }

                break;
            
            case PAUSE_ID:
                pthread_mutex_lock(&m_suspend_mutex);
                pause_flag = 1;
                pthread_mutex_unlock(&m_suspend_mutex);
                WINDOW *pause = newwin(5, 25, (GAME_HEIGHT/2) + 5,  (GAME_WIDTH/2) + 10);
                box(pause, 0, 0);
                mvwprintw(pause, 1, 1 , "Press Q to quit");
                mvwprintw(pause, 2, 1 , "Press P to resume");
                mvwprintw(pause, 3, 1 , "Press M to go to menu");
                wrefresh(pause);
                int ch = getchar();  // Wait for user input
                while (ch != 'p' && ch != 'q' && ch != 'm') {
                    ch = getchar();
                }
                if(ch == 'p'){
                    resume_threads();
                }
                else if(ch == 'q'){
                    resume_threads();
                    pkill_all(thread_timer, thread_frog, thread_crocodile);
                    endwin();
                    return 2;
                }
                else if(ch == 'm'){
                    resume_threads();
                    pkill_all(thread_timer, thread_frog, thread_crocodile);
                    return 3;
                }
                resume_threads();
                
                break;
            case QUIT_ID:
                pkill_all(thread_timer, thread_frog, thread_crocodile);
                endwin();
                return 2;
                break;  
            default:
                if(msg.id >= CROCODILE_MIN_ID && msg.id <= CROCODILE_MAX_ID){
                    for(int i = 0; i < STREAM_NUMBER; i++){
                        for(int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
                            if(crocodiles[i][j].id == msg.id){
                                crocodiles[i][j] = msg;
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
                break;
                }
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
                    pkill_all(thread_timer, thread_frog, thread_crocodile);
                    return 0;
                }
            }
            // Collision between frog bullets and crocodile bullet
            if(is_bullet_frog_active == 1){
                stream = ((SIDEWALK_HEIGHT_2 + 1 - bullet_right->y) / FROG_DIM_Y);
                for(int i=stream*CROCODILE_STREAM_MAX_NUMBER; i<stream*CROCODILE_STREAM_MAX_NUMBER+3; i++){
                    if((crocodiles_bullets[i].x - bullet_right->x) <= 1 && (crocodiles_bullets[i].x - bullet_right->x) >= -1 && crocodiles_bullets[i].y == bullet_right->y){
                        pthread_cancel(thread_bullet_right);
                        pthread_join(thread_bullet_right, NULL);
                        atomic_store(&collided_bullet, crocodiles_bullets[i].id);
                        bullet_right->x = -8;
                        bullet_right->extra = 0;
                        crocodiles_bullets[i].x = -18;
                    }
                    if((crocodiles_bullets[i].x - bullet_left->x) <= 1 && (crocodiles_bullets[i].x - bullet_left->x) >= -1 && crocodiles_bullets[i].y == bullet_left->y){
                        pthread_cancel(thread_bullet_left);
                        pthread_join(thread_bullet_left, NULL);
                        atomic_store(&collided_bullet, crocodiles_bullets[i].id);
                        bullet_left->x = -8;
                        bullet_left->extra = 0;
                        crocodiles_bullets[i].x = -18;
                    }
                }
            }

            if(frog->y < DENS_HEIGHT){
                switch(frog->x){
                    case DENS_1:
                        compute_score(timer->x);
                        dens[0] = FALSE;
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
                        break;
                    case DENS_2:
                        compute_score(timer->x);
                        dens[1] = FALSE;
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
                        break;
                    case DENS_3:
                        compute_score(timer->x);
                        dens[2] = FALSE;
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
                        break;
                    case DENS_4:
                        compute_score(timer->x);
                        dens[3] = FALSE;
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
                        break;
                    case DENS_5:
                        compute_score(timer->x);
                        dens[4] = FALSE;
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 1;
                        break;
                    default:
                        pkill_all(thread_timer, thread_frog, thread_crocodile);
                        return 0;
                }
            }

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
        free(frog);
        free(crocodiles);
        free(bullet_right);
        free(bullet_left);
        free(timer);
        free(crocodiles_bullets);
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