#include <sys/types.h>
#include <unistd.h>
#include "utils.h"
void debuglog(char *message, int arg){
    FILE *f = fopen(DEBUG_FILE_NAME, "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(f, message, arg);
    fclose(f);
}

int rand_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// void create_process(int* pipe_fds, pid_t *pid_list, int index, int sleep, void (*child_function)(int, void*), void* func_params) {
//     pid_t pid = fork();
//     if (pid < 0) {
//         perror("Fork failed");
//         exit(1);
//     } else if (pid == 0) { // Processo figlio
//         if (sleep > 0) {
//             usleep(sleep);
//         }
//         child_function(pipe_fds[1], func_params);
//         exit(0);
//     } else { // Processo padre
//         pid_list[index] = pid;
//     }
// }

time_t timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL); // Get unix time in seconds and the microseconds
    time_t milliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000; // Calc unix time in milliseconds
    return milliseconds;
}