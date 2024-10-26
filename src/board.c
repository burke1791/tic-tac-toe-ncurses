#include <stdlib.h>
#include <stdbool.h>

#include "game.h"
#include "display.h"

Board *new_board() {
  Board *b = malloc(sizeof(Board));

  for (int i = 0; i < 9; i++) {
    b->squares[i] = malloc(sizeof(Square));
    b->squares[i]->piece = PIECE_EMPTY;
  }

  // top-left
  b->squares[0]->row = BOARD_ORIGIN_ROW;
  b->squares[0]->col = BOARD_ORIGIN_COL + 1;

  // top-center
  b->squares[1]->row = BOARD_ORIGIN_ROW;
  b->squares[1]->col = BOARD_ORIGIN_COL + 2 + BOARD_COL_GAP;

  // top-right
  b->squares[2]->row = BOARD_ORIGIN_ROW;
  b->squares[2]->col = BOARD_ORIGIN_COL + 3 + (BOARD_COL_GAP * 2);

  // middle-left
  b->squares[3]->row = BOARD_ORIGIN_ROW + 2;
  b->squares[3]->col = BOARD_ORIGIN_COL + 1;

  // middle-center
  b->squares[4]->row = BOARD_ORIGIN_ROW + 2;
  b->squares[4]->col = BOARD_ORIGIN_COL + 2 + BOARD_COL_GAP;

  // middle-right
  b->squares[5]->row = BOARD_ORIGIN_ROW + 2;
  b->squares[5]->col = BOARD_ORIGIN_COL + 3 + (BOARD_COL_GAP * 2);

  // bottom-left
  b->squares[6]->row = BOARD_ORIGIN_ROW + 4;
  b->squares[6]->col = BOARD_ORIGIN_COL + 1;

  // bottom-center
  b->squares[7]->row = BOARD_ORIGIN_ROW + 4;
  b->squares[7]->col = BOARD_ORIGIN_COL + 2 + BOARD_COL_GAP;

  // bottom-right
  b->squares[8]->row = BOARD_ORIGIN_ROW + 4;
  b->squares[8]->col = BOARD_ORIGIN_COL + 3 + (BOARD_COL_GAP * 2);

  return b;
}

void destroy_board(Board *b) {
  for (int i = 0; i < 9; i++) {
    free(b->squares[i]);
  }
  free(b);
}

static bool validate_new_piece(Board *b, int pos, Piece p) {
  return true;
}

static BoardPlacementResult place_piece(Board *b, int pos, Piece p) {
  b->squares[pos]->piece = p;
}

BoardPlacementResult place_x(Board *b, int pos) {
  place_piece(b, pos, PIECE_X);
}

BoardPlacementResult place_o(Board *b, int pos) {
  place_piece(b, pos, PIECE_O);
}

int get_board_pos_from_cursor(Board *b, Cursor *c) {
  int pos;

  for (int i = 0; i < 9; i++) {
    Square *s = b->squares[i];
    if (s->row == c->row && s->col == c->col) return i;
  }

  return -1;
}