#include <sys/types.h>
#include <unistd.h>
#include "utils.h"
#include <pthread.h>

pthread_mutex_t m_suspend_mutex;  // mutex per sospendere i thread
pthread_cond_t m_resume_cond;     // variabile per riprendere i thread dopo la pausa

/**
 * Funzione per generare un numero casuale compreso tra min e max
 * @param min valore minimo
 * @param max valore massimo
 * @return numero casuale
*/
int rand_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

/**
 * Funziona come la sleep ma non viene interrotta da un segnale
 * @param microseconds numero di microsecondi
*/
void continue_usleep(long microseconds) {
    long elapsed = 0;

    while (elapsed < microseconds) {
        suspend_thread();
        usleep(1000);
        elapsed += 1000;
    }
}

void init_suspend_resume() {
    pthread_mutex_init(&m_suspend_mutex, NULL);
    pthread_cond_init(&m_resume_cond, NULL);
}

/**
 * Sospende i thread se pause_flag == 1 (usata per le pause del gioco)
*/
void suspend_thread() {
    pthread_mutex_lock(&m_suspend_mutex);
    
    while (pause_flag == 1) {
        pthread_cond_wait(&m_resume_cond, &m_suspend_mutex);
    }

    pthread_mutex_unlock(&m_suspend_mutex);
}

/**
 * Riprende i thread impostando pause_flag = 0 e inviando un segnale a tutti i thread in attesa
*/
void resume_threads() {
    pthread_mutex_lock(&m_suspend_mutex);
    
    pause_flag = 0;
    
    pthread_cond_broadcast(&m_resume_cond);
    
    pthread_mutex_unlock(&m_suspend_mutex);
}

void debuglog(char *message, int arg){
    FILE *f = fopen("debuglog.logs", "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(f, message, arg);
    fclose(f);
}

void start_colors(){
    start_color();
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
    init_color(COLOR_RIVER_EASY, 125, 816, 839);
    init_color(COLOR_RIVER_HARD,75, 90, 369);
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
    init_pair(21, COLOR_RIVER_EASY, COLOR_RIVER_EASY);
    init_pair(22, COLOR_RIVER_HARD, COLOR_RIVER_HARD);
    init_pair(23, COLOR_DARKGREEN, COLOR_RIVER_EASY);
    init_pair(24, COLOR_DARKGREEN, COLOR_RIVER_HARD);
    init_pair(25, COLOR_BULLET_TRIGGER, COLOR_RIVER_EASY);
    init_pair(26, COLOR_BULLET_TRIGGER, COLOR_RIVER_HARD);
    init_pair(27, COLOR_BLACK, COLOR_DARKGREEN);
    init_pair(28, COLOR_BLACK, COLOR_BULLET_TRIGGER);
}