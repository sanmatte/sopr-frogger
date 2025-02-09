#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#ifndef UTILS_H
#define UTILS_H

#include "design.h"

#define DEBUG_FILE_NAME "debuglogs.log"

void debuglog(char *message, int arg);
int rand_range(int min, int max);
void start_colors();
time_t timestamp(void);

#endif