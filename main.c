#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>
#include "design.h"
#include "struct.h"
#include "utils.h"
#include "crocodile.h"
#include "frog.h"
#include "menu.h"
#include "game.h"

// Menu options
char *menu_options[] = {
    "GIOCA",
    "Seleziona Difficoltà",
    "Regole di Gioco",
    "Esci"
};

char *difficulties[] = {
    "Facile",
    "Medio",
    "Difficile",
    "Torna al menu"
};

const Difficulty EASY = {
    .bullets_speed = 30000,
    .crocodile_speed_max = 250000,
    .crocodile_speed_min = 150000,
    .crocodile_bullet_speed = 30000,
    .shot_range = 100,
    .frog_movement_limit = 0,
    .river_color = 21,
    .shotload_time = 1500000
};
const Difficulty MEDIUM = {
    .bullets_speed = 40000,
    .crocodile_speed_max = 200000,
    .crocodile_speed_min = 100000,
    .crocodile_bullet_speed = 40000,
    .shot_range = 20,
    .frog_movement_limit = 0,
    .river_color = 3,
    .shotload_time = 1200000
};

const Difficulty HARD = {
    .bullets_speed = 40000,
    .crocodile_speed_max = 100000,
    .crocodile_speed_min = 30000,
    .crocodile_bullet_speed = 20000,
    .shot_range = 50,
    .frog_movement_limit = 50000,
    .river_color = 22,
    .shotload_time = 900000
};

extern Difficulty current_difficulty;


Difficulty current_difficulty;


int main(){
    setlocale(LC_ALL, "");
    initscr();start_colors();curs_set(0);keypad(stdscr, TRUE);noecho();cbreak();nodelay(stdscr, TRUE);srand(time(NULL));
    
    current_difficulty = MEDIUM;

    if(LINES < GAME_HEIGHT || COLS < GAME_WIDTH){
        WINDOW *err_screen = newwin(0, 0, 0, 0);
        while(LINES < GAME_HEIGHT || COLS < GAME_WIDTH){
            mvwprintw(err_screen, 1, 3, "La finestra è troppo piccola per giocare");
            mvwprintw(err_screen, 3, 3, "Dimensione minima: %d x %d", GAME_WIDTH, GAME_HEIGHT);
            mvwprintw(err_screen, 5, 3, "Dimensione attuale: %d x %d", COLS, LINES);
            wrefresh(err_screen);
        }
        wclear(err_screen);
        wrefresh(err_screen);
        delwin(err_screen);
    }
    
    // finestra di gioco centrata
    WINDOW *win = newwin(GAME_HEIGHT, GAME_WIDTH, (LINES - GAME_HEIGHT)/2, (COLS - GAME_WIDTH)/2);

    box(win, 0, 0);
    wrefresh(win);
    keypad(win, TRUE);

    int return_to_menu = 0;

    do{
        return_to_menu = 0;
        print_frogger_sprite(win);
        int selection = showMenu(win, menu_options);

        switch (selection)
        {
        case 0: //play
            startGame(win);
            return_to_menu = 1;
            werase(win);
            wrefresh(win);
            //! TOFIX
            break;
        case 1:
            selection = showMenu(win, difficulties);
            switch (selection) {
            case 0:
                current_difficulty = EASY;
                break;
            case 1:
                current_difficulty = MEDIUM;
                break;
            case 2:
                current_difficulty = HARD;
                break;
            case 3: // return to menu
                break;
            }
            return_to_menu = 1;
            break;
        case 2:
            print_rules(win);
            werase(win);
            return_to_menu = 1;
            break;
        case 3:
            printkitten(win);
            break;
        }
    }while (return_to_menu);
    endwin();
    return 0;

}
