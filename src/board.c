#include <stdlib.h>
#include <stdbool.h>

#include "game.h"
#include "display.h"

Board *new_board() {
  Board *b = malloc(sizeof(Board));

  for (int i = 0; i < 9; i++) {
    b->squares[i] = malloc(sizeof(Square));
    b->squares[i]->loc = new_location(0, 0);
    b->squares[i]->piece = PIECE_EMPTY;
    b->squares[i]->color = SQ_NONE;
  }

  // top-left
  b->squares[0]->loc->row = BOARD_ORIGIN_ROW;
  b->squares[0]->loc->col = BOARD_ORIGIN_COL + 1;

  // top-center
  b->squares[1]->loc->row = BOARD_ORIGIN_ROW;
  b->squares[1]->loc->col = BOARD_ORIGIN_COL + 2 + BOARD_COL_GAP;

  // top-right
  b->squares[2]->loc->row = BOARD_ORIGIN_ROW;
  b->squares[2]->loc->col = BOARD_ORIGIN_COL + 3 + (BOARD_COL_GAP * 2);

  // middle-left
  b->squares[3]->loc->row = BOARD_ORIGIN_ROW + 2;
  b->squares[3]->loc->col = BOARD_ORIGIN_COL + 1;

  // middle-center
  b->squares[4]->loc->row = BOARD_ORIGIN_ROW + 2;
  b->squares[4]->loc->col = BOARD_ORIGIN_COL + 2 + BOARD_COL_GAP;

  // middle-right
  b->squares[5]->loc->row = BOARD_ORIGIN_ROW + 2;
  b->squares[5]->loc->col = BOARD_ORIGIN_COL + 3 + (BOARD_COL_GAP * 2);

  // bottom-left
  b->squares[6]->loc->row = BOARD_ORIGIN_ROW + 4;
  b->squares[6]->loc->col = BOARD_ORIGIN_COL + 1;

  // bottom-center
  b->squares[7]->loc->row = BOARD_ORIGIN_ROW + 4;
  b->squares[7]->loc->col = BOARD_ORIGIN_COL + 2 + BOARD_COL_GAP;

  // bottom-right
  b->squares[8]->loc->row = BOARD_ORIGIN_ROW + 4;
  b->squares[8]->loc->col = BOARD_ORIGIN_COL + 3 + (BOARD_COL_GAP * 2);

  return b;
}

void destroy_board(Board *b) {
  for (int i = 0; i < 9; i++) {
    destroy_location(b->squares[i]->loc);
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
    if (s->loc->row == c->loc->row && s->loc->col == c->loc->col) return i;
  }

  return -1;
}

void set_top_row_color(Board *b, SquareColor c) {
  b->squares[0]->color = c;
  b->squares[1]->color = c;
  b->squares[2]->color = c;
}

void set_middle_row_color(Board *b, SquareColor c) {
  b->squares[3]->color = c;
  b->squares[4]->color = c;
  b->squares[5]->color = c;
}

void set_bottom_row_color(Board *b, SquareColor c) {
  b->squares[6]->color = c;
  b->squares[7]->color = c;
  b->squares[8]->color = c;
}

void set_left_col_color(Board *b, SquareColor c) {
  b->squares[0]->color = c;
  b->squares[3]->color = c;
  b->squares[6]->color = c;
}

void set_center_col_color(Board *b, SquareColor c) {
  b->squares[1]->color = c;
  b->squares[4]->color = c;
  b->squares[7]->color = c;
}

void set_right_col_color(Board *b, SquareColor c) {
  b->squares[2]->color = c;
  b->squares[5]->color = c;
  b->squares[8]->color = c;
}

void set_backslash_color(Board *b, SquareColor c) {
  b->squares[0]->color = c;
  b->squares[4]->color = c;
  b->squares[8]->color = c;
}

void set_forwardslash_color(Board *b, SquareColor c) {
  b->squares[6]->color = c;
  b->squares[4]->color = c;
  b->squares[2]->color = c;
}