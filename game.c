#include <unistd.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <locale.h>

#include "design.h"

int manche = 3;

void Frog(int *pipe_fds, Item *frog){
    close(pipe_fds[0]);
    int max_y, max_x, ch;
    getmaxyx(stdscr, max_y, max_x);
    while(manche>0){
        ch = getch(); 
        switch (ch) {
            case KEY_UP:
                if (frog->y > SCORE_HEIGHT + 2) frog->y = frog->y-FROG_DIM_Y;
                break;
            case KEY_DOWN:
                if (frog->y < max_y - FROG_DIM_Y) frog->y = frog->y+FROG_DIM_Y;
                break;
            case KEY_LEFT:
                if (frog->x > 0) frog->x = frog->x-FROG_DIM_X;
                break;
            case KEY_RIGHT:
                if (frog->x < max_x - FROG_DIM_X * 2) frog->x = frog->x+FROG_DIM_X;
                break;
            case 'q': // Uscita con 'q'
                endwin();
                return;
        }
        if (pipe_fds != NULL) {
            write(pipe_fds[1], frog, sizeof(Item));
        }
    }
}

void ReadAndPrint(int *pipe_fds, Item *frog, Item *crocodile){
    close(pipe_fds[1]);
    Item msg;
    while (manche>0) {
        if (read(pipe_fds[0], &msg, sizeof(Item)) > 0) {
            clear();
            *frog = msg;
            print_background();
            print_crocodile(crocodile);
            print_frog(frog);
        }
        refresh();
    }
}

int main(){
    setlocale(LC_ALL, "");
    initscr();start_color();curs_set(0);keypad(stdscr, TRUE);

    Item frog = {FROG_ID, LINES-4, 0};
    Item crocodile = {CROCCODILE_ID, 34, 0};

    init_color(COLOR_DARKGREEN, 0, 400, 0);
    init_color(COLOR_GREY, 600, 600, 600);

    // Definizione delle coppie di colori
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_GREY);  
    init_pair(3, COLOR_BLACK, COLOR_BLUE);
    init_pair(4, COLOR_BLACK, COLOR_DARKGREEN);

    //define and intialize pipe
    int pipe_fds[2];
    if(pipe(pipe_fds) != 0){
        exit(0);
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        endwin();
        exit(2);
    }
    else if(pid == 0){
        Frog(pipe_fds, &frog);
    }
    else{
        ReadAndPrint(pipe_fds, &frog, &crocodile);
        wait(NULL);
    }
    endwin();
}