#ifndef FROG_H
#define FROG_H
#include <stdbool.h>
#include <errno.h>
extern int manche;
extern bool frog_on_crocodile;
#include "design.h"
#include "struct.h"

extern pthread_mutex_t m_suspend_mutex;
extern pthread_cond_t m_resume_cond;

#define FROG_ID 1
#define FROG_DIM_Y 4
#define FROG_DIM_X 10
#define BULLET_ID_RIGHT 26
#define BULLET_ID_LEFT 27
#define BULLETS_DIM 1
#define BULLETS_SPEED 20000

void* frog_controller();
void* bullet_right_fun(void* args);
void* bullet_left_fun(void* args);

extern shared_buffer_t buffer;
#define RESET_MANCHE_SIG SIGUSR2
#endif