#include <stdlib.h>
#include <ncurses.h>

#include "display.h"
#include "game.h"

static void paint_board(Board *b, Cursor *c);

void init_display() {
  initscr();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);
}

void refresh_display(Game *g) {
  paint_board(g->board, g->cursor);

  refresh();
}

void kill_display() {
  endwin();
}

static char get_piece_char(Square *s) {
  switch (s->piece) {
    case PIECE_EMPTY:
      return ' ';
    case PIECE_X:
      return 'X';
    case PIECE_O:
      return 'O';
  }
}

static bool is_cursor_on_square(Cursor *c, Square *s) {
  return c->row == s->row && c->col == s->col;
}

static void paint_board(Board *b, Cursor *c) {
  // horizontal lines
  mvprintw(BOARD_ORIGIN_ROW + BOARD_ROW_GAP, BOARD_ORIGIN_COL, "-----------");
  mvprintw(BOARD_ORIGIN_ROW + (BOARD_ROW_GAP * 2) + 1, BOARD_ORIGIN_COL, "-----------");

  // left vertical
  mvprintw(BOARD_ORIGIN_ROW, BOARD_ORIGIN_COL + BOARD_COL_GAP, "|");
  mvprintw(BOARD_ORIGIN_ROW + 2, BOARD_ORIGIN_COL + BOARD_COL_GAP, "|");
  mvprintw(BOARD_ORIGIN_ROW + 4, BOARD_ORIGIN_COL + BOARD_COL_GAP, "|");

  // right vertical
  mvprintw(BOARD_ORIGIN_ROW, BOARD_ORIGIN_COL + (BOARD_COL_GAP * 2) + 1, "|");
  mvprintw(BOARD_ORIGIN_ROW + 2, BOARD_ORIGIN_COL + (BOARD_COL_GAP * 2) + 1, "|");
  mvprintw(BOARD_ORIGIN_ROW + 4, BOARD_ORIGIN_COL + (BOARD_COL_GAP * 2) + 1, "|");

  // draw pieces
  for (int i = 0; i < 9; i++) {
    Square *s = b->squares[i];
    char p = get_piece_char(s);
    bool isCursor = is_cursor_on_square(c, s);

    if (isCursor && s->piece == PIECE_EMPTY) {
      mvprintw(s->row, s->col, "%s", CURSOR_SQUARE);
    } else if (isCursor && s->piece != PIECE_EMPTY) {
      move(s->row, s->col);
      addch(p | A_UNDERLINE);
    } else {
      mvprintw(s->row, s->col, "%c", p);
    }
  }
}