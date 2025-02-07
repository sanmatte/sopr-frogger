#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#ifndef UTILS_H
#define UTILS_H

#define DEBUG_FILE_NAME "debuglogs.log"

void debuglog(char *message, int arg);
int rand_range(int min, int max);
//void create_process(int* pipe_fds, pid_t *pid_list, int index, int sleep, void (*func_process)(int, int*), int* func_params);
time_t timestamp(void);
void continue_usleep(long microseconds);
#endif