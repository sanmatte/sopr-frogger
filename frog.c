#include "frog.h"
#include "utils.h"
#include "game.h"

#define SOCKET_PATH "/tmp/mysocket"
#include <sys/socket.h>
#include <sys/un.h>

/**
 * @brief  function that manages the frog movement
 * @param  pipe_fds: pipe file descriptors
 */
void frog_controller(int *pipe_fds){
    int client_fd;
    struct sockaddr_un addr;

    // Create socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }


    // Set up server address
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    int is_connected = -1;

    do{
        is_connected = connect(client_fd, (struct sockaddr*)&addr, sizeof(addr));
    }while (is_connected == -1);

    int ch;
    bool has_moved = TRUE;        // flag to check if the frog has moved
    Item frog = {FROG_ID, 0, 0, 0, 0};
    while (1) {
        // Read the latest key press (ignore older ones)
        do {
            ch = getch();
        } while (getch() != ERR); // Drain extra inputs
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
            case 32:              // space bar to shoot
                frog.extra = 1;
                frog.y = 0;
                frog.x = 0;
                has_moved = TRUE;
                break;
            case 'q':             // quit the game
                frog.id = QUIT_ID;
                has_moved = TRUE;
                break;
            case 'p':             // pause the game
                frog.id = PAUSE_ID;
                has_moved = TRUE;
                break;
        }
        // write new movement
        if(has_moved){
            if (pipe_fds != NULL) {
                send(client_fd, &frog, sizeof(Item), 0);
            }
        }
        frog.extra = 0;
        has_moved = FALSE;
        usleep(current_difficulty.frog_movement_limit); //default 0
    }
}


/**
 * @brief  function that manages the right bullet movement
 * @param  bullet_right: bullet item
 * @param  pipe_fds: pipe file descriptors
 */
void bullet_right_controller(Item *bullet_right, int *pipe_fds){
    close(pipe_fds[0]);
    bullet_right->extra = 1;
    // writes the new position until exiting the screen
    while (bullet_right->x < GAME_WIDTH + 1) {
        bullet_right->x += 1;
        write(pipe_fds[1], bullet_right, sizeof(Item));
        usleep(current_difficulty.bullets_speed);
    }
    _exit(0);
}


/**
 * @brief  function that manages the left bullet movement
 * @param  bullet_left: bullet item
 * @param  pipe_fds: pipe file descriptors
 */
void bullet_left_controller(Item *bullet_left, int *pipe_fds){
    close(pipe_fds[0]);
    bullet_left->extra = -1;
    // writes the new position until exiting the screen
    while (bullet_left->x > -1){
        bullet_left->x -= 1;
        write(pipe_fds[1], bullet_left, sizeof(Item));
        usleep(current_difficulty.bullets_speed);
    }
    _exit(0);
}
