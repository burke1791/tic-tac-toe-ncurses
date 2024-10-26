#include <stdlib.h>
#include <ncurses.h>

#include "game.h"
#include "display.h"

static Cursor *new_cursor() {
  Cursor *c = malloc(sizeof(Cursor));
  c->row = BOARD_ORIGIN_ROW;
  c->col = BOARD_ORIGIN_COL + 1;

  return c;
}

static void destroy_cursor(Cursor *c) {
  free(c);
}

Game *new_game() {
  Game *g = malloc(sizeof(Game));
  g->state = GS_INIT;
  g->board = new_board();
  g->cursor = new_cursor();

  return g;
}

void destroy_game(Game *g) {
  destroy_board(g->board);
  destroy_cursor(g->cursor);
  free(g);
}

static UserAction parse_user_action(int c) {
  switch (c) {
    case 81:
    case 113:
      return UA_QUIT;
    case KEY_UP:
      return UA_CURSOR_UP;
    case KEY_DOWN:
      return UA_CURSOR_DOWN;
    case KEY_LEFT:
      return UA_CURSOR_LEFT;
    case KEY_RIGHT:
      return UA_CURSOR_RIGHT;
    case 32:
      return UA_PLACE_PIECE;
    default:
      return UA_NONE;
  }
}

static void user_place_piece(Board *b, Cursor *c) {
  int pos = get_board_pos_from_cursor(b, c);

  place_x(b, pos);
}

static void move_cursor(Board *b, Cursor *c, CursorDirection d) {
  int pos = get_board_pos_from_cursor(b, c);
  int newPos;

  switch (d) {
    case CUR_UP:
      newPos = pos - 3;
      break;
    case CUR_DOWN:
      newPos = pos + 3;
      break;
    // allowing wraparound movement
    case CUR_LEFT:
      newPos = pos - 1;
      break;
    case CUR_RIGHT:
      newPos = pos + 1;
      break;
    default:
      newPos = pos;
  }

  if (newPos < 0 || newPos > 8) newPos = pos;

  c->row = b->squares[newPos]->row;
  c->col = b->squares[newPos]->col;
}

void play(Game *g) {
  // temp code to kick off the game
  g->state = GS_PLAYER_TURN;

  int input;

  while (true) {
    input = getch();

    switch (parse_user_action(input)) {
      case UA_QUIT:
        return;
      case UA_NONE:
        continue;
      case UA_PLACE_PIECE:
        user_place_piece(g->board, g->cursor);
        break;
      case UA_CURSOR_UP:
        move_cursor(g->board, g->cursor, CUR_UP);
        break;
      case UA_CURSOR_DOWN:
        move_cursor(g->board, g->cursor, CUR_DOWN);
        break;
      case UA_CURSOR_LEFT:
        move_cursor(g->board, g->cursor, CUR_LEFT);
        break;
      case UA_CURSOR_RIGHT:
        move_cursor(g->board, g->cursor, CUR_RIGHT);
        break;
    }

    refresh_display(g);

    /*
      check for an ending condition
      set game state to GS_CPU_TURN if the player played a piece
      run CPU logic
      refresh display
      check for an ending condition
      repeat
    */
  }
}