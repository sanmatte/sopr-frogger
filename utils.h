#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#ifndef UTILS_H
#define UTILS_H

#define DEBUG_FILE_NAME "debuglogs.log"

void debuglog(char *message, int arg);
time_t timestamp(void);

#endif