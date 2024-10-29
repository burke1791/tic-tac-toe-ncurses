#include <stdlib.h>

#include "ai.h"

/**
 * @brief runs the AI logic and returns the position of the square
 *        where the next move should be made
 * 
 * @param g 
 * @return int 
 */
int next_move(Game *g) {

  for (int i = 0; i < 9; i++) {
    if (g->board->squares[i]->piece == PIECE_EMPTY) return i;
  }

  return -1;
}