#include <stdlib.h>
#include <ncurses.h>

#include "display.h"
#include "game.h"

#define SUCCESS_PAIR 1
#define FAIL_PAIR 2

static void paint_board(Board *b, Cursor *c);
static void paint_header(Game *g);

void init_display() {
  initscr();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);

  start_color();
  init_pair(SUCCESS_PAIR, COLOR_GREEN, COLOR_BLACK);
  init_pair(FAIL_PAIR, COLOR_RED, COLOR_BLACK);
}

void refresh_display(Game *g) {
  paint_header(g);
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

    if (s->color == SQ_RED) {
      attron(COLOR_PAIR(FAIL_PAIR));
    } else if (s->color == SQ_GREEN) {
      attron(COLOR_PAIR(SUCCESS_PAIR));
    }

    if (isCursor && s->piece == PIECE_EMPTY) {
      mvprintw(s->row, s->col, "%s", CURSOR_SQUARE);
    } else if (isCursor && s->piece != PIECE_EMPTY) {
      move(s->row, s->col);
      addch(p | A_UNDERLINE);
    } else {
      move(s->row, s->col);
      addch(p);
    }

    if (s->color == SQ_RED) {
      attroff(COLOR_PAIR(FAIL_PAIR));
    } else if (s->color == SQ_GREEN) {
      attroff(COLOR_PAIR(SUCCESS_PAIR));
    }
  }
}

static void paint_header(Game *g) {
  int row = BOARD_ORIGIN_ROW - 2;
  int col = BOARD_ORIGIN_COL;
  move(row, col);
  clrtoeol();

  switch (g->state) {
    case GS_PLAYER_TURN:
      mvprintw(row, col, "Turn: you");
      break;
    case GS_CPU_TURN:
      mvprintw(row, col, "Turn: cpu");
      break;
    case GS_END_TIE:
      mvprintw(row, col, "Result: tie");
      break;
    case GS_END_X:
      mvprintw(row, col, "Result: you win!");
      break;
    case GS_END_O:
      mvprintw(row, col, "Result: cpu wins :(");
      break;
  }
}