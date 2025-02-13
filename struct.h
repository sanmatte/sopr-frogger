#ifndef STRUCT_H
#define STRUCT_H
#include <stdbool.h>
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
    int river_color;
    int shotload_time;
} Difficulty;

extern Difficulty current_difficulty;
#endif