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

time_t timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL); // Get unix time in seconds and the microseconds
    time_t milliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000; // Calc unix time in milliseconds
    return milliseconds;
}