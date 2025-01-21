#include <locale.h>
#include "design.h"
#include "frog.h"
#include "crocodile.h"

void print_score(int manche, int timer){
	//lives counter
	mvprintw(0, 1, "                  ");
	attron(COLOR_PAIR(6));
	switch(manche){
		case 3:
			mvprintw(0, 1, "Lives: %s", "♥ ♥ ♥");
			break;
		case 2:
			mvprintw(0, 1, "Lives: %s", "♥ ♥");
			break;
		case 1:
			mvprintw(0, 1, "Lives: %s", "♥");
			break;
		case 0:
			mvprintw(0, 1, "Lives: %s", "");
			break;
		default:
			break;
	}
	attroff(COLOR_PAIR(6));

	//score counter
	mvprintw(0, COLS/2-20, "                  ");
	mvprintw(0, COLS/2-20, "Score: %d", timer);

	//timer
	for(int i = 0; i < 60; i++){
		mvprintw(0, COLS-60+i, " ");
	}

	attron(COLOR_PAIR(1));
	for(int i = COLS-1; i > COLS-1-timer; i--){
		mvprintw(0, i, "█");
	}
	attroff(COLOR_PAIR(1));
}

void print_background(bool *dens){
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	
	for (int y = SCORE_HEIGHT; y < DENS_HEIGHT ; y++) {
		attron(COLOR_PAIR(8));  
		mvhline(y, 0, ' ', max_x);
		attroff(COLOR_PAIR(8));
		for(int i = 0; i < DENS_NUMBER; i++){
			if(dens[i] == TRUE){
				switch(i){
					case 0:
						for (int j = 10 ; j < 20; j++){
							for (int h=2; h < 6; h++){
								attron(COLOR_PAIR(10));
								mvprintw(h, j, " ");
								attroff(COLOR_PAIR(10));
							}
						}
						break;
					case 1:
						for (int j = 40 ; j < 50; j++){
							for (int h=2; h < 6; h++){
								attron(COLOR_PAIR(10));
								mvprintw(h, j, " ");
								attroff(COLOR_PAIR(10));
							}
						}
						break;
					case 2:
						for (int j = 70 ; j < 80; j++){
							for (int h=2; h < 6; h++){
								attron(COLOR_PAIR(10));
								mvprintw(h, j, " ");
								attroff(COLOR_PAIR(10));
							}
						}
						break;
					case 3:
						for (int j = 100 ; j < 110; j++){
							for (int h=2; h < 6; h++){
								attron(COLOR_PAIR(10));
								mvprintw(h, j, " ");
								attroff(COLOR_PAIR(10));
							}
						}
						break;
					case 4:
						for (int j = 130 ; j < 140; j++){
							for (int h=2; h < 6; h++){
								attron(COLOR_PAIR(10));
								mvprintw(h, j, " ");
								attroff(COLOR_PAIR(10));
							}
						}
						break;
				}
			}
		}
	}
	 

	attron(COLOR_PAIR(10));  
	for (int y = DENS_HEIGHT; y < SIDEWALK_HEIGHT_1; y++) {
		mvhline(y, 0, ' ', max_x);
	}
	attroff(COLOR_PAIR(10)); 

	attron(COLOR_PAIR(3));  
	for (int y = SIDEWALK_HEIGHT_1; y <= SIDEWALK_HEIGHT_2; y++) {
		mvhline(y, 0, ' ', max_x);
	}
	attroff(COLOR_PAIR(3));
	
	attron(COLOR_PAIR(10));  
	for (int y = max_y-1; y > SIDEWALK_HEIGHT_2; y--) {
		mvhline(y, 0, ' ', max_x);
	}
	attroff(COLOR_PAIR(10));
}

void print_frog(Item *frog){
	static const char* sprite_matrix[FROG_DIM_Y][FROG_DIM_X] = {
        {"▄", "█", "", "▀", "▌", "▐", "▀", "", "█", "▄"},
        {"", "▀", "▄", " ", "▄", " ", " ", "▄", "▀", ""},
        {"", "", "▄", "█", "▄", "▀", "▄", "▄", "", ""},
        {"▀", "█", "▀", "", "", "", "", "▀", "█", "▀"},
	};
    
    for (int i = 0; i < FROG_DIM_Y; i++) {
        for (int j = 0; j < FROG_DIM_X; j++) {
			if(i == 0 && (j > 2 && j < 7)){
				attron(COLOR_PAIR(9));
				mvprintw(frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				attroff(COLOR_PAIR(9));
			}
			else{
				attron(COLOR_PAIR(1));  
				mvprintw(frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				attroff(COLOR_PAIR(1)); 
			}
		}
        }
    }

void print_crocodile_right(Item *crocodile){
	static const char* sprite_matrix[CROCODILE_DIM_Y][CROCODILE_DIM_X] = {
        {"", "", "", "", "▀", "▀", "▀", "█", "█", "█", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "█", "█", "▀", "▀", "▀", "", "", ""},
        {"▄", "▄", "▄", "▄", " ", "▄", " ", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", " ", "▀", "█", "▄", "▄", "▄"},
        {"▀", "▀", "▀", "▀", " ", "▀", " ", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", " ", "▄", "█", "▀", "▀", "▀"},
        {"", "", "", "", "▄", "▄", "▄", "█", "█", "█", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "█", "█", "▄", "▄", "▄", "", "", ""},
    };
	
	for (int i = 0; i < CROCODILE_DIM_Y; i++) {
		for (int j = 0; j < CROCODILE_DIM_X; j++) {
			if((i == 1 || i == 2) && (j > 5 && j < 24)){
				attron(COLOR_PAIR(7));
				mvprintw(crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
				attroff(COLOR_PAIR(7));
			}
			else{
				attron(COLOR_PAIR(4));  
				mvprintw(crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
				attroff(COLOR_PAIR(4));
			}
		}
	}
}

void print_crocodile_left(Item *crocodile){
		static const char* sprite_matrix[CROCODILE_DIM_Y][CROCODILE_DIM_X] = {
				{"", "", "", "", "▀", "▀", "▀", "█", "█", "█", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "█", "█", "▀", "▀", "▀", "", "", ""},
				{"▄", "▄", "▄", "█", "▀", " ", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", " ", "▄"," ","▄", "▄", "▄", "▄"},
				{"▀", "▀", "▀", "█", "▄", " ", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", " ", "▀"," ","▀", "▀", "▀", "▀"},
				{"", "", "", "", "▄", "▄", "▄", "█", "█", "█", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "█", "█", "▄", "▄", "▄", "", "", ""},
			};

	
	for (int i = 0; i < CROCODILE_DIM_Y; i++) {
		for (int j = 0; j < CROCODILE_DIM_X; j++) {
			if((i == 1 || i == 2) && (j > 5 && j < 24)){
				attron(COLOR_PAIR(7));
				mvprintw(crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
				attroff(COLOR_PAIR(7));
			}
			else{
				attron(COLOR_PAIR(4));  
				mvprintw(crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
				attroff(COLOR_PAIR(4));
			}
		}
	}
}

void print_bullets(Item *bullets){
	static const char* sprite_matrix[BULLETS_DIM] = {"▄"};
	attron(COLOR_PAIR(5));
	for (int i = 0; i < BULLETS_DIM; i++) {
		mvprintw(bullets->y+i, bullets->x, "%s", sprite_matrix[i]);
	}
	attroff(COLOR_PAIR(5));
}
	

