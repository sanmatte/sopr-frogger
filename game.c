#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>

#include "design.h"

int manche = 3;

void Frog(int *pipe_fds, Item *frog, Item *bullet_left, Item *bullet_right) {
    close(pipe_fds[0]);  
    int max_y, max_x, ch;
    int projectile_active = 0;
    pid_t bullet_pid_right = -1;
    pid_t bullet_pid_left = -1;

    getmaxyx(stdscr, max_y, max_x);

    while (manche > 0) {
        ch = getch(); 
        switch (ch) {
            case KEY_UP:
                if (frog->y > SCORE_HEIGHT + 1) frog->y -= FROG_DIM_Y;
                break;
            case KEY_DOWN:
                if (frog->y < max_y - FROG_DIM_Y) frog->y += FROG_DIM_Y;
                break;
            case KEY_LEFT:
                if (frog->x > 0) frog->x -= 1;
                break;
            case KEY_RIGHT:
                if (frog->x < max_x - FROG_DIM_X) frog->x += 1;
                break;
            case ' ':
                if (projectile_active == 0) {
                    projectile_active = 1;

                    // Processo per il proiettile a destra
                    bullet_pid_right = fork();
                    if (bullet_pid_right == 0) {
                        bullet_right->x = frog->x + FROG_DIM_X;
                        bullet_right->y = frog->y+1;

                        while (bullet_right->x < max_x) {
                            bullet_right->x += 1;
                            if (pipe_fds != NULL) {
                                write(pipe_fds[1], bullet_right, sizeof(Item));
                            }
                            usleep(BULLETS_SPEED);
                        }
                        exit(0);
                    }

                    // Processo per il proiettile a sinistra
                    bullet_pid_left = fork();
                    if (bullet_pid_left == 0) {
                        bullet_left->x = frog->x - 1;
                        bullet_left->y = frog->y + 1;
                        while (bullet_left->x > -1) {
                            bullet_left->x -= 1;
                            if (pipe_fds != NULL) {
                                write(pipe_fds[1], bullet_left, sizeof(Item));
                            }
                            usleep(BULLETS_SPEED);  
                        }
                        exit(0);
                    }
                }
                break;
            case 'q': 
                endwin();
                return;
        }

        // Controlla se i proiettili sono terminati
        if (bullet_pid_right > 0) {
            int status;
            pid_t result = waitpid(bullet_pid_right, &status, WNOHANG);
            if (result == bullet_pid_right) {
                bullet_pid_right = -1;
            }
        }

        if (bullet_pid_left > 0) {
            int status;
            pid_t result = waitpid(bullet_pid_left, &status, WNOHANG);
            if (result == bullet_pid_left) {
                bullet_pid_left = -1;
            }
        }

        // Resetta projectile_active se entrambi i proiettili sono terminati
        if (bullet_pid_right == -1 && bullet_pid_left == -1) {
            projectile_active = 0;
        }

        // Scrive la posizione della rana nella pipe
        if (pipe_fds != NULL) {
            write(pipe_fds[1], frog, sizeof(Item));
        }
    }
}

void Crocodile(int *pipe_fds, Item *crocodile, int random_number){
    close(pipe_fds[0]);
    while(manche>0){
        if (random_number == 1 && crocodile->x < COLS) {
            crocodile->x += 1;
        }
        else if (random_number == 0 && crocodile->x > CROCODILE_DIM_X * -1) {
            crocodile->x -= 1;
        }
        if (pipe_fds != NULL) {
            write(pipe_fds[1], crocodile, sizeof(Item));
        }
        usleep(CROCODILE_SPEED_MAX);
    }
}

void ReadAndPrint(int *pipe_fds, Item *frog, Item *crocodiles, Item *bullets_left, Item *bullets_right) {
    close(pipe_fds[1]);
    Item msg;

    while (manche > 0) {
        if (read(pipe_fds[0], &msg, sizeof(Item)) > 0) {
            if (msg.ID == FROG_ID) {
                *frog = msg;
            } else if (msg.ID == CROCODILE_ID) {
                for (int i = 0; i < CROCODILE_MAX_NUMBER; i++) {
                    if (msg.y == crocodiles[i].y) {
                        crocodiles[i] = msg;
                        break;
                    }
                }
            } else if (msg.ID == BULLETS_ID) {
                if (msg.x > frog->x) {
                    *bullets_right = msg;
                } else {
                    *bullets_left = msg;
                }
            }

            print_background();
            // Stampa tutti i coccodrilli
            for (int i = 0; i < CROCODILE_MAX_NUMBER; i++) {
                print_crocodile(&crocodiles[i]);
            }
            print_frog(frog);
            print_bullets(bullets_left);
            print_bullets(bullets_right);
            
            refresh();
        }
    }
}

int main(){
    setlocale(LC_ALL, "");
    initscr();start_color();curs_set(0);keypad(stdscr, TRUE);noecho();cbreak();nodelay(stdscr, TRUE);

    Item frog = {FROG_ID, LINES-4, 0};
    Item crocodiles[CROCODILE_MAX_NUMBER];
    for (int i = 0; i < CROCODILE_MAX_NUMBER; i++) {
        crocodiles[i].ID = CROCODILE_ID;
        crocodiles[i].y = SIDEWALK_HEIGHT_2 - CROCODILE_DIM_Y + 1 - (i * CROCODILE_DIM_Y);  
        if (i % 2 == 0) {
            crocodiles[i].x = -CROCODILE_DIM_X;  // Da sinistra verso destra
        } else {
            crocodiles[i].x = COLS - 1;         // Da destra verso sinistra
        }
    }
    Item bullet_right = {BULLETS_ID, -1, -1};
    Item bullet_left = {BULLETS_ID, -1, -1};

    init_color(COLOR_DARKGREEN, 0, 400, 0);
    init_color(COLOR_GREY, 600, 600, 600);

    // Definizione delle coppie di colori
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_GREY);  
    init_pair(3, COLOR_BLACK, COLOR_BLUE);
    init_pair(4, COLOR_DARKGREEN, COLOR_DARKGREEN);
    init_pair(5, COLOR_RED, COLOR_RED);

    // Definizione variabili
    int pipe_fds[2], direction = rand() % 2;
    
    if(pipe(pipe_fds) != 0){
        exit(0);
    }
    
    pid_t child_pids[CROCODILE_MAX_NUMBER + 1];

    // Creazione del processo Frog
    pid_t pid_frog = fork();
    if (pid_frog < 0) {
        perror("Errore nella fork");
        endwin();
        exit(EXIT_FAILURE);
    } else if (pid_frog == 0) {
        Frog(pipe_fds, &frog, &bullet_left, &bullet_right);
        exit(0);
    } else {
        child_pids[0] = pid_frog;
    }

    // Creazione dei processi Crocodile
    for (int i = 0; i < CROCODILE_MAX_NUMBER; i++) {
        pid_t pid_croc = fork();
        if (pid_croc < 0) {
            perror("Errore nella fork del coccodrillo");
            endwin();
            exit(EXIT_FAILURE);
        } else if (pid_croc == 0) {
            direction = (i % 2 == 0) ? 1 : 0;
            Crocodile(pipe_fds, &crocodiles[i], direction);
            exit(0);
        } else {
            child_pids[i + 1] = pid_croc;
        }
    }

    ReadAndPrint(pipe_fds, &frog, crocodiles, &bullet_left, &bullet_right);

    for (int i = 0; i < CROCODILE_MAX_NUMBER + 1; i++) {
        kill(child_pids[i], SIGTERM);  // Invia il segnale di terminazione
    }

    for (int i = 0; i < CROCODILE_MAX_NUMBER + 1; i++) {
        waitpid(child_pids[i], NULL, 0);
    }
    endwin();
    return 0;
}