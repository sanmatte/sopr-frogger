#include <locale.h>
#include "design.h"



void print_background(){
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	attron(COLOR_PAIR(4));  
	for (int y = SCORE_HEIGHT; y < DENS_HEIGHT ; y++) {
		mvhline(y, 0, ' ', max_x);
	}
	attroff(COLOR_PAIR(4)); 

	attron(COLOR_PAIR(2));  
	for (int y = DENS_HEIGHT; y < SIDEWALK_HEIGHT_1; y++) {
		mvhline(y, 0, ' ', max_x);
	}
	attroff(COLOR_PAIR(2)); 

	attron(COLOR_PAIR(3));  
	for (int y = SIDEWALK_HEIGHT_1; y <= SIDEWALK_HEIGHT_2; y++) {
		mvhline(y, 0, ' ', max_x);
	}
	attroff(COLOR_PAIR(3));
	
	attron(COLOR_PAIR(2));  
	for (int y = max_y-1; y > SIDEWALK_HEIGHT_2; y--) {
		mvhline(y, 0, ' ', max_x);
	}
	attroff(COLOR_PAIR(2));
}

void print_frog(Item *frog){
	static const char* sprite_matrix[FROG_DIM_Y][FROG_DIM_X] = {
        {"▄", "█", "", "▀", "▌", "▐", "▀", "", "█", "▄"},
        {"", "▀", "▄", " ", "▄", " ", " ", "▄", "▀", ""},
        {"", "", "▄", "█", "▄", "▀", "▄", "▄", "", ""},
        {"▀", "█", "▀", "", "", "", "", "▀", "█", "▀"},
	};
    attron(COLOR_PAIR(1));  
    for (int i = 0; i < FROG_DIM_Y; i++) {
        for (int j = 0; j < FROG_DIM_X; j++) {
            mvprintw(frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
        }
    }
    attroff(COLOR_PAIR(1)); 
}

void print_crocodile(Item *crocodile){
	static const char* sprite_matrix[CROCODILE_DIM_Y][CROCODILE_DIM_X] = {
        {"", "", "", "", "▀", "▀", "▀", "█", "█", "█", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "█", "█", "▀", "▀", "▀", "", "", ""},
        {"▄", "▄", "▄", "▄", " ", "▄", " ", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", " ", "▀", "█", "▄", "▄", "▄"},
        {"▀", "▀", "▀", "▀", " ", "▀", " ", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", " ", "▄", "█", "▀", "▀", "▀"},
        {"", "", "", "", "▄", "▄", "▄", "█", "█", "█", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "█", "█", "▄", "▄", "▄", "", "", ""},
    };
	attron(COLOR_PAIR(4));  
	for (int i = 0; i < CROCODILE_DIM_Y; i++) {
		for (int j = 0; j < CROCODILE_DIM_X; j++) {
			mvprintw(crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
		}
	}
	attroff(COLOR_PAIR(4));
}

void print_bullets(Item *bullets){
	static const char* sprite_matrix[BULLETS_DIM] = {"▄"};
	attron(COLOR_PAIR(5));
	for (int i = 0; i < BULLETS_DIM; i++) {
		mvprintw(bullets->y+i, bullets->x, "%s", sprite_matrix[i]);
	}
	attroff(COLOR_PAIR(5));
}
	

