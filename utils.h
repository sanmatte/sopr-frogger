#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#ifndef UTILS_H
#define UTILS_H

#include "design.h"

int rand_range(int min, int max);
void start_colors();
void continue_usleep(long microseconds);

#endif