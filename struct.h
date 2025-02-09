#ifndef STRUCT_H
#define STRUCT_H

#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 500
typedef struct {
    int id;
    int y;
    int x;
    int speed;
    int extra; // direzione per il coccodrillo e stato per la rana
} Item;

typedef struct {
    int bullets_speed;
    int crocodile_speed_max;
    int crocodile_speed_min;
    int crocodile_bullet_speed;
    int shot_range;
    int frog_movement_limit;
} Difficulty;

typedef struct {
    Item buffer[BUFFER_SIZE];
    int in, out;
    sem_t sem_spazi;    // Semaforo per gli spazi liberi
    sem_t sem_elementi; // Semaforo per gli elementi disponibili
    pthread_mutex_t mutex;
} shared_buffer_t;

extern Difficulty current_difficulty;
#endif