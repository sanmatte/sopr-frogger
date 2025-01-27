#include <locale.h>
#include "design.h"
#include "frog.h"
#include "crocodile.h"

void print_score(WINDOW *game, int manche, int timer, int score){
	//lives counter
	mvwprintw(game, 0, 1, "                  ");
	wattron(game, COLOR_PAIR(6));
	switch(manche){
		case 3:
			mvwprintw(game, 0, 1, "Lives: %s", "♥ ♥ ♥");
			break;
		case 2:
			mvwprintw(game, 0, 1, "Lives: %s", "♥ ♥");
			break;
		case 1:
			mvwprintw(game, 0, 1, "Lives: %s", "♥");
			break;
		case 0:
			mvwprintw(game, 0, 1, "Lives: %s", "");
			break;
		default:
			break;
	}
	wattroff(game, COLOR_PAIR(2));
	
	//score counter
	wattron(game, COLOR_PAIR(11));
	mvwprintw(game,0, GAME_WIDTH/2-20, "                  ");
	mvwprintw(game, 0, GAME_WIDTH/2-20, "Score: %d", score);
	wattroff(game, COLOR_PAIR(11));

	//timer
	for(int i = 0; i < 60; i++){
		mvwprintw(game, 0, GAME_WIDTH-60+i, " ");
	}

	wattron(game, COLOR_PAIR(1));
	for(int i = GAME_WIDTH-1; i > GAME_WIDTH-1-timer; i--){
		mvwprintw(game, 0, i, "█");
	}
	wattroff(game, COLOR_PAIR(1));
}

void print_background(WINDOW *game, bool *dens){

	for (int y = SCORE_HEIGHT; y < DENS_HEIGHT ; y++) {
		wattron(game, COLOR_PAIR(8));  
		mvwhline(game, y, 0, ' ', GAME_WIDTH);
		wattroff(game, COLOR_PAIR(8));
		for(int i = 0; i < DENS_NUMBER; i++){
			if(dens[i] == TRUE){
				switch(i){
					case 0:
						for (int j = DENS_1 ; j < DENS_1 + DENS_DIM; j++){
							for (int h=2; h < DENS_HEIGHT; h++){
								wattron(game, COLOR_PAIR(10));
								mvwprintw(game, h, j, " ");
								wattroff(game, COLOR_PAIR(10));
							}
						}
						break;
					case 1:
						for (int j = DENS_2 ; j < DENS_2 + DENS_DIM; j++){
							for (int h=2; h < DENS_HEIGHT; h++){
								wattron(game, COLOR_PAIR(10));
								mvwprintw(game, h, j, " ");
								wattroff(game, COLOR_PAIR(10));
							}
						}
						break;
					case 2:
						for (int j = DENS_3 ; j < DENS_3 + DENS_DIM; j++){
							for (int h=2; h < DENS_HEIGHT; h++){
								wattron(game, COLOR_PAIR(10));
								mvwprintw(game, h, j, " ");
								wattroff(game, COLOR_PAIR(10));
							}
						}
						break;
					case 3:
						for (int j = DENS_4 ; j < DENS_4 + DENS_DIM; j++){
							for (int h=2; h < DENS_HEIGHT; h++){
								wattron(game, COLOR_PAIR(10));
								mvwprintw(game, h, j, " ");
								wattroff(game, COLOR_PAIR(10));
							}
						}
						break;
					case 4:
						for (int j = DENS_5 ; j < DENS_5 + DENS_DIM; j++){
							for (int h=2; h < 6; h++){
								wattron(game, COLOR_PAIR(10));
								mvwprintw(game, h, j, " ");
								wattroff(game, COLOR_PAIR(10));
							}
						}
						break;
				}
			}
		}
	}
	 

	wattron(game, COLOR_PAIR(10));  
	for (int y = DENS_HEIGHT; y < SIDEWALK_HEIGHT_1; y++) {
		mvwhline(game, y, 0, ' ', GAME_WIDTH);
	}
	wattroff(game, COLOR_PAIR(10)); 

	wattron(game, COLOR_PAIR(3));  
	for (int y = SIDEWALK_HEIGHT_1; y <= SIDEWALK_HEIGHT_2; y++) {
		mvwhline(game, y, 0, ' ', GAME_WIDTH);
	}
	wattroff(game, COLOR_PAIR(3));
	
	wattron(game, COLOR_PAIR(10));  
	for (int y = GAME_HEIGHT-1; y > SIDEWALK_HEIGHT_2; y--) {
		mvwhline(game, y, 0, ' ', GAME_WIDTH);
	}
	wattroff(game, COLOR_PAIR(10));
}

void print_frog(WINDOW *game, Item *frog){
	static const char* sprite_matrix[FROG_DIM_Y][FROG_DIM_X] = {
        {"▄", "█", "", "▀", "▌", "▐", "▀", "", "█", "▄"},
        {"", "▀", "▄", " ", "▄", " ", " ", "▄", "▀", ""},
        {"", "", "▄", "█", "▄", "▀", "▄", "▄", "", ""},
        {"▀", "█", "▀", "", "", "", "", "▀", "█", "▀"},
	};
    
    for (int i = 0; i < FROG_DIM_Y; i++) {
        for (int j = 0; j < FROG_DIM_X; j++) {
			if(i == 0 && (j > 2 && j < 7)){
				wattron(game, COLOR_PAIR(9));
				mvwprintw(game, frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(9));
			}
			else{
				wattron(game, COLOR_PAIR(1));  
				mvwprintw(game, frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(1)); 
			}
		}
        }
    }
	

void print_crocodile_right(WINDOW *game, Item *crocodile){
	static const char* sprite_matrix[CROCODILE_DIM_Y][CROCODILE_DIM_X] = {
        {"", "", "", "", "▀", "▀", "▀", "█", "█", "█", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "█", "█", "▀", "▀", "▀", "", "", ""},
        {"▄", "▄", "▄", "▄", " ", "▄", " ", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", " ", "▀", "█", "▄", "▄", "▄"},
        {"▀", "▀", "▀", "▀", " ", "▀", " ", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", " ", "▄", "█", "▀", "▀", "▀"},
        {"", "", "", "", "▄", "▄", "▄", "█", "█", "█", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "█", "█", "▄", "▄", "▄", "", "", ""},
    };
	
	for (int i = 0; i < CROCODILE_DIM_Y; i++) {
		for (int j = 0; j < CROCODILE_DIM_X; j++) {
			if((i == 1 || i == 2) && (j > 5 && j < 24)){
				wattron(game, COLOR_PAIR(7));
				mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(7));
			}
			else{
				wattron(game, COLOR_PAIR(4));  
				mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(4));
			}
		}
	}
}

void print_death(WINDOW *game){
// 	static const char sprite_matrix[21][39] = {
// 		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '█', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '█', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', '█', '█', '█', '█', '█', '█', '█', '█', '█', '█', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '█', '█', '█', '█', '█', '█', '█', '█', '█', '█', ' ', ' ', ' ', ' '},
// {' ', ' ', '█', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', ' ', ' ', ' ', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█', '█', ' ', ' '},
// {' ', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', ' ', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█', ' '},
// {'█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█'},
// {'█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█'},
// {'█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█'},
// {'█', '█', '▓', '▓', '▓', '▓', '▓', '─', '─', '─', '█', '█', '█', '█', '▄', '─', '█', '█', '█', '─', '█', '█', '█', '█', '─', '█', '█', '█', '█', '█', '─', '─', '▓', '▓', '▓', '▓', '▓', '█', '█'},
// {'█', '█', '█', '▓', '▓', '▓', '▓', '─', '─', '─', '█', '█', '─', '─', '█', '─', '─', '█', '─', '─', '█', '─', '─', '─', '─', '█', '─', '─', '─', '─', '─', '─', '▓', '▓', '▓', '▓', '█', '█', '█'},
// {'█', '█', '█', '▓', '▓', '▓', '▓', '─', '─', '─', '█', '█', '─', '─', '█', '─', '─', '█', '─', '─', '█', '█', '█', '█', '─', '█', '█', '█', '█', '█', '─', '─', '▓', '▓', '▓', '▓', '█', '█', '█'},
// {' ', '█', '█', '█', '▓', '▓', '▓', '─', '─', '─', '█', '█', '─', '─', '█', '─', '─', '█', '─', '─', '█', '─', '─', '─', '─', '─', '─', '─', '─', '█', '─', '─', '▓', '▓', '▓', '▓', '█', '█', ' '},
// {' ', ' ', '█', '█', '█', '█', '▓', '─', '─', '─', '█', '█', '█', '█', '▀', '─', '█', '█', '█', '─', '█', '█', '█', '█', '─', '█', '█', '█', '█', '█', '─', '─', '▓', '█', '█', '█', '█', ' ', ' '},
// {' ', ' ', ' ', '█', '█', '█', '▓', '▓', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', '─', ' ', '▓', '▓', '▓', '█', '█', '█', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', '█', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█', '█', ' ', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', ' ', '█', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█', '█', ' ', ' ', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '█', '▓', '▓', '▓', '▓', '▓', '▓', '▓', '█', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '█', '▓', '▓', '▓', '█', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '▓', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
// {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '█', '█', '█', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
// 	};
static const char* sprite_matrix[21][39] = {{"", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", ""}, {"", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", ""}, {"", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", ""}, {"", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", ""}, {"█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█"}, {"█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█"}, {"█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█"}, {"█", "█", "█", "█", "█", "█", "█", "", "", "", "█", "█", "█", "█", "▄", "", "█", "█", "█", "", "█", "█", "█", "█", "", "█", "█", "█", "█", "█", "", "", "█", "█", "█", "█", "█", "█", "█"}, {"█", "█", "█", "█", "█", "█", "█", "", "", "", "█", "█", "", "", "█", "", "", "█", "", "", "█", "", "", "", "", "█", "", "", "", "", "", "", "█", "█", "█", "█", "█", "█", "█"}, {"█", "█", "█", "█", "█", "█", "█", "", "", "", "█", "█", "", "", "█", "", "", "█", "", "", "█", "█", "█", "█", "", "█", "█", "█", "█", "█", "", "", "█", "█", "█", "█", "█", "█", "█"}, {"", "█", "█", "█", "█", "█", "█", "", "", "", "█", "█", "", "", "█", "", "", "█", "", "", "█", "", "", "", "", "", "", "", "", "█", "", "", "█", "█", "█", "█", "█", "█", ""}, {"", "", "█", "█", "█", "█", "█", "", "", "", "█", "█", "█", "█", "▀", "", "█", "█", "█", "", "█", "█", "█", "█", "", "█", "█", "█", "█", "█", "", "", "█", "█", "█", "█", "█", "", ""}, {"", "", "", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "█", "", "", ""}, {"", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", ""}, {"", "", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", "", ""}, {"", "", "", "", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", "", "", "", ""}, {"", "", "", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", "", "", ""}, {"", "", "", "", "", "", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", "", "", "", "", "", ""}, {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""}, {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "█", "█", "█", "█", "█", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""}, {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "█", "█", "█", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""}};


    // Calculate top-left position
    int start_y = (GAME_HEIGHT - 21) / 2;
    int start_x = (GAME_WIDTH - 39) / 2;

    // Print sprite line by line
    for (int i = 0; i < 21; i++) {
		for (size_t j = 0; j < 39; j++)
		{
			wattron(game, COLOR_PAIR(7));
			mvwprintw(game, start_y + i, start_x+j, "%s", sprite_matrix[i][j]);
			wattroff(game, COLOR_PAIR(7));
		}
		
        
    }

}

void print_crocodile_left(WINDOW *game, Item *crocodile){
		static const char* sprite_matrix[CROCODILE_DIM_Y][CROCODILE_DIM_X] = {
				{"", "", "", "", "▀", "▀", "▀", "█", "█", "█", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "█", "█", "▀", "▀", "▀", "", "", ""},
				{"▄", "▄", "▄", "█", "▀", " ", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", " ", "▄"," ","▄", "▄", "▄", "▄"},
				{"▀", "▀", "▀", "█", "▄", " ", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", " ", "▀"," ","▀", "▀", "▀", "▀"},
				{"", "", "", "", "▄", "▄", "▄", "█", "█", "█", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "█", "█", "▄", "▄", "▄", "", "", ""},
			};

	
	for (int i = 0; i < CROCODILE_DIM_Y; i++) {
		for (int j = 0; j < CROCODILE_DIM_X; j++) {
			if((i == 1 || i == 2) && (j > 5 && j < 24)){
				wattron(game, COLOR_PAIR(7));
				mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(7));
			}
			else{
				wattron(game, COLOR_PAIR(4));  
				mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(4));
			}
		}
	}
}

void print_bullets(WINDOW *game, Item *bullets){
	static const char* sprite_matrix[BULLETS_DIM] = {"▄"};
	wattron(game, COLOR_PAIR(5));
	for (int i = 0; i < BULLETS_DIM; i++) {
		mvwprintw(game, bullets->y+i, bullets->x, "%s", sprite_matrix[i]);
	}
	wattroff(game, COLOR_PAIR(5));
}
	

