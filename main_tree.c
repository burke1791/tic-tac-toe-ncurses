#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>

#include "game.h"

int main() {
  Game *g = new_game();

  g->board->squares[0]->piece = PIECE_EMPTY;
  g->board->squares[1]->piece = PIECE_EMPTY;
  g->board->squares[2]->piece = PIECE_X;

  g->board->squares[3]->piece = PIECE_EMPTY;
  g->board->squares[4]->piece = PIECE_X;
  g->board->squares[5]->piece = PIECE_EMPTY;

  g->board->squares[6]->piece = PIECE_O;
  g->board->squares[7]->piece = PIECE_O;
  g->board->squares[8]->piece = PIECE_X;

  play_noui(g);

  destroy_game(g);

  return 0;
}