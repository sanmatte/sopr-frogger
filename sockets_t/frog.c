#include "frog.h"
#define SOCKET_PATH "/tmp/mysocket"
#include <sys/socket.h>
#include <sys/un.h>

int newmanche = FALSE; 

void frog_cleanup_function(void *arg) {
    close(*(int *)arg);
}

/**
 * @brief  function that manages the frog movement
 */
void* frog_controller() {
    int client_fd;
    struct sockaddr_un addr;

    // Create socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    void *arg_cleanup;
    arg_cleanup = &client_fd;
    pthread_cleanup_push(frog_cleanup_function, arg_cleanup);

    // Set up server address
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    int is_connected = -1;

    do{
        is_connected = connect(client_fd, (struct sockaddr*)&addr, sizeof(addr));
    }while (is_connected == -1);

    int ch;
    bool has_moved = TRUE;               // flag to check if the frog has moved
    Item frog = {FROG_ID, 0, 0, 0, 0};
    while (1) {
        // read the last input from the keyboard
        do {
            ch = getch();
        } while (getch() != ERR);
        frog.id = FROG_ID;
        switch (ch) {
            case KEY_UP:
                frog.y = -FROG_DIM_Y;
                frog.x = 0;
                has_moved = TRUE;
                break;
            case KEY_DOWN:
                frog.y = +FROG_DIM_Y;
                frog.x = 0;
                has_moved = TRUE;
                break;
            case KEY_LEFT:
                frog.y = 0;
                frog.x = -1;
                has_moved = TRUE;
                break;
            case KEY_RIGHT:
                frog.y = 0;
                frog.x = +1;
                has_moved = TRUE;
                break;
            case 32:                     // space bar to shoot
                frog.extra = 1;
                frog.y = 0;
                frog.x = 0;
                has_moved = TRUE;
                break;
            case 'q':                    // quit the game
                frog.id = QUIT_ID;
                has_moved = TRUE;
                break;
            //detect escape key press
            case 'p':                    // pause the game
                frog.id = PAUSE_ID;
                has_moved = TRUE;
                break;
        }
        suspend_thread();                // suspend the thread 
        if(has_moved){
            send(client_fd, &frog, sizeof(Item), 0);
        }
        frog.extra = 0;
        has_moved = FALSE;
        usleep(current_difficulty.frog_movement_limit); //default 0
    }
    pthread_cleanup_pop(1);
    return NULL;
}


/**
 * @brief  function for the movement of the frog's right projectile
 * @param  arg bullet args
 */
void* bullet_right_fun(void *arg) {
    Item bullet = *(Item *)arg;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // move the bullet until the screen limit
    while (bullet.x < GAME_WIDTH + 1) {
        bullet.x += 1;
        suspend_thread();
        buffer_push(&buffer, bullet);
        usleep(current_difficulty.bullets_speed);
    }
    bullet.extra = 0;
    buffer_push(&buffer, bullet);
    return NULL;
}


/**
 * @brief  function for the movement of the frog's left projectile
 * @param  arg bullet args
 */
void* bullet_left_fun(void *arg) {
    Item bullet = *(Item *)arg;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // move the bullet until the screen limit
    while (bullet.x >= 0) {
        bullet.x -= 1;
        suspend_thread();
        buffer_push(&buffer, bullet);
        usleep(current_difficulty.bullets_speed);
    }
    bullet.extra = 0;
    buffer_push(&buffer, bullet);
    return NULL;
}