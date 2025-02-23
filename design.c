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
			mvwprintw(game, 0, 1, "LIFES: %s", "♥ ♥ ♥");
			break;
		case 2:
			mvwprintw(game, 0, 1, "LIFES: %s", "♥ ♥");
			break;
		case 1:
			mvwprintw(game, 0, 1, "LIFES: %s", "♥");
			break;
		case 0:
			mvwprintw(game, 0, 1, "LIFES: %s", "");
			break;
		default:
			break;
	}
	wattroff(game, COLOR_PAIR(2));
	
	//score counter
	switch(score){
		case 0 ... 49:
			wattron(game, COLOR_PAIR(6));
			mvwprintw(game,0, GAME_WIDTH/2-20, "                  ");
			mvwprintw(game, 0, GAME_WIDTH/2-20, "Score: %d", score);
			wattroff(game, COLOR_PAIR(6));
			break;
		case 50 ... 99:
			wattron(game, COLOR_PAIR(14));
			mvwprintw(game,0, GAME_WIDTH/2-20, "                  ");
			mvwprintw(game, 0, GAME_WIDTH/2-20, "Score: %d", score);
			wattroff(game, COLOR_PAIR(14));
			break;
		case 100 ... 150:
			wattron(game, COLOR_PAIR(16));
			mvwprintw(game,0, GAME_WIDTH/2-20, "                  ");
			mvwprintw(game, 0, GAME_WIDTH/2-20, "Score: %d", score);
			wattroff(game, COLOR_PAIR(16));
			break;
	}
	
	//timer
	// Cancella la riga del timer
	for(int i = 0; i < 60; i++){
		mvwprintw(game, 0, GAME_WIDTH-60+i, " ");
	}

	// Se il timer è sotto i 10 secondi, cambia colore e lampeggia
	if(timer <= 10){
		wattron(game, COLOR_PAIR(6) | A_BLINK);
	} else {
		wattron(game, COLOR_PAIR(1));
	}

	// Disegna il timer
	for(int i = GAME_WIDTH-1; i > GAME_WIDTH-1-timer; i--){
		mvwprintw(game, 0, i, "█");
	}

	// Spegne gli attributi
	wattroff(game, COLOR_PAIR(1));
	wattroff(game, COLOR_PAIR(6) | A_BLINK);
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

	wattron(game, COLOR_PAIR(current_difficulty.river_color));  
	for (int y = SIDEWALK_HEIGHT_1; y <= SIDEWALK_HEIGHT_2; y++) {
		mvwhline(game, y, 0, ' ', GAME_WIDTH);
	}
	wattroff(game, COLOR_PAIR(current_difficulty.river_color));
	
	wattron(game, COLOR_PAIR(10));  
	for (int y = GAME_HEIGHT-1; y > SIDEWALK_HEIGHT_2; y--) {
		mvwhline(game, y, 0, ' ', GAME_WIDTH);
	}
	wattroff(game, COLOR_PAIR(10));
}

void print_frog(WINDOW *game, Item *frog){
	static const char* sprite_matrix[FROG_DIM_Y][FROG_DIM_X] = {
        {"▄", "█", "", "▀", "▌", "▐", "▀", "", "█", "▄"},
        {"", "▀", "█", " ", "▄", "▀", "▄", "▄", "▀", ""},
        {"", "", "▄", "█", "▄", "▀", "▄", "▄", "", ""},
        {"▀", "█", "▀", "", "", "", "", "▀", "█", "▀"},
	};
    
    for (int i = 0; i < FROG_DIM_Y; i++) {
        for (int j = 0; j < FROG_DIM_X; j++) {
			if(i == 0 && (j > 2 && j < 7)){
				wattron(game, COLOR_PAIR(12));
				mvwprintw(game, frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(12));
			}
			else if(i == 1 && (j >= 2 && j <= 7)){
				wattron(game, COLOR_PAIR(20));
				mvwprintw(game, frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(20));
			}
			else if(i == 2 && (j >= 2 && j <= 7)){
				wattron(game, COLOR_PAIR(20));
				mvwprintw(game, frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(20));
			}
			else if(i == 3 && ((j >= 0  && j <= 2) || (j >= 7 && j <= 9))){
				wattron(game, COLOR_PAIR(20));
				mvwprintw(game, frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(20));
			}
			else{
				wattron(game, COLOR_PAIR(13));  
				mvwprintw(game, frog->y+i, frog->x+j, "%s", sprite_matrix[i][j]);
				wattroff(game, COLOR_PAIR(13)); 
			}
		}
    }
}
	
/**
 * @brief  function for printing the crocodile
 * @param  game window game
 * @param  crocodile crocodile object
 * @param  color_trigger trigger for the color change
 */
void print_crocodile(WINDOW *game, Item *crocodile, int color_trigger){
	// right crocodile sprite
	static const char* sprite_matrix_right[CROCODILE_DIM_Y][CROCODILE_DIM_X] = {
        {"", "", "", "", "▀", "▀", "▀", "█", "█", "█", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "█", "█", "▀", "▀", "▀", "", "", ""},
        {"▄", "▄", "▄", "▄", "▄", "▄", " ", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", " ", "▀", "█", "▄", "▄", "▄"},
        {"▀", "▀", "▀", "▀", "▀", "▀", " ", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", " ", "▄", "█", "▀", "▀", "▀"},
        {"", "", "", "", "▄", "▄", "▄", "█", "█", "█", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "█", "█", "▄", "▄", "▄", "", "", ""},
    };
	// left crocodile sprite
	static const char* sprite_matrix_left[CROCODILE_DIM_Y][CROCODILE_DIM_X] = {
		{"", "", "", "", "▀", "▀", "▀", "█", "█", "█", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "▄", "█", "█", "▀", "▀", "▀", "", "", ""},
		{"▄", "▄", "▄", "█", "▀", " ", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", " ", "▄","▄","▄", "▄", "▄", "▄"},
		{"▀", "▀", "▀", "█", "▄", " ", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", "▄", "▀", " ", "▀","▀","▀", "▀", "▀", "▀"},
		{"", "", "", "", "▄", "▄", "▄", "█", "█", "█", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "▀", "█", "█", "▄", "▄", "▄", "", "", ""},
	};

	// print of the left crocodile sprite
	if(crocodile->extra == -1){
		for (int i = 0; i < CROCODILE_DIM_Y; i++) {
			for (int j = 0; j < CROCODILE_DIM_X; j++) {
				// background color change based on the difficulty
				switch(current_difficulty.river_color){
					case 3:  // medium difficulty
						if((i == 1 || i == 2) && j == 4){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(27));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(27));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(28));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(28));
							}
						}
						else if((i == 1 || i == 2) && (j > 5 && j < 24)){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(7));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(7));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(19));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(19));
							}
						}
						else{
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(4));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(4));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(18));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(18));
							}
						}
						break;
					case 21:  // easy difficulty
						if((i == 1 || i == 2) && j == 4){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(27));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(27));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(28));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(28));
							}
						}
						else if((i == 1 || i == 2) && (j > 5 && j < 24)){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(7));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(7));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(19));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(19));
							}
						}
						else{
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(23));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(23));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(25));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(25));
							}
						}
						break;
					case 22:  // hard difficulty
						if((i == 1 || i == 2) && j == 4){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(27));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(27));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(28));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(28));
							}
						}
						else if((i == 1 || i == 2) && (j > 5 && j < 24)){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(7));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(7));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(19));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(19));
							}
						}
						else{
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(24));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(24));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(26));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_left[i][j]);
								wattroff(game, COLOR_PAIR(26));
							}
						}
						break;
				}
				
			}
		}
	}
	// print of the right crocodile sprite
	else{
		for (int i = 0; i < CROCODILE_DIM_Y; i++) {
			for (int j = 0; j < CROCODILE_DIM_X; j++) {
				// background color change based on the difficulty
				switch(current_difficulty.river_color){
					case 3:   // medium difficulty
						if((i == 1 || i == 2) && j == 25){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(27));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(27));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(28));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(28));
							}
						}
						else if((i == 1 || i == 2) && (j > 5 && j < 24)){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(7));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(7));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(19));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(19));
							}
						}
						else{
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(4));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(4));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(18));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(18));
							}
						}
						break;
					case 21:   // easy difficulty
						if((i == 1 || i == 2) && j == 25){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(27));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(27));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(28));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(28));
							}
						}
						else if((i == 1 || i == 2) && (j > 5 && j < 24)){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(7));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(7));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(19));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(19));
							}
						}
						else{
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(23));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(23));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(25));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(25));
							}
						}
						break;
					case 22:   // hard difficulty
						if((i == 1 || i == 2) && j == 25){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(27));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(27));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(28));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(28));
							}
						}
						else if((i == 1 || i == 2) && (j > 5 && j < 24)){
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(7));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(7));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(19));
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(19));
							}
						}
						else{
							if(color_trigger == 0){
								wattron(game, COLOR_PAIR(24));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(24));
							}
							else if (color_trigger == 1){
								wattron(game, COLOR_PAIR(26));  
								mvwprintw(game, crocodile->y+i, crocodile->x+j, "%s", sprite_matrix_right[i][j]);
								wattroff(game, COLOR_PAIR(26));
							}
						}
						break;
				}
			}
		}
	
	}
}

void print_bullets(WINDOW *game, Item *bullets, int itemtype){
	static const char* sprite_matrix[BULLETS_DIM] = {"█"};
	// print of the frog bullets
	if(itemtype == BULLET_ID_LEFT || itemtype == BULLET_ID_RIGHT){
		wattron(game, COLOR_PAIR(17));
		mvwprintw(game, bullets->y, bullets->x, "%s", sprite_matrix[0]);
		wattroff(game, COLOR_PAIR(17));
	}
	// print of the crocodile bullets
	else{
		wattron(game, COLOR_PAIR(5));
		for (int i = 0; i < BULLETS_DIM; i++) {
			mvwprintw(game, bullets->y+i, bullets->x, "%s", sprite_matrix[i]);
		}
		wattroff(game, COLOR_PAIR(5));
	}
}

void print_endgame(WINDOW *game, int manche, bool *dens, int score){
	static const char* sprite_matrix_lose[1][5] = {
		{
			"██╗   ██╗ ██████╗ ██╗   ██╗    ██╗      ██████╗ ███████╗███████╗",
			"╚██╗ ██╔╝██╔═══██╗██║   ██║    ██║     ██╔═══██╗██╔════╝██╔════╝",
			" ╚████╔╝ ██║   ██║██║   ██║    ██║     ██║   ██║███████╗█████╗  ",
			"  ╚██╔╝  ██║   ██║██║   ██║    ██║     ██║   ██║╚════██║██╔══╝  ",
			"   ██║   ╚██████╔╝╚██████╔╝    ███████╗╚██████╔╝███████╗███████╗"
		}
	};
	static const char* sprite_matrix_win[1][5] = {
		{
			"██╗   ██╗ ██████╗ ██╗   ██╗    ██╗    ██╗██╗███╗   ██╗",
			"╚██╗ ██╔╝██╔═══██╗██║   ██║    ██║    ██║██║████╗  ██║",
			" ╚████╔╝ ██║   ██║██║   ██║    ██║ █╗ ██║██║██╔██╗ ██║",
			"  ╚██╔╝  ██║   ██║██║   ██║    ██║███╗██║██║██║╚██╗██║",
			"   ██║   ╚██████╔╝╚██████╔╝    ╚███╔███╔╝██║██║ ╚████║"
		}
	};

	static const char* sprite_matrix_score[1][5] = {
		{
			"███████╗ ██████╗ ██████╗ ██████╗ ███████╗",
			"██╔════╝██╔════╝██╔═══██╗██╔══██╗██╔════╝",
			"███████╗██║     ██║   ██║██████╔╝█████╗",
			"╚════██║██║     ██║   ██║██╔══██╗██╔══╝  ",
			"███████║╚██████╗╚██████╔╝██║  ██║███████║"
		}
	};

	char *numbers[10][5] = {
    {
        " ██████╗  ", 
        "██╔═══██╗ ", 
        "██║   ██║ ", 
        "██╚═══██║ ", 
        "╚██████╔╝"
    }, // 0
    {
        "  ██╗ ", 
        " ███║ ", 
        " ╚██║ ", 
        "  ██║ ", 
        "  ██║ "
    }, // 1
    {
        "██████╗  ", 
        "╚════██╗ ", 
        " █████╔╝ ", 
        "██╔═══╝  ", 
        "███████╗ "
    }, // 2
    {
        "██████╗  ", 
        "╚════██╗ ", 
        " █████╔╝ ", 
        " ╚═══██╗ ", 
        "██████╔╝ "
    }, // 3
    {
        "██╗  ██╗ ", 
        "██║  ██║ ", 
        "███████║ ", 
        "╚════██║ ", 
        "     ██║ "
    }, // 4
    {
        "███████╗ ", 
        "██╔════╝ ", 
        "███████╗ ", 
        "╚════██║ ", 
        "███████║ "
    }, // 5
    {
        " ██████╗ ", 
        "██╔════╝ ", 
        "███████╗ ", 
        "██╔═══██╗", 
        "╚██████╔╝"
    }, // 6
    {
        "███████╗ ", 
        "╚════██║ ", 
        "    ██╔╝ ", 
        "   ██╔╝  ", 
        "   ██║   "
    }, // 7
    {
        " █████╗  ", 
        "██╔══██╗ ", 
        " █████╔╝ ", 
        "██╔══██╗ ", 
        "╚█████╔╝ "
    }, // 8
    {
        " ██████╗ ", 
        "██╔═══██╗", 
        " ███████║", 
        " ╚════██║", 
        " ██████╔╝"
    }  // 9
};

	


	//lose screen
	if(manche == 0){
		wattron(game, COLOR_PAIR(17));
		for(int i=0; i<GAME_HEIGHT; i++){
			mvwhline(game, i, 0, ' ', GAME_WIDTH);
		}
		wattroff(game, COLOR_PAIR(17));

		wattron(game, COLOR_PAIR(6));
		for(int i=0; i<5; i++){
			mvwprintw(game, (GAME_HEIGHT/2) + i - 5, GAME_WIDTH/2 - 30, "%s", sprite_matrix_lose[0][i]);
		}

		for(int i=0; i<5; i++){
			mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 - 30, "%s", sprite_matrix_score[0][i]);
		}

		if(score == 0){
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 20, "%s", numbers[0][i]);
			}
		}

		else if(score >= 10 && score < 100){
			int temp = score/10;
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 20, "%s", numbers[temp][i]);
			}
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 30, "%s", numbers[0][i]);
			}
		}

		else {
			int temp = (score/10)-10;
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 20, "%s", numbers[1][i]);
			}
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 30, "%s", numbers[temp][i]);
			}
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 40, "%s", numbers[0][i]);
			}
		}

		wattroff(game, COLOR_PAIR(6));

		wrefresh(game);
		sleep(3);
	}

	//win screen

	// Controlla se tutti i valori di dens[] sono FALSE
	bool all_false = true;
	for(int i = 0; i < 5; i++){
		if (dens[i] != FALSE) {
			all_false = false;
			break;
		}
	}

	// Se tutti i valori sono FALSE, stampa la schermata di vittoria
	if (all_false) {
		wattron(game, COLOR_PAIR(13));
		for(int i=0; i<GAME_HEIGHT; i++){
			mvwhline(game, i, 0, ' ', GAME_WIDTH);
		}
		wattroff(game, COLOR_PAIR(13));

		wattron(game, COLOR_PAIR(15));
		for(int i = 0; i < 5; i++) {
			mvwprintw(game, (GAME_HEIGHT/2) + i - 5, GAME_WIDTH/2 - 30, "%s", sprite_matrix_win[0][i]);
		}

		for(int i=0; i<5; i++){
			mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 - 30, "%s", sprite_matrix_score[0][i]);
		}

		if(score >= 10 && score < 100){
			int temp = score/10;
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 20, "%s", numbers[temp][i]);
			}
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 30, "%s", numbers[0][i]);
			}
		}

		else {
			int temp = (score/10)-10;
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 20, "%s", numbers[1][i]);
			}
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 30, "%s", numbers[temp][i]);
			}
			for(int i=0; i<5; i++){
				mvwprintw(game, (GAME_HEIGHT/2) + i + 2, GAME_WIDTH/2 + 40, "%s", numbers[0][i]);
			}
		}
		
		wattroff(game, COLOR_PAIR(15));
		wrefresh(game);
		sleep(5);
	}

}


void print_frogger_sprite(WINDOW *win){
    static const char* sprite_matrix_frogger[1][5] = {
        {
            "███████╗  ██████╗    ██████╗    ██████╗    ██████╗   ███████╗  ██████╗",
            "██╔════╝  ██╔══██╗  ██╔═══██╗  ██╔════╝   ██╔════╝   ██╔════╝  ██╔══██╗",
            "█████╗    ██████╔╝  ██║   ██║  ██║  ███╗  ██║  ███╗  █████╗    ██████╔╝",
            "██╔══╝    ██╔══██╗  ██║   ██║  ██║   ██║  ██║   ██║  ██╔══╝    ██╔══██╗",
            "██║       ██║  ██║  ╚██████╔╝  ╚██████╔╝  ╚██████╔╝  ███████╗  ██║  ██║"
        }
    };

    static const char* sprite_matrix_resurraction[1][5] = {
        {
            "██████╗  ███████╗ ███████╗ ██╗   ██╗ ██████╗  ██████╗  ███████╗  ██████╗ ╔██████████╗ ║██║   ██████╗  ███╗   ██╗",
            "██╔══██╗ ██╔════╝ ██╔════╝ ██║   ██║ ██╔══██╗ ██╔══██╗ ██╔════╝ ██╔════╝ ╚═══╗██╔═══╝ ║██║  ██╔═══██╗ ████╗  ██║",
            "██████╔╝ █████╗   ███████╗ ██║   ██║ ██████╔╝ ██████╔╝ █████╗   ██║          ║██║     ║██║  ██║   ██║ ██╔██╗ ██║",
            "██╔══██╗ ██╔══╝   ╚════██║ ██║   ██║ ██╔══██╗ ██╔══██╗ ██╔══╝   ██║          ║██║     ║██║  ██║   ██║ ██║╚██╗██║",
            "██║  ██║ ███████╗ ███████║ ╚██████╔╝ ██║  ██║ ██║  ██║ ███████╗ ╚██████╗     ║██║     ║██║  ╚██████╔╝ ██║ ╚████║"
        }
    };
    
    for(int i=0; i<5; i++){
        mvwprintw(win, (GAME_HEIGHT/2) + i - 7, (GAME_WIDTH/2) - 35, "%s", sprite_matrix_frogger[0][i]);
    }

    for(int i=0; i<5; i++){
        mvwprintw(win, (GAME_HEIGHT/2) + i, 20, "%s", sprite_matrix_resurraction[0][i]);
    }
}

void print_pause(WINDOW *pause, WINDOW *game){
	// print of the edges of the pause sign
	for(int i=0; i<25; i++){
		wattron(game, COLOR_PAIR(30));
		mvwprintw(game, (GAME_HEIGHT/2) -1, (GAME_WIDTH/2) - 12 + i, "▄");
		wattroff(game, COLOR_PAIR(30));
	}
	for(int i=0; i<25; i++){
		wattron(game, COLOR_PAIR(30));
		mvwprintw(game, (GAME_HEIGHT/2) + 5, (GAME_WIDTH/2) - 12 + i, "▀");
		wattroff(game, COLOR_PAIR(30));
	}
	for(int i=0; i<7; i++){
		wattron(game, COLOR_PAIR(30));
		mvwprintw(game, (GAME_HEIGHT/2) + i, (GAME_WIDTH/2) - 12, "█");
		wattroff(game, COLOR_PAIR(30));
	}
	for(int i=0; i<7; i++){
		wattron(game, COLOR_PAIR(30));
		mvwprintw(game, (GAME_HEIGHT/2) + i, (GAME_WIDTH/2) + 12, "█");
		wattroff(game, COLOR_PAIR(30));
	}
	// print of the pause sign
	wbkgd(pause, COLOR_PAIR(29));
	wattron(pause, COLOR_PAIR(29));
	mvwprintw(pause, 1, 1 , "Press P to resume");
	mvwprintw(pause, 2, 1 , "Press M to go to menu");
	mvwprintw(pause, 3, 1 , "Press Q to quit");
	wattroff(pause, COLOR_PAIR(29));
	wrefresh(game);
	wrefresh(pause);
}



