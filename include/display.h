#ifndef DISPLAY_H
#define DISPLAY_H

#include "game.h"

#define BOARD_ORIGIN_ROW 3
#define BOARD_ORIGIN_COL 6
#define BOARD_ROW_GAP 1
#define BOARD_COL_GAP 3

#define MENU_ORIGIN_ROW 9
#define MENU_ORIGIN_COL 5
#define MENU_PADDING 2
#define MENU_ROW_GAP 1

#define CURSOR_SQUARE "\u25A0"

void init_display();
void refresh_display(Game *g);
void kill_display();

#endif /* DISPLAY_H */