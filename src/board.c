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

void reset_square(Square *s) {
  s->color = SQ_NONE;
  s->piece = PIECE_EMPTY;
}

void reset_board(Board *b) {
  for (int i = 0; i < 9; i++) {
    reset_square(b->squares[i]);
  }
}

int num_empty_squares(Board *b) {
  int count = 0;
  for (int i = 0; i < 9; i++) {
    if (b->squares[i]->piece == PIECE_EMPTY) count++;
  }
  return count;
}

static bool validate_new_piece(Board *b, int pos, Piece p) {
  // is the requested square occupied?
  if (pos < 0 || pos > 8) return false;
  if (b->squares[pos]->piece != PIECE_EMPTY) return false;

  int numX = 0;
  int numO = 0;

  for (int i = 0; i < 9; i++) {
    if (b->squares[i]->piece == PIECE_X) numX++;
    if (b->squares[i]->piece == PIECE_O) numO++;
  }

  // X's and O's must be equivalent
  if (p == PIECE_X && numX != numO) return false;
  
  // X's must be one higher than O's
  if (p == PIECE_O && (numX - numO) != 1) return false;

  return true;
}

BoardPlacementResult place_piece(Board *b, int pos, Piece p) {
  if (!validate_new_piece(b, pos, p)) return BPR_INVALID;

  b->squares[pos]->piece = p;
  return BPR_OK;
}

int get_board_pos_from_cursor(Board *b, Cursor *c) {
  int pos;

  for (int i = 0; i < 9; i++) {
    Square *s = b->squares[i];
    if (s->loc->row == c->loc->row && s->loc->col == c->loc->col) return i;
  }

  return -1;
}

char get_piece_char(Piece p) {
  switch (p) {
    case PIECE_EMPTY:
      return ' ';
    case PIECE_X:
      return 'X';
    case PIECE_O:
      return 'O';
  }
}

char get_piece_char_from_square(Square *s) {
  return get_piece_char(s->piece);
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