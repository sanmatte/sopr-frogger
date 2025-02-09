#include <sys/types.h>
#include <unistd.h>
#include "utils.h"
#include <pthread.h>

pthread_mutex_t m_suspend_mutex;  // Mutex for suspending threads
pthread_cond_t m_resume_cond;     // Condition variable to resume threads

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

void producer_rand_sleep(){
    int sleep_time = rand_range(0, 10000);
    usleep(sleep_time);
}

time_t timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL); // Get unix time in seconds and the microseconds
    time_t milliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000; // Calc unix time in milliseconds
    return milliseconds;
}

void init_suspend_resume() {
    pthread_mutex_init(&m_suspend_mutex, NULL);  // Mutex initialization
    pthread_cond_init(&m_resume_cond, NULL);    // Condition variable initialization
}

void suspend_thread() {
    // Lock the mutex
    pthread_mutex_lock(&m_suspend_mutex);
    
    // Suspend thread if pause_flag is set
    while (pause_flag == 1) {
        // Wait for the condition variable to signal the thread to resume
        pthread_cond_wait(&m_resume_cond, &m_suspend_mutex);
    }
    
    // Unlock the mutex
    pthread_mutex_unlock(&m_suspend_mutex);
}

void resume_threads() {
    // Lock the mutex
    pthread_mutex_lock(&m_suspend_mutex);
    
    pause_flag = 0;  // Set pause_flag to 0 to resume threads
    
    // Broadcast to all waiting threads to resume
    pthread_cond_broadcast(&m_resume_cond);
    
    // Unlock the mutex
    pthread_mutex_unlock(&m_suspend_mutex);
}