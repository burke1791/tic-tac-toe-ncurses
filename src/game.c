#include <stdlib.h>
#include <ncurses.h>

#include "game.h"
#include "display.h"
#include "ai.h"

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

static Line get_winning_line(Board *b) {
  Piece winningPiece = PIECE_EMPTY;
  // top row
  if (b->squares[0]->piece == b->squares[1]->piece && b->squares[1]->piece == b->squares[2]->piece) return TOP_ROW;
  // middle row
  if (b->squares[3]->piece == b->squares[4]->piece && b->squares[4]->piece == b->squares[5]->piece) return MIDDLE_ROW;
  // bottom row
  if (b->squares[6]->piece == b->squares[7]->piece && b->squares[7]->piece == b->squares[8]->piece) return BOTTOM_ROW;

  // left col
  if (b->squares[0]->piece == b->squares[3]->piece && b->squares[3]->piece == b->squares[6]->piece) return LEFT_COL;
  // center col
  if (b->squares[1]->piece == b->squares[4]->piece && b->squares[4]->piece == b->squares[7]->piece) return CENTER_COL;
  // right col
  if (b->squares[2]->piece == b->squares[5]->piece && b->squares[5]->piece == b->squares[8]->piece) return RIGHT_COL;

  // back-slash diagonal
  if (b->squares[0]->piece == b->squares[4]->piece && b->squares[4]->piece == b->squares[8]->piece) return BACKSLASH;
  // forward-slash diagonal
  if (b->squares[6]->piece == b->squares[4]->piece && b->squares[4]->piece == b->squares[2]->piece) return FORWARD_SLASH;

  return NO_WINNER;
}

static Piece get_winning_piece(Board *b, Line wl) {
  Piece winner;

  switch (wl) {
    case MIDDLE_ROW:
    case CENTER_COL:
    case BACKSLASH:
    case FORWARD_SLASH:
      // these four lines go through the center square
      winner = b->squares[4]->piece;
      break;
    case TOP_ROW:
    case LEFT_COL:
      winner = b->squares[0]->piece;
      break;
    case BOTTOM_ROW:
    case RIGHT_COL:
      winner = b->squares[8]->piece;
      break;
    default:
      winner = PIECE_EMPTY;
  }

  return winner;
}

static SquareColor get_winning_line_color(Board *b, Line wl) {
  Piece winner = get_winning_piece(b, wl);
  SquareColor color = SQ_NONE;

  if (winner == PIECE_X) {
    color = SQ_GREEN;
  } else {
    color = SQ_RED;
  }

  return color;
}

static void set_line_color(Board *b, Line wl) {
  SquareColor c = get_winning_line_color(b, wl);

  switch (wl) {
    case TOP_ROW:
      set_top_row_color(b, c);
      break;
    case MIDDLE_ROW:
      set_middle_row_color(b, c);
      break;
    case BOTTOM_ROW:
      set_bottom_row_color(b, c);
      break;
    case LEFT_COL:
      set_left_col_color(b, c);
      break;
    case CENTER_COL:
      set_center_col_color(b, c);
      break;
    case RIGHT_COL:
      set_right_col_color(b, c);
      break;
    case BACKSLASH:
      set_backslash_color(b, c);
      break;
    case FORWARD_SLASH:
      set_forwardslash_color(b, c);
      break;
  }
}

/**
 * @brief checks if the board is full
 * 
 * @param b 
 * @return true 
 * @return false 
 */
static bool is_board_full(Board *b) {
  for (int i = 0; i < 9; i++) {
    if (b->squares[i]->piece == PIECE_EMPTY) return false;
  }

  return true;
}

static void set_game_state_turn(Game *g) {
  int numX = 0;
  int numO = 0;

  for (int i = 0; i < 9; i++) {
    if (g->board->squares[i]->piece == PIECE_X) numX++;
    if (g->board->squares[i]->piece == PIECE_O) numO++;
  }

  if (numO >= numX) {
    g->state = GS_PLAYER_TURN;
  } else {
    g->state = GS_CPU_TURN;
  }
}

/**
 * @brief checks the board for an ending condition and updates the
 *        game state accordingly
 * 
 * @param g 
 */
static void update_game_state(Game *g) {
  Line wl = get_winning_line(g->board);
  Piece winner = get_winning_piece(g->board, wl);

  switch (winner) {
    case PIECE_X:
      g->state = GS_END_X;
      set_line_color(g->board, wl);
      break;
    case PIECE_O:
      g->state = GS_END_O;
      set_line_color(g->board, wl);
      break;
    case PIECE_EMPTY:
      if (is_board_full(g->board)) {
        g->state = GS_END_TIE;
      } else {
        set_game_state_turn(g);
      }
      break;
  }
}

void play(Game *g) {
  update_game_state(g);
  refresh_display(g);

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

    update_game_state(g);
    refresh_display(g);

    // AI logic
    if (g->state == GS_CPU_TURN) {
      int cpuMove = next_move(g);
      place_o(g->board, cpuMove);

      update_game_state(g);
      refresh_display(g);
    }

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