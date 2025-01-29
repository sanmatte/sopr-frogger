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

#endif