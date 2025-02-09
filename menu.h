#ifndef MENU_H
#define MENU_H

#include <ncurses.h>
#include <menu.h>

// Function prototype to display the menu and return the selected option
int showMenu(WINDOW *win, char *menu_options[]);
int pauseMenu(WINDOW *win);
void print_rules(WINDOW *win);
void printkitten(WINDOW *game);
#endif // MENU_H
