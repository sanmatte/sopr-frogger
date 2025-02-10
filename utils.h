#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "design.h"
#
#ifndef UTILS_H
#define UTILS_H

extern int pause_flag;

int rand_range(int min, int max);
void start_colors();
void continue_usleep(long microseconds);
void producer_rand_sleep();

void init_suspend_resume();
void suspend_thread(); 
void resume_threads();

#endif