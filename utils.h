#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#include "design.h"
extern int pause_flag;

int rand_range(int min, int max);
void start_colors();
void continue_usleep(long microseconds);
void producer_rand_sleep();
void debuglog(char *format, int arg);
void init_suspend_resume();
void suspend_thread(); 
void resume_threads();

#endif