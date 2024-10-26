#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>

#include "display.h"
#include "game.h"

int main() {
  setlocale(LC_ALL, "");
  init_display();

  Game *g = new_game();

  refresh_display(g);

  play(g);

  kill_display();

  destroy_game(g);

  return 0;
}