#include "menu.h"

#define NUM_OPTIONS 4

/**
 * @brief  Home menu function
 * @param  win start window 
 * @param  menu_options[] array of menu options
 */
int showMenu(WINDOW *win, char *menu_options[]) {
    ITEM *items[NUM_OPTIONS + 1]; // (+1 for NULL termination)
    MENU *menu;
    int choice, selected;
    // Create menu items
    for (int i = 0; i < NUM_OPTIONS; i++) {
        items[i] = new_item(menu_options[i], NULL);
    }
    items[NUM_OPTIONS] = NULL;

    // create menu
    menu = new_menu(items);
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, NUM_OPTIONS + 2, 30, 2, 5)); // suubwindow
    set_menu_mark(menu, " > ");
    keypad(win, TRUE);
    box(win, 0, 0);
    post_menu(menu);
    wrefresh(win);
    // menu navigation
    while ((choice = wgetch(win)) != 10) { // 10 is the enter key
        switch (choice) {
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                break;
        }
        wrefresh(win);
    }
    selected = item_index(current_item(menu));
    unpost_menu(menu);
    free_menu(menu);
    for (int i = 0; i < NUM_OPTIONS; i++) {
        free_item(items[i]);
    }
    return selected;
}


/**
 * @brief  Print the rules of the game
 * @param  win start window 
 */
void print_rules(WINDOW *win){
    werase(win); 
    box(win, 0, 0); 
    int y = 2; // Start position
    mvwprintw(win, y++, 2, "RULES:");
    mvwprintw(win, y++, 2, "1. Move the frog using arrow keys.");
    mvwprintw(win, y++, 2, "2. Reach the other side without getting hit.");
    mvwprintw(win, y++, 2, "3. Water and Bullets are dangerous!");
    mvwprintw(win, y++, 2, "4. You can ride on the crocodiles.");
    mvwprintw(win, y++, 2, "5. Press SPACE to shoot.");
    mvwprintw(win, y++, 2, "6. You can shoot only 2 bullets at a time.");
    mvwprintw(win, y++, 2, "7. Change the difficultty level from the main menu.");
    mvwprintw(win, y++, 2, "8. The higher the difficulty, the faster the enemies will be, and the slower you will be.");
    mvwprintw(win, y++, 2, "9. Everyone loves cats, right?.");
    mvwprintw(win, y++, 2, "10. Press 'q' to quit at any time to quit the game.");
    mvwprintw(win, y + 2, 2, "Press any key to return...");
    wrefresh(win); 
    wgetch(win);  
}


/**
 * @brief  Print kitten sprite
 * @param  game game window 
 */
void printkitten(WINDOW *game) {
    werase(game);  
    box(game, 0, 0);
    int start_y = 1; 
    int start_x = 2;
    mvwprintw(game, start_y++, start_x, "                                                                                                    ");
    mvwprintw(game, start_y++, start_x, "                                                                                                    ");
    mvwprintw(game, start_y++, start_x, "                                                                                                    ");
    mvwprintw(game, start_y++, start_x, "                                                                                                    ");
    mvwprintw(game, start_y++, start_x, "                                                                       ..                           ");
    mvwprintw(game, start_y++, start_x, "                                                                .:^!7777~:.                         ");
    mvwprintw(game, start_y++, start_x, "                                                            :^~7?JYJ?777!~.                         ");
    mvwprintw(game, start_y++, start_x, "                                                       ..:~!?JJJJJJ?7!77!~                          ");
    mvwprintw(game, start_y++, start_x, "                                                  ..:^~~!?YYYY?!!!!!!!!!!:                          ");
    mvwprintw(game, start_y++, start_x, "                                              ::^~~~~!7J?J5YJY5J7!!~!!!~:                           ");
    mvwprintw(game, start_y++, start_x, "                                           .:~7!!!!!!!77!!!77?7??77!!!~:                            ");
    mvwprintw(game, start_y++, start_x, "                                         .^7????!~~~!777!!!!!!~~!7!!~!~.                            ");
    mvwprintw(game, start_y++, start_x, "                                        :!7?77???!~~~~!!!777!!!!~~~!77!:                            ");
    mvwprintw(game, start_y++, start_x, "                                       ^777!777~~!??!^^~~!777777!!!7777!:                           ");
    mvwprintw(game, start_y++, start_x, "                                      :!???77??~!YYP5?~~^~!77?????7777!!!:                          ");
    mvwprintw(game, start_y++, start_x, "                                     .^77777??7!YY5B5P?~~!!!7?77777777777!:                         ");
    mvwprintw(game, start_y++, start_x, "                                     :~!~~~!!!!!7Y5PPY!~~^~!?J?????????777!:                        ");
    mvwprintw(game, start_y++, start_x, "                                    :^~~^^^~~~!!!!?7~^^^~!7?JJJJJ?????77?77!.                       ");
    mvwprintw(game, start_y++, start_x, "                                 .:~!~~~~~~^^~!777~^^~!!7????JJJJ??JJ?777777~.                      ");
    mvwprintw(game, start_y++, start_x, "                               .^!77!~~~!!!~7J777!~~!7?77777!!7?JJJJ?77!!!7!!^.                     ");
    mvwprintw(game, start_y++, start_x, "                             .^7??7!~~~~!77?JY?!!77777!!7??7!!!7?JJJJ?7!7JJ?7!!~:..                 ");
    mvwprintw(game, start_y++, start_x, "                           .^7???7!!~~~~!7?JJJYY??J???J5PPP5?~~~!?JYJ?7!?55Y?7777!!~^:              ");
    mvwprintw(game, start_y++, start_x, "                          :!?JJ??7!!~~~~~!777!!7!!??777P5P#PP?~~!?JYYJ?!75YY?777???77!~:            ");
    mvwprintw(game, start_y++, start_x, "                        .^!7??????7!!~~~~~!!!!!!~!777?7J5PGPGJ!~7?Y5YJ??Y55J???7JYYJ??!^.           ");
    mvwprintw(game, start_y++, start_x, "                       .~7????JJJ?7!!!~~~~~!!!!!!!???JJJ??7!JJ!!!7JYYYYYGGGY?JY?JYJ?!^.             ");
    mvwprintw(game, start_y++, start_x, "                       ^7??JJ??JJJ?7!!!!!!~~!!7777???JYYYJ??JY?7!7????Y55Y5JJJ7!^^:.                ");
    mvwprintw(game, start_y++, start_x, "                      .~7?????77??J??7!!!!!~~~!!77???J5YJJYYY55JJJ?777??77!~^:.                     ");
    mvwprintw(game, start_y++, start_x, "                      ^!7777777777???7!!!!7!7!!!!!7??JYYJJYYYYJJJ?777!~^::.                         ");
    mvwprintw(game, start_y++, start_x, "                     .!!777!!777777777!!!!7777777?777?JJJJJJJJJ?7!~^^:..                            ");
    mvwprintw(game, start_y++, start_x, "                    .~77!!!!7777?77777!!!!!7777??JJJ??JJJJ????7!^::..                                ");
    mvwprintw(game, start_y++, start_x, "                    :!!!!!!!!777777777!!!7!77!77!7???7??7777777!^:..                                ");
    mvwprintw(game, start_y++, start_x, "                   .~!7777!!!!7?????7??!!!!!!7!!!!777777!!!!!!!!^.                                  ");
    mvwprintw(game, start_y++, start_x, "                   .~7777777777???????J?7!77!!77!7!!7?77!!!!!!!!~.                                  ");
    mvwprintw(game, start_y++, start_x, "                   :!777??777??JYYJ??7?77!!777????7!77!!!!!!!!!!!~.                                 ");
    mvwprintw(game, start_y++, start_x, "                   :!7???7777???JJJ?7??7!!!!7JJYJ?7!!!!!!!!!!~~!!7~.                                ");
    mvwprintw(game, start_y++, start_x, "                   :77??7!!!!7JJ?7J?777!!!!!???J?7!!!!!!!!!!!!!!!77~.                               ");
    mvwprintw(game, start_y++, start_x, "                   ~????77!7!!?J?JJ7!!!!!!!??7??77!~~~!!!!!!!!~!!!77~.                              ");
    mvwprintw(game, start_y++, start_x, "                   ~777777?JJ?7J???7!!777!7?J??77!!!!~!!!!7!!!!!!!!77~.                             ");
    mvwprintw(game, start_y++, start_x, "                   ^!!!!~~!!7JJJYJ?7!!!7!!7JYJJJY?77!!~~!!!!!!!!!!7777~:.                           ");
    mvwprintw(game, start_y + 2, start_x, "Press any key to exit...");
    wrefresh(game); 
    wgetch(game); 
}
