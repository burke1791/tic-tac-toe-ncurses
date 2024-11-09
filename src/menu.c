#include <stdlib.h>

#include "game.h"
#include "display.h"

Menu *new_menu() {
  Menu *m = malloc(sizeof(Menu));
  int rowSpacing = 0;

  for (int i = 0; i < 2; i++) {
    m->items[i] = malloc(sizeof(MenuItem));
    m->items[i]->loc = new_location(MENU_ORIGIN_ROW + rowSpacing, MENU_ORIGIN_COL);
    rowSpacing += MENU_ROW_GAP;
  }

  m->items[0]->menuAction = MENU_NEW_GAME;
  m->items[1]->menuAction = MENU_QUIT;

  return m;
}

void destroy_menu(Menu *m) {
  for (int i = 0; i < 2; i++) {
    destroy_location(m->items[i]->loc);
    free(m->items[i]);
  }
  
  free(m);
}

int get_menu_pos_from_cursor(Menu *m, Cursor *c) {
  for (int i = 0; i < 2; i++) {
    if (c->loc->row == m->items[i]->loc->row) return i;
  }

  return -1;
}