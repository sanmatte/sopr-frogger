#include "game.h"

// Global variables
int pause_flag = 0;
int manche = 3, score = 0;
bool endgame = FALSE, dens[DENS_NUMBER] = {TRUE, TRUE, TRUE, TRUE, TRUE};
shared_buffer_t buffer;           // Shared buffer
atomic_int collided_bullet = -1;  // -1 = no collision

/**
 * @brief  function for the timer
 * @param  arg arguments
 */
void* game_timer(void *arg){
    Item msg = *(Item *)arg;
    // push the timer value every second
    while(1){
        suspend_thread();
        buffer_push(&buffer, msg);
        usleep(TIMER_SPEED);
    }
}

/**
 * @brief  function for calculating the score
 * @param  timer time left
 */
int compute_score(int timer){
    // compute the score based on the time left
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
 * @brief  active thread control function
 * @param  thread thread to check
 */
int is_thread_active(pthread_t thread) {
    return pthread_tryjoin_np(thread, NULL) == EBUSY;
}


/**
 * @brief  function for the start of the game
 * @param  game game window
 */
void startGame(WINDOW *game) {
    werase(game);
    // play until the game is over
    while (endgame == FALSE)
    {
        // check the return value of the play function
        switch (play(game))
        {
            case MANCHE_LOST:
                manche--;
                if (manche == 0) {
                    endgame = true;
                }
                break;
            case MANCHE_WON:
                endgame = true; 
                for(int i = 0; i < 5; i++){
                    if (dens[i] != FALSE) {
                        endgame = false;
                        break;
                    }
                }
                break;
            case GAME_QUIT:
                exit(0);
                break;
            case BACK_TO_MENU:
                return;
                break;
        }
        werase(game);
        refresh();
    }
    buffer_destroy(&buffer);
    print_endgame(game, manche, dens, score);
    // reset the game
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
    int is_bullet_frog_active = 0;
    atomic_store(&collided_bullet, -1);

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

    // buffer initialization
    buffer_init(&buffer);
    init_suspend_resume();

    // threads declaration
    pthread_t thread_timer, thread_frog, thread_bullet_right = -1, thread_bullet_left = -1;
    pthread_t thread_crocodile[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER];

    // threads creation
    pthread_create(&thread_timer, NULL, game_timer, timer);                  // timer thread
    pthread_create(&thread_frog, NULL, frog_controller, NULL);               // frog thread
    for (int i = 0; i < STREAM_NUMBER; i++) {
        int distance = rand_range(0, stream_speed[i] * CROCODILE_DIM_X);     // random time of spawn
        for (int j = 0; j < CROCODILE_STREAM_MAX_NUMBER; j++) {
            void** args = malloc(2 * sizeof(void*));
            if (args == NULL) {
                perror("Errore nell'allocazione della memoria per gli argomenti");
                return -1;
            }
            int *distance_ptr = malloc(sizeof(int));
            *distance_ptr = distance;
            args[0] = &crocodiles[i][j];
            args[1] = distance_ptr;
            // crocodile thread creation
            if (pthread_create(&thread_crocodile[i][j], NULL, crocodile_fun, args) != 0) {
                perror("Errore nella creazione del thread coccodrillo");
                free(distance_ptr);
                free(args);
                return -1;
            }
            // minimum distance
            distance += crocodiles[i][j].speed * CROCODILE_DIM_X;
            // random distance
            distance += rand_range(crocodiles[i][j].speed * (CROCODILE_DIM_X / 2), crocodiles[i][j].speed * (CROCODILE_DIM_X));
        }
    }

    Item msg;
    int manche_result = -1;
    while (TRUE) {
        msg = buffer_pop(&buffer);
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
                        // set right bullet properties
                        bullet_right->x = frog->x + FROG_DIM_X;
                        bullet_right->y = frog->y + 1;
                        bullet_right->extra = 1;
                        void *argr = bullet_right;  
                        // create right bullet thread
                        if (pthread_create(&thread_bullet_right, NULL, bullet_right_fun, argr) != 0) {
                            perror("Errore nella creazione del thread per il proiettile destro");
                        }
                        // set right bullet properties
                        bullet_left->x = frog->x - 1;
                        bullet_left->y = frog->y + 1;
                        bullet_left->extra = -1;
                        void* argl = bullet_left;
                        // create left bullet thread
                        if (pthread_create(&thread_bullet_left, NULL, bullet_left_fun, argl) != 0) {
                            perror("Errore nella creazione del thread per il proiettile destro");
                        }
                        
                    }
                    // frog bullet deactivation
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
                //check if the bullet is still active and update its position
                if(bullet_right->extra != 0){
                    *bullet_right = msg;
                }
                else
                {
                    pthread_detach(thread_bullet_right);
                    thread_bullet_right = -1;
                }
                break;
            case BULLET_ID_LEFT:
                //check if the bullet is still active and update its position
                if(bullet_left->extra != 0){
                    *bullet_left = msg;
                }
                else
                {
                    pthread_detach(thread_bullet_left);
                    thread_bullet_left = -1;
                }
                break;
            case TIMER_ID:
                timer->x -= 1;
                // check if the timer is expired
                if(timer->x == 0){
                    manche_result = MANCHE_LOST;
                }
                break;
            // crocodile bullet
            case CROCODILE_MIN_BULLETS_ID ... CROCODILE_MAX_BULLETS_ID:
                crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID] = msg;
                // check if the bullet hit the frog
                if(frog->y == (crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].y - 1) && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x >= frog->x && crocodiles_bullets[msg.id - CROCODILE_MIN_BULLETS_ID].x <= frog->x + FROG_DIM_X){  
                    manche_result = MANCHE_LOST;
                }
                break;
            case PAUSE_ID:
                pthread_mutex_lock(&m_suspend_mutex);
                pause_flag = 1;
                pthread_mutex_unlock(&m_suspend_mutex);
                WINDOW *pause = newwin(5, 23, (GAME_HEIGHT/2) + 4,  (GAME_WIDTH/2) + 10);
                print_pause(pause, game);
                int ch = getchar(); 
                // wait for the user to press a key
                while (ch != 'p' && ch != 'q' && ch != 'm') {
                    ch = getchar();
                }
                if(ch == 'p'){
                    resume_threads();
                }
                else if(ch == 'q'){
                    resume_threads();
                    endwin();
                    manche_result = GAME_QUIT;
                }
                else if(ch == 'm'){
                    resume_threads();
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
                                        // check crocodile direction
                                        if (crocodiles[stream][j].extra == -1)
                                        {
                                            if(frog->x == 0) continue;                       //check if the frog is at the edge of the screen                                                        
                                            else frog->x += crocodiles[stream][j].extra;
                                        }
                                        else
                                        {
                                            if(frog->x == GAME_WIDTH - FROG_DIM_X) continue; //check if the frog is at the edge of the screen                                                 
                                            else frog->x += crocodiles[stream][j].extra;
                                        }
                                        print_frog(game, frog);
                                        wrefresh(game);
                                    }
                                    
                                }
                            }

                            //crocodile respawn if it is out of the screen
                            if(crocodiles[i][j].extra == 1 && crocodiles[i][j].x == GAME_WIDTH + 1){   
                                pthread_detach(thread_crocodile[i][j]);
                                thread_crocodile[i][j] = -1;
                                crocodiles[i][j].x = - CROCODILE_DIM_X; 
                                int *distance_ptr = malloc(sizeof(int));
                                *distance_ptr = rand_range(0, crocodiles[i][j].speed * (CROCODILE_DIM_X) / 3);
                                void** args = malloc(2 * sizeof(void*));
                                args[0] = &crocodiles[i][j];
                                args[1] = distance_ptr;
                                // recreate crocodile thread
                                if (pthread_create(&thread_crocodile[i][j], NULL, crocodile_fun, args) != 0) {
                                    perror("Errore nella creazione del thread coccodrillo");
                                    free(distance_ptr);
                                    free(args);
                                    return -1;
                                }
                            }
                            else if(crocodiles[i][j].extra == -1 && crocodiles[i][j].x == - CROCODILE_DIM_X - 1){
                                pthread_detach(thread_crocodile[i][j]);
                                thread_crocodile[i][j] = -1;
                                crocodiles[i][j].x = GAME_WIDTH + CROCODILE_DIM_X;
                                int *distance_ptr = malloc(sizeof(int));
                                *distance_ptr = rand_range(0, crocodiles[i][j].speed * (CROCODILE_DIM_X) / 3);
                                void** args = malloc(2 * sizeof(void*));
                                args[0] = &crocodiles[i][j];
                                args[1] = distance_ptr;
                                // recreate crocodile thread
                                if (pthread_create(&thread_crocodile[i][j], NULL, crocodile_fun, args) != 0) {
                                    perror("Errore nella creazione del thread coccodrillo");
                                    free(distance_ptr);
                                    free(args);
                                    return -1;
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

        // Collision between frog and crocodile
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
                break;
            }
        }

        // Collision between frog bullets and crocodile bullet
        if(is_bullet_frog_active == 1){
            stream = ((SIDEWALK_HEIGHT_2 + 1 - bullet_right->y) / FROG_DIM_Y);
            if( stream >= 0 && stream < STREAM_NUMBER ) {
                for(int i=stream*CROCODILE_STREAM_MAX_NUMBER; i<stream*CROCODILE_STREAM_MAX_NUMBER+3; i++){
                    if((crocodiles_bullets[i].x - bullet_right->x) <= 1 && (crocodiles_bullets[i].x - bullet_right->x) >= -1 && crocodiles_bullets[i].y == bullet_right->y){
                        pthread_cancel(thread_bullet_right);
                        pthread_join(thread_bullet_right, NULL);
                        atomic_store(&collided_bullet, crocodiles_bullets[i].id);
                        thread_bullet_right = -1;
                        bullet_right->x = RESET_FROG_BULLET;
                        bullet_right->extra = 0;
                        crocodiles_bullets[i].x = RESET_CROCODILE_BULLET;
                    }
                    if((crocodiles_bullets[i].x - bullet_left->x) <= 1 && (crocodiles_bullets[i].x - bullet_left->x) >= -1 && crocodiles_bullets[i].y == bullet_left->y){
                        pthread_cancel(thread_bullet_left);
                        pthread_join(thread_bullet_left, NULL);
                        thread_bullet_left = -1;
                        atomic_store(&collided_bullet, crocodiles_bullets[i].id);
                        bullet_left->x = RESET_FROG_BULLET;
                        bullet_left->extra = 0;
                        crocodiles_bullets[i].x = RESET_CROCODILE_BULLET;
                    }
                }
            }
        }

        // check if the frog is in a den
        if(frog->y < DENS_HEIGHT){
            switch(frog->x){
                case DENS_1:
                    compute_score(timer->x);
                    dens[0] = FALSE;
                    manche_result = MANCHE_WON;
                    break;
                case DENS_2:
                    compute_score(timer->x);
                    dens[1] = FALSE;
                    manche_result = MANCHE_WON;
                    break;
                case DENS_3:
                    compute_score(timer->x);
                    dens[2] = FALSE;
                    manche_result = MANCHE_WON;
                    break;
                case DENS_4:
                    compute_score(timer->x);
                    dens[3] = FALSE;
                    manche_result = MANCHE_WON;
                    break;
                case DENS_5:
                    compute_score(timer->x);
                    dens[4] = FALSE;      
                    manche_result = MANCHE_WON;
                    break;
                default: // lost manche if the frog is not in a den and hit the upper edge
                    manche_result = MANCHE_LOST;
            }
        }

        // check if the manche is over
        if (manche_result != -1)
        {
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
    // kill all threads
    pkill_all(thread_timer, thread_frog, thread_crocodile, thread_bullet_left, thread_bullet_right);
    // free memory
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


/**
 * @brief  function for killing all threads
 * @param  thread_timer timer thread
 * @param  thread_frog frog thread
 * @param  thread_crocodile crocodile thread
 * @param  thread_bullet_left left bullet thread
 * @param  thread_bullet_right right bullet thread
 */
void pkill_all(pthread_t thread_timer, pthread_t thread_frog, pthread_t thread_crocodile[STREAM_NUMBER][CROCODILE_STREAM_MAX_NUMBER], pthread_t thread_bullet_left, pthread_t thread_bullet_right){
    pthread_cancel(thread_timer);
    pthread_join(thread_timer, NULL);
    pthread_cancel(thread_frog);
    pthread_join(thread_frog, NULL);
    if ((int)thread_bullet_right != -1 && is_thread_active(thread_bullet_right))
    {
        pthread_cancel(thread_bullet_right);
        pthread_join(thread_bullet_right, NULL);
    }
    if ((int)thread_bullet_left != -1 && is_thread_active(thread_bullet_left))
    {
        pthread_cancel(thread_bullet_left);
        pthread_join(thread_bullet_left, NULL);
    }
    for(int i=0; i < STREAM_NUMBER; i++){
        for(int j=0; j < CROCODILE_STREAM_MAX_NUMBER; j++){
            if ((int)thread_crocodile[i][j] != -1)
            {
                pthread_cancel(thread_crocodile[i][j]);
                pthread_join(thread_crocodile[i][j], NULL);
            }
        }
    }
}