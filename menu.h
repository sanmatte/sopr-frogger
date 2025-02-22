#ifndef MENU_H
#define MENU_H

#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>

int showMenu(WINDOW *win, char *menu_options[]);
void print_rules(WINDOW *win);
void printkitten(WINDOW *game);
#endif
