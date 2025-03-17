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
    int extra; // is used for crocodile direction, projectile type and status
} Item;

typedef struct {
    int bullets_speed;
    int crocodile_speed_max;
    int crocodile_speed_min;
    int crocodile_bullet_speed;
    int shot_range;
    int frog_movement_limit;
    int river_color;
    int shotload_time;
} Difficulty;

typedef struct {
    Item buffer[BUFFER_SIZE];
    int in, out;
    sem_t sem_spazi;    
    sem_t sem_elementi; 
    pthread_mutex_t mutex;
} shared_buffer_t;

extern Difficulty current_difficulty;
#endif